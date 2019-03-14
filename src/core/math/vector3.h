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
    FINLINE Vector3() : x(0), y(0), z(0) { }
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
template <typename T>
FINLINE Vector3<T> operator+(const Vector3<T> &v1, const Vector3<T> &v2) { Vector3<T> v; v.x = v1.x + v2.x; v.y = v1.y + v2.y; v.z = v1.z + v2.z; return v; }
template <typename T>
FINLINE Vector3<T> operator-(const Vector3<T> &v1, const Vector3<T> &v2) { Vector3<T> v; v.x = v1.x - v2.x; v.y = v1.y - v2.y; v.z = v1.z - v2.z; return v; }
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
FINLINE T dot(const Vector3<T> &v1, const Vector3<T> &v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }
template <typename T>
FINLINE Vector3<T> cross(const Vector3<T> &v1, const Vector3<T> &v2) { return Vector3<T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x); }

template <typename T>
FINLINE float length(const Vector3<T> &v) { return sqrt(float(dot(v, v))); }
template <typename T>
FINLINE float lensqr(const Vector3<T> &v) { return float(dot(v, v)); }
template <typename T>
FINLINE Vector3f normalize(const Vector3<T> &v1) { float inv_l = rsqrt(dot(v1, v1)); return v1 * inv_l; }
template <typename T>
FINLINE Vector3f rcp(const Vector3<T> &v1) { return Vector3f(rcp(v1.x), rcp(v1.y), rcp(v1.z)); }
template <typename T>
FINLINE Vector3f sqrt(const Vector3<T> &v1) { return Vector3f(sqrt(v1.x), sqrt(v1.y), sqrt(v1.z)); }

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

    FINLINE SSEVector3f():x(0),y(0),z(0),_w(0){ }
    FINLINE SSEVector3f(const __m128 _xyzw):xyzw(_xyzw){ }
    FINLINE SSEVector3f(const float x,const float y,const float z):x(x),y(y),z(z){ }
    FINLINE explicit SSEVector3f(const float a):xyzw(_mm_set1_ps(a)){ }
    FINLINE explicit SSEVector3f(const Vector3f& v):x(v.x),y(v.y),z(v.z){ }

    FINLINE operator __m128&(){ return xyzw; }
    FINLINE  operator const __m128&() const { return xyzw; }

    FINLINE SSEVector3f& operator=(const Vector3f& v){ x=v.x; y=v.y; z=v.z; return (*this); } 
    //FINLINE SSEVector3f(const SSEVector3f& v):xyzw(v.xyzw){ }
    //FINLINE SSEVector3f& operator=(const SSEVector3f& v){ xyzw=v.xyzw; return (*this); } 

    FINLINE const float& operator[](int idx) const{ assert(idx>=0&&idx<N); return (&x)[idx]; }
    FINLINE  float& operator[](int idx){ assert(idx>=0&&idx<N); return (&x)[idx]; }
};

FINLINE  std::ostream &operator<<(std::ostream &out, const SSEVector3f &v) { out << '(' << v.x << ',' << v.y << ',' << v.z << ')'; return out; }

FINLINE SSEVector3f operator+(const SSEVector3f& v){ return v; }
 //0x80000000 xor x(y,z)
FINLINE SSEVector3f operator-(const SSEVector3f& v){ auto mask=_mm_castsi128_ps(_mm_set1_epi32(0x80000000)); return _mm_xor_ps(v.xyzw,mask); }

FINLINE SSEVector3f operator+(const SSEVector3f& v1,const SSEVector3f& v2){ return _mm_add_ps(v1,v2); }
FINLINE SSEVector3f operator-(const SSEVector3f& v1,const SSEVector3f& v2){ return _mm_sub_ps(v1,v2); }
FINLINE SSEVector3f operator*(const SSEVector3f& v1,const SSEVector3f& v2){ return _mm_mul_ps(v1,v2); }
FINLINE SSEVector3f operator*(const SSEVector3f& v1,const float a){ return _mm_mul_ps(v1,SSEVector3f(a)); }
FINLINE SSEVector3f operator/(const SSEVector3f& v1,const float a){ assert(a!=0); return _mm_div_ps(v1,SSEVector3f(a)); }

FINLINE SSEVector3f& operator+=(SSEVector3f& v1,const SSEVector3f& v2){ v1=v1+v2; return v1; }
FINLINE SSEVector3f& operator-=(SSEVector3f& v1,const SSEVector3f& v2){ v1=v1-v2; return v1; }
FINLINE SSEVector3f& operator*=(SSEVector3f& v1,const float a){ v1=v1*a; return v1; }
FINLINE SSEVector3f& operator/=(SSEVector3f& v1,const float f){ v1=v1/f; return v1; }

//only 3 bit used
FINLINE bool operator==(const SSEVector3f& v1,const SSEVector3f& v2){ return (_mm_movemask_ps(_mm_cmpeq_ps(v1.xyzw,v2.xyzw))&7)==7; }
FINLINE bool operator!=(const SSEVector3f& v1,const SSEVector3f& v2){ return (_mm_movemask_ps(_mm_cmpneq_ps(v1.xyzw,v2.xyzw))&7)!=0; }

FINLINE SSEVector3f rcp(const SSEVector3f& v) { const __m128 r = _mm_rcp_ps(v); return _mm_mul_ps(r,_mm_sub_ps(_mm_set1_ps(2.0f), _mm_mul_ps(r, v))); }


FINLINE float reduce_add(const SSEVector3f& v){ float4 a(v); float4 b=shuffle<1>(float4(v)); float4 c=shuffle<2>(float4(v)); return _mm_cvtss_f32(a+b+c); }

FINLINE float dot(const SSEVector3f& v1,const SSEVector3f& v2){ return reduce_add(v1*v2); }
FINLINE SSEVector3f cross(const SSEVector3f& v1,const SSEVector3f& v2){ auto a0=float4(v1); auto b0=shuffle<1,2,0,3>(float4(v2)); auto a1=shuffle<1,2,0,3>(float4(v1)); auto b1=float4(v1); return shuffle<1,2,0,3>(msub(a0,b0,a1*b1)); }
//0x7FFFFFFF and x(y,z)
FINLINE SSEVector3f abs(const SSEVector3f& v){  auto mask=_mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF)); return _mm_and_ps(v.xyzw,mask); }
FINLINE SSEVector3f sign(const SSEVector3f& v){ auto mask = _mm_cmplt_ps(v,SSEVector3f(Zero)); return _sse_blendv_ps(SSEVector3f(One),-SSEVector3f(One),mask); }

NARUKAMI_END