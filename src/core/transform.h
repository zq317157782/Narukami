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
#include "core/geometry.h"
NARUKAMI_BEGIN

struct SSE_ALIGNAS Transform{
    Matrix4x4 mat;
    Matrix4x4 inv_mat;
    Transform(const Matrix4x4& mat):mat(mat),inv_mat(inverse(mat)){}
    Transform(const float* mat):mat(mat),inv_mat(inverse(this->mat)){}
    Transform(const Matrix4x4& mat,const Matrix4x4& inv_mat):mat(mat),inv_mat(inv_mat){}
    Transform(const float* mat,const float* inv_mat):mat(mat),inv_mat(inv_mat){}

    FINLINE Point3f operator()(const Point3f& p) const{ return mat*p; }
    FINLINE Vector3f operator()(const Vector3f& v) const{ return mat*v;}
    FINLINE Normal3f operator()(const Normal3f& n)const{ return transpose(inv_mat)*Vector3f(n);}
    FINLINE Transform operator()(const Transform& t) const{ return Transform(mat*t.mat,t.inv_mat*inv_mat); }
};

FINLINE Transform inverse(const Transform& transform){
    return Transform(transform.inv_mat,transform.mat);
}

FINLINE Transform transpose(const Transform& transform){
    return Transform(transpose(transform.mat),transpose(transform.inv_mat));
}

FINLINE Transform translate(const Vector3f& delta){
    Matrix4x4 mat(1.0f,0.0f,0.0f,0.0f, 0.0f,1.0f,0.0f, 0.0f,0.0f,0.0f, 1.0f,0.0f,delta.x, delta.y,delta.z,1.0f);
    Matrix4x4 inv_mat(1.0f,0.0f,0.0f,0.0f, 0.0f,1.0f,0.0f, 0.0f,0.0f,0.0f, 1.0f,0.0f,-delta.x, -delta.y,-delta.z,1.0f);
    return Transform(mat,inv_mat);
}

FINLINE Transform scale(const float x,const float y,const float z){
    Matrix4x4 mat(x,0.0f,0.0f,0.0f, 0.0f,y,0.0f, 0.0f,0.0f,0.0f, z,0.0f,0.0f, 0.0f,0.0f,1.0f);
    Matrix4x4 inv_mat(rcp(x),0.0f,0.0f,0.0f, 0.0f,rcp(y),0.0f, 0.0f,0.0f,0.0f, rcp(z),0.0f,0.0f, 0.0f,0.0f,1.0f);
    return Transform(mat,inv_mat);
}

//clockwise
FINLINE Transform rotate_x(const float theta){
    auto rad=deg2rad(theta);
    auto sin_theta=sin(rad);
    auto cos_theta=cos(rad);
    Matrix4x4 mat(1.0f,0.0f,0.0f,0.0f, 0.0f,cos_theta,sin_theta, 0.0f,0.0f,-sin_theta, cos_theta,0.0f,0.0f, 0.0f,0.0f,1.0f);
    //Matrix4x4 inv_mat(1.0f,0.0f,0.0f,0.0f, 0.0f,cos_theta,-sin_theta, 0.0f,0.0f,sin_theta, cos_theta,0.0f,0.0f, 0.0f,0.0f,1.0f);
    return Transform(mat,transpose(mat));
}
//clockwise
FINLINE Transform rotate_y(const float theta){
    auto rad=deg2rad(theta);
    auto sin_theta=sin(rad);
    auto cos_theta=cos(rad);
    Matrix4x4 mat(/*col0*/cos_theta,0.0f,-sin_theta,0.0f, /*col1*/0.0f,1.0f,0.0f,0.0f,/*col2*/sin_theta,0.0f, cos_theta,0.0f,/*col3*/0.0f, 0.0f,0.0f,1.0f);

    return Transform(mat,transpose(mat));
}
//clockwise
FINLINE Transform rotate_z(const float theta){
    auto rad=deg2rad(theta);
    auto sin_theta=sin(rad);
    auto cos_theta=cos(rad);
    Matrix4x4 mat(/*col0*/cos_theta,sin_theta,0.0f,0.0f, /*col1*/-sin_theta,cos_theta,0.0f,0.0f,/*col2*/0.0f,0.0f, 1.0f,0.0f,/*col3*/0.0f, 0.0f,0.0f,1.0f);
    return Transform(mat,transpose(mat));
}

//http://ksuweb.kennesaw.edu/~plaval/math4490/rotgen.pdf
//clockwise
FINLINE Transform rotate(const float theta,const Vector3f& axis){
    auto rad=deg2rad(theta);
    auto sin_theta=sin(rad);
    auto cos_theta=cos(rad);

    auto a =normalize(axis);
    auto t = (1.0f-cos_theta);

    Matrix4x4 mat;
    //col0
    mat[0] = t*a.x*a.x+cos_theta;
    mat[1] = t*a.x*a.y+sin_theta*a.z;
    mat[2] = t*a.x*a.z-sin_theta*a.y;
    

    //col1
    mat[4] = t*a.x*a.y-sin_theta*a.z;
    mat[5] = t*a.y*a.y+cos_theta;
    mat[6] = t*a.y*a.z+sin_theta*a.x;
    

    //col2
    mat[8] = t*a.x*a.z+sin_theta*a.y;
    mat[9] = t*a.y*a.z-sin_theta*a.x;
    mat[10] = t*a.z*a.z+cos_theta;
    

    return Transform(mat,transpose(mat));
}

FINLINE Transform look_at(const Point3f& o,const Point3f& target,const Vector3f& up){
    auto forward=normalize(target-o);
    auto right=cross(normalize(up),forward);
    auto newUp=cross(forward,right);

    Matrix4x4 cam2wrold;
    //right
    cam2wrold.m[0] = right.x;
    cam2wrold.m[1] = right.y;
    cam2wrold.m[2] = right.z;
    cam2wrold.m[3] = 0.0f;
    //newUp
    cam2wrold.m[4] = newUp.x;
    cam2wrold.m[5] = newUp.y;
    cam2wrold.m[6] = newUp.z;
    cam2wrold.m[7] = 0.0f;
    //forward
    cam2wrold.m[8]  = forward.x;
    cam2wrold.m[9]  = forward.y;
    cam2wrold.m[10] = forward.z;
    cam2wrold.m[11] = 0.0f;
    //pos
    cam2wrold.m[12]  = o.x;
    cam2wrold.m[13]  = o.y;
    cam2wrold.m[14]  = o.z;
    cam2wrold.m[15]  = 1.0f;
    return Transform(transform_inverse_noscale(cam2wrold),cam2wrold);
}

FINLINE bool swap_handedness(const Transform& t){
    return (sub_matrix3x3_determinant(t.mat)<0.0f);
}

NARUKAMI_END