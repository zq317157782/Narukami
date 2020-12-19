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

struct Spectrum;
struct RGB;

struct XYZ
{
    float x, y, z;
    inline XYZ() : x(0.0f), y(0.0f), z(0.0f) {}
    inline XYZ(const float x, const float y, const float z) : x(x), y(y), z(z)
    {
        assert(!isnan(x));
        assert(!isnan(y));
        assert(!isnan(z));
    }
    inline float operator[](const int idx) const
    {
        assert(idx >= 0 && idx < 3);
        return (&x)[idx];
    }
    inline float &operator[](const int idx)
    {
        assert(idx >= 0 && idx < 3);
        return (&x)[idx];
    }
};

struct RGB
{
    float r, g, b;
    inline RGB() : r(0.0f), g(0.0f), b(0.0f) {}
    inline RGB(const float r, const float g, const float b) : r(r), g(g), b(b)
    {
        assert(!isnan(r));
        assert(!isnan(g));
        assert(!isnan(b));
    }
    //explicit RGB(const XYZ&);
    explicit RGB(const Spectrum &);
    inline float operator[](const int idx) const
    {
        assert(idx >= 0 && idx < 3);
        return (&r)[idx];
    }
    inline float &operator[](const int idx)
    {
        assert(idx >= 0 && idx < 3);
        return (&r)[idx];
    }
};
inline std::ostream &operator<<(std::ostream &out, const RGB &L)
{
    out << '(' << L.r << ',' << L.g << ',' << L.b << ')';
    return out;
}
inline bool operator==(const RGB &a, const RGB &b)
{
    if ((a.r == b.r) && (a.g == b.g) && (a.b == b.b))
    {
        return true;
    }
    return false;
}
inline bool operator!=(const RGB &a, const RGB &b)
{
    if ((a.r != b.r) || (a.g != b.g) || (a.b != b.b))
    {
        return true;
    }
    return false;
}

inline RGB operator+(const RGB &L1, const RGB &L2)
{
    RGB L;
    L.r = L1.r + L2.r;
    L.g = L1.g + L2.g;
    L.b = L1.b + L2.b;
    return L;
}
inline RGB operator*(const RGB &L1, const RGB &L2)
{
    RGB L;
    L.r = L1.r * L2.r;
    L.g = L1.g * L2.g;
    L.b = L1.b * L2.b;
    return L;
}
inline RGB operator*(const RGB &L1, const float f)
{
    assert(!isnan(f));
    assert(f != 0.0f);
    RGB L;
    L.r = L1.r * f;
    L.g = L1.g * f;
    L.b = L1.b * f;
    return L;
}
inline RGB operator*(const float f, const RGB &L1) { return L1 * f; }
inline RGB operator/(const RGB &L1, const float f)
{
    assert(!isnan(f));
    assert(f != 0.0f);
    RGB L;
    L.r = L1.r / f;
    L.g = L1.g / f;
    L.b = L1.b / f;
    return L;
}

inline bool is_black(const RGB &L) { return L.r == 0 && L.g == 0 && L.b == 0; }

struct RGBA
{
    float r, g, b, a;
    inline RGBA() : r(0.0f), g(0.0f), b(0.0f), a(0.0f) {}
    inline RGBA(const float r, const float g, const float b, const float a) : r(r), g(g), b(b), a(a)
    {
        assert(!isnan(r));
        assert(!isnan(g));
        assert(!isnan(b));
        assert(!isnan(a));
    }
    inline float operator[](const int idx) const
    {
        assert(idx >= 0 && idx < 4);
        return (&r)[idx];
    }
    inline float &operator[](const int idx)
    {
        assert(idx >= 0 && idx < 4);
        return (&r)[idx];
    }
};

//--------------------------------------------------------------------------------------------------------------------------------
constexpr int WAVELENGTH_MIN = 360;
constexpr int WAVELENGTH_MAX = 830;
constexpr int WAVELENGTH_COUNT = WAVELENGTH_MAX - WAVELENGTH_MIN + 1;
//从pbrt抄来的CIE XYZ曲线系数，正好和我们的频谱宽度一致
extern const float CIE_X[WAVELENGTH_COUNT];
extern const float CIE_Y[WAVELENGTH_COUNT];
extern const float CIE_Z[WAVELENGTH_COUNT];
extern const float CIE_lambda[WAVELENGTH_COUNT];
static const float CIE_Y_integral = 106.856895f; //Y曲线的积分，用来标准化xyz

constexpr int WAVELENGTH_SAMPLE_RATE = 5;
constexpr int SPD_SAMPLE_COUNT = (WAVELENGTH_MAX - WAVELENGTH_MIN) / WAVELENGTH_SAMPLE_RATE + 1;
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

Spectrum from_sample_data(const float *lambda, float *sample, int n);

void blackbody(const float *lambda, int n, float T, float *Le);
void blackbody_normalized(const float *lambda, int n, float T, float *Le);

Spectrum tungsten_lamp_2700k(float intensity);
Spectrum tungsten_lamp_3000k(float intensity);

NARUKAMI_END