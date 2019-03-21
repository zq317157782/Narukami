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
#include "core/sse/sse.h"
#include "core/sse/float4.h"
#include "core/math/math.h"
#include "core/math/vector2.h"
#include "core/math/vector3.h"
#include "core/math/point3.h"
#include "core/math/matrix4x4.h"
NARUKAMI_BEGIN
template <typename T>
FINLINE Point3<T> operator+(const Point3<T> &p, const Vector3<T> &v) { Point3<T> rp; rp.x = p.x + v.x; rp.y = p.y + v.y; rp.z = p.z + v.z; return rp; }
template <typename T>
FINLINE Point3<T> operator-(const Point3<T> &p, const Vector3<T> &v) {return p+(-v);}
template <typename T>
FINLINE Vector3<T> operator-(const Point3<T> &p1, const Point3<T> &p2) { Vector3<T> v; p.x = p1.x - p2.x; v.y = p1.y - p2.y; v.z = p1.z - p2.z; return v; }


FINLINE SSEVector3f operator+(const SSEPoint3f&  p ,const SSEVector3f& v ){ return _mm_add_ps(p,v); }
FINLINE SSEVector3f operator-(const SSEPoint3f&  p ,const SSEVector3f& v ){ return _mm_sub_ps(p,v); }
FINLINE SSEVector3f operator-(const SSEPoint3f& p1,const SSEPoint3f& p2){ return _mm_sub_ps(p1,p2); }


FINLINE Vector3f operator*(const Matrix4x4& M,const Vector3f& v){
    // 8ns   
    // float4 r=float4(M.mVec[0])*float4(v.x);
    // r+=float4(M.mVec[1])*float4(v.y);
    // r+=float4(M.mVec[2])*float4(v.z);
    // return Vector3f(r.x,r.y,r.z);
    
    // 7ns
    float x =  M.m[0]*v.x+M.m[4]*v.y+M.m[8]*v.z;
    float y =  M.m[1]*v.x+M.m[5]*v.y+M.m[9]*v.z;
    float z =  M.m[3]*v.x+M.m[6]*v.y+M.m[10]*v.z;

    return Vector3f(x,y,z);
}

FINLINE Point3f operator*(const Matrix4x4& M,const Point3f& v){
    float4 r=float4(M.mVec[0])*float4(v.x);
    r+=float4(M.mVec[1])*float4(v.y);
    r+=float4(M.mVec[2])*float4(v.z);
    r+=float4(M.mVec[3])*float4(One);
    return Point3f(r.x,r.y,r.z);
}

FINLINE SSEVector3f operator*(const Matrix4x4& M,const SSEVector3f& v){
    float4 vx=swizzle<0,0,0,0>(v.xyzw);
    float4 vy=swizzle<1,1,1,1>(v.xyzw);
    float4 vz=swizzle<2,2,2,2>(v.xyzw);
    
    float4 r=float4(M.mVec[0])*vx;
    r+=float4(M.mVec[1])*vy;
    r+=float4(M.mVec[2])*vz;
    return SSEVector3f(r.xyzw);
}


FINLINE SoAVector3f operator*(const Matrix4x4& M,const SoAVector3f& v){
    float4 r_xxxx=    swizzle<0,0,0,0>(M.mVec[0])/*m00*/*v.xxxx;
    r_xxxx = r_xxxx + swizzle<0,0,0,0>(M.mVec[1])/*m10*/*v.yyyy;
    r_xxxx = r_xxxx + swizzle<0,0,0,0>(M.mVec[2])/*m20*/*v.zzzz;

    float4 r_yyyy=    swizzle<1,1,1,1>(M.mVec[0])/*m01*/*v.xxxx;
    r_yyyy = r_yyyy + swizzle<1,1,1,1>(M.mVec[1])/*m11*/*v.yyyy;
    r_yyyy = r_yyyy + swizzle<1,1,1,1>(M.mVec[2])/*m21*/*v.zzzz;

    float4 r_zzzz=    swizzle<2,2,2,2>(M.mVec[0])/*m02*/*v.xxxx;
    r_zzzz = r_zzzz + swizzle<2,2,2,2>(M.mVec[1])/*m12*/*v.yyyy;
    r_zzzz = r_zzzz + swizzle<2,2,2,2>(M.mVec[2])/*m22*/*v.zzzz;

    return SoAVector3f(r_xxxx,r_yyyy,r_zzzz);
}

NARUKAMI_END
