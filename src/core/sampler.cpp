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
#include "core/narukami.h"
#include "core/sampler.h"

NARUKAMI_BEGIN
Sampler::Sampler(const uint32_t spp, const uint32_t max_dim):_dim_1d(0),_dim_2d(0),_sample_idx(0),_spp(round_up_pow2(spp)),_max_dim(max_dim)
{
    _samples_1d.resize(_max_dim);
    for(int i=0;i<_samples_1d.size();++i)
    {
        _samples_1d[i].resize(_spp);
    }

    _samples_2d.resize(_max_dim);
     for(int i=0;i<_samples_2d.size();++i)
    {
        _samples_2d[i].resize(_spp);
    }
}

void Sampler::start_pixel(const Point2i &p)
{
    _dim_1d = 0;
    _dim_2d = 0;
    _sample_idx = 0;

    for(int i=0;i<_samples_1d.size();++i)
    {
        van_der_corput(_spp,1,&_samples_1d[i][0],_rng);
    }

    for(int i=0;i<_samples_2d.size();++i)
    {
        sobol02(_spp,1,&_samples_2d[i][0],_rng);
    }
}

bool Sampler::start_next_sample()
{
    _dim_1d = 0;
    _dim_2d = 0;
    _sample_idx += 1;
    if(_sample_idx == _spp)
    {
        _sample_idx=0;
        return false;
    }
    return true;
}


Point2f Sampler::get_2D()
{
    
    if (EXPECT_TAKEN(_dim_2d < _max_dim))
    {
        return _samples_2d[_dim_2d++][_sample_idx];
    }
    else
    {
        auto sample = Point2f(_rng.next_float(), _rng.next_float());
        _dim_2d++;
        return sample;
    }
}
float Sampler::get_1D()
{
    if (EXPECT_TAKEN(_dim_1d < _max_dim))
    {
        return _samples_1d[_dim_1d++][_sample_idx];
    }
    else
    {
        _dim_1d++;
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
    sampler->_rng.set_seed(seed);
    return sampler;
}
NARUKAMI_END