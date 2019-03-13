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
class Vector2
{
  public:
    T x, y;

    typedef T Scalar;
    enum
    {
        N = 2
    };

  public:
    FINLINE Vector2() : x(0), y(0)
    {
    }

    FINLINE explicit Vector2(const float a) : x(a), y(a)
    {
        assert(!isnan(a));
    }

    FINLINE Vector2(const T &a, const T &b) : x(a), y(b)
    {
        assert(!isnan(a));
        assert(!isnan(b));
    }
    //just for checking assert for debug
#ifdef NARUKAMI_DEBUG
    FINLINE Vector2(const Vector2 &v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        x = v1.x;
        y = v1.y;
    }

    FINLINE Vector2 &operator=(const Vector2 &v1)
    {
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        x = v1.x;
        y = v1.y;
        return (*this);
    }
#endif
    FINLINE T operator[](const int idx) const
    {
        assert(idx >= 0 && idx < 2);
        return (&x)[idx];
    }

    FINLINE T &operator[](const int idx)
    {
        assert(idx >= 0 && idx < 2);
        return (&x)[idx];
    }
};

template <typename T>
FINLINE  std::ostream &operator<<(std::ostream &out, const Vector2<T> &v)
{
        out << '(' << v.x << ',' << v.y  << ')';
        return out;
}

typedef Vector2<float> Vector2f;
typedef Vector2<int> Vector2i;

template <typename T>
FINLINE Vector2<T> operator+(const Vector2<T> &v1, const Vector2<T> &v2)
{
    Vector2<T> v;
    v.x = v1.x + v2.x;
    v.y = v1.y + v2.y;
    return v;
}

template <typename T>
FINLINE Vector2<T> operator-(const Vector2<T> &v1, const Vector2<T> &v2)
{
    Vector2<T> v;
    v.x = v1.x - v2.x;
    v.y = v1.y - v2.y;
    return v;
}

//compenont wise
template <typename T>
FINLINE Vector2<T> operator*(const Vector2<T> &v1, const Vector2<T> &v2)
{
    Vector2<T> v;
    v.x = v1.x * v2.x;
    v.y = v1.y * v2.y;
    return v;
}

template <typename T>
FINLINE Vector2<T> operator*(const Vector2<T> &v1, const T &f)
{
    Vector2<T> v;
    v.x = v1.x * f;
    v.y = v1.y * f;
    return v;
}
template <typename T>
FINLINE Vector2<T> operator/(const Vector2<T> &v1, const T &f)
{
    assert(f != 0);
    Vector2<T> v;
    v.x = v1.x / f;
    v.y = v1.y / f;
    return v;
}

template <typename T>
FINLINE Vector2<T> &operator+=(Vector2<T> &v, const Vector2<T> &v1)
{
    v.x += v1.x;
    v.y += v1.y;
    return v;
}

template <typename T>
FINLINE Vector2<T> &operator-=(Vector2<T> &v, const Vector2<T> &v1)
{
    v.x -= v1.x;
    v.y -= v1.y;
    return v;
}

template <typename T>
FINLINE Vector2<T> &operator*=(Vector2<T> &v,const Vector2<T> &v1)
{
    v.x *= v1.x;
    v.y *= v1.y;
    return v;
}

template <typename T>
FINLINE Vector2<T> &operator*=(Vector2<T> &v,const T &f)
{
    v.x *= f;
    v.y *= f;
    return v;
}

template <typename T>
FINLINE Vector2<T> &operator/=(Vector2<T> &v,const T &f)
{
    assert(f != 0);
    v.x /= f;
    v.y /= f;
    return v;
}

template <typename T>
FINLINE bool operator==(const Vector2<T> &v1, const Vector2<T> &v2)
{
    if ((v1.x == v2.x) && (v1.y == v2.y))
    {
        return true;
    }
    return false;
}

template <typename T>
FINLINE bool operator!=(const Vector2<T> &v1, const Vector2<T> &v2)
{
    if ((v1.x != v2.x) || (v1.y != v2.y))
    {
        return true;
    }
    return false;
}

template <typename T>
FINLINE T dot(const Vector2<T> &v1, const Vector2<T> &v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

template <typename T>
FINLINE float length(const Vector2<T> &v)
{
    return sqrt(float(dot(v, v)));
}

template <typename T>
FINLINE float lensqr(const Vector2<T> &v)
{
    return float(dot(v, v));
}

template <typename T>
FINLINE T cross(const Vector2<T> &v1, const Vector2<T> &v2)
{
    return v1.x * v2.y - v1.y * v2.x;
}

template <typename T>
FINLINE Vector2f normalize(const Vector2<T> &v1)
{
    float inv_l = rsqrt(dot(v1, v1));
    return v1 * inv_l;
}

template <typename T>
FINLINE Vector2f rcp(const Vector2<T> &v1)
{
    return Vector2f(rcp(v1.x), rcp(v1.y));
}

template <typename T>
FINLINE Vector2f sqrt(const Vector2<T> &v1)
{
    return Vector2f(sqrt(v1.x), sqrt(v1.y));
}

NARUKAMI_END