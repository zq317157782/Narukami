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
#include <vector>
NARUKAMI_BEGIN

template <typename T>
struct DataBuffer
{
    int size;
    std::shared_ptr<T[]> buffer;
    DataBuffer(const int size):size(size){
        assert(size>0);
        buffer = std::shared_ptr<T[]>(new T[size]);
    }
    
    DataBuffer(const int size,void* data):DataBuffer(size){
        memcpy(buffer.get(),data,sizeof(T)*size);
    }
    
    FINLINE T& operator[](const int idx){ assert(idx>=0&&idx<size); return buffer[idx]; }
    FINLINE T& operator[](const int idx) const{ assert(idx>=0&&idx<size); return buffer[idx]; }
};

template <typename T>
void load(DataBuffer<T>& vb,void* data,const  int size,const int offset=0){
    memcpy(vb.buffer.get()+offset,data,sizeof(T)*size);
}



struct Ray{
   Point3f o;
   Vector3f d;
   mutable float t_max;

   FINLINE Ray(const Point3f& o,const Vector3f& d,const float t_max = Infinite):o(o),d(d),t_max(t_max){

   }
};
FINLINE  std::ostream &operator<<(std::ostream &out, const Ray &ray) {
    out<<"[o:"<<ray.o<<" d:"<<ray.d<<" t:"<<ray.t_max<<"]";
} 

struct SSE_ALIGNAS SoARay
{
    SoAPoint3f o;
    SoAVector3f d;
    mutable float4 t_max;
     
    FINLINE SoARay(const Point3f& o,const Vector3f& d,const float t_max = Infinite):o(SoAPoint3f(o)),d(SoAVector3f(d)),t_max(t_max){}
    FINLINE explicit SoARay(const Ray& ray):o(ray.o),d(ray.d),t_max(ray.t_max){}
};
FINLINE  std::ostream &operator<<(std::ostream &out, const SoARay &ray) {
    out<<"[o:"<<ray.o<<" d:"<<ray.d<<" t:"<<float4(ray.t_max)<<"]";
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

struct Box
{
    Point3f min_point;
    Point3f max_point;
    const Point3f& operator[](const int idx) const{
        assert(idx>=0&&idx<2);
        return (&min_point)[idx];
    }
};
FINLINE  std::ostream &operator<<(std::ostream &out, const Box &box) {
    out<<"[min point:"<<box.min_point<<" max point:"<<box.max_point<<"]";
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
FINLINE  std::ostream &operator<<(std::ostream &out, const SoABox &box) {
    out<<"[min point:"<<box.min_point<<" max point:"<<box.max_point<<"]";
} 

struct GeometryInteraction
{
    float t;
    float u;
    float v;
    int   index;
};

//Tomas Moll https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
FINLINE bool intersect(const Ray& ray,const Triangle& triangle,GeometryInteraction* hit=nullptr){
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
    if(t>=0&&t<=ray.t_max){
        if(hit){
            hit->t= t;
            hit->u= u;
            hit->v= v;
        }
        return true;
    }
    return false;
}

//Tomas Moll https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
FINLINE bool intersect(const SoARay& ray,const SoATriangle& triangle,GeometryInteraction* hit=nullptr,__m128 mask=SSE_MASK_TRUE){
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
        return false;
    }
    auto Q_dot_E2 = dot(Q,E2);
    float4 t = float4(Q_dot_E2)*inv_P_dot_E1;
    //check t
    mask = mask&(t <= float4(ray.t_max));
    mask = mask&(t >= zero);

    if(none(mask)){
        return false;
    }

    if(hit){
        int min_mask=reduce_min_mask(t,&hit->t);
        int idx=0;
        if(min_mask&0x1){
            idx=0;
        }
        else if(min_mask&0x2){
            idx=1;
        }
        else if(min_mask&0x4){
            idx=2;
        }
        else if(min_mask&0x8){
            idx=3;
        }
        
        hit->u=u[idx];
        hit->v=v[idx];
        hit->index = idx;
    }
    return true;
}



//https://www.slideshare.net/ssuser2848d3/qbv
//single ray with four box
FINLINE bool collide(const Point3f& o,const Vector3f& inv_d,float t_min,float t_max,const int isPositive[3],const Box& box){
    //x
    t_min = max(t_min,(box[1-isPositive[0]].x-o.x)*inv_d.x);
    t_max = min(t_max,(box[isPositive[0]].x-o.x)*inv_d.x);
    //y
    t_min = max(t_min,(box[1-isPositive[1]].y-o.y)*inv_d.y);
    t_max = min(t_max,(box[isPositive[1]].y-o.y)*inv_d.y);
    //z
    t_min = max(t_min,(box[1-isPositive[2]].z-o.z)*inv_d.z);
    t_max = min(t_max,(box[isPositive[2]].z-o.z)*inv_d.z);

    return t_min<=t_max;
}


FINLINE int collide(const SoAPoint3f& o,const SoAVector3f& inv_d,float4 t_min,float4 t_max,const int isPositive[3],const SoABox& box){
    // x
    t_min = max(t_min,(box[1-isPositive[0]].xxxx-o.xxxx)*inv_d.xxxx);
    t_max = min(t_max,(box[isPositive[0]].xxxx-o.xxxx)*inv_d.xxxx);
    
    //y
    t_min = max(t_min,(box[1-isPositive[1]].yyyy-o.yyyy)*inv_d.yyyy);
    t_max = min(t_max,(box[isPositive[1]].yyyy-o.yyyy)*inv_d.yyyy);
    
    //z
    t_min = max(t_min,(box[1-isPositive[2]].zzzz-o.zzzz)*inv_d.zzzz);
    t_max = min(t_max,(box[isPositive[2]].zzzz-o.zzzz)*inv_d.zzzz);

    //check
    return movemask(t_min<=t_max);
}


// FINLINE int collide(const SoAPoint3f& o,const SoAVector3f& inv_d,float4 t_min,float4 t_max,const float4 isPositive[3],const SoABox& box){
//     // x
//     t_min = max(t_min,(select(isPositive[0],box[0].xxxx,box[1].xxxx)-o.xxxx)*inv_d.xxxx);
//     t_max = min(t_max,(select(isPositive[0],box[1].xxxx,box[0].xxxx)-o.xxxx)*inv_d.xxxx);
    
//     //y
//     t_min = max(t_min,(select(isPositive[1],box[0].yyyy,box[1].yyyy)-o.yyyy)*inv_d.yyyy);
//     t_max = min(t_max,(select(isPositive[1],box[1].yyyy,box[0].yyyy)-o.yyyy)*inv_d.yyyy);
    
//     //z
//     t_min = max(t_min,(select(isPositive[2],box[0].zzzz,box[1].zzzz)-o.yyyy)*inv_d.zzzz);
//     t_max = min(t_max,(select(isPositive[2],box[1].zzzz,box[0].zzzz)-o.yyyy)*inv_d.zzzz);

//     //check
//     return movemask(t_min<=t_max);
// }

NARUKAMI_END
