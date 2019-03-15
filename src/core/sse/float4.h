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

struct float4
{
  public:
    typedef float Scalar;
    enum
    {
        size = 4
    };

    union {
        __m128 xyzw;
        struct
        {
            float x, y, z, w;
        };
    };

  public:
    FINLINE float4() : x(0), y(0), z(0), w(0) {}
    FINLINE float4(const __m128 a) : xyzw(a) {}
    FINLINE explicit float4(const float a) : x(a), y(a), z(a), w(a) {}
    FINLINE float4(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w) {}
    FINLINE operator __m128&(){return xyzw;}
    FINLINE operator const __m128&() const{return xyzw;}
    FINLINE const float &operator[](const int idx) const{assert(idx >= 0 && idx < size);return (&x)[idx];}
    FINLINE float &operator[](const int idx){assert(idx >= 0 && idx < size);return (&x)[idx];}
};


FINLINE std::ostream &operator<<(std::ostream &out, const float4 &v){out << '(' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << ')';return out;}

FINLINE float4 operator+(const float4 &v){return v;}
//0x80000000 xor x(y,z)
FINLINE float4 operator-(const float4 &v){auto mask = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));return _mm_xor_ps(v.xyzw, mask);}

FINLINE float4 operator+(const float4& v1,const float4& v2){return _mm_add_ps(v1,v2);}
FINLINE float4 operator-(const float4& v1,const float4& v2){return _mm_sub_ps(v1,v2);}
FINLINE float4 operator*(const float4& v1,const float4& v2){return _mm_mul_ps(v1,v2);}
FINLINE float4 operator*(const float4& v1,const float f){return _mm_mul_ps(v1,float4(f));}
FINLINE float4 operator/(const float4& v1,const float4& v2){return _mm_div_ps(v1,v2);}
FINLINE float4 operator/(const float4& v1,const float f){assert(f!=0);return _mm_div_ps(v1,float4(f));}

FINLINE float4& operator+=(float4& v1,const float4& v2){v1=v1+v2;return v1;}
FINLINE float4& operator-=(float4& v1,const float4& v2){v1=v1-v2;return v1;}
FINLINE float4& operator*=(float4& v1,const float4& v2){v1=v1*v2;return v1;}
FINLINE float4& operator*=(float4& v1,const float f){v1=v1*f;return v1;}
FINLINE float4& operator/=(float4& v1,const float4& v2){v1=v1/v2;return v1;}
FINLINE float4& operator/=(float4& v1,const float f){assert(f!=0);v1=v1/f;return v1;}

FINLINE bool operator==(const float4 &a, const float4 &b){return (_mm_movemask_ps(_mm_cmpeq_ps(a.xyzw, b.xyzw)) & 15) == 15;}
FINLINE bool operator!=(const float4 &a, const float4 &b){return (_mm_movemask_ps(_mm_cmpneq_ps(a.xyzw, b.xyzw)) & 15) != 0;}

FINLINE  float4 rcp(const float4& x){ const __m128 r = _mm_rcp_ps(x); return _mm_mul_ps(r,_mm_sub_ps(_mm_set1_ps(2.0f), _mm_mul_ps(r, x))); }

FINLINE float4 madd(const float4 &a,const float4 &b,const float4 &c){ return a*b+c; }
FINLINE float4 msub(const float4 &a,const float4 &b,const float4 &c){ return a*b-c; }

template<int i0, int i1, int i2, int i3>
FINLINE float4 shuffle(const float4& v) { return _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v), _MM_SHUFFLE(i3, i2, i1, i0))); }
template<int i>
FINLINE float4 shuffle(const float4& v){return shuffle<i,i,i,i>(v);}
NARUKAMI_END