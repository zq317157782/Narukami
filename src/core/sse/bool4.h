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
#include "sse.h"
NARUKAMI_BEGIN

#define BOOL_TO_LUT_IDX(a0,a1,a2,a3) size_t(a0)|(size_t(a1)<<1)|(size_t(a2)<<2)|(size_t(a3)<<3)

struct bool4
{
    typedef bool Scalar; 
    enum { size = 4 };
    union { __m128 xyzw; int i[4]; struct { int x,y,z,w; }; };
    

    FINLINE bool4(const __m128 v):xyzw(v){}
    FINLINE explicit bool4(const bool a):xyzw(sse_lookup_masks[BOOL_TO_LUT_IDX(a,a,a,a)]){}
    FINLINE explicit bool4(const bool a0,const bool a1,const bool a2,const bool a3):xyzw(sse_lookup_masks[BOOL_TO_LUT_IDX(a0,a1,a2,a3)]){}

    FINLINE operator __m128&(){return xyzw;}
    FINLINE operator const __m128&() const{return xyzw;}
    FINLINE const bool operator[] (const int index){
        assert(index<size);
        return ((_mm_movemask_ps(xyzw)>>index)&0x1)==1;
    }
};

FINLINE std::ostream &operator<<(std::ostream &out, const bool4 &b){out << '(' << ((b.x!=0)?"true":"false")<< ',' << ((b.y!=0)?"true":"false") << ',' << ((b.z!=0)?"true":"false") << ',' << ((b.w!=0)?"true":"false") << ')';return out;}

FINLINE bool4 operator!(const bool4 &a){ return _mm_xor_ps(a,bool4(True)); }
FINLINE bool4 operator&(const bool4 &a,const bool4 &b){ return _mm_and_ps(a.xyzw,b.xyzw);}
FINLINE bool4 operator|(const bool4 &a,const bool4 &b){ return _mm_or_ps (a.xyzw,b.xyzw);}

FINLINE bool all(const bool4& b){ return (_mm_movemask_ps(b.xyzw)&0xF)==0xF;}
FINLINE bool any(const bool4& b){ return (_mm_movemask_ps(b.xyzw)&0xF)!=0x0;}
FINLINE bool none(const bool4& b){ return (_mm_movemask_ps(b.xyzw)&0xF)==0x0;}
//#TODO 这个的命名可能不太对
FINLINE bool not_all(const bool4& b){ return (_mm_movemask_ps(b.xyzw)&0xF)!=0xF;}


NARUKAMI_END
