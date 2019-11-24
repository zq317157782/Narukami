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
#include "core/affine.h"
NARUKAMI_BEGIN

void Integrator::render(const Scene &scene)
{
    MemoryArena arena;
    auto film = _camera->get_film();
    auto cropped_pixel_bounds = film->cropped_pixel_bounds();
    for (auto &&pixel : cropped_pixel_bounds)
    {
        auto clone_sampler = _sampler->clone(pixel.x+pixel.y*width(cropped_pixel_bounds));
        clone_sampler->start_pixel(pixel);
        do
        {
           // film->add_sample(pixel,{clone_sampler->get_1D(),clone_sampler->get_1D(),clone_sampler->get_1D()}, 1);
            auto camera_sample = clone_sampler->get_camera_sample(pixel);
            Ray ray;
            float w = _camera->generate_normalized_ray(camera_sample, &ray);
            Interaction interaction;

            if (scene.intersect(arena, ray, &interaction))
            {
                if (is_surface_interaction(interaction))
                {
                    SurfaceInteraction &surface_interaction = static_cast<SurfaceInteraction &>(interaction);
                    Spectrum L(0.0f, 0.0f, 0.0f);

                    L = L + Le(surface_interaction,ray.d);

                    for (auto &light : scene.lights)
                    {
                        Vector3f wi;
                        float pdf;
                        VisibilityTester tester;
                        auto Li = light->sample_Li(surface_interaction, clone_sampler->get_2D(), &wi, &pdf, &tester);
                        if (pdf > 0 && !is_black(Li)  && tester.unoccluded(scene))
                        {
                            L = L + INV_PI * saturate(dot(surface_interaction.n, wi)) * Li * rcp(pdf);
                        }
                    }

                    film->add_sample(camera_sample.pFilm, L, w);
                //    auto nn = surface_interaction.n * 0.5f + 0.5f;
                //     film->add_sample(camera_sample.pFilm,Spectrum(nn.x,nn.y,nn.z), w);
                }
            }
            else
            {
                film->add_sample(camera_sample.pFilm, {0, 0, 0}, w);
            }

        } while (clone_sampler->start_next_sample());
        arena.reset();
    }
}

NARUKAMI_END