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
#include "core/quaternion.h"
NARUKAMI_BEGIN
Quaternion slerp(const Quaternion &q1, const Quaternion &q2,float t)
{
    float cos_theta = dot(q1, q2);
    if (EXPECT_NOT_TAKEN(cos_theta > 0.9995f))
    {
        return normalize((1.0f - t) * q1 + t * q2);
    }
    else
    {
        float theta = acos(clamp(cos_theta, -1.0f, 1.0f));
        float theta_t = theta * t;
        Quaternion qp = normalize(q2 - cos_theta * q1);
        return q1 * cos(theta_t) + qp * sin(theta_t);
    }
}
NARUKAMI_END