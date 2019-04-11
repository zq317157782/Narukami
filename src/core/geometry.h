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
#include "core/euclid.h"
#include "core/simd.h"

NARUKAMI_BEGIN

struct Ray{
   Point3f o;
   Vector3f d;
   float tMax;

   Ray(const Point3f& o,const Vector3f& d,const float tMax = Infinite):o(o),d(d),tMax(tMax){

   }
};


struct SSE_ALIGNAS SoATriangle{
    SoAPoint3f v0;
    SoAVector3f e1;
    SoAVector3f e2;
};


FINLINE int intersect(const Ray& ray,const SoATriangle& triangle){
    SoAPoint3f O(ray.o);
    SoAVector3f D(ray.d);
    auto V0 = triangle.v0;
    auto E1 = triangle.e1;
    auto E2 = triangle.e2;

    auto T = O - V0;
    
    auto P = cross(D,E2);
    auto Q = cross(T,E1);

    auto P_dot_E1 = dot(P,E1);
    auto P_dot_T = dot(P,T);
    auto Q_dot_D = dot(Q,D);

    float4 u = float4(P_dot_T)/float4(P_dot_E1);
    float4 v = float4(Q_dot_D)/float4(P_dot_E1);

    bool4 mask0=u>=float4(Zero);
    bool4 mask1=v>=float4(Zero);
    bool4 mask2=(u+v)<=float4(One);
    bool4 mask3=(mask0&mask1)&mask2;
    if(none(mask3)){
        return 0;
    }
  
    float4 t = dot(Q,E2)/float4(P_dot_E1);
    bool4 mask4 = (t <= float4(ray.tMax))&mask3;
    return movemask(mask4);
}

NARUKAMI_END
