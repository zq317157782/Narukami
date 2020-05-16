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

template <typename T>
struct Vector3;
template <typename T>
struct Normal3;

template <typename T>
struct Point3;

//---VECTOR3 BEGIN---
template <typename T>
struct Vector3
{
    T x, y, z;

    inline Vector3() : x((T)0), y((T)0), z((T)0) {}
    inline explicit Vector3(const float a) : x(a), y(a), z(a) { assert(!isnan(a)); }
    inline Vector3(const T &a, const T &b, const T &c) : x(a), y(b), z(c)
    {
        assert(!isnan(a));
        assert(!isnan(b));
        assert(!isnan(c));
    }
    inline Vector3(const Normal3<T> &n) : x(n.x), y(n.y), z(n.z) {}
    inline Vector3(const Point3<T> &n) : x(n.x), y(n.y), z(n.z) {}
    //just for checking assert for debug
#ifdef NARUKAMI_DEBUG
    inline Vector3(const Vector3 &v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        assert(!isnan(v1.z));
        x = v1.x;
        y = v1.y;
        z = v1.z;
    }
    inline Vector3 &operator=(const Vector3 &v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        assert(!isnan(v1.z));
        x = v1.x;
        y = v1.y;
        z = v1.z;
        return (*this);
    }
    inline Vector3(Vector3 &&v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        assert(!isnan(v1.z));
        x = std::move(v1.x);
        y = std::move(v1.y);
        z = std::move(v1.z);
    }
    inline Vector3 &operator=(Vector3 &&v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        assert(!isnan(v1.z));
        x = std::move(v1.x);
        y = std::move(v1.y);
        z = std::move(v1.z);
        return (*this);
    }
#endif
    inline const T &operator[](const int idx) const
    {
        assert(idx >= 0 && idx < 3);
        return (&x)[idx];
    }
    inline T &operator[](const int idx)
    {
        assert(idx >= 0 && idx < 3);
        return (&x)[idx];
    }
};
typedef Vector3<float> Vector3f;
typedef Vector3<int> Vector3i;

template <typename T>
inline std::ostream &operator<<(std::ostream &out, const Vector3<T> &v)
{
    out << '(' << v.x << ',' << v.y << ',' << v.z << ')';
    return out;
}

template <typename T>
inline Vector3<T> operator+(const Vector3<T> &v) { return v; }
template <typename T>
inline Vector3<T> operator-(const Vector3<T> &v1)
{
    Vector3<T> v;
    v.x = -v1.x;
    v.y = -v1.y;
    v.z = -v1.z;
    return v;
}
//compenont wise
template <typename T>
inline Vector3<T> operator*(const Vector3<T> &v1, const Vector3<T> &v2)
{
    Vector3<T> v;
    v.x = v1.x * v2.x;
    v.y = v1.y * v2.y;
    v.z = v1.z * v2.z;
    return v;
}
template <typename T>
inline Vector3<T> operator*(const Vector3<T> &v1, const T &f)
{
    Vector3<T> v;
    v.x = v1.x * f;
    v.y = v1.y * f;
    v.z = v1.z * f;
    return v;
}
template <typename T>
inline Vector3<T> operator/(const Vector3<T> &v1, const Vector3<T> &v2)
{
    assert(v2.x != 0);
    assert(v2.y != 0);
    assert(v2.z != 0);
    Vector3<T> v;
    v.x = v1.x / v2.x;
    v.y = v1.y / v2.y;
    v.z = v1.z / v2.z;
    return v;
}
template <typename T>
inline Vector3<T> operator/(const Vector3<T> &v1, const T &f)
{
    assert(f != 0);
    Vector3<T> v;
    v.x = v1.x / f;
    v.y = v1.y / f;
    v.z = v1.z / f;
    return v;
}
template <typename T>
inline Vector3<T> &operator+=(Vector3<T> &v, const Vector3<T> &v1)
{
    v.x += v1.x;
    v.y += v1.y;
    v.z += v1.z;
    return v;
}
template <typename T>
inline Vector3<T> &operator-=(Vector3<T> &v, const Vector3<T> &v1)
{
    v.x -= v1.x;
    v.y -= v1.y;
    v.z -= v1.z;
    return v;
}
template <typename T>
inline Vector3<T> &operator*=(Vector3<T> &v, const Vector3<T> &v1)
{
    v.x *= v1.x;
    v.y *= v1.y;
    v.z *= v1.z;
    return v;
}
template <typename T>
inline Vector3<T> &operator*=(Vector3<T> &v, const T &f)
{
    v.x *= f;
    v.y *= f;
    v.z *= f;
    return v;
}
template <typename T>
inline Vector3<T> &operator/=(Vector3<T> &v, const T &f)
{
    assert(f != 0);
    v.x /= f;
    v.y /= f;
    v.z /= f;
    return v;
}

template <typename T>
inline bool operator==(const Vector3<T> &v1, const Vector3<T> &v2)
{
    if ((v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z))
    {
        return true;
    }
    return false;
}
template <typename T>
inline bool operator!=(const Vector3<T> &v1, const Vector3<T> &v2)
{
    if ((v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z))
    {
        return true;
    }
    return false;
}

template <typename T>
inline Vector3f rcp(const Vector3<T> &v1) { return Vector3f(rcp(v1.x), rcp(v1.y), rcp(v1.z)); }
template <typename T>
inline Vector3<T> abs(const Vector3<T> &v1) { return Vector3f(abs(v1.x), abs(v1.y), abs(v1.z)); }
template <typename T>
inline Vector3<T> sign(const Vector3<T> &v1) { return Vector3f(sign(v1.x), sign(v1.y), sign(v1.z)); }

template <typename T>
inline T dot(const Vector3<T> &v1, const Vector3<T> &v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }
template <typename T>
inline Vector3<T> cross(const Vector3<T> &v1, const Vector3<T> &v2) { return Vector3<T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x); }

template <typename T>
inline float length(const Vector3<T> &v) { return sqrt(static_cast<float>(dot(v, v))); }
template <typename T>
inline float sqrlen(const Vector3<T> &v) { return float(dot(v, v)); }
template <typename T>
inline Vector3f normalize(const Vector3<T> &v1)
{
    float inv_l = rsqrt(dot(v1, v1));
    return v1 * inv_l;
}

template <typename T>
inline Vector3f sqrt(const Vector3<T> &v1) { return Vector3f(sqrt(v1.x), sqrt(v1.y), sqrt(v1.z)); }
template <typename T>
inline Vector3f rsqrt(const Vector3<T> &v1) { return Vector3f(rsqrt(v1.x), rsqrt(v1.y), rsqrt(v1.z)); }

template <typename T>
inline Vector3<T> min(const Vector3<T> &v0, const Vector3<T> &v1) { return Vector3<T>(min(v0.x, v1.x), min(v0.y, v1.y), min(v0.z, v1.z)); }
template <typename T>
inline Vector3<T> max(const Vector3<T> &v0, const Vector3<T> &v1) { return Vector3<T>(max(v0.x, v1.x), max(v0.y, v1.y), max(v0.z, v1.z)); }

//--- [SSE] ---
//16 bit
struct SSE_ALIGNAS SSEVector3f
{
    union {
        float4 xyzw;
        struct
        {
            float x, y, z, _w;
        };
    };

    inline SSEVector3f() : xyzw(float4()) {}
    inline SSEVector3f(const float4 &_xyzw) : xyzw(_xyzw) {}
    inline SSEVector3f(const float x, const float y, const float z) : xyzw(float4(x, y, z, 0.0f)) {}
    inline explicit SSEVector3f(const float a) : xyzw(float4(a)) {}
    inline explicit SSEVector3f(const Vector3f &v) : xyzw(float4(v.x, v.y, v.z, 0.0f)) {}

    inline operator float4 &() { return xyzw; }
    inline operator const float4 &() const { return xyzw; }

    inline SSEVector3f &operator=(const Vector3f &v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return (*this);
    }

    inline const float &operator[](int idx) const
    {
        assert(idx >= 0 && idx < 3);
        return (&x)[idx];
    }
    inline float &operator[](int idx)
    {
        assert(idx >= 0 && idx < 3);
        return (&x)[idx];
    }
};

inline std::ostream &operator<<(std::ostream &out, const SSEVector3f &v)
{
    out << '(' << v.x << ',' << v.y << ',' << v.z << ')';
    return out;
}

inline SSEVector3f operator+(const SSEVector3f &v) { return v; }
//0x80000000 xor x(y,z)
inline SSEVector3f operator-(const SSEVector3f &v) { return -v.xyzw; }

inline SSEVector3f operator*(const SSEVector3f &v1, const SSEVector3f &v2) { return v1.xyzw * v2.xyzw; }
inline SSEVector3f operator*(const SSEVector3f &v1, const float a) { return v1.xyzw * float4(a); }
inline SSEVector3f operator/(const SSEVector3f &v1, const float a)
{
    assert(a != 0);
    return v1.xyzw / float4(a);
}

inline SSEVector3f &operator*=(SSEVector3f &v1, const float a)
{
    v1 = v1 * a;
    return v1;
}
inline SSEVector3f &operator/=(SSEVector3f &v1, const float f)
{
    v1 = v1 / f;
    return v1;
}

//only 3 bit used
inline bool operator==(const SSEVector3f &v1, const SSEVector3f &v2) { return (_mm_movemask_ps(_mm_cmpeq_ps(v1.xyzw, v2.xyzw)) & 7) == 7; }
inline bool operator!=(const SSEVector3f &v1, const SSEVector3f &v2) { return (_mm_movemask_ps(_mm_cmpneq_ps(v1.xyzw, v2.xyzw)) & 0x7) != 0x0; }

inline SSEVector3f rcp(const SSEVector3f &v) { return rcp(v.xyzw); }
//0x7FFFFFFF and x(y,z)
inline SSEVector3f abs(const SSEVector3f &v) { return abs(v.xyzw); } //TODO 在我的败家之眼上，输入(-1,-1,-1,-1)返回的是(nan,nan,nan,nan)
inline SSEVector3f sign(const SSEVector3f &v) { return sign(v.xyzw); }

//inline float reduce_add(const SSEVector3f& v){ float4 a(v); float4 b=swizzle<1>(v); float4 c=swizzle<2>(v); return _mm_cvtss_f32(a+b+c);}
//Use SSE 4.0's _mm_dp_ps
inline float dot(const SSEVector3f &v1, const SSEVector3f &v2) { return _mm_cvtss_f32(_mm_dp_ps(v1.xyzw, v2.xyzw, 0x7F)); }
inline SSEVector3f cross(const SSEVector3f &v1, const SSEVector3f &v2)
{
    float4 a0 = v1.xyzw;
    float4 b0 = swizzle<1, 2, 0, 3>(v2.xyzw);
    float4 a1 = swizzle<1, 2, 0, 3>(v1.xyzw);
    float4 b1 = v2.xyzw;
    return swizzle<1, 2, 0, 3>(msub(a0, b0, a1 * b1));
}

inline float length(const SSEVector3f &v) { return sqrt(dot(v, v)); }
inline float sqrlen(const SSEVector3f &v) { return dot(v, v); }
inline SSEVector3f normalize(const SSEVector3f &v) { return v * rsqrt(dot(v, v)); }

inline SSEVector3f sqrt(const SSEVector3f &v) { return sqrt(v.xyzw); }
inline SSEVector3f rsqrt(const SSEVector3f &v) { return rsqrt(v.xyzw); }

//SoA struct vector3f
struct SSE_ALIGNAS Vector3f4p
{
    union {
        float4 xxxx;
        struct
        {
            float x0, x1, x2, x3;
        };
    };

    union {
        float4 yyyy;
        struct
        {
            float y0, y1, y2, y3;
        };
    };

    union {
        float4 zzzz;
        struct
        {
            float z0, z1, z2, z3;
        };
    };

    inline Vector3f4p() : xxxx(0.0f), yyyy(0.0f), zzzz(0.0f) {}
    inline explicit Vector3f4p(const float a) : xxxx(a), yyyy(a), zzzz(a) { assert(!isnan(a)); }
    inline Vector3f4p(const Vector3f &v0, const Vector3f &v1, const Vector3f &v2, const Vector3f &v3) : xxxx(v0.x, v1.x, v2.x, v3.x), yyyy(v0.y, v1.y, v2.y, v3.y), zzzz(v0.z, v1.z, v2.z, v3.z) {}
    inline explicit Vector3f4p(const Vector3f &v) : xxxx(v.x), yyyy(v.y), zzzz(v.z) {}
    inline Vector3f4p(const float4 &x, const float4 &y, const float4 &z) : xxxx(x), yyyy(y), zzzz(z) {}
    inline Vector3f4p(const float x, const float y, const float z) : xxxx(x), yyyy(y), zzzz(z) {}
    
    inline Vector3f operator[](const int idx) const
    {
        assert(idx >= 0 && idx < SSE_FLOAT_COUNT);
        return Vector3f(xxxx[idx], yyyy[idx], zzzz[idx]);
    }
};

inline std::ostream &operator<<(std::ostream &out, const Vector3f4p &v)
{
    out << '(' << v.x0 << ',' << v.y0 << ',' << v.z0 << ')';
    out << '(' << v.x1 << ',' << v.y1 << ',' << v.z1 << ')';
    out << '(' << v.x2 << ',' << v.y2 << ',' << v.z2 << ')';
    out << '(' << v.x3 << ',' << v.y3 << ',' << v.z3 << ')';
    return out;
}

inline int operator==(const Vector3f4p &v0, const Vector3f4p &v1)
{
    bool4 mask_xxxx = (v0.xxxx == v1.xxxx);
    bool4 mask_yyyy = (v0.yyyy == v1.yyyy);
    bool4 mask_zzzz = (v0.zzzz == v1.zzzz);

    return movemask((mask_xxxx & mask_yyyy) & mask_zzzz);
}
inline int operator!=(const Vector3f4p &v0, const Vector3f4p &v1)
{
    bool4 mask_xxxx = (v0.xxxx != v1.xxxx);
    bool4 mask_yyyy = (v0.yyyy != v1.yyyy);
    bool4 mask_zzzz = (v0.zzzz != v1.zzzz);
    return movemask((mask_xxxx | mask_yyyy) | mask_zzzz);
}

inline float4 dot(const Vector3f4p &v0, const Vector3f4p &v1) { return v0.xxxx * v1.xxxx + v0.yyyy * v1.yyyy + v0.zzzz * v1.zzzz; }

inline Vector3f4p cross(const Vector3f4p &v0, const Vector3f4p &v1)
{
    float4 xxxx = v0.yyyy * v1.zzzz - v0.zzzz * v1.yyyy;
    float4 yyyy = v0.zzzz * v1.xxxx - v0.xxxx * v1.zzzz;
    float4 zzzz = v0.xxxx * v1.yyyy - v0.yyyy * v1.xxxx;
    return Vector3f4p(xxxx, yyyy, zzzz);
}

inline Vector3f4p rcp(const Vector3f4p &v)
{
    Vector3f4p vv;
    vv.xxxx = rcp(v.xxxx);
    vv.yyyy = rcp(v.yyyy);
    vv.zzzz = rcp(v.zzzz);
    return vv;
}
inline Vector3f4p safe_rcp(const Vector3f4p &v)
{
    Vector3f4p vv;
    vv.xxxx = safe_rcp(v.xxxx);
    vv.yyyy = safe_rcp(v.yyyy);
    vv.zzzz = safe_rcp(v.zzzz);
    return vv;
}
inline Vector3f4p robust_rcp(const Vector3f4p &v)
{
    Vector3f4p vv;
    auto one = float4(1.0f);
    vv.xxxx = one / v.xxxx;
    vv.yyyy = one / v.yyyy;
    vv.zzzz = one / v.zzzz;
    return vv;
}
inline Vector3f4p load(const Vector3f *vector_array) { return Vector3f4p(vector_array[0], vector_array[1], vector_array[2], vector_array[3]); }
//---VECTOR3 END---

//---VECTOR2 BEGIN---
template <typename T>
struct Vector2
{
    T x, y;

    inline Vector2() : x((T)0), y((T)0) {}
    inline explicit Vector2(const float a) : x(a), y(a) { assert(!isnan(a)); }
    inline Vector2(const T &a, const T &b) : x(a), y(b)
    {
        assert(!isnan(a));
        assert(!isnan(b));
    }
    //just for checking assert for debug
#ifdef NARUKAMI_DEBUG
    inline Vector2(const Vector2 &v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        x = v1.x;
        y = v1.y;
    }
    inline Vector2 &operator=(const Vector2 &v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        x = v1.x;
        y = v1.y;
        return (*this);
    }
    inline Vector2(Vector2 &&v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        x = std::move(v1.x);
        y = std::move(v1.y);
    }
    inline Vector2 &operator=(Vector2 &&v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        x = std::move(v1.x);
        y = std::move(v1.y);
        return (*this);
    }
#endif
    inline const T &operator[](const int idx) const
    {
        assert(idx >= 0 && idx < 2);
        return (&x)[idx];
    }
    inline T &operator[](const int idx)
    {
        assert(idx >= 0 && idx < 2);
        return (&x)[idx];
    }
};
typedef Vector2<float> Vector2f;
typedef Vector2<int> Vector2i;
//---VECTOR2 END---

//---POINT3 BEGIN---
template <typename T>
struct Point3
{
    T x, y, z;

    inline Point3() : x((T)0), y((T)0), z((T)0) {}
    inline explicit Point3(const float a) : x(a), y(a), z(a) { assert(!isnan(a)); }
    inline Point3(const T &a, const T &b, const T &c) : x(a), y(b), z(c)
    {
        assert(!isnan(a));
        assert(!isnan(b));
        assert(!isnan(c));
    }
    //just for checking assert for debug
#ifdef NARUKAMI_DEBUG
    inline Point3(const Point3 &v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        assert(!isnan(v1.z));
        x = v1.x;
        y = v1.y;
        z = v1.z;
    }
    inline Point3 &operator=(const Point3 &v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        assert(!isnan(v1.z));
        x = v1.x;
        y = v1.y;
        z = v1.z;
        return (*this);
    }
    inline Point3(Point3 &&v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        assert(!isnan(v1.z));
        x = std::move(v1.x);
        y = std::move(v1.y);
        z = std::move(v1.z);
    }
    inline Point3 &operator=(Point3 &&v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        assert(!isnan(v1.z));
        x = std::move(v1.x);
        y = std::move(v1.y);
        z = std::move(v1.z);
        return (*this);
    }
#endif
    inline const T &operator[](const int idx) const
    {
        assert(idx >= 0 && idx < 3);
        return (&x)[idx];
    }
    inline T &operator[](const int idx)
    {
        assert(idx >= 0 && idx < 3);
        return (&x)[idx];
    }
};
typedef Point3<float> Point3f;
typedef Point3<int> Point3i;

template <typename T>
inline std::ostream &operator<<(std::ostream &out, const Point3<T> &v)
{
    out << '(' << v.x << ',' << v.y << ',' << v.z << ')';
    return out;
}

template <typename T>
inline Point3<T> operator+(const Point3<T> &v) { return v; }
template <typename T>
inline Point3<T> operator-(const Point3<T> &v1)
{
    Point3<T> v;
    v.x = -v1.x;
    v.y = -v1.y;
    v.z = -v1.z;
    return v;
}

//operator+  =>  just for affine interpolation
template <typename T>
inline Point3<T> operator+(const Point3<T> &v1, const Point3<T> &v2)
{
    Point3<T> v;
    v.x = v1.x + v2.x;
    v.y = v1.y + v2.y;
    v.z = v1.z + v2.z;
    return v;
}
//compenont wise
template <typename T>
inline Point3<T> operator*(const Point3<T> &v1, const Point3<T> &v2)
{
    Point3<T> v;
    v.x = v1.x * v2.x;
    v.y = v1.y * v2.y;
    v.z = v1.z * v2.z;
    return v;
}
template <typename T>
inline Point3<T> operator*(const Point3<T> &v1, const T &f)
{
    Point3<T> v;
    v.x = v1.x * f;
    v.y = v1.y * f;
    v.z = v1.z * f;
    return v;
}
template <typename T>
inline Point3<T> operator/(const Point3<T> &v1, const T &f)
{
    assert(f != 0);
    Point3<T> v;
    v.x = v1.x / f;
    v.y = v1.y / f;
    v.z = v1.z / f;
    return v;
}
template <typename T>
inline Point3<T> &operator*=(Point3<T> &v, const Point3<T> &v1)
{
    v.x *= v1.x;
    v.y *= v1.y;
    v.z *= v1.z;
    return v;
}
template <typename T>
inline Point3<T> &operator*=(Point3<T> &v, const T &f)
{
    v.x *= f;
    v.y *= f;
    v.z *= f;
    return v;
}
template <typename T>
inline Point3<T> &operator/=(Point3<T> &v, const T &f)
{
    assert(f != 0);
    v.x /= f;
    v.y /= f;
    v.z /= f;
    return v;
}

template <typename T>
inline bool operator==(const Point3<T> &v1, const Point3<T> &v2)
{
    if ((v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z))
    {
        return true;
    }
    return false;
}
template <typename T>
inline bool operator!=(const Point3<T> &v1, const Point3<T> &v2)
{
    if ((v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z))
    {
        return true;
    }
    return false;
}

template <typename T>
inline Point3f rcp(const Point3<T> &v1) { return Point3f(rcp(v1.x), rcp(v1.y), rcp(v1.z)); }
template <typename T>
inline Point3<T> abs(const Point3<T> &v1) { return Point3f(abs(v1.x), abs(v1.y), abs(v1.z)); }
template <typename T>
inline Point3<T> sign(const Point3<T> &v1) { return Point3f(sign(v1.x), sign(v1.y), sign(v1.z)); }

template <typename T>
inline Point3f sqrt(const Point3<T> &v1) { return Point3f(sqrt(v1.x), sqrt(v1.y), sqrt(v1.z)); }
template <typename T>
inline Point3f rsqrt(const Point3<T> &v1) { return Point3f(rsqrt(v1.x), rsqrt(v1.y), rsqrt(v1.z)); }

template <typename T>
inline Point3<T> min(const Point3<T> &p0, const Point3<T> &p1) { return Point3<T>(min(p0.x, p1.x), min(p0.y, p1.y), min(p0.z, p1.z)); }
template <typename T>
inline Point3<T> max(const Point3<T> &p0, const Point3<T> &p1) { return Point3<T>(max(p0.x, p1.x), max(p0.y, p1.y), max(p0.z, p1.z)); }

//--- [SSE] ---
//16 bit
struct SSE_ALIGNAS SSEPoint3f
{
    union {
        float4 xyzw;
        struct
        {
            float x, y, z, _w;
        };
    };

    inline SSEPoint3f() : xyzw(float4()) {}
    inline SSEPoint3f(const float4 &_xyzw) : xyzw(_xyzw) {}
    inline SSEPoint3f(const float x, const float y, const float z) : xyzw(float4(x, y, z, 1.0f)) {}
    inline explicit SSEPoint3f(const float a) : xyzw(float4(a)) {}
    inline explicit SSEPoint3f(const Point3f &v) : xyzw(float4(v.x, v.y, v.z, 1.0f)) {}

    inline operator float4 &() { return xyzw; }
    inline operator const float4 &() const { return xyzw; }

    inline SSEPoint3f &operator=(const Point3f &v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return (*this);
    }

    inline const float &operator[](int idx) const
    {
        assert(idx >= 0 && idx < 3);
        return (&x)[idx];
    }
    inline float &operator[](int idx)
    {
        assert(idx >= 0 && idx < 3);
        return (&x)[idx];
    }
};

inline std::ostream &operator<<(std::ostream &out, const SSEPoint3f &v)
{
    out << '(' << v.x << ',' << v.y << ',' << v.z << ')';
    return out;
}

inline SSEPoint3f operator+(const SSEPoint3f &v) { return v; }
//0x80000000 xor x(y,z)
inline SSEPoint3f operator-(const SSEPoint3f &v) { return -v.xyzw; }

inline SSEPoint3f operator*(const SSEPoint3f &v1, const SSEPoint3f &v2) { return v1.xyzw * v2.xyzw; }
inline SSEPoint3f operator*(const SSEPoint3f &v1, const float a) { return v1.xyzw * float4(a); }
inline SSEPoint3f operator/(const SSEPoint3f &v1, const float a)
{
    assert(a != 0);
    return v1.xyzw / float4(a);
}

inline SSEPoint3f &operator*=(SSEPoint3f &v1, const float a)
{
    v1 = v1 * a;
    return v1;
}
inline SSEPoint3f &operator/=(SSEPoint3f &v1, const float f)
{
    v1 = v1 / f;
    return v1;
}

//only 3 bit used
inline bool operator==(const SSEPoint3f &v1, const SSEPoint3f &v2) { return (_mm_movemask_ps(_mm_cmpeq_ps(v1.xyzw, v2.xyzw)) & 7) == 7; }
inline bool operator!=(const SSEPoint3f &v1, const SSEPoint3f &v2) { return (_mm_movemask_ps(_mm_cmpneq_ps(v1.xyzw, v2.xyzw)) & 7) != 0; }

inline SSEPoint3f rcp(const SSEPoint3f &v) { return rcp(v.xyzw); }
//0x7FFFFFFF and x(y,z)
inline SSEPoint3f abs(const SSEPoint3f &v) { return abs(v.xyzw); }
inline SSEPoint3f sign(const SSEPoint3f &v) { return sign(v.xyzw); }

//inline float sum(const SSEPoint3f& v){ float4 a(v); float4 b=swizzle<1>(v.xyzw); float4 c=swizzle<2>(v.xyzw); return _mm_cvtss_f32(a+b+c);}

inline SSEPoint3f sqrt(const SSEPoint3f &v) { return sqrt(v.xyzw); }
inline SSEPoint3f rsqrt(const SSEPoint3f &v) { return rsqrt(v.xyzw); }

//TODO : need to refactor
struct SSE_ALIGNAS Point3f4p
{
    union {
        float4 xxxx;
        struct
        {
            float x0, x1, x2, x3;
        };
    };

    union {
        float4 yyyy;
        struct
        {
            float y0, y1, y2, y3;
        };
    };

    union {
        float4 zzzz;
        struct
        {
            float z0, z1, z2, z3;
        };
    };

    typedef float Scalar;

    inline Point3f4p() : xxxx(0.0f), yyyy(0.0f), zzzz(0.0f) {}
    inline explicit Point3f4p(const float a) : xxxx(a), yyyy(a), zzzz(a) { assert(!isnan(a)); }
    inline Point3f4p(const Point3f &v0, const Point3f &v1, const Point3f &v2, const Point3f &v3) : xxxx(v0.x, v1.x, v2.x, v3.x), yyyy(v0.y, v1.y, v2.y, v3.y), zzzz(v0.z, v1.z, v2.z, v3.z) {}
    inline explicit Point3f4p(const Point3f &v) : xxxx(v.x), yyyy(v.y), zzzz(v.z) {}
    inline Point3f4p(const float4 &x, const float4 &y, const float4 &z) : xxxx(x), yyyy(y), zzzz(z) {}
    inline Point3f4p(const float x, const float y, const float z) : xxxx(x), yyyy(y), zzzz(z) {}

    inline Point3f operator[](const int idx) const
    {
        assert(idx >= 0 && idx < SSE_FLOAT_COUNT);
        return Point3f(xxxx[idx], yyyy[idx], zzzz[idx]);
    }
};

inline std::ostream &operator<<(std::ostream &out, const Point3f4p &v)
{
    out << '(' << v.x0 << ',' << v.y0 << ',' << v.z0 << ')';
    out << '(' << v.x1 << ',' << v.y1 << ',' << v.z1 << ')';
    out << '(' << v.x2 << ',' << v.y2 << ',' << v.z2 << ')';
    out << '(' << v.x3 << ',' << v.y3 << ',' << v.z3 << ')';
    return out;
}

inline int operator==(const Point3f4p &v0, const Point3f4p &v1)
{

    bool4 mask_xxxx = (v0.xxxx == v1.xxxx);
    bool4 mask_yyyy = (v0.yyyy == v1.yyyy);
    bool4 mask_zzzz = (v0.zzzz == v1.zzzz);

    return movemask((mask_xxxx & mask_yyyy) & mask_zzzz);
}
inline int operator!=(const Point3f4p &v0, const Point3f4p &v1)
{
    bool4 mask_xxxx = (v0.xxxx != v1.xxxx);
    bool4 mask_yyyy = (v0.yyyy != v1.yyyy);
    bool4 mask_zzzz = (v0.zzzz != v1.zzzz);
    return movemask((mask_xxxx | mask_yyyy) | mask_zzzz);
}

inline Point3f4p min(const Point3f4p &p0, const Point3f4p &p1)
{
    return Point3f4p(min(p0.xxxx, p1.xxxx), min(p0.yyyy, p1.yyyy), min(p0.zzzz, p1.zzzz));
}

inline Point3f4p max(const Point3f4p &p0, const Point3f4p &p1)
{
    return Point3f4p(max(p0.xxxx, p1.xxxx), max(p0.yyyy, p1.yyyy), max(p0.zzzz, p1.zzzz));
}

inline Point3f4p load(const Point3f *point_array)
{
    return Point3f4p(point_array[0], point_array[1], point_array[2], point_array[3]);
}
//---POINT3 END---

//---NORMAL3 BEGIN---
template <typename T>
struct Normal3
{
public:
    T x, y, z;
public:
    inline Normal3() : x((T)0), y((T)0), z((T)0) {}
    inline explicit Normal3(const float a) : x(a), y(a), z(a) { assert(!isnan(a)); }
    inline Normal3(const T &a, const T &b, const T &c) : x(a), y(b), z(c)
    {
        assert(!isnan(a));
        assert(!isnan(b));
        assert(!isnan(c));
    }
    inline Normal3(const Vector3<T> &v) : x(v.x), y(v.y), z(v.z) {}
    //just for checking assert for debug
#ifdef NARUKAMI_DEBUG
    inline Normal3(const Normal3 &v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        assert(!isnan(v1.z));
        x = v1.x;
        y = v1.y;
        z = v1.z;
    }
    inline Normal3 &operator=(const Normal3 &v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        assert(!isnan(v1.z));
        x = v1.x;
        y = v1.y;
        z = v1.z;
        return (*this);
    }
    inline Normal3(Normal3 &&v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        assert(!isnan(v1.z));
        x = std::move(v1.x);
        y = std::move(v1.y);
        z = std::move(v1.z);
    }
    inline Normal3 &operator=(Normal3 &&v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        assert(!isnan(v1.z));
        x = std::move(v1.x);
        y = std::move(v1.y);
        z = std::move(v1.z);
        return (*this);
    }
#endif
    inline const T &operator[](const int idx) const
    {
        assert(idx >= 0 && idx < 3);
        return (&x)[idx];
    }
    inline T &operator[](const int idx)
    {
        assert(idx >= 0 && idx < 3);
        return (&x)[idx];
    }
};
typedef Normal3<float> Normal3f;
typedef Normal3<int> Normal3i;

template <typename T>
inline Normal3<T> operator-(const Normal3<T> &v1)
{
    Normal3<T> v;
    v.x = -v1.x;
    v.y = -v1.y;
    v.z = -v1.z;
    return v;
}
template <typename T>
inline Normal3<T> operator-(const Normal3<T> &v1, const T &f)
{
    Normal3<T> v;
    v.x = v1.x - f;
    v.y = v1.y - f;
    v.z = v1.z - f;
    return v;
}
template <typename T>
inline std::ostream &operator<<(std::ostream &out, const Normal3<T> &v)
{
    out << '(' << v.x << ',' << v.y << ',' << v.z << ')';
    return out;
}
template <typename T>
inline Normal3<T> operator+(const Normal3<T> &v1, const T &x)
{
    Normal3<T> v;
    v.x = v1.x + x;
    v.y = v1.y + x;
    v.z = v1.z + x;
    return v;
}
template <typename T>
inline Normal3<T> operator*(const Normal3<T> &v1, const T &f)
{
    Normal3<T> v;
    v.x = v1.x * f;
    v.y = v1.y * f;
    v.z = v1.z * f;
    return v;
}
template <typename T>
inline bool operator==(const Normal3<T> &v1, const Normal3<T> &v2)
{
    if ((v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z))
    {
        return true;
    }
    return false;
}
template <typename T>
inline T dot(const Normal3<T> &v1, const Vector3<T> &v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }
template <typename T>
inline T dot(const Vector3<T> &v1, const Normal3<T> &v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }
template <typename T>
inline T dot(const Normal3<T> &v1, const Normal3<T> &v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }
template <typename T>
inline Normal3f normalize(const Normal3<T> &v1)
{
    float inv_l = rsqrt(dot(v1, v1));
    return v1 * inv_l;
}
template <typename T>
inline float length(const Normal3<T> &v) { return sqrt(static_cast<float>(dot(v, v))); }
//---NORMAL3 END---

//---POINT2 BEGIN---
template <typename T>
struct Point2
{
public:
    T x, y;
public:
    inline Point2() : x((T)0), y((T)0) {}
    inline explicit Point2(const float a) : x(a), y(a) { assert(!isnan(a)); }
    inline Point2(const T &a, const T &b) : x(a), y(b)
    {
        assert(!isnan(a));
        assert(!isnan(b));
    }
    template <typename U>
    inline explicit Point2(const Point2<U> &p) : x((T)p.x), y((T)p.y) {}
    //just for checking assert for debug
#ifdef NARUKAMI_DEBUG
    inline Point2(const Point2 &v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        x = v1.x;
        y = v1.y;
    }
    inline Point2 &operator=(const Point2 &v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        x = v1.x;
        y = v1.y;
        return (*this);
    }
    inline Point2(Point2 &&v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        x = std::move(v1.x);
        y = std::move(v1.y);
    }
    inline Point2 &operator=(Point2 &&v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        x = std::move(v1.x);
        y = std::move(v1.y);
        return (*this);
    }
#endif
    inline const T &operator[](const int idx) const
    {
        assert(idx >= 0 && idx < 2);
        return (&x)[idx];
    }
    inline T &operator[](const int idx)
    {
        assert(idx >= 0 && idx < 2);
        return (&x)[idx];
    }
};

template <typename T>
inline std::ostream &operator<<(std::ostream &out, const Point2<T> &v)
{
    out << '(' << v.x << ',' << v.y << ')';
    return out;
}

template <typename T>
inline bool operator==(const Point2<T> &v1, const Point2<T> &v2)
{
    if ((v1.x == v2.x) && (v1.y == v2.y))
    {
        return true;
    }
    return false;
}
template <typename T>
inline bool operator!=(const Point2<T> &v1, const Point2<T> &v2)
{
    if ((v1.x != v2.x) || (v1.y != v2.y))
    {
        return true;
    }
    return false;
}

template <typename T>
inline bool operator<(const Point2<T> &v1, const Point2<T> &v2)
{
    if (v1.x < v2.x)
    {
        return true;
    }
    else if ((v1.x == v2.x) && (v1.y < v2.y))
    {
        return true;
    }

    return false;
}

//operator+  =>  just for affine interpolation
template <typename T>
inline Point2<T> operator+(const Point2<T> &v1, const Point2<T> &v2)
{
    Point2<T> v;
    v.x = v1.x + v2.x;
    v.y = v1.y + v2.y;
    return v;
}
template <typename T>
inline Point2<T> operator+(const Point2<T> &v1, const T &f)
{
    Point2<T> v;
    v.x = v1.x + f;
    v.y = v1.y + f;
    return v;
}
template <typename T>
inline Point2<T> operator-(const Point2<T> &v1, const T &f)
{
    Point2<T> v;
    v.x = v1.x - f;
    v.y = v1.y - f;
    return v;
}
template <typename T>
inline Point2<T> operator*(const Point2<T> &v1, const Point2<T> &v2)
{
    Point2<T> v;
    v.x = v1.x * v2.x;
    v.y = v1.y * v2.y;
    return v;
}
template <typename T>
inline Point2<T> operator*(const Point2<T> &v1, const T &f)
{
    Point2<T> v;
    v.x = v1.x * f;
    v.y = v1.y * f;
    return v;
}

template <typename T>
inline Point2<T> min(const Point2<T> &p0, const Point2<T> &p1) { return Point2<T>(min(p0.x, p1.x), min(p0.y, p1.y)); }
template <typename T>
inline Point2<T> max(const Point2<T> &p0, const Point2<T> &p1) { return Point2<T>(max(p0.x, p1.x), max(p0.y, p1.y)); }

typedef Point2<float> Point2f;
typedef Point2<int> Point2i;

inline Point2f ceil(const Point2f &p)
{
    return Point2f(ceil(p.x), ceil(p.y));
}

inline Point2f floor(const Point2f &p)
{
    return Point2f(floor(p.x), floor(p.y));
}

//---POINT2 END---

//---MATRIX4X4 BEGIN---
struct SSE_ALIGNAS Matrix4x4
{
public:
    union {
        float m[16];
        float mn[4][4];
        float4 col[4];
    };
public:
    inline Matrix4x4()
    {
        //col 0
        m[0] = 1.0f;
        m[1] = 0.0f;
        m[2] = 0.0f;
        m[3] = 0.0f;
        //col 1
        m[4] = 0.0f;
        m[5] = 1.0f;
        m[6] = 0.0f;
        m[7] = 0.0f;
        //col 2
        m[8] = 0.0f;
        m[9] = 0.0f;
        m[10] = 1.0f;
        m[11] = 0.0f;
        //col 3
        m[12] = 0.0f;
        m[13] = 0.0f;
        m[14] = 0.0f;
        m[15] = 1.0f;
    }
    inline Matrix4x4(const float m0, const float m1, const float m2, const float m3, const float m4, const float m5, const float m6, const float m7, const float m8, const float m9, const float m10, const float m11, const float m12, const float m13, const float m14, const float m15)
    {
        //col 0
        m[0] = m0;
        m[1] = m1;
        m[2] = m2;
        m[3] = m3;
        //col 1
        m[4] = m4;
        m[5] = m5;
        m[6] = m6;
        m[7] = m7;
        //col 2
        m[8] = m8;
        m[9] = m9;
        m[10] = m10;
        m[11] = m11;
        //col 3
        m[12] = m12;
        m[13] = m13;
        m[14] = m14;
        m[15] = m15;
    }

    inline Matrix4x4(const float *v)
    {
        //col 0
        m[0] = v[0];
        m[1] = v[1];
        m[2] = v[2];
        m[3] = v[3];
        //col 1
        m[4] = v[4];
        m[5] = v[5];
        m[6] = v[6];
        m[7] = v[7];
        //col 2
        m[8] = v[8];
        m[9] = v[9];
        m[10] = v[10];
        m[11] = v[11];
        //col 3
        m[12] = v[12];
        m[13] = v[13];
        m[14] = v[14];
        m[15] = v[15];
    }
    inline Matrix4x4(const float4 &col0, const float4 &col1, const float4 &col2, const float4 &col3)
    {
        col[0] = col0;
        col[1] = col1;
        col[2] = col2;
        col[3] = col3;
    }

    inline const float &operator[](const int idx) const
    {
        assert(idx >= 0 && idx < 16);
        return m[idx];
    }
    inline float &operator[](const int idx)
    {
        assert(idx >= 0 && idx < 16);
        return m[idx];
    }
};

inline void store(const Matrix4x4 &m, float *array)
{
    memcpy(array, m.m, 16 * sizeof(float));
}

inline std::ostream &operator<<(std::ostream &out, const Matrix4x4 &v)
{
    out << '(';
    for (int i = 0; i < 16; ++i)
    {
        out << v.m[i];
        out << (((i + 1) == 16) ? ')' : ',');
    }
    return out;
}

inline bool operator==(const Matrix4x4 &A, const Matrix4x4 &B)
{

    auto bool_i0 = A.col[0] == B.col[0];
    auto bool_i1 = A.col[1] == B.col[1];
    auto bool_i2 = A.col[2] == B.col[2];
    auto bool_i3 = A.col[3] == B.col[3];

    return all((bool_i0 & bool_i1) & (bool_i2 & bool_i3));
}

inline bool operator!=(const Matrix4x4 &A, const Matrix4x4 &B)
{

    auto bool_i0 = A.col[0] == B.col[0];
    auto bool_i1 = A.col[1] == B.col[1];
    auto bool_i2 = A.col[2] == B.col[2];
    auto bool_i3 = A.col[3] == B.col[3];

    return not_all((bool_i0 & bool_i1) & (bool_i2 & bool_i3));
}

inline Matrix4x4 operator+(const Matrix4x4 &A, const Matrix4x4 &B)
{
    Matrix4x4 r;
    r.col[0] = A.col[0] + B.col[0];
    r.col[1] = A.col[1] + B.col[1];
    r.col[2] = A.col[2] + B.col[2];
    r.col[3] = A.col[3] + B.col[3];
    return r;
}

inline Matrix4x4 operator*(const Matrix4x4 &A, const Matrix4x4 &B)
{

    Matrix4x4 ret;

    float4 r = A.col[0] * B.m[0];
    r += A.col[1] * B.m[1];
    r += A.col[2] * B.m[2];
    r += A.col[3] * B.m[3];
    ret.col[0] = r.xyzw;

    r = A.col[0] * B.m[4];
    r += A.col[1] * B.m[5];
    r += A.col[2] * B.m[6];
    r += A.col[3] * B.m[7];
    ret.col[1] = r.xyzw;

    r = A.col[0] * B.m[8];
    r += A.col[1] * B.m[9];
    r += A.col[2] * B.m[10];
    r += A.col[3] * B.m[11];
    ret.col[2] = r.xyzw;

    r = A.col[0] * B.m[12];
    r += A.col[1] * B.m[13];
    r += A.col[2] * B.m[14];
    r += A.col[3] * B.m[15];
    ret.col[3] = r.xyzw;

    return ret;
}

//https://lxjk.github.io/2017/09/03/Fast-4x4-Matrix-Inverse-with-SSE-SIMD-Explained.html#_appendix_2
inline Matrix4x4 transform_inverse_noscale(const Matrix4x4 &mat)
{
    Matrix4x4 r;
    //transpose left-upper 3x3
    __m128 t0 = shuffle<0, 1, 0, 1>(mat.col[0], mat.col[1]); // 00,10,01,11
    __m128 t1 = shuffle<2, 3, 2, 3>(mat.col[0], mat.col[1]); // 20,30,21,31
    // mat.col[2] : 02,12,22,32
    r.col[0] = shuffle<0, 2, 0, 3>(t0, mat.col[2]); //00,01,02,32(=0)
    r.col[1] = shuffle<1, 3, 1, 3>(t0, mat.col[2]); //10,11,12,32(=0)
    r.col[2] = shuffle<0, 2, 2, 3>(t1, mat.col[2]); //20,21,22,32(=0)

    //translate
    r.col[3] = _mm_mul_ps(r.col[0], swizzle<0>(mat.col[3]));
    r.col[3] = _mm_add_ps(r.col[3], _mm_mul_ps(r.col[1], swizzle<1>(mat.col[3])));
    r.col[3] = _mm_add_ps(r.col[3], _mm_mul_ps(r.col[2], swizzle<2>(mat.col[3])));
    r.col[3] = _mm_sub_ps(_mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f), r.col[3]);

    return r;
}

inline Matrix4x4 transform_inverse(const Matrix4x4 &mat)
{
    Matrix4x4 r;
    //transpose left-upper 3x3
    __m128 t0 = shuffle<0, 1, 0, 1>(mat.col[0], mat.col[1]); // 00,10,01,11
    __m128 t1 = shuffle<2, 3, 2, 3>(mat.col[0], mat.col[1]); // 20,30,21,31
    // mat.col[2] : 02,12,22,32
    r.col[0] = shuffle<0, 2, 0, 3>(t0, mat.col[2]); //00,01,02,32(=0)
    r.col[1] = shuffle<1, 3, 1, 3>(t0, mat.col[2]); //10,11,12,32(=0)
    r.col[2] = shuffle<0, 2, 2, 3>(t1, mat.col[2]); //20,21,22,32(=0)

    //calculate sqr size
    __m128 sqrl = _mm_mul_ps(r.col[0], r.col[0]);
    sqrl = _mm_add_ps(sqrl, _mm_mul_ps(r.col[1], r.col[1]));
    sqrl = _mm_add_ps(sqrl, _mm_mul_ps(r.col[2], r.col[2]));

    //here need avoid divide by zero?
    //but I will remove it here,and add (0,0,0,1) to it
    sqrl = _mm_add_ps(sqrl, _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f));

    __m128 rsqrl = _mm_div_ps(_mm_set1_ps(1.0f), sqrl);

    r.col[0] = _mm_mul_ps(r.col[0], rsqrl);
    r.col[1] = _mm_mul_ps(r.col[1], rsqrl);
    r.col[2] = _mm_mul_ps(r.col[2], rsqrl);

    //translate
    r.col[3] = _mm_mul_ps(r.col[0], swizzle<0>(mat.col[3]));
    r.col[3] = _mm_add_ps(r.col[3], _mm_mul_ps(r.col[1], swizzle<1>(mat.col[3])));
    r.col[3] = _mm_add_ps(r.col[3], _mm_mul_ps(r.col[2], swizzle<2>(mat.col[3])));
    r.col[3] = _mm_sub_ps(_mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f), r.col[3]);

    return r;
}

static inline __m128 adj2x2(const __m128 m) { return _mm_mul_ps(swizzle<3, 2, 1, 0>(m), _mm_set_ps(1.0f, -1.0f, -1.0f, 1.0f)); }
static inline __m128 mul2x2(const __m128 m0, const __m128 m1) { return _mm_add_ps(_mm_mul_ps(swizzle<0, 1, 0, 1>(m0), swizzle<0, 0, 2, 2>(m1)), _mm_mul_ps(swizzle<2, 3, 2, 3>(m0), swizzle<1, 1, 3, 3>(m1))); }
static inline __m128 det2x2(const __m128 m) { return _mm_sub_ps(_mm_mul_ps(swizzle<0, 0, 0, 0>(m) /*a*/, swizzle<3, 3, 3, 3>(m) /*d*/), _mm_mul_ps(swizzle<1, 1, 1, 1>(m) /*c*/, swizzle<2, 2, 2, 2>(m) /*b*/)); }

//Blockwise inversion
//https://en.wikipedia.org/wiki/Invertible_matrix#Blockwise_inversion
inline Matrix4x4 blockwise_inverse(const Matrix4x4 &mat)
{
    Matrix4x4 r;
    //extract 4 sub-matrix2x2
    // | A B |
    // | C D |
    __m128 A = shuffle<0, 1, 0, 1>(mat.col[0], mat.col[1]);
    __m128 C = shuffle<2, 3, 2, 3>(mat.col[0], mat.col[1]);
    __m128 B = shuffle<0, 1, 0, 1>(mat.col[2], mat.col[3]);
    __m128 D = shuffle<2, 3, 2, 3>(mat.col[2], mat.col[3]);

    //2x2 det
    // h->  |D| |B| |C| |A| <-l
    // __m128 det=_mm_sub_ps(
    // _mm_mul_ps(shuffle<0,2,0,2>(mat.col[0],mat.col[2])/*a*/,shuffle<1,3,1,3>(mat.col[1],mat.col[3])/*d*/),
    // _mm_mul_ps(shuffle<1,3,1,3>(mat.col[0],mat.col[2])/*c*/,shuffle<0,2,0,2>(mat.col[1],mat.col[3])/*b*/)
    // );

    //2x2 det
    __m128 detA = det2x2(A);
    // __m128 detB = swizzle<2>(det);
    // __m128 detC = swizzle<1>(det);
    // __m128 detD = swizzle<3>(det);

    __m128 invA = _mm_div_ps(adj2x2(A), detA);
    __m128 D_CinvAB = _mm_sub_ps(D, mul2x2(mul2x2(C, invA), B));
    __m128 det_D_CinvAB = _mm_sub_ps(
        _mm_mul_ps(swizzle<0, 0, 0, 0>(D_CinvAB) /*a*/, swizzle<3, 3, 3, 3>(D_CinvAB) /*d*/),
        _mm_mul_ps(swizzle<1, 1, 1, 1>(D_CinvAB) /*c*/, swizzle<2, 2, 2, 2>(D_CinvAB) /*b*/));

    __m128 DD = _mm_div_ps(adj2x2(D_CinvAB), det_D_CinvAB);
    __m128 CinvA = _mm_mul_ps(C, invA);
    __m128 invAB = _mm_mul_ps(invA, B);
    __m128 invABinv_D_CinvAB = _mm_mul_ps(invAB, DD);
    __m128 BB = _mm_sub_ps(_mm_set1_ps(0.0f), invABinv_D_CinvAB);
    __m128 AA = _mm_add_ps(invA, _mm_mul_ps(invABinv_D_CinvAB, CinvA));
    __m128 CC = _mm_sub_ps(_mm_set1_ps(0.0f), _mm_mul_ps(DD, CinvA));

    r.col[0] = shuffle<0, 1, 0, 1>(AA, CC);
    r.col[1] = shuffle<2, 3, 2, 3>(AA, CC);
    r.col[2] = shuffle<0, 1, 0, 1>(BB, DD);
    r.col[3] = shuffle<2, 3, 2, 3>(BB, DD);

    return r;
}

inline Matrix4x4 transpose(const Matrix4x4 &mat)
{
    Matrix4x4 r(mat);
    _MM_TRANSPOSE4_PS(r.col[0], r.col[1], r.col[2], r.col[3]);
    return r;
}

//TODO need optimal
inline Matrix4x4 minor(const Matrix4x4 &mat)
{
    float4 m0 = swizzle<1, 0, 0, 0>(mat.col[1]) * swizzle<2, 2, 1, 1>(mat.col[2]) * swizzle<3, 3, 3, 2>(mat.col[3]);
    m0 = m0 + swizzle<2, 2, 1, 1>(mat.col[1]) * swizzle<3, 3, 3, 2>(mat.col[2]) * swizzle<1, 0, 0, 0>(mat.col[3]);
    m0 = m0 + swizzle<3, 3, 3, 2>(mat.col[1]) * swizzle<1, 0, 0, 0>(mat.col[2]) * swizzle<2, 2, 1, 1>(mat.col[3]);
    m0 = m0 - swizzle<3, 3, 3, 2>(mat.col[1]) * swizzle<2, 2, 1, 1>(mat.col[2]) * swizzle<1, 0, 0, 0>(mat.col[3]);
    m0 = m0 - swizzle<2, 2, 1, 1>(mat.col[1]) * swizzle<1, 0, 0, 0>(mat.col[2]) * swizzle<3, 3, 3, 2>(mat.col[3]);
    m0 = m0 - swizzle<1, 2, 1, 1>(mat.col[1]) * swizzle<3, 3, 3, 2>(mat.col[2]) * swizzle<2, 0, 0, 0>(mat.col[3]);

    float4 m1 = swizzle<1, 0, 0, 0>(mat.col[0]) * swizzle<2, 2, 1, 1>(mat.col[2]) * swizzle<3, 3, 3, 2>(mat.col[3]);
    m1 = m1 + swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<3, 3, 3, 2>(mat.col[2]) * swizzle<1, 0, 0, 0>(mat.col[3]);
    m1 = m1 + swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<1, 0, 0, 0>(mat.col[2]) * swizzle<2, 2, 1, 1>(mat.col[3]);
    m1 = m1 - swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<2, 2, 1, 1>(mat.col[2]) * swizzle<1, 0, 0, 0>(mat.col[3]);
    m1 = m1 - swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<1, 0, 0, 0>(mat.col[2]) * swizzle<3, 3, 3, 2>(mat.col[3]);
    m1 = m1 - swizzle<1, 2, 1, 1>(mat.col[0]) * swizzle<3, 3, 3, 2>(mat.col[2]) * swizzle<2, 0, 0, 0>(mat.col[3]);

    float4 m2 = swizzle<1, 0, 0, 0>(mat.col[0]) * swizzle<2, 2, 1, 1>(mat.col[1]) * swizzle<3, 3, 3, 2>(mat.col[3]);
    m2 = m2 + swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<3, 3, 3, 2>(mat.col[1]) * swizzle<1, 0, 0, 0>(mat.col[3]);
    m2 = m2 + swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<1, 0, 0, 0>(mat.col[1]) * swizzle<2, 2, 1, 1>(mat.col[3]);
    m2 = m2 - swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<2, 2, 1, 1>(mat.col[1]) * swizzle<1, 0, 0, 0>(mat.col[3]);
    m2 = m2 - swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<1, 0, 0, 0>(mat.col[1]) * swizzle<3, 3, 3, 2>(mat.col[3]);
    m2 = m2 - swizzle<1, 2, 1, 1>(mat.col[0]) * swizzle<3, 3, 3, 2>(mat.col[1]) * swizzle<2, 0, 0, 0>(mat.col[3]);

    float4 m3 = swizzle<1, 0, 0, 0>(mat.col[0]) * swizzle<2, 2, 1, 1>(mat.col[1]) * swizzle<3, 3, 3, 2>(mat.col[2]);
    m3 = m3 + swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<3, 3, 3, 2>(mat.col[1]) * swizzle<1, 0, 0, 0>(mat.col[2]);
    m3 = m3 + swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<1, 0, 0, 0>(mat.col[1]) * swizzle<2, 2, 1, 1>(mat.col[2]);
    m3 = m3 - swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<2, 2, 1, 1>(mat.col[1]) * swizzle<1, 0, 0, 0>(mat.col[2]);
    m3 = m3 - swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<1, 0, 0, 0>(mat.col[1]) * swizzle<3, 3, 3, 2>(mat.col[2]);
    m3 = m3 - swizzle<1, 2, 1, 1>(mat.col[0]) * swizzle<3, 3, 3, 2>(mat.col[1]) * swizzle<2, 0, 0, 0>(mat.col[2]);
    return Matrix4x4(m0, m1, m2, m3);
}

inline Matrix4x4 cofactor(const Matrix4x4 &mat)
{
    float4 m0 = swizzle<1, 0, 0, 0>(mat.col[1]) * swizzle<2, 3, 1, 2>(mat.col[2]) * swizzle<3, 2, 3, 1>(mat.col[3]);
    m0 = m0 + swizzle<2, 2, 1, 1>(mat.col[1]) * swizzle<3, 0, 3, 0>(mat.col[2]) * swizzle<1, 3, 0, 2>(mat.col[3]);
    m0 = m0 + swizzle<3, 3, 3, 2>(mat.col[1]) * swizzle<1, 2, 0, 1>(mat.col[2]) * swizzle<2, 0, 1, 0>(mat.col[3]);
    m0 = m0 - swizzle<3, 3, 3, 2>(mat.col[1]) * swizzle<2, 0, 1, 0>(mat.col[2]) * swizzle<1, 2, 0, 1>(mat.col[3]);
    m0 = m0 - swizzle<2, 2, 1, 1>(mat.col[1]) * swizzle<1, 3, 0, 2>(mat.col[2]) * swizzle<3, 0, 3, 0>(mat.col[3]);
    m0 = m0 - swizzle<1, 0, 1, 0>(mat.col[1]) * swizzle<3, 2, 3, 1>(mat.col[2]) * swizzle<2, 3, 0, 2>(mat.col[3]);

    float4 m1 = swizzle<1, 0, 0, 0>(mat.col[0]) * swizzle<3, 2, 3, 1>(mat.col[2]) * swizzle<2, 3, 1, 2>(mat.col[3]);
    m1 = m1 + swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<1, 3, 0, 2>(mat.col[2]) * swizzle<3, 0, 3, 0>(mat.col[3]);
    m1 = m1 + swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<2, 0, 1, 0>(mat.col[2]) * swizzle<1, 2, 0, 1>(mat.col[3]);
    m1 = m1 - swizzle<1, 0, 0, 0>(mat.col[0]) * swizzle<2, 3, 1, 2>(mat.col[2]) * swizzle<3, 2, 3, 1>(mat.col[3]);
    m1 = m1 - swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<3, 0, 3, 0>(mat.col[2]) * swizzle<1, 3, 0, 2>(mat.col[3]);
    m1 = m1 - swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<1, 2, 0, 1>(mat.col[2]) * swizzle<2, 0, 1, 0>(mat.col[3]);

    float4 m2 = swizzle<1, 0, 0, 0>(mat.col[0]) * swizzle<2, 3, 1, 2>(mat.col[1]) * swizzle<3, 2, 3, 1>(mat.col[3]);
    m2 = m2 + swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<3, 0, 3, 0>(mat.col[1]) * swizzle<1, 3, 0, 2>(mat.col[3]);
    m2 = m2 + swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<1, 2, 0, 1>(mat.col[1]) * swizzle<2, 0, 1, 0>(mat.col[3]);
    m2 = m2 - swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<2, 0, 1, 0>(mat.col[1]) * swizzle<1, 2, 0, 1>(mat.col[3]);
    m2 = m2 - swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<1, 3, 0, 2>(mat.col[1]) * swizzle<3, 0, 3, 0>(mat.col[3]);
    m2 = m2 - swizzle<1, 0, 1, 0>(mat.col[0]) * swizzle<3, 2, 3, 1>(mat.col[1]) * swizzle<2, 3, 0, 2>(mat.col[3]);

    float4 m3 = swizzle<1, 0, 0, 0>(mat.col[0]) * swizzle<3, 2, 3, 1>(mat.col[1]) * swizzle<2, 3, 1, 2>(mat.col[2]);
    m3 = m3 + swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<1, 3, 0, 2>(mat.col[1]) * swizzle<3, 0, 3, 0>(mat.col[2]);
    m3 = m3 + swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<2, 0, 1, 0>(mat.col[1]) * swizzle<1, 2, 0, 1>(mat.col[2]);
    m3 = m3 - swizzle<1, 0, 0, 0>(mat.col[0]) * swizzle<2, 3, 1, 2>(mat.col[1]) * swizzle<3, 2, 3, 1>(mat.col[2]);
    m3 = m3 - swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<3, 0, 3, 0>(mat.col[1]) * swizzle<1, 3, 0, 2>(mat.col[2]);
    m3 = m3 - swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<1, 2, 0, 1>(mat.col[1]) * swizzle<2, 0, 1, 0>(mat.col[2]);
    return Matrix4x4(m0, m1, m2, m3);
}

inline float determinant(const Matrix4x4 &mat)
{
    auto cofactor_mat = cofactor(mat);
    return reduce_add(float4(mat.col[0]) * float4(cofactor_mat.col[0]));
}

inline float sub_matrix3x3_determinant(const Matrix4x4 &mat)
{
    auto temp = mat.col[0] * swizzle<1, 2, 0, 3>(mat.col[1]) * swizzle<2, 0, 1, 3>(mat.col[2]) - swizzle<2, 1, 0, 3>(mat.col[0]) * swizzle<1, 0, 2, 3>(mat.col[1]) * swizzle<0, 2, 1, 3>(mat.col[1]);
    float4 a = temp;
    float4 b = swizzle<1>(temp);
    float4 c = swizzle<2>(temp);
    return _mm_cvtss_f32(a + b + c);
}

//Cramer's rule
inline Matrix4x4 inverse(const Matrix4x4 &mat)
{
    auto tran_mat = transpose(mat);
    auto cofactor_mat = cofactor(tran_mat);
    auto det = vreduce_add(float4(tran_mat.col[0]) * float4(cofactor_mat.col[0]));
    auto inv_det = rcp(det);
    cofactor_mat.col[0] = cofactor_mat.col[0] * inv_det;
    cofactor_mat.col[1] = cofactor_mat.col[1] * inv_det;
    cofactor_mat.col[2] = cofactor_mat.col[2] * inv_det;
    cofactor_mat.col[3] = cofactor_mat.col[3] * inv_det;
    return cofactor_mat;
}

//---MATRIX4X4 END---

//---GENERAL BEGIN---
template <typename T>
inline Point3<T> operator+(const Point3<T> &p, const Vector3<T> &v)
{
    Point3<T> rp;
    rp.x = p.x + v.x;
    rp.y = p.y + v.y;
    rp.z = p.z + v.z;
    return rp;
}
template <typename T>
inline Point3<T> operator-(const Point3<T> &p, const Vector3<T> &v) { return p + (-v); }
template <typename T>
inline Vector3<T> operator-(const Point3<T> &p1, const Point3<T> &p2)
{
    Vector3<T> p;
    p.x = p1.x - p2.x;
    p.y = p1.y - p2.y;
    p.z = p1.z - p2.z;
    return p;
}

inline SSEVector3f operator+(const SSEPoint3f &p, const SSEVector3f &v) { return p.xyzw + v.xyzw; }
inline SSEVector3f operator-(const SSEPoint3f &p, const SSEVector3f &v) { return p.xyzw - v.xyzw; }
inline SSEVector3f operator-(const SSEPoint3f &p1, const SSEPoint3f &p2) { return p1.xyzw - p2.xyzw; }

inline float distance(const Point3f &p1, const Point3f &p2) { return length(p2 - p1); }

inline float distance(const SSEPoint3f &p1, const SSEPoint3f &p2) { return length(p2 - p1); }

template <typename T>
inline Point2<T> operator+(const Point2<T> &p, const Vector2<T> &v)
{
    Point2<T> rp;
    rp.x = p.x + v.x;
    rp.y = p.y + v.y;
    return rp;
}
template <typename T>
inline Point2<T> operator-(const Point2<T> &p, const Vector2<T> &v)
{
    Point2<T> rp;
    rp.x = p.x - v.x;
    rp.y = p.y - v.y;
    return rp;
}

inline Vector3f operator*(const Matrix4x4 &M, const Vector3f &v)
{
    // 8ns
    // float4 r=float4(M.col[0])*float4(v.x);
    // r+=float4(M.col[1])*float4(v.y);
    // r+=float4(M.col[2])*float4(v.z);
    // return Vector3f(r.x,r.y,r.z);

    // 7ns
    float x = M.m[0] * v.x + M.m[4] * v.y + M.m[8] * v.z;
    float y = M.m[1] * v.x + M.m[5] * v.y + M.m[9] * v.z;
    float z = M.m[2] * v.x + M.m[6] * v.y + M.m[10] * v.z;
    return Vector3f(x, y, z);
}

inline Point3f operator*(const Matrix4x4 &M, const Point3f &v)
{
    float4 r = M.col[0] * v.x;
    r += M.col[1] * v.y;
    r += M.col[2] * v.z;
    r += M.col[3] * 1.0f;
    return Point3f(r.x, r.y, r.z);
}

inline Point3f mul(const Matrix4x4 &M, const Point3f &v)
{
    return M * v;
}

//general
inline Point3f mul_h(const Matrix4x4 &M, const Point3f &v)
{
    float4 r = M.col[0] * v.x;
    r += M.col[1] * v.y;
    r += M.col[2] * v.z;
    r += M.col[3] * 1.0f;
    float inv_w = 1.0f / r.w;
    return Point3f(r.x * inv_w, r.y * inv_w, r.z * inv_w);
}

inline SSEVector3f operator*(const Matrix4x4 &M, const SSEVector3f &v)
{
    float4 vx = swizzle<0, 0, 0, 0>(v.xyzw);
    float4 vy = swizzle<1, 1, 1, 1>(v.xyzw);
    float4 vz = swizzle<2, 2, 2, 2>(v.xyzw);

    float4 r = float4(M.col[0]) * vx;
    r += float4(M.col[1]) * vy;
    r += float4(M.col[2]) * vz;
    return SSEVector3f(r.xyzw);
}

inline SSEPoint3f operator*(const Matrix4x4 &M, const SSEPoint3f &v)
{
    float4 vx = swizzle<0, 0, 0, 0>(v.xyzw);
    float4 vy = swizzle<1, 1, 1, 1>(v.xyzw);
    float4 vz = swizzle<2, 2, 2, 2>(v.xyzw);

    float4 r = float4(M.col[0]) * vx;
    r += float4(M.col[1]) * vy;
    r += float4(M.col[2]) * vz;
    r += float4(M.col[3]);
    return SSEPoint3f(r.xyzw);
}

inline Vector3f4p operator*(const Matrix4x4 &M, const Vector3f4p &v)
{
    float4 r_xxxx = swizzle<0, 0, 0, 0>(M.col[0]) /*m00*/ * v.xxxx;
    r_xxxx = r_xxxx + swizzle<0, 0, 0, 0>(M.col[1]) /*m10*/ * v.yyyy;
    r_xxxx = r_xxxx + swizzle<0, 0, 0, 0>(M.col[2]) /*m20*/ * v.zzzz;

    float4 r_yyyy = swizzle<1, 1, 1, 1>(M.col[0]) /*m01*/ * v.xxxx;
    r_yyyy = r_yyyy + swizzle<1, 1, 1, 1>(M.col[1]) /*m11*/ * v.yyyy;
    r_yyyy = r_yyyy + swizzle<1, 1, 1, 1>(M.col[2]) /*m21*/ * v.zzzz;

    float4 r_zzzz = swizzle<2, 2, 2, 2>(M.col[0]) /*m02*/ * v.xxxx;
    r_zzzz = r_zzzz + swizzle<2, 2, 2, 2>(M.col[1]) /*m12*/ * v.yyyy;
    r_zzzz = r_zzzz + swizzle<2, 2, 2, 2>(M.col[2]) /*m22*/ * v.zzzz;

    return Vector3f4p(r_xxxx, r_yyyy, r_zzzz);
}

inline Point3f4p operator*(const Matrix4x4 &M, const Point3f4p &v)
{
    float4 r_xxxx = swizzle<0, 0, 0, 0>(M.col[0]) /*m00*/ * v.xxxx;
    r_xxxx = r_xxxx + swizzle<0, 0, 0, 0>(M.col[1]) /*m10*/ * v.yyyy;
    r_xxxx = r_xxxx + swizzle<0, 0, 0, 0>(M.col[2]) /*m20*/ * v.zzzz;
    r_xxxx = r_xxxx + swizzle<0, 0, 0, 0>(M.col[3]);

    float4 r_yyyy = swizzle<1, 1, 1, 1>(M.col[0]) /*m01*/ * v.xxxx;
    r_yyyy = r_yyyy + swizzle<1, 1, 1, 1>(M.col[1]) /*m11*/ * v.yyyy;
    r_yyyy = r_yyyy + swizzle<1, 1, 1, 1>(M.col[2]) /*m21*/ * v.zzzz;
    r_yyyy = r_yyyy + swizzle<1, 1, 1, 1>(M.col[3]);

    float4 r_zzzz = swizzle<2, 2, 2, 2>(M.col[0]) /*m02*/ * v.xxxx;
    r_zzzz = r_zzzz + swizzle<2, 2, 2, 2>(M.col[1]) /*m12*/ * v.yyyy;
    r_zzzz = r_zzzz + swizzle<2, 2, 2, 2>(M.col[2]) /*m22*/ * v.zzzz;
    r_zzzz = r_zzzz + swizzle<2, 2, 2, 2>(M.col[3]);

    return Point3f4p(r_xxxx, r_yyyy, r_zzzz);
}

inline Vector3f4p operator-(const Point3f4p &p0, const Point3f4p &p1)
{
    auto xxxx = p0.xxxx - p1.xxxx;
    auto yyyy = p0.yyyy - p1.yyyy;
    auto zzzz = p0.zzzz - p1.zzzz;
    return Vector3f4p(xxxx, yyyy, zzzz);
}
inline Normal3f flip_normal(const Normal3f &n, const Vector3f &wo)
{
    return dot(n, wo) > 0 ? n : -n;
}
//---GENERAL END---
NARUKAMI_END
