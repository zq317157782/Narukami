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
};

FINLINE Transform inverse(const Transform& transform){
    return Transform(transform.inv_mat,transform.mat);
}

FINLINE Transform transpose(const Transform& transform){
    return Transform(transpose(transform.mat),transpose(transform.inv_mat));
}

FINLINE Transform translate(const Vector3f& delta){
    Matrix4x4 mat(One,Zero,Zero,Zero, Zero,One,Zero, Zero,Zero,Zero, One,Zero,delta.x, delta.y,delta.z,Zero);
    Matrix4x4 inv_mat(One,Zero,Zero,Zero, Zero,One,Zero, Zero,Zero,Zero, One,Zero,-delta.x, -delta.y,-delta.z,Zero);
    return Transform(mat,inv_mat);
}

FINLINE Transform scale(const float x,const float y,const float z){
    Matrix4x4 mat(x,Zero,Zero,Zero, Zero,y,Zero, Zero,Zero,Zero, z,Zero,Zero, Zero,Zero,One);
    Matrix4x4 inv_mat(rcp(x),Zero,Zero,Zero, Zero,rcp(y),Zero, Zero,Zero,Zero, rcp(z),Zero,Zero, Zero,Zero,One);
    return Transform(mat,inv_mat);
}

//clockwise
FINLINE Transform rotate_x(const float theta){
    auto rad=deg2rad(theta);
    auto sin_theta=sin(rad);
    auto cos_theta=cos(rad);
    Matrix4x4 mat(One,Zero,Zero,Zero, Zero,cos_theta,sin_theta, Zero,Zero,-sin_theta, cos_theta,Zero,Zero, Zero,Zero,One);
    Matrix4x4 inv_mat(One,Zero,Zero,Zero, Zero,cos_theta,-sin_theta, Zero,Zero,sin_theta, cos_theta,Zero,Zero, Zero,Zero,One);
    return Transform(mat,inv_mat);
}

FINLINE Transform rotate_y(const float theta){
    auto rad=deg2rad(theta);
    auto sin_theta=sin(rad);
    auto cos_theta=cos(rad);
    Matrix4x4 mat(/*col0*/One,Zero,Zero,Zero, /*col1*/Zero,One,Zero,Zero,/*col2*/Zero,Zero, One,Zero,/*col3*/Zero, Zero,Zero,One);
    Matrix4x4 inv_mat(/*col0*/cos_theta,Zero,-sin_theta,Zero, /*col1*/Zero,One,Zero,Zero,/*col2*/sin_theta,Zero, cos_theta,Zero,/*col3*/Zero, Zero,Zero,One);
    return Transform(mat,inv_mat);
}

FINLINE Transform rotate_z(const float theta){
    auto rad=deg2rad(theta);
    auto sin_theta=sin(rad);
    auto cos_theta=cos(rad);
    Matrix4x4 mat(/*col0*/One,Zero,Zero,Zero, /*col1*/Zero,One,Zero,Zero,/*col2*/Zero,Zero, One,Zero,/*col3*/Zero, Zero,Zero,One);
    Matrix4x4 inv_mat(/*col0*/cos_theta,sin_theta,Zero,Zero, /*col1*/-sin_theta,cos_theta,Zero,Zero,/*col2*/Zero,Zero, One,Zero,/*col3*/Zero, Zero,Zero,One);
    return Transform(mat,inv_mat);
}

//http://ksuweb.kennesaw.edu/~plaval/math4490/rotgen.pdf
FINLINE Transform rotate(const float theta,const Vector3f& normalized_axis){
    auto rad=deg2rad(theta);
    auto sin_theta=sin(rad);
    auto cos_theta=cos(rad);

    auto a =normalized_axis;
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

NARUKAMI_END