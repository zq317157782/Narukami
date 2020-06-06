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
#include "core/affine.h"
NARUKAMI_BEGIN

class Quaternion
{
public:
    float x, y, z, w;

public:
    Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    //from PBRT
    Quaternion(const Matrix4x4 &m)
    {
        float trace = m.element(0, 0) + m.element(1, 1) + m.element(2, 2);
        if (trace > 0.f)
        {
            // Compute w from matrix trace, then xyz
            // 4w^2 = m[0][0] + m[1][1] + m[2][2] + m[3][3] (but m[3][3] == 1)
            float s = sqrt(trace + 1.0f);
            w = s /2.0f;
            s = 0.5f/s;
            x = (m.element(2, 1) - m.element(1, 2)) * s;
            y = (m.element(0, 2) - m.element(2, 0)) * s;
            z = (m.element(1, 0) - m.element(0, 1)) * s;
        }
        else
        {
            // Compute largest of $x$, $y$, or $z$, then remaining components
            const int nxt[3] = {1, 2, 0};
            float q[3];
            int i = 0;
            if (m.element(1, 1) > m.element(0, 0))
            {
                i = 1;
            }
            if (m.element(2, 2) > m.element(i, i))
            {
                i = 2;
            }
            int j = nxt[i];
            int k = nxt[j];
            float s = std::sqrt((m.element(i, i) - (m.element(j, j) + m.element(k, k))) + 1.0f);
            q[i] = s * 0.5f;
            if (s != 0.f)
            {
                s = 0.5f/s;
            }
            w = (m.element(k, j) - m.element(j, k)) * s;
            q[j] = (m.element(j, i) + m.element(i, j)) * s;
            q[k] = (m.element(k, i) + m.element(i, k)) * s;
            x = q[0];
            y = q[1];
            z = q[2];
        }
    }
};

inline std::ostream &operator<<(std::ostream &out, const Quaternion &q)
{
    out << "[ " << q.w << " + " << q.x << "i + " << q.y << "j + " << q.z << "k ]";
    return out;
}

//--------------------------------------------------------
inline Quaternion operator-(const Quaternion &q)
{
    return Quaternion(-q.x, -q.y, -q.z, -q.w);
}
inline Quaternion operator+(const Quaternion &q1, const Quaternion &q2)
{
    return Quaternion(q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w);
}
inline Quaternion operator-(const Quaternion &q1, const Quaternion &q2)
{
    return Quaternion(q1.x - q2.x, q1.y - q2.y, q1.z - q2.z, q1.w - q2.w);
}
inline Quaternion operator*(const Quaternion &q1, const Quaternion &q2)
{
    return Quaternion(q1.x * q2.x, q1.y * q2.y, q1.z * q2.z, q1.w * q2.w);
}
inline Quaternion operator*(const Quaternion &q1, float v)
{
    return Quaternion(q1.x * v, q1.y * v, q1.z * v, q1.w * v);
}

inline Quaternion operator*(float v, const Quaternion &q1)
{
    return q1 * v;
}

inline Quaternion operator/(const Quaternion &q1, float v)
{
    assert(v != 0.0f);
    float inv_v = 1.0f/v;
    return Quaternion(q1.x * inv_v, q1.y * inv_v, q1.z * inv_v, q1.w * inv_v);
}
//--------------------------------------------------------

inline float dot(const Quaternion &q1, const Quaternion &q2)
{
    return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
}

inline Quaternion normalize(const Quaternion &q)
{
    return q * rsqrt(dot(q, q));
}

inline Matrix4x4 to_matrix(const Quaternion &q)
{
    float x = q.x;
    float y = q.y;
    float z = q.z;
    float w = q.w;

    return Matrix4x4(
        1.0f - 2.0f * (y * y + z * z), 2.0f * (x * y - z * w), 2.0f * (x * z + y * w), 0.0f,
        2.0f * (x * y + z * w), 1.0f - 2.0f * (x * x + z * z), 2.0f * (y * z - x * w), 0.0f,
        2.0f * (x * z - y * w), 2.0f * (y * z + x * w), 1.0f - 2.0f * (x * x + y * y), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

Quaternion slerp(const Quaternion &q1, const Quaternion &q2, float t);
NARUKAMI_END