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
#include "core/platform.h"
#include <xmmintrin.h>//sse
#include <emmintrin.h>//sse2

NARUKAMI_BEGIN
#define SSE_ALIGNAS alignas(16)

//SSE4.1's _mm_blendv_ps 
FINLINE __m128 _sse_blendv_ps(__m128 a, __m128 b, __m128 mask){
    return _mm_or_ps(_mm_and_ps(mask,b),_mm_andnot_ps(mask,a));
}

template<int i0,int i1,int i2,int i3>
FINLINE __m128 swizzle(const __m128 v){
    return _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), _MM_SHUFFLE(i3, i2, i1, i0)));
}

template<int i>
FINLINE __m128 swizzle(const __m128 v){
    return swizzle<i,i,i,i>(v);
}

template<int i0,int i1,int i2,int i3>
FINLINE __m128 shuffle(const __m128 v1,const __m128 v2){
    return _mm_shuffle_ps(v1,v2,_MM_SHUFFLE(i3,i2,i1,i0));
}

template<int i>
FINLINE __m128 shuffle(const __m128 v1,const __m128 v2){
    return shuffle<i,i,i,i>(v1,v2);
}

FINLINE float sum(const __m128 a){
    const __m128 b = swizzle<1>(a);
    const __m128 c = swizzle<2>(a);
    const __m128 d = swizzle<3>(a);
    const __m128 e = _mm_add_ps(a,b);
    const __m128 f = _mm_add_ps(e,c);
    const __m128 g = _mm_add_ps(f,d);
    return _mm_cvtss_f32(g);
}


NARUKAMI_END