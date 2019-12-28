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
    uint32_t _current_sample_index;
    uint32_t _spp;
    const uint32_t _max_dim;
    uint32_t _sample_1d_offset;
    uint32_t _sample_2d_offset;
    std::vector<uint32_t> _scramble_1d;
    std::vector<uint32_t> _scramble_2d;

    Point2i _current_pixel;
    RNG _rng;
    uint64_t _rng_seed;

public:
    Sampler(const uint32_t spp, const uint32_t max_dim = 5);
    void start_pixel(const Point2i &p);
    bool start_next_sample();
    Point2f get_2D();
    float get_1D();
    CameraSample get_camera_sample(const Point2i &raster);
    inline void set_sample_index(const uint32_t idx)
    {
        _current_sample_index = idx;
    }
    inline uint32_t get_spp() const
    {
        return _spp;
    }
    std::unique_ptr<Sampler> clone(const uint64_t seed) const;
};
NARUKAMI_END