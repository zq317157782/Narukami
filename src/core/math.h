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
#pragma warning(disable : 4723) //忽略除以0的警告
#include "core/sse.h"
#include "core/platform.h"
#include "core/narukami.h"
#include <cmath>
NARUKAMI_BEGIN

//constants 
static MAYBE_UNUSED constexpr float MIN_RCP_INPUT = 1E-18f;
static MAYBE_UNUSED constexpr float EPSION = std::numeric_limits<float>::epsilon(); //大于1的最小可表示浮点数
static MAYBE_UNUSED constexpr float INFINITE = std::numeric_limits<float>::infinity();
static MAYBE_UNUSED constexpr float OVERFLOW_LIMIT = std::numeric_limits<float>::max();
static MAYBE_UNUSED constexpr float UNDERFLOW_LIMIT = std::numeric_limits<float>::lowest();
static MAYBE_UNUSED constexpr float PI = 3.14159265358979323846f;
static MAYBE_UNUSED constexpr float INV_PI = 0.31830988618379067154f;
static MAYBE_UNUSED constexpr float INV_TWO_PI = 0.15915494309189533577f;
static MAYBE_UNUSED constexpr float INV_FOUR_PI = 0.07957747154594766788f;
static MAYBE_UNUSED constexpr float PI_OVER_TWO = 1.57079632679489661923f;
static MAYBE_UNUSED constexpr float PI_OVER_FOUR = 0.78539816339744830961f;
static MAYBE_UNUSED constexpr float ONE_MINUS_EPSILON = 1.0f - EPSION;

//IEEE 754 float
//[31][30 : 23][22 : 0]
//[sign][exp][ma]

/**IEEE 754 标准下的单精度浮点数类型**/
union IEEE_754_32bit
{
    IEEE_754_32bit():f(0.0f){}
    IEEE_754_32bit(const float x) : f(x){};
    IEEE_754_32bit(const int x) : i(x){};
    IEEE_754_32bit(const unsigned int x) : ui(x){};
    float f;
    int i;
    unsigned int ui;
    struct
    {
        uint32_t mantissa : 23;
        uint32_t exponent : 8;
        uint32_t sign : 1;
    };
};
inline float cast_i2f(const int x)
{
    IEEE_754_32bit v = x;
    return v.f;
}
inline float cast_ui2f(const int x)
{
    IEEE_754_32bit v = x;
    return v.f;
}
inline int cast_f2i(const float x)
{
    IEEE_754_32bit v = x;
    return v.i;
}
inline bool isnan(float x)
{
    return std::isnan(x);
}

inline bool isnan(int x)
{
    return false;
}

inline bool isinf(float x)
{
    return std::isinf(x);
}

inline bool isinf(int x)
{
    return false;
}
inline float ceil(const float x) { return std::ceilf(x); }
inline float floor(const float x) { return std::floorf(x); }
inline float abs(const float x)
{
    return std::abs(x);
}
inline float sign(const float x)
{
    return x >= 0.0f ? 1.0f : -1.0f;
}

inline int sign(const int x)
{
    return x >= 0 ? 1 : -1;
}

inline float fma(const float x, const float y, const float z)
{
    return std::fmaf(x, y, z);
}
inline int fma(const int x, const int y, const int z)
{
    return x * y + z;
}

template <typename T>
inline T min(const T &x, const T &y)
{
    return x < y ? x : y;
}
template <typename T>
inline T min(const T &x, const T &y, const T &z)
{
    return min(min(x, y), z);
}
template <typename T>
inline T min(const T &x, const T &y, const T &z, const T &w)
{
    return min(min(x, y, z), w);
}
template <typename T>
inline T max(const T &x, const T &y)
{
    return x > y ? x : y;
}
template <typename T>
inline T max(const T &x, const T &y, const T &z)
{
    return max(max(x, y), z);
}
template <typename T>
inline T max(const T &x, const T &y, const T &z, const T &w)
{
    return max(max(x, y, z), w);
}
inline float sqr(const float x)
{
    return x * x;
}

inline float sqrt(const float x)
{
    return std::sqrtf(x);
}

inline float rcp(const float x)
{
    assert(x != 0);
#ifdef MATH_SSE_RCP
    const __m128 a = _mm_set_ss(x);
    const __m128 r = _mm_rcp_ss(a);
    return _mm_cvtss_f32(_mm_mul_ss(r, _mm_sub_ss(_mm_set_ss(2.0f), _mm_mul_ss(r, a))));
#else
    return 1.0f / x;
#endif
}

inline float safe_rcp(const float x)
{
#ifdef MATH_SSE_RCP
    auto xxxx = _mm_set1_ps(x);
    auto min_rcp_input = _mm_set1_ps(MIN_RCP_INPUT);
    /**abs**/
    auto mask = _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF));
    auto abs_xxxx = _mm_and_ps(xxxx, mask);
    /**zero fix**/
    float v = _mm_blendv_ps(xxxx, min_rcp_input, _mm_cmplt_ps(abs_xxxx, min_rcp_input)).m128_f32[0];
    return rcp(v);
#else
    return rcp(x);
#endif
}
//速度快但是近似
inline float rsqrt(const float x)
{
    assert(x != 0);
    const __m128 a = _mm_set_ss(x);
    const __m128 r = _mm_rsqrt_ss(a);
    const __m128 c = _mm_add_ss(_mm_mul_ss(_mm_set_ss(1.5f), r), _mm_mul_ss(_mm_mul_ss(_mm_mul_ss(a, _mm_set_ss(-0.5f)), r), _mm_mul_ss(r, r)));
    return _mm_cvtss_f32(c);
}
// from QUAKE-III
// 误差比较大
// rsqrt的性能和误差都要比rsqrt_quake好
// 只作为参考
inline float rsqrt_quake(float x)
{
    float xhalf = 0.5f * x;
    int i = *(int *)&x;
    i = 0x5f3759df - (i >> 1);
    x = *(float *)&i;
    x = x * (1.5f - xhalf * x * x);
    return x;
}
inline float exp(const float x)
{
    return std::exp(x);
}
inline float exp2(const float x)
{
    return std::exp2f(x);
}
/**优化版本的exp2,幂需要是整数**/
inline float exp2(const int x)
{
    IEEE_754_32bit v = 1.0f;
    v.exponent += x;
    return v.f;
}
inline float pow(const float x, const float exp)
{
    return std::powf(x, exp);
}
inline float sin(const float x)
{
    return std::sinf(x);
}
inline float cos(const float x)
{
    return std::cosf(x);
}
inline float tan(const float x)
{
    return std::tanf(x);
}
inline float asin(const float x)
{
    return std::asinf(x);
}
inline float acos(const float x)
{
    return std::acosf(x);
}
inline float atan(const float x)
{
    return std::atanf(x);
}
inline float atan2(const float x, const float y)
{
    return std::atan2f(x, y);
}
inline float clamp(const float x, const float x0, const float x1)
{
    assert(x0 <= x1);
    return min(max(x, x0), x1);
}
inline float saturate(const float x)
{
    return min(max(x, 0.0f), 1.0f);
}
inline float lerp(const float x0, const float x1, const float t)
{
    return fma(1.0f - t, x0, t * x1);
}
inline float between(const float x0, const float x1, const float x)
{
    assert((x1 - x0) != 0);
    return (x - x0) / (x1 - x0);
}
//质心坐标插值
template <typename T>
T bycerp(const T &x0, const T &x1, const T &x2, float t, float s)
{
    return x0 * (1.0f - t - s) + x1 * t + x2 * s;
}
inline float deg2rad(const float x) { return x * 1.74532925199432957692e-2f; }
inline float rad2deg(const float x) { return x * 5.72957795130823208768e1f; }
//from pbrt
inline uint32_t reverse_bits_u32(uint32_t x)
{
    x = (x << 16) | (x >> 16);
    x = ((x & 0x00ff00ff) << 8) | ((x & 0xff00ff00) >> 8);
    x = ((x & 0x0f0f0f0f) << 4) | ((x & 0xf0f0f0f0) >> 4);
    x = ((x & 0x33333333) << 2) | ((x & 0xcccccccc) >> 2);
    x = ((x & 0x55555555) << 1) | ((x & 0xaaaaaaaa) >> 1);
    return x;
}
//TODO 搞清楚原理
inline uint32_t round_up_pow2(uint32_t v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    return v + 1;
}
inline int count_trailing_zero(uint32_t v)
{
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_ctz(v);
#else
    unsigned long index;
    if (_BitScanForward(&index, v))
    {
        return index;
    }
    else
    {
        return 32;
    }
#endif
}

NARUKAMI_END