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
#include <pmmintrin.h>//sse3
#include <smmintrin.h>//sse4.1

NARUKAMI_BEGIN
#define SSE_LINE_SIZE 128
#define SSE_FLOAT_COUNT 4
#define SSE_ALIGNAS alignas(16)

extern const __m128 sse_lookup_masks[16];

#define SSE_MASK_FALSE sse_lookup_masks[0]
#define SSE_MASK_TRUE  sse_lookup_masks[15]
#define SSE_BOOL_TO_LUT_IDX(a0,a1,a2,a3) size_t(a0)|(size_t(a1)<<1)|(size_t(a2)<<2)|(size_t(a3)<<3)
#define SSE_MASK(a0,a1,a2,a3) sse_lookup_masks[SSE_BOOL_TO_LUT_IDX(a0,a1,a2,a3)]

NARUKAMI_END