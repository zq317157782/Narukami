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
#include "sse.h"
#include "platform.h"
#include "narukami.h"
#include <cmath>
NARUKAMI_BEGIN

//IEEE float format
//32 bits
//[31][30-23][22-0]
//[31] sign
//[30-23] exp
//[22-0] mag

FINLINE float cast_i2f(const int x){
    union {float f;int i;} v;
    v.i=x;
    return v.f;
}

FINLINE float cast_f2i(const float x){
    union {float f;int i;} v;
    v.f=x;
    return v.i;
}

FINLINE float rcp(const float x){
    const __m128 a = _mm_set_ss(x);
    const __m128 r = _mm_rcp_ss(a);
    return _mm_cvtss_f32(_mm_mul_ss(r,_mm_sub_ss(_mm_set_ss(2.0f), _mm_mul_ss(r, a))));
}

FINLINE float sign(const float x){
    return x>=0.0f?1.0f:-1.0f;
}

FINLINE int sign(const int x){
    return x>=0?1:-1;
}

FINLINE bool isnan(const float x){
    return std::isnan(x);
}

FINLINE bool isinf(const float x){
    return std::isinf(x);
}

FINLINE float sqr(const float x){
    return x*x;
}

FINLINE float sqrt(const float x){
    return std::sqrtf(x);
}

FINLINE float rsqrt( const float x )
{
    const __m128 a = _mm_set_ss(x);
    const __m128 r = _mm_rsqrt_ss(a);
    const __m128 c = _mm_add_ss(_mm_mul_ss(_mm_set_ss(1.5f), r),_mm_mul_ss(_mm_mul_ss(_mm_mul_ss(a, _mm_set_ss(-0.5f)), r), _mm_mul_ss(r, r)));
    return _mm_cvtss_f32(c);
}

FINLINE float min(const float x,const float y){
    return x<y?x:y;
}

FINLINE int min(const int x,const int y){
    return x<y?x:y;
}

template <typename T>
FINLINE T min(const T x,const T y,const T z){
    return min(min(x,y),z);
}

template <typename T>
FINLINE T min(const T x,const T y,const T z,const T w){
    return min(min(x,y,z),w);
}

FINLINE float max(const float x,const float y){
    return x>y?x:y;
}

FINLINE int max(const int x,const int y){
    return x>y?x:y;
}

template <typename T>
FINLINE T max(const T x,const T y,const T z){
    return max(max(x,y),z);
}

template <typename T>
FINLINE T max(const T x,const T y,const T z,const T w){
    return max(max(x,y,z),w);
}






NARUKAMI_END