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

FINLINE float rcp(const float x){
    const __m128 a = _mm_set_ss(x);
    const __m128 r = _mm_rcp_ss(a);
    return _mm_cvtss_f32(_mm_mul_ss(r,_mm_sub_ss(_mm_set_ss(2.0f), _mm_mul_ss(r, a))));
}

FINLINE float sign(const float x){
    return x>=0.0f?1.0f:-1.0f;
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


NARUKAMI_END