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
#include "core/sampler.h"

NARUKAMI_BEGIN
Sampler::Sampler(const uint32_t spp, const uint32_t max_dim) : _spp(spp), _max_dim(max_dim)
{
    _scramble_1d = std::vector<uint32_t>(_max_dim);
    _scramble_2d = std::vector<uint32_t>(_max_dim * 2);
}

void Sampler::start_pixel(const Point2i &p)
{
    _current_pixel = p;
    _current_sample_index = 0;
    _sample_1d_offset = 0;
    _sample_2d_offset = 0;

    _rng.set_seed(_rng_seed);
    //generate all scramble number
    for (size_t i = 0; i < _max_dim; i++)
    {
        _scramble_1d[i] = _rng.next_uint32();
        _scramble_2d[i * 2] = _rng.next_uint32();
        _scramble_2d[i * 2 + 1] = _rng.next_uint32();
    }
}

bool Sampler::start_next_sample()
{
    _current_sample_index++;
    if (EXPECT_NOT_TAKEN(_current_sample_index >= _spp))
    {
        return false;
    }
    _sample_1d_offset = 0;
    _sample_2d_offset = 0;
    return true;
}

Point2f Sampler::get_2D()
{
    if (EXPECT_TAKEN(_sample_2d_offset < _max_dim))
    {
        auto sample = sample_scrambled_gray_code_sobol02(_current_sample_index, &_scramble_2d[_sample_2d_offset * 2], &_scramble_2d[_sample_2d_offset * 2 + 1]);
        _sample_2d_offset++;
        return sample;
    }
    else
    {
        auto sample = Point2f(_rng.next_float(), _rng.next_float());
        _sample_2d_offset++;
        return sample;
    }
}
float Sampler::get_1D()
{
    if (EXPECT_TAKEN(_sample_1d_offset < _max_dim))
    {
        auto sample = sample_scrambled_gray_code_van_der_corput(_current_sample_index, &_scramble_1d[_sample_1d_offset]);
        _sample_1d_offset++;
        return sample;
    }
    else
    {
        _sample_1d_offset++;
        return _rng.next_float();
    }
}
CameraSample Sampler::get_camera_sample(const Point2i &raster)
{
    CameraSample cs;
    cs.pFilm = get_2D() + Point2f(raster);
    cs.pLens = get_2D();
    cs.time = get_1D();
    return cs;
}
std::unique_ptr<Sampler> Sampler::clone(const uint64_t seed) const
{
    auto sampler = narukami::make_unique<Sampler>(*this);
    sampler->_rng_seed = seed;
    return sampler;
}
NARUKAMI_END