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

struct SamplerState
{
    uint32_t current_sample_index;
    std::vector<uint32_t> sample_1d_offsets;
    std::vector<uint32_t> sample_2d_offsets;
    std::vector<uint32_t> scramble_1d;
    std::vector<uint32_t> scramble_2d;
    Point2i pixel;
};

class Sampler
{
private:
    uint32_t _spp;
    const uint32_t _max_dim;
    RNG _rng;
    std::map<Point2i,SamplerState> _states;
    SamplerState* _current_state;
public:
    Sampler(const uint32_t spp, const uint32_t max_dim = 5);

    void switch_pixel(const Point2i &p);
    

    void switch_sample(const uint32_t sample_index)
    {
        assert(sample_index<_spp);
        _current_state->current_sample_index = sample_index;
    }

    bool switch_to_next_sample();

    bool is_completed() const;

    Point2f get_2D();
    float get_1D();
    CameraSample get_camera_sample(const Point2i &raster);
    inline void set_sample_index(const uint32_t idx)
    {
        _current_state->current_sample_index = idx;
    }
    inline uint32_t get_spp() const
    {
        return _spp;
    }
    std::unique_ptr<Sampler> clone(const uint64_t seed) const;
};
NARUKAMI_END