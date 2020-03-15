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
#include "core/interaction.h"
#include "core/stat.h"
NARUKAMI_BEGIN

STAT_PERCENT("integrator/miss intersection's ratio",miss_intersection_num,miss_intersection_denom)
STAT_COUNTER("integrator/scene ray's count",ray_count)
STAT_COUNTER("integrator/shadow ray's count",shadow_ray_count)
class Integrator{
    private:
        Camera* _camera;
        Sampler* _sampler;
    public:
        Integrator(Camera* camera,Sampler* sampler):_camera(camera),_sampler(sampler){}
        void render(const Scene& scene);
};
NARUKAMI_END