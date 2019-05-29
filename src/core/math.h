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
#include "core/sse.h"
#include "core/platform.h"
#include "core/narukami.h"
#include <cmath>
NARUKAMI_BEGIN

//IEEE float format
//32 bits
//[31][30-23][22-0]
//[31] sign 
//[30-23] exp
//[22-0] mag

FINLINE  float cast_i2f(const int x){
    union {float f;int i;} v;
    v.i=x; 
    return v.f;
}

FINLINE  float cast_ui2f(const int x){
    union {float f;unsigned int i;} v;
    v.i=x;
    return v.f;
}

FINLINE  int cast_f2i(const float x){
    union {float f;int i;} v;
    v.f=x;
    return v.i;
}

FINLINE  bool isnan(const float x){
    return std::isnan(x);
}

FINLINE  bool isnan(const int x){
    return false;
}


FINLINE  bool isinf(const float x){
    return std::isinf(x);
}



FINLINE  float sign(const float x){
    return x>=0.0f?1.0f:-1.0f;
}

FINLINE  int sign(const int x){
    return x>=0?1:-1;
}



FINLINE  float sqr(const float x){
    return x*x;
}

FINLINE  float sqrt(const float x){
    return std::sqrtf(x);
}

FINLINE  float rcp(const float x){
    const __m128 a = _mm_set_ss(x);
    const __m128 r = _mm_rcp_ss(a);
    return _mm_cvtss_f32(_mm_mul_ss(r,_mm_sub_ss(_mm_set_ss(2.0f), _mm_mul_ss(r, a))));
}

FINLINE  float rsqrt( const float x )
{
    const __m128 a = _mm_set_ss(x);
    const __m128 r = _mm_rsqrt_ss(a);
    const __m128 c = _mm_add_ss(_mm_mul_ss(_mm_set_ss(1.5f), r),_mm_mul_ss(_mm_mul_ss(_mm_mul_ss(a, _mm_set_ss(-0.5f)), r), _mm_mul_ss(r, r)));
    return _mm_cvtss_f32(c);
}
// from QUAKE-III 
FINLINE  float fast_rsqrt(float x)
{
    float xhalf = 0.5f * x;
    int i = *(int *)&x;
    i = 0x5f3759df - (i >> 1);
    x = *(float *)&i;
    x = x * (1.5f - xhalf * x * x);
    return x;
}

FINLINE  float min(const float x,const float y){
    return x<y?x:y;
}

FINLINE  int min(const int x,const int y){
    return x<y?x:y;
}

template <typename T>
FINLINE  T min(const T& x,const T& y,const T& z){
    return min(min(x,y),z);
}

template <typename T>
FINLINE  T min(const T& x,const T& y,const T& z,const T& w){
    return min(min(x,y,z),w);
}

FINLINE  float max(const float x,const float y){
    return x>y?x:y;
}

FINLINE  int max(const int x,const int y){
    return x>y?x:y;
}

template <typename T>
FINLINE  T max(const T& x,const T& y,const T& z){
    return max(max(x,y),z);
}

template <typename T>
FINLINE  T max(const T& x,const T& y,const T& z,const T& w){
    return max(max(x,y,z),w);
}

FINLINE  float abs(const float x){
    return std::abs(x);
}

FINLINE  float sin(const float x){
    return std::sinf(x);
}

FINLINE  float cos(const float x){
    return std::cosf(x);
}

FINLINE  float tan(const float x){
    return std::tanf(x);
}

FINLINE  float asin(const float x){
    return std::asinf(x);
}

FINLINE  float acos(const float x){
    return std::acosf(x);
}

FINLINE  float atan(const float x){
    return std::atanf(x);
}

FINLINE  float atan2(const float x,const float y){
    return std::atan2f(x,y);
}

FINLINE  float madd(const float x,const float y,const float z){
    return x*y+z;
}

FINLINE  float msub(const float x,const float y,const float z){
    return x*y-z;
}

FINLINE  float nmadd(const float x,const float y,const float z){
    return -x*y+z;
}


FINLINE  float nmsub(const float x,const float y,const float z){
    return -x*y-z;
}

FINLINE  float lerp(const float x0,const float x1,const float t){
    return madd(1.0f-t,x0,t*x1);
}

FINLINE  float  deg2rad ( const float x )  { return x * 1.74532925199432957692e-2f; }
FINLINE  float  rad2deg ( const float x )  { return x * 5.72957795130823208768e1f; }

FINLINE float ceil(const float x){ return std::ceilf(x); }
FINLINE float floor(const float x){ return std::floorf(x); }


template<uint32_t base> uint32_t reverse_u32(uint32_t num);
//from pbrt
template<>
FINLINE uint32_t reverse_u32<2>(uint32_t x){
    x=(x<<16)|(x>>16);
    x=((x&0x00ff00ff)<<8)|((x&0xff00ff00)>>8);
    x=((x&0x0f0f0f0f)<<4)|((x&0xf0f0f0f0)>>4);
    x=((x&0x33333333)<<2)|((x&0xcccccccc)>>2);
    x=((x&0x55555555)<<1)|((x&0xaaaaaaaa)>>1);
    return x;
}


NARUKAMI_END