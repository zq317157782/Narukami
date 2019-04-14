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
#include "core/math.h"
#include "core/constant.h"
#include "core/sse.h"

NARUKAMI_BEGIN

struct bool4
{
    typedef bool Scalar; 
    enum { size = 4 };
    union { __m128 xyzw; int i[4]; struct { int x,y,z,w; }; };
    

    FINLINE bool4(const __m128 v):xyzw(v){}
    FINLINE explicit bool4(const bool a):xyzw(SSE_MASK(a,a,a,a)){}
    FINLINE explicit bool4(const bool a0,const bool a1,const bool a2,const bool a3):xyzw(SSE_MASK(a0,a1,a2,a3)){}

    FINLINE operator __m128&(){return xyzw;}
    FINLINE operator const __m128&() const{return xyzw;}
    FINLINE const bool operator[] (const int index){
        assert(index<size);
        return ((_mm_movemask_ps(xyzw)>>index)&0x1)==1;
    }
};

FINLINE std::ostream &operator<<(std::ostream &out, const bool4 &b){out << '(' << ((b.x!=0)?"true":"false")<< ',' << ((b.y!=0)?"true":"false") << ',' << ((b.z!=0)?"true":"false") << ',' << ((b.w!=0)?"true":"false") << ')';return out;}

FINLINE bool4 operator!(const bool4 &a){ return _mm_xor_ps(a,bool4(True));}
FINLINE bool4 operator&(const bool4 &a,const bool4 &b){ return _mm_and_ps(a.xyzw,b.xyzw);}
FINLINE bool4 operator|(const bool4 &a,const bool4 &b){ return _mm_or_ps (a.xyzw,b.xyzw);}

FINLINE bool all(const bool4& b){ return (_mm_movemask_ps(b.xyzw)&0xF)==0xF;}
FINLINE bool any(const bool4& b){ return (_mm_movemask_ps(b.xyzw)&0xF)!=0x0;}
FINLINE bool none(const bool4& b){ return (_mm_movemask_ps(b.xyzw)&0xF)==0x0;}
//#TODO 这个的命名可能不太对
FINLINE bool not_all(const bool4& b){ return (_mm_movemask_ps(b.xyzw)&0xF)!=0xF;}

FINLINE int movemask(const bool4& b){ return _mm_movemask_ps(b); }

struct float4
{
  public:
    typedef float Scalar; 
    enum { size = 4 };
    union { __m128 xyzw; struct { float x, y, z, w; }; };

  public:
    FINLINE float4() : xyzw(_mm_set1_ps(Zero)) {}
    FINLINE float4(const __m128 a) : xyzw(a) {}
    FINLINE explicit float4(const float a) : xyzw(_mm_set1_ps(a)) {}
    FINLINE float4(const float x, const float y, const float z, const float w) : xyzw(_mm_set_ps(w,z,y,x)) {}
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

FINLINE bool4 operator==(const float4 &a, const float4 &b){return bool4(_mm_cmpeq_ps(a.xyzw, b.xyzw)); }
FINLINE bool4 operator!=(const float4 &a, const float4 &b){return bool4(_mm_cmpneq_ps(a.xyzw, b.xyzw));}

FINLINE bool4 operator>(const float4 &a, const float4 &b){ return  bool4(_mm_cmpgt_ps(a.xyzw, b.xyzw)); }
FINLINE bool4 operator<(const float4 &a, const float4 &b){ return  bool4(_mm_cmplt_ps(a.xyzw, b.xyzw)); }
FINLINE bool4 operator>=(const float4 &a, const float4 &b){ return  bool4(_mm_cmpge_ps(a.xyzw, b.xyzw)); }
FINLINE bool4 operator<=(const float4 &a, const float4 &b){ return  bool4(_mm_cmple_ps(a.xyzw, b.xyzw)); }

FINLINE  float4 rcp(const float4& x){ const __m128 r = _mm_rcp_ps(x); return _mm_mul_ps(r,_mm_sub_ps(_mm_set1_ps(2.0f), _mm_mul_ps(r, x))); }

FINLINE  float4 min(const float4& x,const float4& y){ return _mm_min_ps(x.xyzw,y.xyzw); }
FINLINE  float4 max(const float4& x,const float4& y){ return _mm_max_ps(x.xyzw,y.xyzw); }

FINLINE  float4 vreduce_min(const float4& x){ auto a = min(swizzle<1,0,3,2>(x),x); auto b = min(swizzle<2,3,0,1>(a),a); return b; }

FINLINE  float4 vreduce_max(const float4& x){ auto a = max(swizzle<1,0,3,2>(x),x); auto b = max(swizzle<2,3,0,1>(a),a); return b; }


FINLINE float4 madd(const float4 &a,const float4 &b,const float4 &c){ return a*b+c; }
FINLINE float4 msub(const float4 &a,const float4 &b,const float4 &c){ return a*b-c; }

FINLINE void store(const float4 &x,float* data){ _mm_store_ps(data,x.xyzw);}

NARUKAMI_END