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
#include "core/sse/sse.h"
#include "core/sse/float4.h"

NARUKAMI_BEGIN
//colmun major
struct SSE_ALIGNAS Matrix4x4{
public:
    union{
        float m[16];
        __m128 mVec[4];
    };
    typedef float Scalar;
    enum
    {
        N = 16
    };
public:
    Matrix4x4(){
         //col 0
        m[0]=One;
        m[1]=Zero;
        m[2]=Zero;
        m[3]=Zero;
        //col 1
        m[4]=Zero;
        m[5]=One;
        m[6]=Zero;
        m[7]=Zero;
        //col 2
        m[8]=Zero;
        m[9]=Zero;
        m[10]=One;
        m[11]=Zero;
        //col 3
        m[12]=Zero;
        m[13]=Zero;
        m[14]=Zero;
        m[15]=One;
    }
    Matrix4x4(const float m0,const float m1,const float m2,const float m3,const float m4,const float m5,const float m6,const float m7,const float m8,const float m9,const float m10,const float m11,const float m12,const float m13,const float m14,const float m15)
    {
        //col 0
        m[0]=m0;
        m[1]=m1;
        m[2]=m2;
        m[3]=m3;
        //col 1
        m[4]=m4;
        m[5]=m5;
        m[6]=m6;
        m[7]=m7;
        //col 2
        m[8]=m8;
        m[9]=m9;
        m[10]=m10;
        m[11]=m11;
        //col 3
        m[12]=m12;
        m[13]=m13;
        m[14]=m14;
        m[15]=m15;
    }

    Matrix4x4(const float* v){
        //col 0
        m[0]=v[0];
        m[1]=v[1];
        m[2]=v[2];
        m[3]=v[3];
        //col 1
        m[4]=v[4];
        m[5]=v[5];
        m[6]=v[6];
        m[7]=v[7];
        //col 2
        m[8]=v[8];
        m[9]=v[9];
        m[10]=v[10];
        m[11]=v[11];
        //col 3
        m[12]=v[12];
        m[13]=v[13];
        m[14]=v[14];
        m[15]=v[15];
    }
    FINLINE const float& operator[](const int idx) const { assert(idx >= 0 && idx < N); return m[idx];}
    FINLINE float& operator[](const int idx){ assert(idx >= 0 && idx < N); return m[idx];}
};

FINLINE std::ostream &operator<<(std::ostream &out, const Matrix4x4 &v){
    for(int i=0;i<Matrix4x4::N;++i){
        out<<v.m[i];
        out<<((i+1)==Matrix4x4::N)?')':',';
    }
    return out;
}

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

    return narukami::Vector3f(x,y,z);
}

NARUKAMI_END