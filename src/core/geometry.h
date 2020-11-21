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
#include "core/matrix.h"
#include "core/simd.h"
#include <vector>
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
inline Vector3<T> operator+(const Vector3<T> &v1, const Vector3<T> &v2)
{
    Vector3<T> v;
    v.x = v1.x + v2.x;
    v.y = v1.y + v2.y;
    v.z = v1.z + v2.z;
    return v;
}

template <typename T>
inline Vector3<T> operator-(const Vector3<T> &v1, const Vector3<T> &v2)
{
    Vector3<T> v;
    v.x = v1.x - v2.x;
    v.y = v1.y - v2.y;
    v.z = v1.z - v2.z;
    return v;
}

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
inline Vector3<T> operator*(const T &f, const Vector3<T> &v1)
{
    return v1 * f;
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
    union
    {
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
struct SSE_ALIGNAS Vector3fPack
{
    union
    {
        float4 xxxx;
        struct
        {
            float x0, x1, x2, x3;
        };
    };

    union
    {
        float4 yyyy;
        struct
        {
            float y0, y1, y2, y3;
        };
    };

    union
    {
        float4 zzzz;
        struct
        {
            float z0, z1, z2, z3;
        };
    };

    inline Vector3fPack() : xxxx(0.0f), yyyy(0.0f), zzzz(0.0f) {}
    inline explicit Vector3fPack(const float a) : xxxx(a), yyyy(a), zzzz(a) { assert(!isnan(a)); }
    inline Vector3fPack(const Vector3f &v0, const Vector3f &v1, const Vector3f &v2, const Vector3f &v3) : xxxx(v0.x, v1.x, v2.x, v3.x), yyyy(v0.y, v1.y, v2.y, v3.y), zzzz(v0.z, v1.z, v2.z, v3.z) {}
    inline explicit Vector3fPack(const Vector3f &v) : xxxx(v.x), yyyy(v.y), zzzz(v.z) {}
    inline Vector3fPack(const float4 &x, const float4 &y, const float4 &z) : xxxx(x), yyyy(y), zzzz(z) {}
    inline Vector3fPack(const float x, const float y, const float z) : xxxx(x), yyyy(y), zzzz(z) {}

    inline Vector3f operator[](const int idx) const
    {
        assert(idx >= 0 && idx < SSE_WIDTH);
        return Vector3f(xxxx[idx], yyyy[idx], zzzz[idx]);
    }
};

inline std::ostream &operator<<(std::ostream &out, const Vector3fPack &v)
{
    out << '(' << v.x0 << ',' << v.y0 << ',' << v.z0 << ')';
    out << '(' << v.x1 << ',' << v.y1 << ',' << v.z1 << ')';
    out << '(' << v.x2 << ',' << v.y2 << ',' << v.z2 << ')';
    out << '(' << v.x3 << ',' << v.y3 << ',' << v.z3 << ')';
    return out;
}

inline int operator==(const Vector3fPack &v0, const Vector3fPack &v1)
{
    bool4 mask_xxxx = (v0.xxxx == v1.xxxx);
    bool4 mask_yyyy = (v0.yyyy == v1.yyyy);
    bool4 mask_zzzz = (v0.zzzz == v1.zzzz);

    return movemask((mask_xxxx & mask_yyyy) & mask_zzzz);
}
inline int operator!=(const Vector3fPack &v0, const Vector3fPack &v1)
{
    bool4 mask_xxxx = (v0.xxxx != v1.xxxx);
    bool4 mask_yyyy = (v0.yyyy != v1.yyyy);
    bool4 mask_zzzz = (v0.zzzz != v1.zzzz);
    return movemask((mask_xxxx | mask_yyyy) | mask_zzzz);
}

inline Vector3fPack operator-(const Vector3fPack &v)
{
    return Vector3fPack(-v.xxxx, -v.yyyy, -v.zzzz);
}

inline Vector3fPack operator*(const Vector3fPack &v, float4 c)
{
    float4 xxxx = v.xxxx * c;
    float4 yyyy = v.yyyy * c;
    float4 zzzz = v.zzzz * c;
    return Vector3fPack(xxxx, yyyy, zzzz);
}

inline Vector3fPack operator/(float4 c, const Vector3fPack &v)
{
    Vector3fPack vv;
    vv.xxxx = c / v.xxxx;
    vv.yyyy = c / v.yyyy;
    vv.zzzz = c / v.zzzz;
    return vv;
}

inline float4 dot(const Vector3fPack &v0, const Vector3fPack &v1) { return v0.xxxx * v1.xxxx + v0.yyyy * v1.yyyy + v0.zzzz * v1.zzzz; }

inline Vector3fPack cross(const Vector3fPack &v0, const Vector3fPack &v1)
{
    float4 xxxx = v0.yyyy * v1.zzzz - v0.zzzz * v1.yyyy;
    float4 yyyy = v0.zzzz * v1.xxxx - v0.xxxx * v1.zzzz;
    float4 zzzz = v0.xxxx * v1.yyyy - v0.yyyy * v1.xxxx;
    return Vector3fPack(xxxx, yyyy, zzzz);
}

inline Vector3fPack normalize(const Vector3fPack &v)
{
    float4 sqr = dot(v, v);
    return v * rsqrt(sqr);
}

inline Vector3fPack rcp(const Vector3fPack &v)
{
    Vector3fPack vv;
    vv.xxxx = rcp(v.xxxx);
    vv.yyyy = rcp(v.yyyy);
    vv.zzzz = rcp(v.zzzz);
    return vv;
}

inline Vector3fPack safe_rcp(const Vector3fPack &v)
{
    Vector3fPack vv;
    vv.xxxx = safe_rcp(v.xxxx);
    vv.yyyy = safe_rcp(v.yyyy);
    vv.zzzz = safe_rcp(v.zzzz);
    return vv;
}

inline Vector3fPack load(const Vector3f *vector_array) { return Vector3fPack(vector_array[0], vector_array[1], vector_array[2], vector_array[3]); }
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

template <typename T>
inline Point3<T> operator+(const Vector3<T> &v1, const Point3<T> &v2)
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
inline Point3<T> &operator+=(Point3<T> &v, float a)
{
    v.x += a;
    v.y += a;
    v.z += a;
    return v;
}
template <typename T>
inline Point3<T> &operator-=(Point3<T> &v, float a)
{
    v.x -= a;
    v.y -= a;
    v.z -= a;
    return v;
}
template <typename T>
inline Point3<T> &operator+=(Point3<T> &v, const Point3<T> &v1)
{
    v.x += v1.x;
    v.y += v1.y;
    v.z += v1.z;
    return v;
}
template <typename T>
inline Point3<T> &operator-=(Point3<T> &v, const Point3<T> &v1)
{
    v.x -= v1.x;
    v.y -= v1.y;
    v.z -= v1.z;
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
inline Point3<T> abs(const Point3<T> &v1) { return Point3<T>(abs(v1.x), abs(v1.y), abs(v1.z)); }
template <typename T>
inline Point3<T> sign(const Point3<T> &v1) { return Point3<T>(sign(v1.x), sign(v1.y), sign(v1.z)); }

template <typename T>
inline Point3f sqrt(const Point3<T> &v1) { return Point3f(sqrt(v1.x), sqrt(v1.y), sqrt(v1.z)); }
template <typename T>
inline Point3f rsqrt(const Point3<T> &v1) { return Point3f(rsqrt(v1.x), rsqrt(v1.y), rsqrt(v1.z)); }

template <typename T>
inline Point3<T> min(const Point3<T> &p0, const Point3<T> &p1) { return Point3<T>(min(p0.x, p1.x), min(p0.y, p1.y), min(p0.z, p1.z)); }
template <typename T>
inline Point3<T> max(const Point3<T> &p0, const Point3<T> &p1) { return Point3<T>(max(p0.x, p1.x), max(p0.y, p1.y), max(p0.z, p1.z)); }

template <typename T>
inline Point3<T> lerp(const Point3<T> &p0, const Point3<T> &p1, float a)
{
    float x = lerp(p0.x, p1.x, a);
    float y = lerp(p0.y, p1.y, a);
    float z = lerp(p0.z, p1.z, a);
    return Point3<T>(x, y, z);
}

//--- [SSE] ---
//16 bit
struct SSE_ALIGNAS SSEPoint3f
{
    union
    {
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
struct SSE_ALIGNAS Point3fPack
{
    union
    {
        float4 xxxx;
        struct
        {
            float x0, x1, x2, x3;
        };
    };

    union
    {
        float4 yyyy;
        struct
        {
            float y0, y1, y2, y3;
        };
    };

    union
    {
        float4 zzzz;
        struct
        {
            float z0, z1, z2, z3;
        };
    };

    typedef float Scalar;

    inline Point3fPack() : xxxx(0.0f), yyyy(0.0f), zzzz(0.0f) {}
    inline explicit Point3fPack(const float a) : xxxx(a), yyyy(a), zzzz(a) { assert(!isnan(a)); }
    inline Point3fPack(const Point3f &v0, const Point3f &v1, const Point3f &v2, const Point3f &v3) : xxxx(v0.x, v1.x, v2.x, v3.x), yyyy(v0.y, v1.y, v2.y, v3.y), zzzz(v0.z, v1.z, v2.z, v3.z) {}
    inline explicit Point3fPack(const Point3f &v) : xxxx(v.x), yyyy(v.y), zzzz(v.z) {}
    inline Point3fPack(const float4 &x, const float4 &y, const float4 &z) : xxxx(x), yyyy(y), zzzz(z) {}
    inline Point3fPack(const float x, const float y, const float z) : xxxx(x), yyyy(y), zzzz(z) {}

    inline Point3f operator[](const int idx) const
    {
        assert(idx >= 0 && idx < SSE_WIDTH);
        return Point3f(xxxx[idx], yyyy[idx], zzzz[idx]);
    }
};

inline std::ostream &operator<<(std::ostream &out, const Point3fPack &v)
{
    out << '(' << v.x0 << ',' << v.y0 << ',' << v.z0 << ')';
    out << '(' << v.x1 << ',' << v.y1 << ',' << v.z1 << ')';
    out << '(' << v.x2 << ',' << v.y2 << ',' << v.z2 << ')';
    out << '(' << v.x3 << ',' << v.y3 << ',' << v.z3 << ')';
    return out;
}

inline int operator==(const Point3fPack &v0, const Point3fPack &v1)
{

    bool4 mask_xxxx = (v0.xxxx == v1.xxxx);
    bool4 mask_yyyy = (v0.yyyy == v1.yyyy);
    bool4 mask_zzzz = (v0.zzzz == v1.zzzz);

    return movemask((mask_xxxx & mask_yyyy) & mask_zzzz);
}
inline int operator!=(const Point3fPack &v0, const Point3fPack &v1)
{
    bool4 mask_xxxx = (v0.xxxx != v1.xxxx);
    bool4 mask_yyyy = (v0.yyyy != v1.yyyy);
    bool4 mask_zzzz = (v0.zzzz != v1.zzzz);
    return movemask((mask_xxxx | mask_yyyy) | mask_zzzz);
}

inline Point3fPack min(const Point3fPack &p0, const Point3fPack &p1)
{
    return Point3fPack(min(p0.xxxx, p1.xxxx), min(p0.yyyy, p1.yyyy), min(p0.zzzz, p1.zzzz));
}

inline Point3fPack max(const Point3fPack &p0, const Point3fPack &p1)
{
    return Point3fPack(max(p0.xxxx, p1.xxxx), max(p0.yyyy, p1.yyyy), max(p0.zzzz, p1.zzzz));
}

inline Point3fPack load(const Point3f *point_array)
{
    return Point3fPack(point_array[0], point_array[1], point_array[2], point_array[3]);
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
inline T dot(const Point3<T> &v1, const Normal3<T> &v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }
template <typename T>
inline T dot(const Normal3<T> &v1, const Point3<T> &v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }
template <typename T>
inline Vector3<T> cross(const Normal3<T> &v1, const Vector3<T> &v2) { return Vector3<T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x); }
template <typename T>
inline Vector3<T> cross(const Vector3<T> &v1, const Normal3<T> &v2) { return Vector3<T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x); }
template <typename T>
inline Normal3f normalize(const Normal3<T> &v1)
{
    float inv_l = rsqrt(dot(v1, v1));
    return v1 * inv_l;
}
template <typename T>
inline float length(const Normal3<T> &v) { return sqrt(static_cast<float>(dot(v, v))); }

struct SSE_ALIGNAS Normal3fPack
{
    union
    {
        float4 xxxx;
        struct
        {
            float x0, x1, x2, x3;
        };
    };

    union
    {
        float4 yyyy;
        struct
        {
            float y0, y1, y2, y3;
        };
    };

    union
    {
        float4 zzzz;
        struct
        {
            float z0, z1, z2, z3;
        };
    };

    typedef float Scalar;

    inline Normal3fPack() : xxxx(0.0f), yyyy(0.0f), zzzz(0.0f) {}
    inline explicit Normal3fPack(const float a) : xxxx(a), yyyy(a), zzzz(a) { assert(!isnan(a)); }
    inline Normal3fPack(const Normal3f &v0, const Normal3f &v1, const Normal3f &v2, const Normal3f &v3) : xxxx(v0.x, v1.x, v2.x, v3.x), yyyy(v0.y, v1.y, v2.y, v3.y), zzzz(v0.z, v1.z, v2.z, v3.z) {}
    inline explicit Normal3fPack(const Normal3f &v) : xxxx(v.x), yyyy(v.y), zzzz(v.z) {}
    inline Normal3fPack(const float4 &x, const float4 &y, const float4 &z) : xxxx(x), yyyy(y), zzzz(z) {}
    inline Normal3fPack(const float x, const float y, const float z) : xxxx(x), yyyy(y), zzzz(z) {}

    inline Normal3f operator[](const int idx) const
    {
        assert(idx >= 0 && idx < SSE_WIDTH);
        return Normal3f(xxxx[idx], yyyy[idx], zzzz[idx]);
    }
};

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
    inline Point2(const Point3<T> &p) : x(p.x), y(p.y) {}
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
inline Point2<T> operator+(const Vector2<T> &v1, const Point2<T> &v2)
{
    Point2<T> v;
    v.x = v1.x + v2.x;
    v.y = v1.y + v2.y;
    return v;
}

template <typename T>
inline Point2<T> operator+(const Point2<T> &v1, const Vector2<T> &v2)
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
inline Point2<T> abs(const Point2<T> &v1) { return Point2(abs(v1.x), abs(v1.y), abs(v1.z)); }

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
void polar_decompose(const Matrix4x4 &m, Vector3f *T, Quaternion *Q, Matrix4x4 *S);

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
inline Point2<T> operator-(const Point2<T> &p, const Vector2<T> &v)
{
    Point2<T> rp;
    rp.x = p.x - v.x;
    rp.y = p.y - v.y;
    return rp;
}

template <typename T>
inline Vector2<T> operator-(const Point2<T> &p0, const Point2<T> &p1)
{
    Vector2<T> v;
    v.x = p0.x - p1.x;
    v.y = p0.y - p1.y;
    return v;
}

//matrix3x4
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
//matrix3x4
inline Point3f operator*(const Matrix4x4 &M, const Point3f &v)
{
    float4 r = M.col[0] * v.x;
    r += M.col[1] * v.y;
    r += M.col[2] * v.z;
    r += M.col[3] * 1.0f;
    return Point3f(r.x, r.y, r.z);
}

inline Point3f mul_3x4(const Matrix4x4 &M, const Point3f &v)
{
    return M * v;
}

//general
inline Point3f mul_4x4(const Matrix4x4 &M, const Point3f &v)
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

inline Vector3fPack operator*(const Matrix4x4 &M, const Vector3fPack &v)
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

    return Vector3fPack(r_xxxx, r_yyyy, r_zzzz);
}

inline Point3fPack operator*(const Matrix4x4 &M, const Point3fPack &v)
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

    return Point3fPack(r_xxxx, r_yyyy, r_zzzz);
}

inline Vector3fPack operator-(const Point3fPack &p0, const Point3fPack &p1)
{
    auto xxxx = p0.xxxx - p1.xxxx;
    auto yyyy = p0.yyyy - p1.yyyy;
    auto zzzz = p0.zzzz - p1.zzzz;
    return Vector3fPack(xxxx, yyyy, zzzz);
}

inline Point3fPack operator+(const Point3fPack &p0, const Vector3fPack &v)
{
    auto xxxx = p0.xxxx + v.xxxx;
    auto yyyy = p0.yyyy + v.yyyy;
    auto zzzz = p0.zzzz + v.zzzz;
    return Point3fPack(xxxx, yyyy, zzzz);
}

inline Point3fPack operator-(const Point3fPack &p0, const Vector3fPack &v)
{
    auto xxxx = p0.xxxx - v.xxxx;
    auto yyyy = p0.yyyy - v.yyyy;
    auto zzzz = p0.zzzz - v.zzzz;
    return Point3fPack(xxxx, yyyy, zzzz);
}

template <typename T, typename U>
inline T hemisphere_flip(const T &n, const U &wo)
{
    return dot(n, wo) > 0 ? n : -n;
}

template <typename T>
void coordinate_system(const Vector3<T> &v0, Vector3<T> *v1, Vector3<T> *v2)
{
    if (abs(v0.x) > abs(v0.y))
    {
        *v1 = normalize(Vector3<T>(-v0.z, 0, v0.x));
    }
    else
    {
        *v1 = normalize(Vector3<T>(0, v0.z, -v0.y));
    }

    *v2 = cross(v0, *v1);
}

template <typename T>
void coordinate_system(const Normal3<T> &v0, Vector3<T> *v1, Vector3<T> *v2)
{
    if (abs(v0.x) > abs(v0.y))
    {
        *v1 = normalize(Vector3<T>(-v0.z, 0, v0.x));
    }
    else
    {
        *v1 = normalize(Vector3<T>(0, v0.z, -v0.y));
    }

    *v2 = cross(v0, *v1);
}

struct Ray
{
    Point3f o;
    Vector3f d;
    mutable float t_max;
    float time;

    inline Ray() : o(Point3f(0, 0, 0)), d(Vector3f(0, 0, 1)), t_max(INFINITE), time(0.0f) {}
    inline Ray(const Point3f &o, const Vector3f &d, const float t_max = INFINITE) : o(o), d(d), t_max(t_max), time(0.0f) {}
};
inline std::ostream &operator<<(std::ostream &out, const Ray &ray)
{
    out << "[o:" << ray.o << " d:" << ray.d << " t:" << ray.t_max << "]";
    return out;
}

// Normal points outward for rays exiting the surface, else is flipped
// "Realtime Ray Tracing Gems" chapter 6
inline Ray offset_ray(const Ray &ray, const Normal3f &n)
{
    constexpr float origin = 1.0f / 32.0f;
    constexpr float int_scale = 256.0f;
    constexpr float float_scale = 1.0f / 65536.0f;
    Point3i of_i(static_cast<int>(int_scale * n.x), static_cast<int>(int_scale * n.y), static_cast<int>(int_scale * n.z));
    Point3f p_i(cast_i2f(cast_f2i(ray.o.x) + ((ray.o.x < 0) ? -of_i.x : of_i.x)),
                cast_i2f(cast_f2i(ray.o.y) + ((ray.o.y < 0) ? -of_i.y : of_i.y)),
                cast_i2f(cast_f2i(ray.o.z) + ((ray.o.z < 0) ? -of_i.z : of_i.z)));
    Point3f o_offseted(fabsf(ray.o.x) < origin ? ray.o.x + float_scale * n.x : p_i.x,
                       fabsf(ray.o.y) < origin ? ray.o.y + float_scale * n.y : p_i.y,
                       fabsf(ray.o.z) < origin ? ray.o.z + float_scale * n.z : p_i.z);
    float epsion = length(o_offseted - ray.o);
    return Ray(o_offseted, ray.d, max(0.0f, ray.t_max - epsion));
}

struct SSE_ALIGNAS RayPack
{
    Point3fPack o;
    Vector3fPack d;
    mutable float4 t_max;

    inline RayPack(const Point3f &o, const Vector3f &d, const float t_max = INFINITE) : o(Point3fPack(o)), d(Vector3fPack(d)), t_max(t_max) {}
    inline explicit RayPack(const Ray &ray) : o(ray.o), d(ray.d), t_max(ray.t_max) {}
};
inline std::ostream &operator<<(std::ostream &out, const RayPack &ray)
{
    out << "[o:" << ray.o << " d:" << ray.d << " t:" << float4(ray.t_max) << "]";
    return out;
}

struct RayDifferential : public Ray
{
    Point3f ox, oy;
    Vector3f dx, dy;

    inline RayDifferential() : Ray() {}
    inline RayDifferential(const Point3f &o, const Vector3f &d, const float t_max = INFINITE) : Ray(o, d, t_max) {}
};

inline std::ostream &operator<<(std::ostream &out, const RayDifferential &ray)
{
    out << "[o:" << ray.o << " d:" << ray.d << " t:" << float4(ray.t_max) << " ox:" << ray.ox << " dx:" << ray.dx << " oy:" << ray.oy << " dy:" << ray.dy << "]";
    return out;
}

struct Triangle
{
    Point3f v0;
    Vector3f e1;
    Vector3f e2;
};

inline Normal3f get_unnormalized_normal(const Triangle &tri)
{
    return cross(tri.e1, tri.e2);
}

inline Normal3f get_normalized_normal(const Triangle &tri)
{
    return normalize(cross(tri.e1, tri.e2));
}

inline Point3f get_vertex(const Triangle &tri, const Point2f &barycentric)
{
    return tri.v0 + tri.e1 * barycentric.x + tri.e2 * barycentric.y;
}

inline bool is_degraded(const Triangle &tri)
{
    float inner_product = dot(tri.e1, tri.e2);
    //TODO can simper?
    if (inner_product == 0 || inner_product == 1 || inner_product == -1)
    {
        return true;
    }
    return false;
}

inline float area(const Triangle &tri)
{
    return length(cross(tri.e1, tri.e2)) / 2;
}

inline std::ostream &operator<<(std::ostream &out, const Triangle &triangle)
{
    out << "[v0:" << triangle.v0 << " e1:" << triangle.e1 << " e2:" << triangle.e2 << "]";
    return out;
}

//v0(128*3)+e1(128*3)+e2(128*3)
struct SSE_ALIGNAS TrianglePack
{
    Point3fPack v0;
    Vector3fPack e1;
    Vector3fPack e2;

    TrianglePack() = default;
    TrianglePack(const Point3fPack &v0, const Vector3fPack &e1, const Vector3fPack &e2) : v0(v0), e1(e1), e2(e2) {}

    Triangle operator[](const uint32_t idx) const
    {
        assert(idx >= 0 && idx < SSE_WIDTH);
        Triangle triangle;
        triangle.v0 = v0[idx];
        triangle.e1 = e1[idx];
        triangle.e2 = e2[idx];
        return triangle;
    }
};

inline std::ostream &operator<<(std::ostream &out, const TrianglePack &triangle)
{
    out << "[v0:" << triangle.v0 << " e1:" << triangle.e1 << " e2:" << triangle.e2 << "]";
    return out;
}

//Tomas Moll https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
FINLINE bool intersect(const Point3f &ray_o, const Vector3f &ray_d, const float ray_t_max, const Point3f &v0, const Vector3f &e1, const Vector3f &e2, float *tt = nullptr, Point2f *uv = nullptr)
{
    auto O = ray_o;
    auto D = ray_d;

    auto V0 = v0;
    auto E1 = e1;
    auto E2 = e2;

    auto T = O - V0;
    auto P = cross(D, E2);
    auto Q = cross(T, E1);

    auto P_dot_E1 = dot(P, E1);

    if (P_dot_E1 <= EPSION && P_dot_E1 >= -EPSION)
    {
        return false;
    }

    auto P_dot_T = dot(P, T);
    auto Q_dot_D = dot(Q, D);

    auto inv_P_dot_E1 = 1.0f / P_dot_E1;
    auto u = P_dot_T * inv_P_dot_E1;
    auto v = Q_dot_D * inv_P_dot_E1;

    if (EXPECT_TAKEN(!(u >= 0.0f && v >= 0.0f && (u + v) <= 1.0f)))
    {
        return false;
    }
    float t = dot(Q, E2) * inv_P_dot_E1;
    if (t >= 0 && t <= ray_t_max)
    {
        if (tt)
        {
            (*tt) = t;
        }
        if (uv)
        {
            uv->x = u;
            uv->y = v;
        }
        return true;
    }
    return false;
}

//Tomas Moll https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
inline bool intersect(const Ray &ray, const Triangle &triangle, float *t = nullptr, Point2f *uv = nullptr)
{
    return intersect(ray.o, ray.d, ray.t_max, triangle.v0, triangle.e1, triangle.e2, t, uv);
}

FINLINE int check(const bool4 &mask, const float4 &t_results, const float4 &u_results, const float4 &v_results, float *t, Point2f *uv)
{
    float min_t = INFINITE;
    auto valid_mask = movemask(mask);
    int idx = -1;
    for (int x = valid_mask, i = 0; x != 0; x >>= 1, ++i)
    {
        if ((x & 0x1) && min_t > t_results[i])
        {
            min_t = t_results[i];
            idx = i;
        }
    }

    if (idx != -1)
    {
        (*t) = min_t;
        uv->x = u_results[idx];
        uv->y = v_results[idx];
    }

    return idx;
}

inline bool4 intersect(const RayPack &ray, const TrianglePack &triangle, float4 *t_results, float4 *u_results, float4 *v_results, bool4 mask = SSE_MASK_TRUE)
{
    auto O = ray.o;
    auto D = ray.d;
    auto V0 = triangle.v0;
    auto E1 = triangle.e1;
    auto E2 = triangle.e2;

    auto T = O - V0;

    auto P = cross(D, E2);
    auto Q = cross(T, E1);

    auto P_dot_E1 = dot(P, E1);

    auto P_dot_T = dot(P, T);
    auto Q_dot_D = dot(Q, D);

    float4 zero = _mm_setzero_ps();
    float4 one = float4(1.0f);

    float4 inv_P_dot_E1 = one / float4(P_dot_E1);

    float4 u = float4(P_dot_T) * float4(inv_P_dot_E1);
    float4 v = float4(Q_dot_D) * float4(inv_P_dot_E1);

    //check det
    bool4 mask_det = (float4(P_dot_E1) >= float4(EPSION));
    mask_det = mask_det | (float4(P_dot_E1) <= float4(-EPSION));

    mask = mask & mask_det;

    //check u/v
    mask = mask & (u >= zero);
    mask = mask & (v >= zero);
    mask = mask & ((u + v) <= one);
    if (EXPECT_TAKEN(none(mask)))
    {
        return mask;
    }

    auto Q_dot_E2 = dot(Q, E2);
    float4 t = float4(Q_dot_E2) * inv_P_dot_E1;
    //check t
    mask = mask & (t <= float4(ray.t_max));
    mask = mask & (t >= zero);

    if (EXPECT_TAKEN(t_results))
    {
        (*t_results) = t;
    }

    if (EXPECT_TAKEN(u_results))
    {
        (*t_results) = u;
    }

    if (EXPECT_TAKEN(v_results))
    {
        (*t_results) = v;
    }

    return mask;
}

//Tomas Moll https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
inline bool intersect(const RayPack &ray, const TrianglePack &triangle, float *t_result, Point2f *uv, int *index, bool4 mask = SSE_MASK_TRUE)
{
    auto O = ray.o;
    auto D = ray.d;
    auto V0 = triangle.v0;
    auto E1 = triangle.e1;
    auto E2 = triangle.e2;

    auto T = O - V0;

    auto P = cross(D, E2);
    auto Q = cross(T, E1);

    auto P_dot_E1 = dot(P, E1);

    auto P_dot_T = dot(P, T);
    auto Q_dot_D = dot(Q, D);

    float4 zero = _mm_setzero_ps();
    float4 one = float4(1.0f);

    float4 inv_P_dot_E1 = one / float4(P_dot_E1);

    float4 u = float4(P_dot_T) * float4(inv_P_dot_E1);
    float4 v = float4(Q_dot_D) * float4(inv_P_dot_E1);

    //check det
    bool4 mask_det = (float4(P_dot_E1) >= float4(EPSION));
    mask_det = mask_det | (float4(P_dot_E1) <= float4(-EPSION));

    mask = mask & mask_det;

    //check u/v
    mask = mask & (u >= zero);
    mask = mask & (v >= zero);
    mask = mask & ((u + v) <= one);
    if (EXPECT_TAKEN(none(mask)))
    {
        return false;
    }
    auto Q_dot_E2 = dot(Q, E2);
    float4 t = float4(Q_dot_E2) * inv_P_dot_E1;
    //check t
    mask = mask & (t <= float4(ray.t_max));
    mask = mask & (t >= zero);

    if (EXPECT_TAKEN(none(mask)))
    {
        return false;
    }

    if (EXPECT_TAKEN(t_result))
    {
        float min_t = INFINITE;
        auto valid_mask = movemask(mask);
        int idx = -1;
        for (int x = valid_mask, i = 0; x != 0; x >>= 1, ++i)
        {
            if ((x & 0x1) && min_t > t[i])
            {
                min_t = t[i];
                idx = i;
            }
        }

        if (EXPECT_NOT_TAKEN(idx != -1))
        {
            (*t_result) = min_t;
            if (EXPECT_TAKEN(index))
            {
                (*index) = idx;
            }
            if (EXPECT_TAKEN(uv))
            {
                uv->x = u[idx];
                uv->y = v[idx];
            }
        }
    }
    return true;
}

inline bool intersect(const RayPack &ray, const TrianglePack &triangle, bool4 mask = SSE_MASK_TRUE)
{
    auto O = ray.o;
    auto D = ray.d;
    auto V0 = triangle.v0;
    auto E1 = triangle.e1;
    auto E2 = triangle.e2;

    auto T = O - V0;

    auto P = cross(D, E2);
    auto Q = cross(T, E1);

    auto P_dot_E1 = dot(P, E1);

    auto P_dot_T = dot(P, T);
    auto Q_dot_D = dot(Q, D);

    float4 zero = _mm_setzero_ps();
    float4 one = float4(1.0f);

    float4 inv_P_dot_E1 = one / float4(P_dot_E1);

    float4 u = float4(P_dot_T) * float4(inv_P_dot_E1);
    float4 v = float4(Q_dot_D) * float4(inv_P_dot_E1);

    //check det
    bool4 mask_det = (float4(P_dot_E1) >= float4(EPSION));
    mask_det = mask_det | (float4(P_dot_E1) <= float4(-EPSION));

    mask = mask & mask_det;

    //check u/v
    mask = mask & (u >= zero);
    mask = mask & (v >= zero);
    mask = mask & ((u + v) <= one);
    if (EXPECT_TAKEN(none(mask)))
    {
        return false;
    }
    auto Q_dot_E2 = dot(Q, E2);
    float4 t = float4(Q_dot_E2) * inv_P_dot_E1;
    //check t
    mask = mask & (t <= float4(ray.t_max));
    mask = mask & (t >= zero);

    if (EXPECT_TAKEN(none(mask)))
    {
        return false;
    }
    return true;
}

template <typename T>
struct Bounds2
{
    Point2<T> min_point;
    Point2<T> max_point;
    inline const Point2<T> &operator[](const int idx) const
    {
        assert(idx >= 0 && idx < 2);
        return (&min_point)[idx];
    }
    inline Point2<T> &operator[](const int idx)
    {
        assert(idx >= 0 && idx < 2);
        return (&min_point)[idx];
    }

    inline Bounds2()
    {
        T min_value = std::numeric_limits<T>::lowest();
        T max_value = std::numeric_limits<T>::max();
        min_point = Point2<T>(max_value, max_value);
        max_point = Point2<T>(min_value, min_value);
    }

    inline Bounds2(const Point2<T> &p0, const Point2<T> &p1)
    {
        min_point = min(p0, p1);
        max_point = max(p0, p1);
    }

    template <typename U>
    inline explicit Bounds2(const Bounds2<U> &bounds)
    {
        min_point = Point2<T>(bounds.min_point);
        max_point = Point2<T>(bounds.max_point);
    }
};
template <typename T>
inline std::ostream &operator<<(std::ostream &out, const Bounds2<T> &box)
{
    out << "[min point:" << box.min_point << " max point:" << box.max_point << "]";
    return out;
}

template <typename T>
inline T area(const Bounds2<T> &bounds)
{
    T w = bounds.max_point.x - bounds.min_point.x;
    T h = bounds.max_point.y - bounds.min_point.y;
    return w * h;
}

template <typename T>
inline Vector2<T> diagonal(const Bounds2<T> &bounds)
{
    T w = bounds.max_point.x - bounds.min_point.x;
    T h = bounds.max_point.y - bounds.min_point.y;
    return Vector2<T>(w, h);
}

template <typename T>
inline bool inside_exclusive(const Point2<T> &p, const Bounds2<T> &bounds)
{
    bool cond_x = p.x >= bounds.min_point.x && p.x < bounds.max_point.x;
    bool cond_y = p.y >= bounds.min_point.y && p.y < bounds.max_point.y;
    return (cond_x && cond_y);
}

template <typename T>
inline Bounds2<T> intersect(const Bounds2<T> &b0, const Bounds2<T> &b1)
{
    return Bounds2<T>(max(b0.min_point, b1.min_point), min(b0.max_point, b1.max_point));
}

template <typename T>
inline T width(const Bounds2<T> &bounds) { return bounds.max_point.x - bounds.min_point.x; }
template <typename T>
inline T height(const Bounds2<T> &bounds) { return bounds.max_point.y - bounds.min_point.y; }

typedef Bounds2<float> Bounds2f;
typedef Bounds2<int> Bounds2i;

// from PBRT
class Bounds2iIterator : public std::forward_iterator_tag
{
public:
    inline Bounds2iIterator(const Bounds2i &b, const Point2i &pt) : _p(pt), _bounds(&b)
    {
    }
    inline Bounds2iIterator operator++()
    {
        advance();
        return *this;
    }
    inline Bounds2iIterator operator++(int)
    {
        Bounds2iIterator old = *this;
        advance();
        return old;
    }
    inline bool operator==(const Bounds2iIterator &bi) const
    {
        return _p == bi._p && _bounds == bi._bounds;
    }
    inline bool operator!=(const Bounds2iIterator &bi) const
    {
        return _p != bi._p || _bounds != bi._bounds;
    }

    inline Point2i operator*() const
    {
        return _p;
    }

private:
    inline void advance()
    {
        ++_p.x;
        if (_p.x == _bounds->max_point.x)
        {
            _p.x = _bounds->min_point.x;
            ++_p.y;
        }
    }
    Point2i _p;
    const Bounds2i *_bounds;
};

inline Bounds2iIterator begin(const Bounds2i &b)
{
    return Bounds2iIterator(b, b.min_point);
}

inline Bounds2iIterator end(const Bounds2i &b)
{
    Point2i pEnd(b.min_point.x, b.max_point.y);
    if (b.min_point.x >= b.max_point.x || b.min_point.y >= b.max_point.y)
        pEnd = b.min_point;
    return Bounds2iIterator(b, pEnd);
}

template <typename T>
struct Bounds3
{
    Point3<T> min_point;
    Point3<T> max_point;
    // 0:min point
    // 1:max point
    inline const Point3<T> &operator[](const int idx) const
    {
        assert(idx >= 0 && idx < 2);
        return (&min_point)[idx];
    }

    inline Bounds3()
    {
        T min_value = std::numeric_limits<T>::lowest();
        T max_value = std::numeric_limits<T>::max();
        min_point = Point3<T>(max_value, max_value, max_value);
        max_point = Point3<T>(min_value, min_value, min_value);
    }

    inline Bounds3(const Point3<T> &p0, const Point3<T> &p1)
    {
        min_point = min(p0, p1);
        max_point = max(p0, p1);
    }
};

template <typename T>
inline std::ostream &operator<<(std::ostream &out, const Bounds3<T> &box)
{
    out << "[min point:" << box.min_point << " max point:" << box.max_point << "]";
    return out;
}

typedef Bounds3<float> Bounds3f;
typedef Bounds3<int> Bounds3i;

template <typename T>
inline bool operator==(const Bounds3<T> &b0, const Bounds3<T> &b1)
{
    return (b0.min_point == b1.min_point) && (b0.max_point == b1.max_point);
}
template <typename T>
inline bool operator!=(const Bounds3<T> &b0, const Bounds3<T> &b1)
{
    return (b0.min_point != b1.min_point) || (b0.max_point != b1.max_point);
}

template <typename T>
inline Bounds3<T> _union(const Point3<T> &p0, const Point3<T> &p1)
{
    return Bounds3<T>(p0, p1);
}
template <typename T>
inline Bounds3<T> _union(const Bounds3<T> &b0, const Point3<T> &p1)
{
    return Bounds3<T>(min(b0.min_point, p1), max(b0.max_point, p1));
}
template <typename T>
inline Bounds3<T> _union(const Point3<T> &p1, const Bounds3<T> &b0)
{
    return _union(b0, p1);
}
template <typename T>
inline Bounds3<T> _union(const Bounds3<T> &b0, const Bounds3<T> &b1)
{
    return _union(min(b0.min_point, b1.min_point), max(b0.max_point, b1.max_point));
}
template <typename T>
inline Bounds3<T> intersect(const Bounds3<T> &b0, const Bounds3<T> &b1)
{
    return Bounds3<T>(max(b0.min_point, b1.min_point), min(b0.max_point, b1.max_point));
}
template <typename T>
inline Bounds3<T> expand(const Bounds3<T> &b, float w)
{
    Bounds3<T> bounds = b;
    float hw = w * 0.5f;
    bounds.min_point -= hw;
    bounds.max_point += hw;
    return bounds;
}
template <typename T>
inline int max_extent(const Bounds3<T> &b0)
{
    Vector3<T> v = b0.max_point - b0.min_point;
    if (v[0] > v[1] && v[0] > v[2])
    {
        return 0;
    }
    else if (v[1] > v[2])
    {
        return 1;
    }
    else
    {
        return 2;
    }
}

template <typename T>
inline Vector3f offset(const Bounds3<T> &b0, const Point3<T> &p0)
{
    return static_cast<Vector3f>(p0 - b0.min_point) / static_cast<Vector3f>(b0.max_point - b0.min_point);
}

template <typename T>
inline T surface_area(const Bounds3<T> &bounds)
{
    T w = bounds.max_point.x - bounds.min_point.x;
    T h = bounds.max_point.y - bounds.min_point.y;
    T d = bounds.max_point.z - bounds.min_point.z;

    return (w * h + w * d + d * h) * 2;
}

// 0 => (0,0,0)
// 1 => (1,0,0)
// 2 => (0,1,0)
// 3 => (1,1,0)
// 4 => (0,0,1)
// 5 => (1,0,1)
// 6 => (0,1,1)
// 7 => (1,1,1)

template <typename T>
inline const Point3<T> corner(const Bounds3<T> &b, int idx)
{
    int x = idx & 0x01;
    int y = (idx >> 1) & 0x01;
    int z = (idx >> 2) & 0x01;
    return Point3<T>(b[x].x, b[y].y, b[z].z);
}

struct SSE_ALIGNAS Bounds3fPack
{
    Point3fPack min_point;
    Point3fPack max_point;

    // 0:min point
    // 1:max point
    inline const Point3fPack &column(const int idx) const
    {
        assert(idx >= 0 && idx < 2);
        return (&min_point)[idx];
    }

    inline Bounds3f operator[](const int idx) const
    {
        assert(idx >= 0 && idx < SSE_WIDTH);
        return Bounds3f(min_point[idx], max_point[idx]);
    }

    inline Bounds3fPack()
    {
        min_point = Point3fPack(OVERFLOW_LIMIT, OVERFLOW_LIMIT, OVERFLOW_LIMIT);
        max_point = Point3fPack(UNDERFLOW_LIMIT, UNDERFLOW_LIMIT, UNDERFLOW_LIMIT);
    }

    inline Bounds3fPack(const Point3fPack &p0, const Point3fPack &p1)
    {
        min_point = min(p0, p1);
        max_point = max(p0, p1);
    }

    inline Bounds3fPack(const Bounds3f bounds[4])
    {
        min_point = Point3fPack(bounds[0].min_point, bounds[1].min_point, bounds[2].min_point, bounds[3].min_point);
        max_point = Point3fPack(bounds[0].max_point, bounds[1].max_point, bounds[2].max_point, bounds[3].max_point);
    }
};

inline void store(const Bounds3fPack &bounds, Bounds3f *array)
{

    array[0].min_point = bounds.min_point[0];
    array[1].min_point = bounds.min_point[1];
    array[2].min_point = bounds.min_point[2];
    array[3].min_point = bounds.min_point[3];

    array[0].max_point = bounds.max_point[0];
    array[1].max_point = bounds.max_point[1];
    array[2].max_point = bounds.max_point[2];
    array[3].max_point = bounds.max_point[3];
}

inline std::ostream &operator<<(std::ostream &out, const Bounds3fPack &box)
{
    out << "[min point:" << box.min_point << " max point:" << box.max_point << "]";
    return out;
}

inline Bounds3fPack load(const Bounds3f *bound_array)
{
    return Bounds3fPack(bound_array);
}

//https://www.slideshare.net/ssuser2848d3/qbv
//single ray with four box
inline bool intersect(const Point3f &o, const Vector3f &inv_d, float t_min, float t_max, const int isPositive[3], const Bounds3f &box)
{
    //x
    t_min = max((box[1 - isPositive[0]].x - o.x) * inv_d.x, t_min);
    t_max = min((box[isPositive[0]].x - o.x) * inv_d.x, t_max);
    //y
    t_min = max((box[1 - isPositive[1]].y - o.y) * inv_d.y, t_min);
    t_max = min((box[isPositive[1]].y - o.y) * inv_d.y, t_max);
    //z
    t_min = max((box[1 - isPositive[2]].z - o.z) * inv_d.z, t_min);
    t_max = min((box[isPositive[2]].z - o.z) * inv_d.z, t_max);

    return t_min <= t_max;
}

inline bool4 intersect(const Point3fPack &o, const Vector3fPack &inv_d, float4 t_min, float4 t_max, const int isPositive[3], const Bounds3fPack &box)
{
    // x
    t_min = max((box.column(1 - isPositive[0]).xxxx - o.xxxx) * inv_d.xxxx, t_min);
    t_max = min((box.column(isPositive[0]).xxxx - o.xxxx) * inv_d.xxxx, t_max);

    //y
    t_min = max((box.column(1 - isPositive[1]).yyyy - o.yyyy) * inv_d.yyyy, t_min);
    t_max = min((box.column(isPositive[1]).yyyy - o.yyyy) * inv_d.yyyy, t_max);

    //z
    t_min = max((box.column(1 - isPositive[2]).zzzz - o.zzzz) * inv_d.zzzz, t_min);
    t_max = min((box.column(isPositive[2]).zzzz - o.zzzz) * inv_d.zzzz, t_max);

    //check
    return t_min <= t_max;
}

inline bool4 intersect(const Point3fPack &o, const Vector3fPack &inv_d, float4 t_min, float4 t_max, const int isPositive[3], const Bounds3fPack &box, float4 *t)
{
    // x
    t_min = max((box.column(1 - isPositive[0]).xxxx - o.xxxx) * inv_d.xxxx, t_min);
    t_max = min((box.column(isPositive[0]).xxxx - o.xxxx) * inv_d.xxxx, t_max);

    //y
    t_min = max((box.column(1 - isPositive[1]).yyyy - o.yyyy) * inv_d.yyyy, t_min);
    t_max = min((box.column(isPositive[1]).yyyy - o.yyyy) * inv_d.yyyy, t_max);

    //z
    t_min = max((box.column(1 - isPositive[2]).zzzz - o.zzzz) * inv_d.zzzz, t_min);
    t_max = min((box.column(isPositive[2]).zzzz - o.zzzz) * inv_d.zzzz, t_max);

    (*t) = t_min;

    //check
    return t_min <= t_max;
}

//TODO need to test
inline bool intersect(const Point3fPack &o, const Vector3fPack &inv_d, float4 t_min, float4 t_max, const int isPositive[3], const Bounds3fPack &box, float *t_result, int *index)
{
    float4 t;
    auto mask = intersect(o, inv_d, t_min, t_max, isPositive, box, &t);
    if (EXPECT_TAKEN(none(mask)))
    {
        return false;
    }

    if (EXPECT_TAKEN(t_result))
    {
        float min_t = INFINITE;
        auto valid_mask = movemask(mask);
        int idx = -1;
        for (int x = valid_mask, i = 0; x != 0; x >>= 1, ++i)
        {
            if ((x & 0x1) && min_t > t[i])
            {
                min_t = t[i];
                idx = i;
            }
        }
        if (EXPECT_NOT_TAKEN(idx != -1))
        {
            (*t_result) = min_t;
            if (EXPECT_TAKEN(index))
            {
                (*index) = idx;
            }
        }
    }
    return true;
}

struct Plane
{
    float d;
    Normal3f n;
};

inline bool intersect(const Ray &ray, const Plane &plane, float *t)
{
    float ddotn = dot(ray.d, plane.n);
    if (ddotn != 0)
    {
        (*t) = (plane.d - dot(ray.o, plane.n)) / ddotn;
        return true;
    }
    return false;
}

// p0        p1
//   +-----+
//   |     |
//   |     |
//   |     |
//   |     |
//   +-----+
// p2        p3
struct Quad
{
    Point3f p0, p1, p2, p3;
};

struct QuadPack
{
    Point3fPack p0, p1, p2, p3;
};

inline std::ostream &operator<<(std::ostream &out, const QuadPack &quad)
{
    out << "[top-left:" << quad.p0 << " top-right:" << quad.p1 << " bottom-left:" << quad.p2 << " bottom-right:" << quad.p3 << "]";
    return out;
}

inline bool intersect(const RayPack &ray, const QuadPack &quad, int *triangle_index, float *t_result, Point2f *uv, int *index, bool4 mask = SSE_MASK_TRUE)
{

    TrianglePack triangle0(quad.p0, quad.p2 - quad.p0, quad.p3 - quad.p0);
    float t0 = INFINITE;
    int index0;
    Point2f uv0;
    bool is_hit0 = intersect(ray, triangle0, &t0, &uv0, &index0, mask);

    TrianglePack triangle1(quad.p0, quad.p3 - quad.p0, quad.p1 - quad.p0);
    float t1 = INFINITE;
    int index1;
    Point2f uv1;
    bool is_hit1 = intersect(ray, triangle1, &t1, &uv1, &index1, mask);

    if (is_hit0 || is_hit1)
    {
        if (t0 <= t1)
        {
            (*t_result) = t0;
            (*uv) = uv0;
            (*index) = index0;
            (*triangle_index) = 0;
        }
        else
        {
            (*t_result) = t1;
            (*uv) = uv1;
            (*index) = index1;
            (*triangle_index) = 1;
        }
        return true;
    }
    return false;
}

inline bool intersect(const RayPack &ray, const QuadPack &quad, bool4 mask = SSE_MASK_TRUE)
{

    TrianglePack triangle0(quad.p0, quad.p2 - quad.p0, quad.p3 - quad.p0);
    bool is_hit0 = intersect(ray, triangle0, mask);

    TrianglePack triangle1(quad.p0, quad.p3 - quad.p0, quad.p1 - quad.p0);
    bool is_hit1 = intersect(ray, triangle1, mask);

    if (is_hit0 || is_hit1)
    {
        return true;
    }
    return false;
}

struct Curve
{
    Point3f cp0, cp1, cp2, cp3;
};

//from pbrt
inline Point3f blossom(const Curve &curve, float u0, float u1, float u2)
{
    Point3f a[3] = {lerp(curve.cp0, curve.cp1, u0), lerp(curve.cp0, curve.cp1, u0), lerp(curve.cp0, curve.cp1, u0)};
    Point3f b[2] = {lerp(a[0], a[1], u1), lerp(a[1], a[2], u1)};
    return lerp(b[0], b[1], u2);
}
NARUKAMI_END
