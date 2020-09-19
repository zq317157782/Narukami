/*
MIT License

Copyright (c) 2019 ZhuQian

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "core/integrator.h"
#include "core/geometry.h"
#include "core/sampling.h"
#include "core/light.h"
#include "core/parallel.h"
#include "core/interaction.h"
#include "core/progressreporter.h"
NARUKAMI_BEGIN

void Integrator::render(const Scene &scene)
{

    auto film = _camera->get_film();
    auto sample_bounds = film->get_sample_bounds();
    auto sample_extent = diagonal(sample_bounds);
    const int tile_size = 64;
    const auto tile_count_x = (sample_extent.x + (tile_size - 1)) / tile_size;
    const auto tile_count_y = (sample_extent.y + (tile_size - 1)) / tile_size;
    const Point2i tile_count(tile_count_x, tile_count_y);
    ProgressReporter rendering_reporter(tile_count_x * tile_count_y, "rendering");
    parallel_for_2D(
        [&](Point2i tile_index) {
            MemoryArena arena;
            int seed = tile_index.y * tile_count.x + tile_index.x;
            auto clone_sampler = _sampler->clone(seed);

            auto tile_bounds_min_x = sample_bounds.min_point.x + tile_index.x * tile_size;
            auto tile_bounds_min_y = sample_bounds.min_point.y + tile_index.y * tile_size;
            auto tile_bounds_max_x = min(tile_bounds_min_x + tile_size, sample_bounds.max_point.x);
            auto tile_bounds_max_y = min(tile_bounds_min_y + tile_size, sample_bounds.max_point.y);
            Bounds2i tile_bounds(Point2i(tile_bounds_min_x, tile_bounds_min_y), Point2i(tile_bounds_max_x, tile_bounds_max_y));

            auto film_tile = film->get_film_tile(tile_bounds);

            for (auto &&pixel : tile_bounds)
            {
                clone_sampler->start_pixel(pixel);
                do
                {
                    STAT_INCREASE_COUNTER(miss_intersection_denom, 1)
                    // film->add_sample(pixel,{clone_sampler->get_1D(),clone_sampler->get_1D(),clone_sampler->get_1D()}, 1);
                    auto camera_sample = clone_sampler->get_camera_sample(pixel);
                    RayDifferential ray;
                    float w = _camera->generate_normalized_ray_differential(camera_sample, &ray);
                    STAT_INCREASE_MEMORY_COUNTER(ray_count, 1)
                    SurfaceInteraction interaction;
                    constexpr int bounce_count = 0;
                    Color L(0.0f, 0.0f, 0.0f);
                    float throughout = 1.0f;
#if 1 //Debug
                     if (scene.intersect(arena, ray, &interaction))
                     {
                         compute_differential(ray,interaction);
                         
                        //Normal Debug
                        // {
                        //     float r = interaction.n.x * 0.5f + 0.5f;
                        //     float g = interaction.n.y * 0.5f + 0.5f;
                        //     float b = interaction.n.z * 0.5f + 0.5f;
                        //     L = Color(r,g,b); 
                        // }
                        // UV
                        // {
                        //     float r = interaction.uv.x;
                        //     float g = interaction.uv.y;
                        //     float b = 0;
                        //     L = Color(r,g,b); 
                        // }

                        //dot(dpdx,dpdy)
                        {
                            Normal3f n = normalize(cross(interaction.dpdx,interaction.dpdy));
                            float r = n.x * 0.5f + 0.5f;
                            float g = n.y * 0.5f + 0.5f;
                            float b = n.z * 0.5f + 0.5f;
                            L = Color(r,g,b); 
                        }
                       
                         
                     }
#else
                    int bounce = 0;
                    for (; bounce <= bounce_count; ++bounce)
                    {

                        if (scene.intersect(arena, ray, &interaction))
                        {
                            if (is_surface_interaction(interaction))
                            {
                                SurfaceInteraction &surface_interaction = static_cast<SurfaceInteraction &>(interaction);

                                L = L + Le(surface_interaction, ray.d);

                                for (auto light : scene.lights)
                                {
                                    Vector3f wi;
                                    float pdf;
                                    VisibilityTester tester;
                                    auto Li = light->sample_Li(surface_interaction, clone_sampler->get_2D(), &wi, &pdf, &tester);
                                    if (pdf > 0 && !is_black(Li) && tester.unoccluded(scene))
                                    {
                                        L = L + INV_PI * saturate(dot(surface_interaction.n, wi)) * throughout * Li * rcp(pdf);
                                    }
                                }

                                // if (bounce < bounce_count)
                                // {
                                //     auto direction_object = cosine_sample_hemisphere(clone_sampler->get_2D());
                                //     auto object_to_world = get_object_to_world(surface_interaction);
                                //     auto direction_world =  hemisphere_flip(normalize(object_to_world(direction_object)),interaction.n);
                                //     ray = Ray(interaction.p, direction_world);
                                //     ray = offset_ray(ray, interaction.n);
                                //     STAT_INCREASE_MEMORY_COUNTER(ray_count, 1)

                                //     throughout *= INV_PI * abs(direction_object.z);
                                // }
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                    STAT_INCREASE_COUNTER_CONDITION(miss_intersection_num, 1, bounce == 0)
#endif
                    film_tile->add_sample(camera_sample.pFilm, L, w);
                    arena.reset();
                } while (clone_sampler->start_next_sample());
            }
            film->merge_film_tile(std::move(film_tile));
            rendering_reporter.update(1);
        },
        tile_count);
    rendering_reporter.done();
}

NARUKAMI_END