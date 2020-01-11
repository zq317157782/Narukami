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
NARUKAMI_BEGIN

// d_omega = costheta * d_area / distance^2
inline float to_solid_angle_measure_pdf(const float area_pdf, const float distance_sqr, const float costheta)
{
    return area_pdf * distance_sqr * rcp(costheta);
}

inline float to_area_measure_pdf(const float solid_angle_pdf, const float distance_sqr, const float costheta)
{
    return solid_angle_pdf * costheta * rcp(distance_sqr);
}

Point2f polar_to_cartesian(const float radius,const float theta)
{
    return Point2f(radius * cos(theta), radius * sin(theta));
}

//p(x,y) is a constant 1\PI.
//p(radius,theta) is p(x,y) mul radius  => (radius\PI).
//p(radius) is 2*radius.
//p(theta|radius) is p(radius,theta)/p(radius)  => (1/2PI)
inline Point2f uniform_sample_disk(const Point2f &u)
{
    float radius = sqrt(u.x);
    float theta = 2.0f * PI * u.y;
    return polar_to_cartesian(radius,theta);
}


//"A low distortion map between disk and square"
inline Point2f concentric_sample_disk(const Point2f &u)
{
    auto u_offset = u * 2.0f - 1.0f;
    if(u_offset.x == 0 && u_offset.y == 0)
    {
        return Point2f(0.0f,0.0f);
    }

    float radius,theta;
    if(abs(u_offset.x) > abs(u_offset.y))
    {
        radius = u_offset.x;
        theta = (u_offset.y/u_offset.x) * PI_OVER_FOUR;
    }
    else
    {
        radius = u_offset.y;
        theta = PI_OVER_TWO - (u_offset.x/u_offset.y) * PI_OVER_FOUR;
    }
    return polar_to_cartesian(radius,theta);
}
NARUKAMI_END
