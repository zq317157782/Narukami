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
#include "core/constant.h"
#include "core/math.h"
NARUKAMI_BEGIN
    struct Spectrum
    {
        union { struct { float x,y,z; }; struct { float r,g,b; }; };
        typedef float Scalar;
        enum { N = 3 };
        FINLINE Spectrum():r(Zero),g(Zero),b(Zero){}
        FINLINE Spectrum(const float r,const float g,const float b):r(r),g(g),b(b){ }
        FINLINE float operator[](const int idx) const{ assert(idx>=0&&idx<N); return (&x)[idx]; }
        FINLINE float& operator[](const int idx){ assert(idx>=0&&idx<N); return (&x)[idx]; }
    };

    FINLINE bool operator==(const Spectrum& a,const Spectrum& b){ if((a.r==b.r)&&(a.g==b.g)&&(a.b==b.b)){ return true; } return false; }
    FINLINE bool operator!=(const Spectrum& a,const Spectrum& b){ if((a.r!=b.r)||(a.g!=b.g)||(a.b!=b.b)){ return true; } return false; }
    
NARUKAMI_END