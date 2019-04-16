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
#include "core/euclid.h"
#include "core/simd.h"

NARUKAMI_BEGIN


struct Ray{
   Point3f o;
   Vector3f d;
   mutable float tMax;

   FINLINE Ray(const Point3f& o,const Vector3f& d,const float tMax = Infinite):o(o),d(d),tMax(tMax){

   }
};
FINLINE  std::ostream &operator<<(std::ostream &out, const Ray &ray) {
    out<<"[o:"<<ray.o<<" d:"<<ray.d<<" t:"<<ray.tMax<<"]";
} 

struct SSE_ALIGNAS SoARay
{
    SoAPoint3f o;
    SoAVector3f d;
    mutable __m128  tMax;
     
    FINLINE SoARay(const Point3f& o,const Vector3f& d,const float tMax = Infinite):o(SoAPoint3f(o)),d(SoAVector3f(d)),tMax(_mm_set1_ps(tMax)){}
    FINLINE explicit SoARay(const Ray& ray):o(ray.o),d(ray.d),tMax(_mm_set1_ps(ray.tMax)){}
};
FINLINE  std::ostream &operator<<(std::ostream &out, const SoARay &ray) {
    out<<"[o:"<<ray.o<<" d:"<<ray.d<<" t:"<<float4(ray.tMax)<<"]";
} 

struct Triangle{
    Point3f v0;
    Vector3f e1;
    Vector3f e2;
};
FINLINE  std::ostream &operator<<(std::ostream &out, const Triangle &triangle) {
    out<<"[v0:"<<triangle.v0<<" e1:"<<triangle.e1<<" e2:"<<triangle.e2<<"]";
} 

struct SSE_ALIGNAS SoATriangle{
    SoAPoint3f v0;
    SoAVector3f e1;
    SoAVector3f e2;
};
FINLINE  std::ostream &operator<<(std::ostream &out, const SoATriangle &triangle) {
    out<<"[v0:"<<triangle.v0<<" e1:"<<triangle.e1<<" e2:"<<triangle.e2<<"]";
} 


struct SSE_ALIGNAS SoABox
{
    SoAPoint3f min_point;
    SoAPoint3f max_point;

    const SoAPoint3f& operator[](const int idx) const{
        assert(idx>=0&&idx<2);
        return (&min_point)[idx];
    }
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

    if(P_dot_E1<=EPSION&&P_dot_E1>=-EPSION){
        return false;
    }

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
        ray.tMax=t;
        return true;
    }
    return false;
}

//Tomas Moll https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
FINLINE __m128 intersect(const SoARay& ray,const SoATriangle& triangle,__m128 mask=SSE_MASK_TRUE){
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
    
    //check det
    bool4 mask_det = (float4(P_dot_E1)>=float4(EPSION));
    mask_det = mask_det | (float4(P_dot_E1)<=float4(-EPSION));

    mask=mask&mask_det;

    //check u/v
    mask=mask&(u>=zero);
    mask=mask&(v>=zero);
    mask=mask&((u+v)<=one);
    if(none(mask)){
        return mask;
    }
    auto Q_dot_E2 = dot(Q,E2);
    float4 t = float4(Q_dot_E2)*inv_P_dot_E1;
    //check t
    mask = mask&(t <= float4(ray.tMax));
    mask = mask&(t >= zero);

    if(none(mask)){
        return mask;
    }

    ray.tMax =_mm_blendv_ps(ray.tMax,t,mask);
    return mask;
}

//https://www.slideshare.net/ssuser2848d3/qbv
FINLINE __m128 intersect(const SoAPoint3f& o,const SoAVector3f& inv_d,__m128 t_min,__m128 t_max,const int isPositive[3],const SoABox& box){
    // x
    t_min = max(t_min,(float4(box[1-isPositive[0]].xxxx)-float4(o.xxxx))*float4(inv_d.xxxx));
    t_max = min(t_max,(float4(box[isPositive[0]].xxxx)-float4(o.xxxx))*float4(inv_d.xxxx));
    
    //y
    t_min = max(t_min,(float4(box[1-isPositive[1]].yyyy)-float4(o.yyyy))*float4(inv_d.yyyy));
    t_max = min(t_max,(float4(box[isPositive[1]].yyyy)-float4(o.yyyy))*float4(inv_d.yyyy));
    
    //z
    t_min = max(t_min,(float4(box[1-isPositive[2]].zzzz)-float4(o.zzzz))*float4(inv_d.zzzz));
    t_max = min(t_max,(float4(box[isPositive[2]].zzzz)-float4(o.zzzz))*float4(inv_d.zzzz));

    //check
    return float4(t_min)<=float4(t_max);
}


FINLINE __m128 intersect(const SoAPoint3f& o,const SoAVector3f& inv_d,__m128 t_min,__m128 t_max,const __m128 isPositive[3],const SoABox& box){
    // x
    
    t_min = max(t_min,(float4(select(isPositive[0],box[0].xxxx,box[1].xxxx))-float4(o.xxxx))*float4(inv_d.xxxx));
    t_max = min(t_max,(float4(select(isPositive[0],box[1].xxxx,box[0].xxxx))-float4(o.xxxx))*float4(inv_d.xxxx));
    
    //y
    t_min = max(t_min,(float4(select(isPositive[1],box[0].yyyy,box[1].yyyy))-float4(o.yyyy))*float4(inv_d.yyyy));
    t_max = min(t_max,(float4(select(isPositive[1],box[1].yyyy,box[0].yyyy))-float4(o.yyyy))*float4(inv_d.yyyy));
    
    //z
    t_min = max(t_min,(float4(select(isPositive[2],box[0].zzzz,box[1].zzzz))-float4(o.yyyy))*float4(inv_d.zzzz));
    t_max = min(t_max,(float4(select(isPositive[2],box[1].zzzz,box[0].zzzz))-float4(o.yyyy))*float4(inv_d.zzzz));

    //check
    return float4(t_min)<=float4(t_max);
}

NARUKAMI_END
