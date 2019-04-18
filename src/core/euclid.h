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
#include "core/sse.h"
#include "core/simd.h"
#include "core/math.h"
#include "core/constant.h"
NARUKAMI_BEGIN

//---VECTOR3 BEGIN---
template <typename T>
struct Vector3
{
  public:
    T x, y, z;
    typedef T Scalar;
    enum
    {
        N = 3
    };

  public:
    FINLINE Vector3() : x(Zero), y(Zero), z(Zero) { }
    FINLINE explicit Vector3(const float a) : x(a), y(a), z(a) { assert(!isnan(a)); }
    FINLINE Vector3(const T &a, const T &b, const T &c) : x(a), y(b), z(c) { assert(!isnan(a)); assert(!isnan(b)); assert(!isnan(c)); }
    //just for checking assert for debug
#ifdef NARUKAMI_DEBUG
    FINLINE Vector3(const Vector3 &v1) { assert(!isnan(v1.x)); assert(!isnan(v1.y)); assert(!isnan(v1.z)); x = v1.x; y = v1.y; z = v1.z; }
    FINLINE Vector3 &operator=(const Vector3 &v1) { assert(!isnan(v1.x)); assert(!isnan(v1.y)); assert(!isnan(v1.z)); x = v1.x; y = v1.y; z = v1.z; return (*this); }
#endif
    FINLINE const T& operator[](const int idx) const { assert(idx >= 0 && idx < N); return (&x)[idx]; }
    FINLINE T &operator[](const int idx) { assert(idx >= 0 && idx < N); return (&x)[idx]; }
};
typedef Vector3<float> Vector3f;
typedef Vector3<int> Vector3i;

template <typename T>
FINLINE  std::ostream &operator<<(std::ostream &out, const Vector3<T> &v) { out << '(' << v.x << ',' << v.y << ',' << v.z << ')'; return out; }


template <typename T>
FINLINE Vector3<T> operator+(const Vector3<T>& v){ return v; }
template <typename T>
FINLINE Vector3<T> operator-(const Vector3<T>& v1){ Vector3<T> v; v.x=-v1.x; v.y=-v1.y; v.z=-v1.z; return v; }
//compenont wise
template <typename T>
FINLINE Vector3<T> operator*(const Vector3<T> &v1, const Vector3<T> &v2) { Vector3<T> v; v.x = v1.x * v2.x; v.y = v1.y * v2.y; v.z = v1.z * v2.z; return v; }
template <typename T>
FINLINE Vector3<T> operator*(const Vector3<T> &v1, const T &f) { Vector3<T> v; v.x = v1.x * f; v.y = v1.y * f; v.z = v1.z * f; return v; }
template <typename T>
FINLINE Vector3<T> operator/(const Vector3<T> &v1, const T &f) { assert(f != 0); Vector3<T> v; v.x = v1.x / f; v.y = v1.y / f; v.z = v1.z / f; return v; }
template <typename T>
FINLINE Vector3<T> &operator+=(Vector3<T> &v, const Vector3<T> &v1) { v.x += v1.x; v.y += v1.y; v.z += v1.z; return v; }
template <typename T>
FINLINE Vector3<T> &operator-=(Vector3<T> &v, const Vector3<T> &v1) { v.x -= v1.x; v.y -= v1.y; v.z -= v1.z; return v; }
template <typename T>
FINLINE Vector3<T> &operator*=(Vector3<T> &v, const Vector3<T> &v1) { v.x *= v1.x; v.y *= v1.y; v.z *= v1.z; return v; }
template <typename T>
FINLINE Vector3<T> &operator*=(Vector3<T> &v, const T &f) { v.x *= f; v.y *= f; v.z *= f; return v; }
template <typename T>
FINLINE Vector3<T> &operator/=(Vector3<T> &v, const T &f) { assert(f != 0); v.x /= f; v.y /= f; v.z /= f; return v; }

template <typename T>
FINLINE bool operator==(const Vector3<T> &v1, const Vector3<T> &v2) { if ((v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z)) { return true; } return false; }
template <typename T>
FINLINE bool operator!=(const Vector3<T> &v1, const Vector3<T> &v2) { if ((v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z)) { return true; } return false; }

template <typename T>
FINLINE Vector3f rcp(const Vector3<T> &v1) { return Vector3f(rcp(v1.x), rcp(v1.y), rcp(v1.z)); }
template <typename T>
FINLINE Vector3<T> abs(const Vector3<T> &v1) { return Vector3f(abs(v1.x), abs(v1.y), abs(v1.z)); }
template <typename T>
FINLINE Vector3<T> sign(const Vector3<T> &v1) { return Vector3f(sign(v1.x), sign(v1.y), sign(v1.z)); }

template <typename T>
FINLINE T dot(const Vector3<T> &v1, const Vector3<T> &v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }
template <typename T>
FINLINE Vector3<T> cross(const Vector3<T> &v1, const Vector3<T> &v2) { return Vector3<T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x); }

template <typename T>
FINLINE float length(const Vector3<T> &v) { return sqrt(float(dot(v, v))); }
template <typename T>
FINLINE float sqrlen(const Vector3<T> &v) { return float(dot(v, v)); }
template <typename T>
FINLINE Vector3f normalize(const Vector3<T> &v1) { float inv_l = rsqrt(dot(v1, v1)); return v1 * inv_l; }


template <typename T>
FINLINE Vector3f sqrt(const Vector3<T> &v1) { return Vector3f(sqrt(v1.x), sqrt(v1.y), sqrt(v1.z)); }
template <typename T>
FINLINE Vector3f rsqrt(const Vector3<T> &v1){ return Vector3f(rsqrt(v1.x), rsqrt(v1.y), rsqrt(v1.z)); }

//--- [SSE] ---
//16 bit
struct SSE_ALIGNAS SSEVector3f{
    typedef float Scalar;
    enum
    {
        N = 3
    };
    
    union{
        __m128 xyzw;
        struct{float x,y,z,_w;};
    };

    FINLINE SSEVector3f():xyzw(_mm_set1_ps(Zero)){ }
    FINLINE SSEVector3f(const __m128 _xyzw):xyzw(_xyzw){ }
    FINLINE SSEVector3f(const float x,const float y,const float z):xyzw(_mm_set_ps(z,z,y,x)){ }
    FINLINE explicit SSEVector3f(const float a):xyzw(_mm_set1_ps(a)){ }
    FINLINE explicit SSEVector3f(const Vector3f& v):xyzw(_mm_set_ps(v.z,v.z,v.y,v.x)){ }

    FINLINE operator __m128&(){ return xyzw; }
    FINLINE  operator const __m128&() const { return xyzw; }

    FINLINE SSEVector3f& operator=(const Vector3f& v){ x=v.x; y=v.y; z=v.z; return (*this); } 

    FINLINE const float& operator[](int idx) const{ assert(idx>=0&&idx<N); return (&x)[idx]; }
    FINLINE  float& operator[](int idx){ assert(idx>=0&&idx<N); return (&x)[idx]; }
};

FINLINE  std::ostream &operator<<(std::ostream &out, const SSEVector3f &v) { out << '(' << v.x << ',' << v.y << ',' << v.z << ')'; return out; }

FINLINE SSEVector3f operator+(const SSEVector3f& v){ return v; }
 //0x80000000 xor x(y,z)
FINLINE SSEVector3f operator-(const SSEVector3f& v){ auto mask=_mm_castsi128_ps(_mm_set1_epi32(0x80000000)); return _mm_xor_ps(v.xyzw,mask); }

FINLINE SSEVector3f operator*(const SSEVector3f& v1,const SSEVector3f& v2){ return _mm_mul_ps(v1,v2); }
FINLINE SSEVector3f operator*(const SSEVector3f& v1,const float a){ return _mm_mul_ps(v1,SSEVector3f(a)); }
FINLINE SSEVector3f operator/(const SSEVector3f& v1,const float a){ assert(a!=0); return _mm_div_ps(v1,SSEVector3f(a)); }

FINLINE SSEVector3f& operator*=(SSEVector3f& v1,const float a){ v1=v1*a; return v1; }
FINLINE SSEVector3f& operator/=(SSEVector3f& v1,const float f){ v1=v1/f; return v1; }

//only 3 bit used
FINLINE bool operator==(const SSEVector3f& v1,const SSEVector3f& v2){ return (_mm_movemask_ps(_mm_cmpeq_ps(v1.xyzw,v2.xyzw))&0x7)==0x7; }
FINLINE bool operator!=(const SSEVector3f& v1,const SSEVector3f& v2){ return (_mm_movemask_ps(_mm_cmpneq_ps(v1.xyzw,v2.xyzw))&0x7)!=0x0; }

FINLINE SSEVector3f rcp(const SSEVector3f& v) { const __m128 r = _mm_rcp_ps(v); return _mm_mul_ps(r,_mm_sub_ps(_mm_set1_ps(2.0f), _mm_mul_ps(r, v))); }
//0x7FFFFFFF and x(y,z)
FINLINE SSEVector3f abs(const SSEVector3f& v){  auto mask=_mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF)); return _mm_and_ps(v.xyzw,mask); }
FINLINE SSEVector3f sign(const SSEVector3f& v){ auto mask = _mm_cmplt_ps(v,SSEVector3f(Zero)); return _mm_blendv_ps(SSEVector3f(One),-SSEVector3f(One),mask); }

//FINLINE float reduce_add(const SSEVector3f& v){ float4 a(v); float4 b=swizzle<1>(v); float4 c=swizzle<2>(v); return _mm_cvtss_f32(a+b+c);}
//Use SSE 4.0's _mm_dp_ps
FINLINE float dot(const SSEVector3f& v1,const SSEVector3f& v2){ return _mm_cvtss_f32(_mm_dp_ps(v1.xyzw,v2.xyzw,0x7F)); }
FINLINE SSEVector3f cross(const SSEVector3f& v1,const SSEVector3f& v2){ float4 a0=float4(v1); float4 b0=swizzle<1,2,0,3>(v2); float4 a1=swizzle<1,2,0,3>(v1); float4 b1=float4(v2); return swizzle<1,2,0,3>(msub(a0,b0,a1*b1)); }

FINLINE float length(const SSEVector3f& v){ return sqrt(dot(v,v));}
FINLINE float sqrlen(const SSEVector3f& v){ return dot(v,v);}
FINLINE SSEVector3f normalize(const SSEVector3f& v){ return v*rsqrt(dot(v,v)); }

FINLINE SSEVector3f sqrt(const SSEVector3f& v){ return _mm_sqrt_ps(v.xyzw); }
FINLINE SSEVector3f rsqrt(const SSEVector3f& v){ const __m128 r = _mm_rsqrt_ps(v.xyzw); const __m128 c = _mm_add_ps(_mm_mul_ps(_mm_set1_ps(1.5f), r),_mm_mul_ps(_mm_mul_ps(_mm_mul_ps(v.xyzw, _mm_set1_ps(-0.5f)), r), _mm_mul_ps(r, r))); return c; }


//SoA struct vector3f
struct SSE_ALIGNAS SoAVector3f{
    union{
        float4 xxxx;
        struct
        {
            float x0,x1,x2,x3;
        };   
    };

    union{
        float4 yyyy;
        struct
        {
            float y0,y1,y2,y3;
        };   
    };

    union{
        float4 zzzz;
        struct
        {
            float z0,z1,z2,z3;
        };   
    };
    
    typedef float Scalar;


    SoAVector3f():xxxx(Zero),yyyy(Zero),zzzz(Zero){}
    explicit SoAVector3f(const float a):xxxx(a),yyyy(a),zzzz(a){assert(!isnan(a));}
    SoAVector3f(const Vector3f& v0,const Vector3f& v1,const Vector3f& v2,const Vector3f& v3):xxxx(v3.x,v2.x,v1.x,v0.x),yyyy(v3.y,v2.y,v1.y,v0.y),zzzz(v3.z,v2.z,v1.z,v0.z){ }
    explicit SoAVector3f(const Vector3f& v):xxxx(v.x),yyyy(v.y),zzzz(v.z){ }
    SoAVector3f(const float4& x,const float4& y,const float4& z):xxxx(x),yyyy(y),zzzz(z){}
    SoAVector3f(const float x,const float y,const float z):xxxx(x),yyyy(y),zzzz(z){}
};

FINLINE  std::ostream &operator<<(std::ostream &out, const SoAVector3f &v) { 
    out << '(' << v.x0 << ',' << v.y0 << ',' << v.z0 << ')';
    out << '(' << v.x1 << ',' << v.y1 << ',' << v.z1 << ')';
    out << '(' << v.x2 << ',' << v.y2 << ',' << v.z2 << ')';
    out << '(' << v.x3 << ',' << v.y3 << ',' << v.z3 << ')'; 
    return out;
}

FINLINE int operator==(const SoAVector3f& v0,const SoAVector3f& v1){

    bool4  mask_xxxx =(v0.xxxx==v1.xxxx);
    bool4  mask_yyyy =(v0.yyyy==v1.yyyy);
    bool4  mask_zzzz =(v0.zzzz==v1.zzzz);

    return movemask((mask_xxxx&mask_yyyy)&mask_zzzz);
}
FINLINE int operator!=(const SoAVector3f& v0,const SoAVector3f& v1){
    bool4  mask_xxxx =(v0.xxxx!=v1.xxxx);
    bool4  mask_yyyy =(v0.yyyy!=v1.yyyy);
    bool4  mask_zzzz =(v0.zzzz!=v1.zzzz);
    return movemask((mask_xxxx|mask_yyyy)|mask_zzzz);
}

FINLINE float4 dot(const SoAVector3f& v0,const SoAVector3f& v1){ return v0.xxxx*v1.xxxx+v0.yyyy*v1.yyyy+v0.zzzz*v1.zzzz; }

FINLINE SoAVector3f cross(const SoAVector3f& v0,const SoAVector3f& v1){ float4 xxxx=v0.yyyy*v1.zzzz-v0.zzzz*v1.yyyy; float4 yyyy=v0.zzzz*v1.xxxx-v0.xxxx*v1.zzzz; float4 zzzz=v0.xxxx*v1.yyyy-v0.yyyy*v1.xxxx; return SoAVector3f(xxxx,yyyy,zzzz); }

FINLINE SoAVector3f rcp(const SoAVector3f& v){ SoAVector3f vv; vv.xxxx = rcp(v.xxxx); vv.yyyy = rcp(v.yyyy); vv.zzzz = rcp(v.zzzz); return vv; }
FINLINE SoAVector3f safe_rcp(const SoAVector3f& v){ SoAVector3f vv; vv.xxxx = safe_rcp(v.xxxx); vv.yyyy = safe_rcp(v.yyyy); vv.zzzz = safe_rcp(v.zzzz); return vv; }
//---VECTOR3 END---

//---POINT3 BEGIN---
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
    SoAPoint3f(const Point3f& v0,const Point3f& v1,const Point3f& v2,const Point3f& v3):xxxx(_mm_set_ps(v3.x,v2.x,v1.x,v0.x)),yyyy(_mm_set_ps(v3.y,v2.y,v1.y,v0.y)),zzzz(_mm_set_ps(v3.z,v2.z,v1.z,v0.z)){ }
    explicit SoAPoint3f(const Point3f& v):xxxx(_mm_set1_ps(v.x)),yyyy(_mm_set1_ps(v.y)),zzzz(_mm_set1_ps(v.z)){ }
    SoAPoint3f(const float x0,const float y0,const float z0,const float x1,const float y1,const float z1,const float x2,const float y2,const float z2,const float x3,const float y3,const float z3):xxxx(_mm_set_ps(x3,x2,x1,x3)),yyyy(_mm_set_ps(y3,y2,y1,y0)),zzzz(_mm_set_ps(z3,z2,z1,z0)){ assert(!isnan(x0));assert(!isnan(y0));assert(!isnan(z0)); assert(!isnan(x1));assert(!isnan(y1));assert(!isnan(z1)); assert(!isnan(x2));assert(!isnan(y2));assert(!isnan(z2)); assert(!isnan(x3));assert(!isnan(y3));assert(!isnan(z3)); }
    SoAPoint3f(const __m128 x,const __m128 y,const __m128 z):xxxx(x),yyyy(y),zzzz(z){}
    SoAPoint3f(const float x,const float y,const float z):xxxx(_mm_set1_ps(x)),yyyy(_mm_set1_ps(y)),zzzz(_mm_set1_ps(z)){}
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

//---POINT3 END---

//---POINT2 BEGIN---
template <typename T>
struct Point2
{
  public:
    T x, y;
    typedef T Scalar;
    enum
    {
        N = 2
    };

  public:
    FINLINE Point2() : x(Zero), y(Zero) { }
    FINLINE explicit Point2(const float a) : x(a), y(a){ assert(!isnan(a)); }
    FINLINE Point2(const T &a, const T &b) : x(a), y(b) { assert(!isnan(a)); assert(!isnan(b)); }
    //just for checking assert for debug
#ifdef NARUKAMI_DEBUG
    FINLINE Point2(const Point2 &v1) { assert(!isnan(v1.x)); assert(!isnan(v1.y));  x = v1.x; y = v1.y }
    FINLINE Point2 &operator=(const Point2 &v1) { assert(!isnan(v1.x)); assert(!isnan(v1.y));  x = v1.x; y = v1.y; return (*this); }
#endif
    FINLINE const T& operator[](const int idx) const { assert(idx >= 0 && idx < N); return (&x)[idx]; }
    FINLINE T &operator[](const int idx) { assert(idx >= 0 && idx < N); return (&x)[idx]; }
};
typedef Point2<float> Point2f;
typedef Point2<int> Point2i;
//---POINT2 END---

//---MATRIX4X4 BEGIN---
struct SSE_ALIGNAS Matrix4x4{
public:
    union{
        float m[16];
        float mn[4][4];
        __m128 mVec[4];
    };
    typedef float Scalar;
    enum
    {
        N = 16
    };
public:
    Matrix4x4(){
         //col 0
        m[0]=One;
        m[1]=Zero;
        m[2]=Zero;
        m[3]=Zero;
        //col 1
        m[4]=Zero;
        m[5]=One;
        m[6]=Zero;
        m[7]=Zero;
        //col 2
        m[8]=Zero;
        m[9]=Zero;
        m[10]=One;
        m[11]=Zero;
        //col 3
        m[12]=Zero;
        m[13]=Zero;
        m[14]=Zero;
        m[15]=One;
    }
    Matrix4x4(const float m0,const float m1,const float m2,const float m3,const float m4,const float m5,const float m6,const float m7,const float m8,const float m9,const float m10,const float m11,const float m12,const float m13,const float m14,const float m15)
    {
        //col 0
        m[0]=m0;
        m[1]=m1;
        m[2]=m2;
        m[3]=m3;
        //col 1
        m[4]=m4;
        m[5]=m5;
        m[6]=m6;
        m[7]=m7;
        //col 2
        m[8]=m8;
        m[9]=m9;
        m[10]=m10;
        m[11]=m11;
        //col 3
        m[12]=m12;
        m[13]=m13;
        m[14]=m14;
        m[15]=m15;
    }

    Matrix4x4(const float* v){
        //col 0
        m[0]=v[0];
        m[1]=v[1];
        m[2]=v[2];
        m[3]=v[3];
        //col 1
        m[4]=v[4];
        m[5]=v[5];
        m[6]=v[6];
        m[7]=v[7];
        //col 2
        m[8]=v[8];
        m[9]=v[9];
        m[10]=v[10];
        m[11]=v[11];
        //col 3
        m[12]=v[12];
        m[13]=v[13];
        m[14]=v[14];
        m[15]=v[15];
    }
    FINLINE const float& operator[](const int idx) const { assert(idx >= 0 && idx < N); return m[idx];}
    FINLINE float& operator[](const int idx){ assert(idx >= 0 && idx < N); return m[idx];}
};

FINLINE std::ostream &operator<<(std::ostream &out, const Matrix4x4 &v){
    for(int i=0;i<Matrix4x4::N;++i){
        out<<v.m[i];
        out<<(((i+1)==Matrix4x4::N)?')':',');
    }
    return out;
}

FINLINE bool operator==(const Matrix4x4& A,const Matrix4x4& B){

    auto bool_i0 = float4(A.mVec[0])==float4(B.mVec[0]);
    auto bool_i1 = float4(A.mVec[1])==float4(B.mVec[1]);
    auto bool_i2 = float4(A.mVec[2])==float4(B.mVec[2]);
    auto bool_i3 = float4(A.mVec[3])==float4(B.mVec[3]);

    return all((bool_i0&bool_i1)&(bool_i2&bool_i3));
}

FINLINE bool operator!=(const Matrix4x4& A,const Matrix4x4& B){

    auto bool_i0 = float4(A.mVec[0])==float4(B.mVec[0]);
    auto bool_i1 = float4(A.mVec[1])==float4(B.mVec[1]);
    auto bool_i2 = float4(A.mVec[2])==float4(B.mVec[2]);
    auto bool_i3 = float4(A.mVec[3])==float4(B.mVec[3]);

   return not_all((bool_i0&bool_i1)&(bool_i2&bool_i3));
}

FINLINE Matrix4x4 operator+(const Matrix4x4& A,const Matrix4x4& B){
    Matrix4x4 r;
    r.mVec[0] = float4(A.mVec[0])+float4(B.mVec[0]);
    r.mVec[1] = float4(A.mVec[1])+float4(B.mVec[1]);
    r.mVec[2] = float4(A.mVec[2])+float4(B.mVec[2]);
    r.mVec[3] = float4(A.mVec[3])+float4(B.mVec[3]);
    return r;
}

FINLINE Matrix4x4 operator*(const Matrix4x4& A,const Matrix4x4& B){
    
    Matrix4x4 ret;

    float4 r=float4(A.mVec[0])*float4(B.m[0]);
    r+=float4(A.mVec[1])*float4(B.m[1]);
    r+=float4(A.mVec[2])*float4(B.m[2]);
    r+=float4(A.mVec[3])*float4(B.m[3]);
    ret.mVec[0]=r.xyzw;

    r =float4(A.mVec[0])*float4(B.m[4]);
    r+=float4(A.mVec[1])*float4(B.m[5]);
    r+=float4(A.mVec[2])*float4(B.m[6]);
    r+=float4(A.mVec[3])*float4(B.m[7]);
    ret.mVec[1]=r.xyzw;

    r =float4(A.mVec[0])*float4(B.m[8]);
    r+=float4(A.mVec[1])*float4(B.m[9]);
    r+=float4(A.mVec[2])*float4(B.m[10]);
    r+=float4(A.mVec[3])*float4(B.m[11]);
    ret.mVec[2]=r.xyzw;

    r =float4(A.mVec[0])*float4(B.m[12]);
    r+=float4(A.mVec[1])*float4(B.m[13]);
    r+=float4(A.mVec[2])*float4(B.m[14]);
    r+=float4(A.mVec[3])*float4(B.m[15]);
    ret.mVec[3]=r.xyzw;
    
    return ret;
}

//https://lxjk.github.io/2017/09/03/Fast-4x4-Matrix-Inverse-with-SSE-SIMD-Explained.html#_appendix_2
FINLINE Matrix4x4 transform_inverse_noscale(const Matrix4x4& mat){
    Matrix4x4 r;
    //transpose left-upper 3x3
    __m128 t0 = shuffle<0,1,0,1>(mat.mVec[0],mat.mVec[1]);// 00,10,01,11
    __m128 t1 = shuffle<2,3,2,3>(mat.mVec[0],mat.mVec[1]);// 20,30,21,31
    // mat.mVec[2] : 02,12,22,32
    r.mVec[0] = shuffle<0,2,0,3>(t0,mat.mVec[2]);//00,01,02,32(=0)
    r.mVec[1] = shuffle<1,3,1,3>(t0,mat.mVec[2]);//10,11,12,32(=0)
    r.mVec[2] = shuffle<0,2,2,3>(t1,mat.mVec[2]);//20,21,22,32(=0)

    //translate
    r.mVec[3] =                      _mm_mul_ps(r.mVec[0],swizzle<0>(mat.mVec[3]));
    r.mVec[3] = _mm_add_ps(r.mVec[3],_mm_mul_ps(r.mVec[1],swizzle<1>(mat.mVec[3])));
    r.mVec[3] = _mm_add_ps(r.mVec[3],_mm_mul_ps(r.mVec[2],swizzle<2>(mat.mVec[3])));
    r.mVec[3] =  _mm_sub_ps(_mm_set_ps(One,Zero,Zero,Zero),r.mVec[3]);
    
    return r;
}


FINLINE Matrix4x4 transform_inverse(const Matrix4x4& mat){
    Matrix4x4 r;
    //transpose left-upper 3x3
    __m128 t0 = shuffle<0,1,0,1>(mat.mVec[0],mat.mVec[1]);// 00,10,01,11
    __m128 t1 = shuffle<2,3,2,3>(mat.mVec[0],mat.mVec[1]);// 20,30,21,31
    // mat.mVec[2] : 02,12,22,32
    r.mVec[0] = shuffle<0,2,0,3>(t0,mat.mVec[2]);//00,01,02,32(=0)
    r.mVec[1] = shuffle<1,3,1,3>(t0,mat.mVec[2]);//10,11,12,32(=0)
    r.mVec[2] = shuffle<0,2,2,3>(t1,mat.mVec[2]);//20,21,22,32(=0)

    //calculate sqr size
    __m128 sqrl = _mm_mul_ps(r.mVec[0],r.mVec[0]);
    sqrl=_mm_add_ps(sqrl,_mm_mul_ps(r.mVec[1],r.mVec[1]));
    sqrl=_mm_add_ps(sqrl,_mm_mul_ps(r.mVec[2],r.mVec[2]));

    //here need avoid divide by zero?
    //but I will remove it here,and add (0,0,0,1) to it
     sqrl=_mm_add_ps(sqrl,_mm_set_ps(One,Zero,Zero,Zero));

    __m128 rsqrl= _mm_div_ps(_mm_set1_ps(One),sqrl);

    r.mVec[0] = _mm_mul_ps(r.mVec[0],rsqrl);
    r.mVec[1] = _mm_mul_ps(r.mVec[1],rsqrl);
    r.mVec[2] = _mm_mul_ps(r.mVec[2],rsqrl);

    //translate
    r.mVec[3] =                      _mm_mul_ps(r.mVec[0],swizzle<0>(mat.mVec[3]));
    r.mVec[3] = _mm_add_ps(r.mVec[3],_mm_mul_ps(r.mVec[1],swizzle<1>(mat.mVec[3])));
    r.mVec[3] = _mm_add_ps(r.mVec[3],_mm_mul_ps(r.mVec[2],swizzle<2>(mat.mVec[3])));
    r.mVec[3] =  _mm_sub_ps(_mm_set_ps(One,Zero,Zero,Zero),r.mVec[3]);
    
    return r;
}


static FINLINE __m128 adj2x2(const __m128 m){ return _mm_mul_ps(swizzle<3,2,1,0>(m),_mm_set_ps(One,NegOne,NegOne,One)); }
static FINLINE __m128 mul2x2(const __m128 m0,const __m128 m1){ return _mm_add_ps(_mm_mul_ps(swizzle<0,1,0,1>(m0),swizzle<0,0,2,2>(m1)),_mm_mul_ps(swizzle<2,3,2,3>(m0),swizzle<1,1,3,3>(m1))); }
static FINLINE __m128 det2x2(const __m128 m){ return _mm_sub_ps( _mm_mul_ps(swizzle<0,0,0,0>(m)/*a*/,swizzle<3,3,3,3>(m)/*d*/), _mm_mul_ps(swizzle<1,1,1,1>(m)/*c*/,swizzle<2,2,2,2>(m)/*b*/) ); }


//Blockwise inversion
//https://en.wikipedia.org/wiki/Invertible_matrix#Blockwise_inversion
FINLINE Matrix4x4 inverse(const Matrix4x4& mat){
    Matrix4x4 r;
    //extract 4 sub-matrix2x2
    // | A B |
    // | C D |
    __m128 A = shuffle<0,1,0,1>(mat.mVec[0],mat.mVec[1]);
    __m128 C = shuffle<2,3,2,3>(mat.mVec[0],mat.mVec[1]);
    __m128 B = shuffle<0,1,0,1>(mat.mVec[2],mat.mVec[3]);
    __m128 D = shuffle<2,3,2,3>(mat.mVec[2],mat.mVec[3]);

    //2x2 det
    // h->  |D| |B| |C| |A| <-l
    // __m128 det=_mm_sub_ps(
    // _mm_mul_ps(shuffle<0,2,0,2>(mat.mVec[0],mat.mVec[2])/*a*/,shuffle<1,3,1,3>(mat.mVec[1],mat.mVec[3])/*d*/),
    // _mm_mul_ps(shuffle<1,3,1,3>(mat.mVec[0],mat.mVec[2])/*c*/,shuffle<0,2,0,2>(mat.mVec[1],mat.mVec[3])/*b*/)
    // );

    //2x2 det
    __m128 detA = det2x2(A);
    // __m128 detB = swizzle<2>(det);
    // __m128 detC = swizzle<1>(det);
    // __m128 detD = swizzle<3>(det);


    __m128 invA= _mm_div_ps(adj2x2(A),detA);
    __m128 D_CinvAB= _mm_sub_ps(D,mul2x2(mul2x2(C,invA),B));
    __m128 det_D_CinvAB=_mm_sub_ps(
    _mm_mul_ps(swizzle<0,0,0,0>(D_CinvAB)/*a*/,swizzle<3,3,3,3>(D_CinvAB)/*d*/),
    _mm_mul_ps(swizzle<1,1,1,1>(D_CinvAB)/*c*/,swizzle<2,2,2,2>(D_CinvAB)/*b*/)
    );

    __m128 DD=_mm_div_ps(adj2x2(D_CinvAB),det_D_CinvAB);
    __m128 CinvA = _mm_mul_ps(C,invA);
    __m128 invAB = _mm_mul_ps(invA,B);
    __m128 invABinv_D_CinvAB = _mm_mul_ps(invAB,DD);
    __m128 BB =  _mm_sub_ps(_mm_set1_ps(Zero),invABinv_D_CinvAB);
    __m128 AA =  _mm_add_ps(invA,_mm_mul_ps(invABinv_D_CinvAB,CinvA));
    __m128 CC =  _mm_sub_ps(_mm_set1_ps(Zero),_mm_mul_ps(DD,CinvA));

    r.mVec[0] = shuffle<0,1,0,1>(AA,CC);
    r.mVec[1] = shuffle<2,3,2,3>(AA,CC);
    r.mVec[2] = shuffle<0,1,0,1>(BB,DD);
    r.mVec[3] = shuffle<2,3,2,3>(BB,DD);

    return r;
}

FINLINE Matrix4x4 transpose(const Matrix4x4& mat){
    Matrix4x4 r(mat);
    _MM_TRANSPOSE4_PS(r.mVec[0],r.mVec[1],r.mVec[2],r.mVec[3]);
    return r;
}
//---MATRIX4X4 END---

//---GENERAL BEGIN---
template <typename T>
FINLINE Point3<T> operator+(const Point3<T> &p, const Vector3<T> &v) { Point3<T> rp; rp.x = p.x + v.x; rp.y = p.y + v.y; rp.z = p.z + v.z; return rp; }
template <typename T>
FINLINE Point3<T> operator-(const Point3<T> &p, const Vector3<T> &v) {return p+(-v);}
template <typename T>
FINLINE Vector3<T> operator-(const Point3<T> &p1, const Point3<T> &p2) { Vector3<T> p; p.x = p1.x - p2.x; p.y = p1.y - p2.y; p.z = p1.z - p2.z; return p; }


FINLINE SSEVector3f operator+(const SSEPoint3f&  p ,const SSEVector3f& v ){ return _mm_add_ps(p,v); }
FINLINE SSEVector3f operator-(const SSEPoint3f&  p ,const SSEVector3f& v ){ return _mm_sub_ps(p,v); }
FINLINE SSEVector3f operator-(const SSEPoint3f& p1,const SSEPoint3f& p2){ return _mm_sub_ps(p1,p2); }

FINLINE float distance(const Point3f& p1,const Point3f& p2){ return length(p2-p1); }

FINLINE float distance(const SSEPoint3f& p1,const SSEPoint3f& p2){ return length(p2-p1); }

FINLINE Vector3f operator*(const Matrix4x4& M,const Vector3f& v){
    // 8ns   
    // float4 r=float4(M.mVec[0])*float4(v.x);
    // r+=float4(M.mVec[1])*float4(v.y);
    // r+=float4(M.mVec[2])*float4(v.z);
    // return Vector3f(r.x,r.y,r.z);
    
    // 7ns
    float x =  M.m[0]*v.x+M.m[4]*v.y+M.m[8]*v.z;
    float y =  M.m[1]*v.x+M.m[5]*v.y+M.m[9]*v.z;
    float z =  M.m[3]*v.x+M.m[6]*v.y+M.m[10]*v.z;

    return Vector3f(x,y,z);
}

FINLINE Point3f operator*(const Matrix4x4& M,const Point3f& v){
    float4 r=float4(M.mVec[0])*float4(v.x);
    r+=float4(M.mVec[1])*float4(v.y);
    r+=float4(M.mVec[2])*float4(v.z);
    r+=float4(M.mVec[3])*float4(One);
    return Point3f(r.x,r.y,r.z);
}



FINLINE SSEVector3f operator*(const Matrix4x4& M,const SSEVector3f& v){
    float4 vx=swizzle<0,0,0,0>(v.xyzw);
    float4 vy=swizzle<1,1,1,1>(v.xyzw);
    float4 vz=swizzle<2,2,2,2>(v.xyzw);
    
    float4 r=float4(M.mVec[0])*vx;
    r+=float4(M.mVec[1])*vy;
    r+=float4(M.mVec[2])*vz;
    return SSEVector3f(r.xyzw);
}

FINLINE SSEPoint3f operator*(const Matrix4x4& M,const SSEPoint3f& v){
    float4 vx=swizzle<0,0,0,0>(v.xyzw);
    float4 vy=swizzle<1,1,1,1>(v.xyzw);
    float4 vz=swizzle<2,2,2,2>(v.xyzw);
    
    float4 r=float4(M.mVec[0])*vx;
    r+=float4(M.mVec[1])*vy;
    r+=float4(M.mVec[2])*vz;
    r+=float4(M.mVec[3]);
    return SSEPoint3f(r.xyzw);
}


FINLINE SoAVector3f operator*(const Matrix4x4& M,const SoAVector3f& v){
    float4 r_xxxx=    float4(swizzle<0,0,0,0>(M.mVec[0]))/*m00*/*float4(v.xxxx);
    r_xxxx = r_xxxx + float4(swizzle<0,0,0,0>(M.mVec[1]))/*m10*/*float4(v.yyyy);
    r_xxxx = r_xxxx + float4(swizzle<0,0,0,0>(M.mVec[2]))/*m20*/*float4(v.zzzz);

    float4 r_yyyy=    float4(swizzle<1,1,1,1>(M.mVec[0]))/*m01*/*float4(v.xxxx);
    r_yyyy = r_yyyy + float4(swizzle<1,1,1,1>(M.mVec[1]))/*m11*/*float4(v.yyyy);
    r_yyyy = r_yyyy + float4(swizzle<1,1,1,1>(M.mVec[2]))/*m21*/*float4(v.zzzz);

    float4 r_zzzz=    float4(swizzle<2,2,2,2>(M.mVec[0]))/*m02*/*float4(v.xxxx);
    r_zzzz = r_zzzz + float4(swizzle<2,2,2,2>(M.mVec[1]))/*m12*/*float4(v.yyyy);
    r_zzzz = r_zzzz + float4(swizzle<2,2,2,2>(M.mVec[2]))/*m22*/*float4(v.zzzz);

    return SoAVector3f(r_xxxx,r_yyyy,r_zzzz);
}

FINLINE SoAPoint3f operator*(const Matrix4x4& M,const SoAPoint3f& v){
    float4 r_xxxx=    float4(swizzle<0,0,0,0>(M.mVec[0]))/*m00*/*float4(v.xxxx);
    r_xxxx = r_xxxx + float4(swizzle<0,0,0,0>(M.mVec[1]))/*m10*/*float4(v.yyyy);
    r_xxxx = r_xxxx + float4(swizzle<0,0,0,0>(M.mVec[2]))/*m20*/*float4(v.zzzz);
    r_xxxx = r_xxxx + float4(swizzle<0,0,0,0>(M.mVec[3]));
   
    float4 r_yyyy=    float4(swizzle<1,1,1,1>(M.mVec[0]))/*m01*/*float4(v.xxxx);
    r_yyyy = r_yyyy + float4(swizzle<1,1,1,1>(M.mVec[1]))/*m11*/*float4(v.yyyy);
    r_yyyy = r_yyyy + float4(swizzle<1,1,1,1>(M.mVec[2]))/*m21*/*float4(v.zzzz);
    r_yyyy = r_yyyy + float4(swizzle<1,1,1,1>(M.mVec[3]));

    float4 r_zzzz=    float4(swizzle<2,2,2,2>(M.mVec[0]))/*m02*/*float4(v.xxxx);
    r_zzzz = r_zzzz + float4(swizzle<2,2,2,2>(M.mVec[1]))/*m12*/*float4(v.yyyy);
    r_zzzz = r_zzzz + float4(swizzle<2,2,2,2>(M.mVec[2]))/*m22*/*float4(v.zzzz);
    r_zzzz = r_zzzz + float4(swizzle<2,2,2,2>(M.mVec[3]));

    return SoAPoint3f(r_xxxx,r_yyyy,r_zzzz);
}

FINLINE SoAVector3f operator-(const SoAPoint3f& p0, const SoAPoint3f& p1){
    auto xxxx=float4(p0.xxxx) - float4(p1.xxxx);
    auto yyyy=float4(p0.yyyy) - float4(p1.yyyy);
    auto zzzz=float4(p0.zzzz) - float4(p1.zzzz);
    return SoAVector3f(xxxx,yyyy,zzzz);
}

//---GENERAL END---
NARUKAMI_END
