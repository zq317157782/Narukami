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
#include "core/affine.h"
#include "core/rng.h"
#include "core/lowdiscrepancy.h"
#include "core/memory.h"
#include <vector>
#include <map>
NARUKAMI_BEGIN
struct CameraSample
{
    Point2f pFilm;
    Point2f pLens;
    float time;
};

class Sampler
{
private:
    
    uint32_t _dim_1d;
    uint32_t _dim_2d;
    uint32_t _sample_idx;
    std::vector<std::vector<float>> _samples_1d;
    std::vector<std::vector<Point2f>> _samples_2d;
    RNG _rng;
    const uint32_t _spp;
    const uint32_t _max_dim;
public:
    Sampler(const uint32_t spp, const uint32_t max_dim = 5);
    void start_pixel(const Point2i &p);
    bool start_next_sample();
    Point2f get_2D();
    float get_1D();
    CameraSample get_camera_sample(const Point2i &raster);
    inline uint32_t get_spp() const{return _spp;}
    std::unique_ptr<Sampler> clone(const uint64_t seed) const;
};
NARUKAMI_END