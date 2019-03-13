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

NARUKAMI_BEGIN
template <typename T>
class Vector3
{
  public:
    T x, y, z;

    typedef T Scalar;
    enum
    {
        N = 3
    };

  public:
    FINLINE Vector3() : x(0), y(0), z(0)
    {
    }

    FINLINE explicit Vector3(const float a) : x(a), y(a), z(a)
    {
        assert(!isnan(a));
    }

    FINLINE Vector3(const T &a, const T &b, const T &c) : x(a), y(b), z(c)
    {
        assert(!isnan(a));
        assert(!isnan(b));
        assert(!isnan(c));
    }
    //just for checking assert for debug
#ifdef NARUKAMI_DEBUG
    FINLINE Vector3(const Vector3 &v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        assert(!isnan(v1.z));
        x = v1.x;
        y = v1.y;
        z = v1.z;
    }

    FINLINE Vector3 &operator=(const Vector3 &v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        assert(!isnan(v1.z));
        x = v1.x;
        y = v1.y;
        z = v1.z;
        return (*this);
    }
#endif
    FINLINE Vector3 &operator+=(const Vector3 &v1)
    {
        x += v1.x;
        y += v1.y;
        z += v1.z;
        return (*this);
    }

    FINLINE Vector3 &operator-=(const Vector3 &v1)
    {
        x -= v1.x;
        y -= v1.y;
        z -= v1.z;
        return (*this);
    }

    FINLINE Vector3 &operator*=(const Vector3 &v1)
    {
        x *= v1.x;
        y *= v1.y;
        z *= v1.z;
        return (*this);
    }

    FINLINE Vector3 &operator*=(const T &f)
    {
        x *= f;
        y *= f;
        z *= f;
        return (*this);
    }

    FINLINE Vector3 &operator/=(const T &f)
    {
        assert(f != 0);
        x /= f;
        y /= f;
        z /= f;
        return (*this);
    }

    FINLINE T operator[](const int idx) const
    {
        assert(idx >= 0 && idx < 3);
        return (&x)[idx];
    }

    FINLINE T &operator[](const int idx)
    {
        assert(idx >= 0 && idx < 3);
        return (&x)[idx];
    }

    FINLINE friend std::ostream &operator<<(std::ostream &out, const Vector3 &v)
    {
        out << '(' << v.x << ',' << v.y << ',' << v.z << ')';
        return out;
    }
};

typedef Vector3<float> Vector3f;
typedef Vector3<int> Vector3i;

template <typename T>
FINLINE Vector3<T> operator+(const Vector3<T> &v1, const Vector3<T> &v2)
{
    Vector3<T> v;
    v.x = v1.x + v2.x;
    v.y = v1.y + v2.y;
    v.z = v1.z + v2.z;
    return v;
}

template <typename T>
FINLINE Vector3<T> operator-(const Vector3<T> &v1, const Vector3<T> &v2)
{
    Vector3<T> v;
    v.x = v1.x - v2.x;
    v.y = v1.y - v2.y;
    v.z = v1.z - v2.z;
    return v;
}

//compenont wise
template <typename T>
FINLINE Vector3<T> operator*(const Vector3<T> &v1, const Vector3<T> &v2)
{
    Vector3<T> v;
    v.x = v1.x * v2.x;
    v.y = v1.y * v2.y;
    v.z = v1.z * v2.z;
    return v;
}

template <typename T>
FINLINE Vector3<T> operator*(const Vector3<T> &v1, const T &f)
{
    Vector3<T> v;
    v.x = v1.x * f;
    v.y = v1.y * f;
    v.z = v1.z * f;
    return v;
}
template <typename T>
FINLINE Vector3<T> operator/(const Vector3<T> &v1, const T &f)
{
    assert(f != 0);
    Vector3<T> v;
    v.x = v1.x / f;
    v.y = v1.y / f;
    v.z = v1.z / f;
    return v;
}

template <typename T>
FINLINE bool operator==(const Vector3<T> &v1, const Vector3<T> &v2)
{
    if ((v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z))
    {
        return true;
    }
    return false;
}

template <typename T>
FINLINE bool operator!=(const Vector3<T> &v1, const Vector3<T> &v2)
{
    if ((v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z))
    {
        return true;
    }
    return false;
}

template <typename T>
FINLINE T dot(const Vector3<T> &v1, const Vector3<T> &v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

template <typename T>
FINLINE float length(const Vector3<T> &v)
{
    return sqrt(float(dot(v, v)));
}

template <typename T>
FINLINE float lensqr(const Vector3<T> &v)
{
    return float(dot(v, v));
}

template <typename T>
FINLINE Vector3<T> cross(const Vector3<T> &v1, const Vector3<T> &v2)
{
    return Vector3<T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

template <typename T>
FINLINE Vector3f normalize(const Vector3<T> &v1)
{
    float inv_l = rsqrt(dot(v1, v1));
    return v1 * inv_l;
}

template <typename T>
FINLINE Vector3f rcp(const Vector3<T> &v1)
{
    return Vector3f(rcp(v1.x), rcp(v1.y), rcp(v1.z));
}

template <typename T>
FINLINE Vector3f sqrt(const Vector3<T> &v1)
{
    return Vector3f(sqrt(v1.x), sqrt(v1.y), sqrt(v1.z));
}

NARUKAMI_END