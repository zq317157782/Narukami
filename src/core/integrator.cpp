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
NARUKAMI_BEGIN

void Integrator::render(const Scene& scene){
            MemoryArena arena;
            auto film = _camera->get_film();
            auto cropped_pixel_bounds=film->cropped_pixel_bounds();
            for (auto &&pixel : cropped_pixel_bounds)
            {
                auto sampler = _sampler->clone(pixel.x+pixel.y*width(cropped_pixel_bounds));
                sampler->start_pixel(pixel);
                do{
                    auto camera_sample=sampler->get_camera_sample(pixel);
                    Ray ray;
                    float w=_camera->generate_normalized_ray(camera_sample,&ray);
                    Interaction interaction;
                    if(scene.intersect(arena,ray,&interaction)){
                        float t =clamp(interaction.hit_t,0,1);
                        auto N = (interaction.n + 1.0f) * 0.5f;
                        film->add_sample(camera_sample.pFilm,{N.x,N.y,N.z},w);
                    }else{
                        film->add_sample(camera_sample.pFilm,{0,0,0},w);
                    }
                   
                }while(sampler->start_next_sample());
                
            }
        }

NARUKAMI_END