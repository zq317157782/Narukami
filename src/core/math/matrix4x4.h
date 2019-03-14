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
#include "core/math/math.h"
#include "core/sse/float4.h"

NARUKAMI_BEGIN
//column  major
struct Matrix4x4{
public:
    float4 c0,c1,c2,c3;
    typedef float Scalar;
    enum
    {
        N = 16
    };
public:
    Matrix4x4():c0(float4(1,0,0,0)),c1(float4(0,1,0,0)),c2(float4(0,0,1,0)),c3(float4(0,0,0,1)){}
    Matrix4x4(const float4& c0,const float4&c1,const float4& c2,const float4&c3):c0(c0),c1(c1),c2(c2),c3(c3){}
    FINLINE const float& operator[](const int idx) const { assert(idx >= 0 && idx < N); return (&c0.x)[idx];}
    FINLINE float& operator[](const int idx){ assert(idx >= 0 && idx < N); return (&c0.x)[idx];}
};

NARUKAMI_END