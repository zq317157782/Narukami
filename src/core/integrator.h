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
#pragma once

#include "core/narukami.h"
#include "core/camera.h"
#include "core/scene.h"
#include "core/sampler.h"
NARUKAMI_BEGIN
class Integrator{
    private:
        std::shared_ptr<const Camera> _camera;
        std::shared_ptr<Sampler> _sampler;
    public:
        Integrator(std::shared_ptr<const Camera> camera,std::shared_ptr<Sampler> sampler):_camera(std::move(camera)),_sampler(std::move(sampler)){}

        void render(const Scene& scene){
            auto film = _camera->get_film();
            auto cropped_pixel_bounds=film->cropped_pixel_bounds();
            for (auto &&pixel : cropped_pixel_bounds)
            {
                auto sampler = _sampler->clone(pixel.x+pixel.y*width(cropped_pixel_bounds));
                sampler->start_pixel(pixel);
                do{
                    auto camera_sample=sampler->get_camera_sample(pixel);
                }while(sampler->start_next_sample());
            }
        }
};
NARUKAMI_END