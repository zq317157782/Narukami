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
#include "core/math.h"
#include "simd.h"
NARUKAMI_BEGIN

constexpr int WAVELENGTH_MIN = 360;
constexpr int WAVELENGTH_MAX = 830;
constexpr int SPD_SAMPLE_COUNT = 95;
constexpr int SPD_SSE_SAMPLE_COUNT = SPD_SAMPLE_COUNT / SSE_WIDTH + 1;
struct Spectrum
{

    float4 samples[SPD_SSE_SAMPLE_COUNT];

    Spectrum()
    {
        for (int i = 0; i < SPD_SSE_SAMPLE_COUNT; ++i)
        {
            samples[i] = float4();
        }
    }

    Spectrum(float v)
    {
        for (int i = 0; i < SPD_SSE_SAMPLE_COUNT; ++i)
        {
            samples[i] = float4(v);
        }
        //最后一个空间是无效样本,因此设置成0.0f
        samples[SPD_SSE_SAMPLE_COUNT - 1][3] = 0.0f;
    }

    Spectrum(float *input)
    {
        memcpy(samples, input, SPD_SAMPLE_COUNT * sizeof(float));
        //最后一个空间是无效样本,因此设置成0.0f
        samples[SPD_SSE_SAMPLE_COUNT - 1][3] = 0.0f;
    }

    float operator[](int idx) const
    {
        assert(idx >= 0 && idx < SPD_SAMPLE_COUNT);
        return samples[idx >> 2][idx & 0x3];
    }
    float &operator[](int idx)
    {
        assert(idx >= 0 && idx < SPD_SAMPLE_COUNT);
        return samples[idx >> 2][idx & 0x3];
    }

    //static number value
    static Spectrum X;
    static Spectrum Y;
    static Spectrum Z;

    static void init();
    static void free();
};

inline bool isnan(const Spectrum &spd)
{
    for (int i = 0; i < SPD_SAMPLE_COUNT; ++i)
    {
        if (isnan(spd[i]))
        {
            return true;
        }
    }
    return false;
}

inline std::ostream &operator<<(std::ostream &out, const Spectrum &spd)
{
    out << '( ';
    for (int i = 0; i < SPD_SAMPLE_COUNT; ++i)
    {
        out << spd[i] << " ";
    }
    out << ')';
    return out;
}

inline bool is_black(const Spectrum &spd)
{
    for (int i = 0; i < SPD_SSE_SAMPLE_COUNT; ++i)
    {
        if (EXPECT_TAKEN(any(spd.samples[i] != float4::zero)))
        {
            return false;
        }
    }
    return true;
}

inline Spectrum operator+(const Spectrum &lhs, const Spectrum &rhs)
{
    Spectrum spd;
    for (int i = 0; i < SPD_SSE_SAMPLE_COUNT; ++i)
    {
        spd.samples[i] = lhs.samples[i] + rhs.samples[i];
    }
    return spd;
}

inline Spectrum operator+=(Spectrum &lhs, const Spectrum &rhs)
{
    lhs = lhs + rhs;
    return lhs;
}

inline Spectrum operator-(const Spectrum &lhs, const Spectrum &rhs)
{
    Spectrum spd;
    for (int i = 0; i < SPD_SSE_SAMPLE_COUNT; ++i)
    {
        spd.samples[i] = lhs.samples[i] - rhs.samples[i];
    }
    return spd;
}

inline Spectrum operator-=(Spectrum &lhs, const Spectrum &rhs)
{
    lhs = lhs - rhs;
    return lhs;
}

inline Spectrum operator*(const Spectrum &lhs, const Spectrum &rhs)
{
    Spectrum spd;
    for (int i = 0; i < SPD_SSE_SAMPLE_COUNT; ++i)
    {
        spd.samples[i] = lhs.samples[i] * rhs.samples[i];
    }
    return spd;
}

inline Spectrum operator*=(Spectrum &lhs, const Spectrum &rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

inline Spectrum operator*(const Spectrum &lhs, float rhs)
{
    Spectrum spd;
    for (int i = 0; i < SPD_SSE_SAMPLE_COUNT; ++i)
    {
        spd.samples[i] = lhs.samples[i] * rhs;
    }
    return spd;
}

inline Spectrum operator/(const Spectrum &lhs, const Spectrum &rhs)
{
    Spectrum spd;
    for (int i = 0; i < SPD_SSE_SAMPLE_COUNT; ++i)
    {
        spd.samples[i] = lhs.samples[i] / rhs.samples[i];
    }
    return spd;
}

inline Spectrum operator/(const Spectrum &lhs, float rhs)
{
    assert(rhs != 0);
    Spectrum spd;
    for (int i = 0; i < SPD_SSE_SAMPLE_COUNT; ++i)
    {
        spd.samples[i] = lhs.samples[i] / rhs;
    }
    return spd;
}

inline float average(const Spectrum &spd)
{
    float4 sum4;
    for (int i = 0; i < SPD_SSE_SAMPLE_COUNT; ++i)
    {
        sum4 += spd.samples[i];
    }
    return vreduce_add(sum4).x / SPD_SAMPLE_COUNT;
}

void from_spd_to_xyz(const Spectrum &spd, float xyz[3]);
void from_xyz_to_srgb(const float xyz[3], float rgb[3]);

void from_srgb_to_spd(float rgb[3],Spectrum &spd);

Spectrum from_sample_data(const float *lambda, float *sample, int n);

void blackbody(const float *lambda, int n, float T, float *Le);
void blackbody_normalized(const float *lambda, int n, float T, float *Le);

Spectrum tungsten_lamp_2700k(float intensity);
Spectrum tungsten_lamp_3000k(float intensity);

NARUKAMI_END