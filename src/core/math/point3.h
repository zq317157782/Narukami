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

template <typename T>
struct Point3
{
  public:
    T x, y, z;
    typedef T Scalar;
    enum
    {
        N = 3
    };

  public:
    FINLINE Point3() : x(Zero), y(Zero), z(Zero) { }
    FINLINE explicit Point3(const float a) : x(a), y(a), z(a) { assert(!isnan(a)); }
    FINLINE Point3(const T &a, const T &b, const T &c) : x(a), y(b), z(c) { assert(!isnan(a)); assert(!isnan(b)); assert(!isnan(c)); }
    //just for checking assert for debug
#ifdef NARUKAMI_DEBUG
    FINLINE Point3(const Point3 &v1) { assert(!isnan(v1.x)); assert(!isnan(v1.y)); assert(!isnan(v1.z)); x = v1.x; y = v1.y; z = v1.z; }
    FINLINE Point3 &operator=(const Point3 &v1) { assert(!isnan(v1.x)); assert(!isnan(v1.y)); assert(!isnan(v1.z)); x = v1.x; y = v1.y; z = v1.z; return (*this); }
#endif
    FINLINE const T& operator[](const int idx) const { assert(idx >= 0 && idx < N); return (&x)[idx]; }
    FINLINE T &operator[](const int idx) { assert(idx >= 0 && idx < N); return (&x)[idx]; }
};
typedef Point3<float> Point3f;
typedef Point3<int> Point3i;

template <typename T>
FINLINE  std::ostream &operator<<(std::ostream &out, const Point3<T> &v) { out << '(' << v.x << ',' << v.y << ',' << v.z << ')'; return out; }


template <typename T>
FINLINE Point3<T> operator+(const Point3<T>& v){ return v; }
template <typename T>
FINLINE Point3<T> operator-(const Point3<T>& v1){ Point3<T> v; v.x=-v1.x; v.y=-v1.y; v.z=-v1.z; return v; }
//compenont wise
template <typename T>
FINLINE Point3<T> operator*(const Point3<T> &v1, const Point3<T> &v2) { Point3<T> v; v.x = v1.x * v2.x; v.y = v1.y * v2.y; v.z = v1.z * v2.z; return v; }
template <typename T>
FINLINE Point3<T> operator*(const Point3<T> &v1, const T &f) { Point3<T> v; v.x = v1.x * f; v.y = v1.y * f; v.z = v1.z * f; return v; }
template <typename T>
FINLINE Point3<T> operator/(const Point3<T> &v1, const T &f) { assert(f != 0); Point3<T> v; v.x = v1.x / f; v.y = v1.y / f; v.z = v1.z / f; return v; }
template <typename T>
FINLINE Point3<T> &operator*=(Point3<T> &v, const Point3<T> &v1) { v.x *= v1.x; v.y *= v1.y; v.z *= v1.z; return v; }
template <typename T>
FINLINE Point3<T> &operator*=(Point3<T> &v, const T &f) { v.x *= f; v.y *= f; v.z *= f; return v; }
template <typename T>
FINLINE Point3<T> &operator/=(Point3<T> &v, const T &f) { assert(f != 0); v.x /= f; v.y /= f; v.z /= f; return v; }

template <typename T>
FINLINE bool operator==(const Point3<T> &v1, const Point3<T> &v2) { if ((v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z)) { return true; } return false; }
template <typename T>
FINLINE bool operator!=(const Point3<T> &v1, const Point3<T> &v2) { if ((v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z)) { return true; } return false; }

template <typename T>
FINLINE Point3f rcp(const Point3<T> &v1) { return Point3f(rcp(v1.x), rcp(v1.y), rcp(v1.z)); }
template <typename T>
FINLINE Point3<T> abs(const Point3<T> &v1) { return Point3f(abs(v1.x), abs(v1.y), abs(v1.z)); }
template <typename T>
FINLINE Point3<T> sign(const Point3<T> &v1) { return Point3f(sign(v1.x), sign(v1.y), sign(v1.z)); }


template <typename T>
FINLINE Point3f sqrt(const Point3<T> &v1) { return Point3f(sqrt(v1.x), sqrt(v1.y), sqrt(v1.z)); }
template <typename T>
FINLINE Point3f rsqrt(const Point3<T> &v1){ return Point3f(rsqrt(v1.x), rsqrt(v1.y), rsqrt(v1.z)); }



//--- [SSE] ---
//16 bit
struct SSE_ALIGNAS SSEPoint3f{
    typedef float Scalar;
    enum
    {
        N = 3
    };
    
    union{
        __m128 xyzw;
        struct{float x,y,z,_w;};
    };

    FINLINE SSEPoint3f():xyzw(_mm_set1_ps(Zero)){ }
    FINLINE SSEPoint3f(const __m128 _xyzw):xyzw(_xyzw){ }
    FINLINE SSEPoint3f(const float x,const float y,const float z):xyzw(_mm_set_ps(z,z,y,x)){ }
    FINLINE explicit SSEPoint3f(const float a):xyzw(_mm_set1_ps(a)){ }
    FINLINE explicit SSEPoint3f(const Point3f& v):xyzw(_mm_set_ps(v.z,v.z,v.y,v.x)){ }

    FINLINE operator __m128&(){ return xyzw; }
    FINLINE  operator const __m128&() const { return xyzw; }

    FINLINE SSEPoint3f& operator=(const Point3f& v){ x=v.x; y=v.y; z=v.z; return (*this); } 

    FINLINE const float& operator[](int idx) const{ assert(idx>=0&&idx<N); return (&x)[idx]; }
    FINLINE  float& operator[](int idx){ assert(idx>=0&&idx<N); return (&x)[idx]; }
};

FINLINE  std::ostream &operator<<(std::ostream &out, const SSEPoint3f &v) { out << '(' << v.x << ',' << v.y << ',' << v.z << ')'; return out; }

FINLINE SSEPoint3f operator+(const SSEPoint3f& v){ return v; }
 //0x80000000 xor x(y,z)
FINLINE SSEPoint3f operator-(const SSEPoint3f& v){ auto mask=_mm_castsi128_ps(_mm_set1_epi32(0x80000000)); return _mm_xor_ps(v.xyzw,mask); }

FINLINE SSEPoint3f operator*(const SSEPoint3f& v1,const SSEPoint3f& v2){ return _mm_mul_ps(v1,v2); }
FINLINE SSEPoint3f operator*(const SSEPoint3f& v1,const float a){ return _mm_mul_ps(v1,SSEPoint3f(a)); }
FINLINE SSEPoint3f operator/(const SSEPoint3f& v1,const float a){ assert(a!=0); return _mm_div_ps(v1,SSEPoint3f(a)); }

FINLINE SSEPoint3f& operator*=(SSEPoint3f& v1,const float a){ v1=v1*a; return v1; }
FINLINE SSEPoint3f& operator/=(SSEPoint3f& v1,const float f){ v1=v1/f; return v1; }

//only 3 bit used
FINLINE bool operator==(const SSEPoint3f& v1,const SSEPoint3f& v2){ return (_mm_movemask_ps(_mm_cmpeq_ps(v1.xyzw,v2.xyzw))&7)==7; }
FINLINE bool operator!=(const SSEPoint3f& v1,const SSEPoint3f& v2){ return (_mm_movemask_ps(_mm_cmpneq_ps(v1.xyzw,v2.xyzw))&7)!=0; }

FINLINE SSEPoint3f rcp(const SSEPoint3f& v) { const __m128 r = _mm_rcp_ps(v); return _mm_mul_ps(r,_mm_sub_ps(_mm_set1_ps(2.0f), _mm_mul_ps(r, v))); }
//0x7FFFFFFF and x(y,z)
FINLINE SSEPoint3f abs(const SSEPoint3f& v){  auto mask=_mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF)); return _mm_and_ps(v.xyzw,mask); }
FINLINE SSEPoint3f sign(const SSEPoint3f& v){ auto mask = _mm_cmplt_ps(v,SSEPoint3f(Zero)); return _mm_blendv_ps(SSEPoint3f(One),-SSEPoint3f(One),mask); }

FINLINE float sum(const SSEPoint3f& v){ float4 a(v); float4 b=swizzle<1>(v); float4 c=swizzle<2>(v); return _mm_cvtss_f32(a+b+c);}

FINLINE SSEPoint3f sqrt(const SSEPoint3f& v){ return _mm_sqrt_ps(v.xyzw); }
FINLINE SSEPoint3f rsqrt(const SSEPoint3f& v){ const __m128 r = _mm_rsqrt_ps(v.xyzw); const __m128 c = _mm_add_ps(_mm_mul_ps(_mm_set1_ps(1.5f), r),_mm_mul_ps(_mm_mul_ps(_mm_mul_ps(v.xyzw, _mm_set1_ps(-0.5f)), r), _mm_mul_ps(r, r))); return c; }


//TODO : need to refactor 
//SoA struct vector3f
struct SSE_ALIGNAS SoAPoint3f{
    union{
        __m128 xxxx;
        struct
        {
            float x0,x1,x2,x3;
        };   
    };

    union{
        __m128 yyyy;
        struct
        {
            float y0,y1,y2,y3;
        };   
    };

    union{
        __m128 zzzz;
        struct
        {
            float z0,z1,z2,z3;
        };   
    };
    
    typedef float Scalar;


    SoAPoint3f():xxxx(_mm_set1_ps(Zero)),yyyy(_mm_set1_ps(Zero)),zzzz(_mm_set1_ps(Zero)){}
    explicit SoAPoint3f(const float a):xxxx(_mm_set1_ps(a)),yyyy(_mm_set1_ps(a)),zzzz(_mm_set1_ps(a)){assert(!isnan(a));}
    SoAPoint3f(const Vector3f& v0,const Vector3f& v1,const Vector3f& v2,const Vector3f& v3):xxxx(_mm_set_ps(v3.x,v2.x,v1.x,v0.x)),yyyy(_mm_set_ps(v3.y,v2.y,v1.y,v0.y)),zzzz(_mm_set_ps(v3.z,v2.z,v1.z,v0.z)){ }
    explicit SoAPoint3f(const Vector3f& v):xxxx(_mm_set1_ps(v.x)),yyyy(_mm_set1_ps(v.y)),zzzz(_mm_set1_ps(v.z)){ }
    SoAPoint3f(const float x0,const float y0,const float z0,const float x1,const float y1,const float z1,const float x2,const float y2,const float z2,const float x3,const float y3,const float z3):xxxx(_mm_set_ps(x3,x2,x1,x3)),yyyy(_mm_set_ps(y3,y2,y1,y0)),zzzz(_mm_set_ps(z3,z2,z1,z0)){ assert(!isnan(x0));assert(!isnan(y0));assert(!isnan(z0)); assert(!isnan(x1));assert(!isnan(y1));assert(!isnan(z1)); assert(!isnan(x2));assert(!isnan(y2));assert(!isnan(z2)); assert(!isnan(x3));assert(!isnan(y3));assert(!isnan(z3)); }
    SoAPoint3f(const __m128 x,const __m128 y,const __m128 z):xxxx(x),yyyy(y),zzzz(z){}
};

FINLINE  std::ostream &operator<<(std::ostream &out, const SoAPoint3f &v) { 
    out << '(' << v.x0 << ',' << v.y0 << ',' << v.z0 << ')';
    out << '(' << v.x1 << ',' << v.y1 << ',' << v.z1 << ')';
    out << '(' << v.x2 << ',' << v.y2 << ',' << v.z2 << ')';
    out << '(' << v.x3 << ',' << v.y3 << ',' << v.z3 << ')'; 
    return out;
}

FINLINE bool operator==(const SoAPoint3f& v0,const SoAPoint3f& v1){ 
    __m128 mask_xxxx=_mm_cmpeq_ps(v0.xxxx,v1.xxxx);
    __m128 mask_yyyy=_mm_cmpeq_ps(v0.yyyy,v1.yyyy);
    __m128 mask_zzzz=_mm_cmpeq_ps(v0.zzzz,v1.zzzz);
    return (_mm_movemask_ps(_mm_and_ps(_mm_and_ps(mask_xxxx,mask_yyyy),mask_zzzz))&15)==15;
}
FINLINE bool operator!=(const SoAPoint3f& v0,const SoAPoint3f& v1){
    __m128 mask_xxxx=_mm_cmpeq_ps(v0.xxxx,v1.xxxx);
    __m128 mask_yyyy=_mm_cmpeq_ps(v0.yyyy,v1.yyyy);
    __m128 mask_zzzz=_mm_cmpeq_ps(v0.zzzz,v1.zzzz);
    return (_mm_movemask_ps(_mm_and_ps(_mm_and_ps(mask_xxxx,mask_yyyy),mask_zzzz))&15)!=15;
}
NARUKAMI_END 