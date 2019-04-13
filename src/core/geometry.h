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

   FINLINE Ray(const Point3f& o,const Vector3f& d,const float tMax = Infinite):o(o),d(d),tMax(tMax){

   }
};


struct SSE_ALIGNAS SoARay
{
    SoAPoint3f o;
    SoAVector3f d;
    __m128  tMax;
     
    FINLINE SoARay(const Point3f& o,const Vector3f& d,const float tMax = Infinite):o(SoAPoint3f(o)),d(SoAVector3f(d)),tMax(_mm_set1_ps(tMax)){}
    FINLINE explicit SoARay(const Ray& ray):o(ray.o),d(ray.d),tMax(_mm_set1_ps(ray.tMax)){}
};


struct Triangle{
    Point3f v0;
    Vector3f e1;
    Vector3f e2;
};

struct SSE_ALIGNAS SoATriangle{
    SoAPoint3f v0;
    SoAVector3f e1;
    SoAVector3f e2;
};

//Tomas Moll https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
FINLINE bool intersect(const Ray& ray,const Triangle& triangle){
    auto O = ray.o;
    auto D = ray.d;

    auto V0 = triangle.v0;
    auto E1 = triangle.e1;
    auto E2 = triangle.e2;

    auto T = O - V0;
    auto P = cross(D,E2);
    auto Q = cross(T,E1);

    auto P_dot_E1 = dot(P,E1);
    auto P_dot_T = dot(P,T);
    auto Q_dot_D = dot(Q,D);

    auto inv_P_dot_E1 = 1.0f/P_dot_E1;
    auto u = P_dot_T*inv_P_dot_E1;
    auto v = Q_dot_D*inv_P_dot_E1;

    if(!(u>=0.0f&&v>=0.0f&&(u+v)<=1.0f)){
        return false;
    }
    float t = dot(Q,E2)*inv_P_dot_E1;
    if(t>=0&&t<=ray.tMax){
        return true;
    }
    return false;
}

//Tomas Moll https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
FINLINE bool intersect(const SoARay& ray,const SoATriangle& triangle){
    auto O =ray.o;
    auto D =ray.d;
    auto V0 = triangle.v0;
    auto E1 = triangle.e1;
    auto E2 = triangle.e2;

    auto T = O - V0;
    
    auto P = cross(D,E2);
    auto Q = cross(T,E1);

    auto P_dot_E1 = dot(P,E1);
    auto P_dot_T = dot(P,T);
    auto Q_dot_D = dot(Q,D);
    
    
    float4 zero = _mm_setzero_ps();
    float4 one = float4(One);
    
    float4 inv_P_dot_E1 = one/float4(P_dot_E1);

    float4 u = float4(P_dot_T)*float4(inv_P_dot_E1);
    float4 v = float4(Q_dot_D)*float4(inv_P_dot_E1);
    
    bool4 mask0=(u>=zero);
    mask0=mask0&(v>=zero);
    mask0=mask0&((u+v)<=one);
    // if(none(mask0)){
    //    return false;
    // }
    auto Q_dot_E2 = dot(Q,E2);
    float4 t = float4(Q_dot_E2)*inv_P_dot_E1;
    mask0 = mask0&(t <= float4(ray.tMax));
    mask0 = mask0&(t >= zero);


    if(none(mask0)){
       return false;
    }
    return true;
}

NARUKAMI_END
