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
#include "core/affine.h"
#include "core/geometry.h"
#include "core/memory.h"
NARUKAMI_BEGIN

struct SSE_ALIGNAS Transform
{
    Matrix4x4 mat;
    Matrix4x4 inv_mat;
    inline Transform() : mat(Matrix4x4()), inv_mat(Matrix4x4()) {}
    inline Transform(const Matrix4x4 &mat) : mat(mat), inv_mat(inverse(mat)) {}
    inline Transform(const float *mat) : mat(mat), inv_mat(inverse(this->mat)) {}
    inline Transform(const Matrix4x4 &mat, const Matrix4x4 &inv_mat) : mat(mat), inv_mat(inv_mat) {}
    inline Transform(const float *mat, const float *inv_mat) : mat(mat), inv_mat(inv_mat) {}
    inline Point3f operator()(const Point3f &p) const { return mat * p; }
    inline Point3f4p operator()(const Point3f4p &p) const { return mat * p; }
    inline Vector3f operator()(const Vector3f &v) const { return mat * v; }
    inline Vector3f4p operator()(const Vector3f4p &v) const { return mat * v; }
    inline Normal3f operator()(const Normal3f &n) const { return transpose(inv_mat) * Vector3f(n); }
    Ray operator()(const Ray &ray) const;
    Bounds3f operator()(const Bounds3f &b) const;
    Interaction operator()(const Interaction &i) const;
    inline Transform operator()(const Transform &t) const { return Transform(mat * t.mat, t.inv_mat * inv_mat); }

    void * operator new(size_t size);
    void  operator delete(void * ptr);
};

inline std::ostream &operator<<(std::ostream &out, const Transform &t)
{
    out << t.mat;
    return out;
}

inline Point3f transform_h(const Transform &t,const Point3f &p)
{
    return mul_h(t.mat,p);
}

inline Transform identity()
{
    return Transform();
}

inline Transform inverse(const Transform &transform)
{
    return Transform(transform.inv_mat, transform.mat);
}

inline Transform transpose(const Transform &transform)
{
    return Transform(transpose(transform.mat), transpose(transform.inv_mat));
}

inline Transform translate(const float delta_x, const float delta_y, const float delta_z)
{
    Matrix4x4 mat(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, delta_x, delta_y, delta_z, 1.0f);
    Matrix4x4 inv_mat(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -delta_x, -delta_y, -delta_z, 1.0f);
    return Transform(mat, inv_mat);
}

inline Transform translate(const Vector3f &delta)
{
    return translate(delta.x, delta.y, delta.z);
}

inline Transform scale(const float x, const float y, const float z)
{
    Matrix4x4 mat(x, 0.0f, 0.0f, 0.0f, 0.0f, y, 0.0f, 0.0f, 0.0f, 0.0f, z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    Matrix4x4 inv_mat(rcp(x), 0.0f, 0.0f, 0.0f, 0.0f, rcp(y), 0.0f, 0.0f, 0.0f, 0.0f, rcp(z), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    return Transform(mat, inv_mat);
}

inline Transform scale(const Vector3f &vec)
{
    return scale(vec.x, vec.y, vec.z);
}

//clockwise
inline Transform rotate_x(const float theta)
{
    auto rad = deg2rad(theta);
    auto sin_theta = sin(rad);
    auto cos_theta = cos(rad);
    Matrix4x4 mat(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cos_theta, sin_theta, 0.0f, 0.0f, -sin_theta, cos_theta, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    //Matrix4x4 inv_mat(1.0f,0.0f,0.0f,0.0f, 0.0f,cos_theta,-sin_theta, 0.0f,0.0f,sin_theta, cos_theta,0.0f,0.0f, 0.0f,0.0f,1.0f);
    return Transform(mat, transpose(mat));
}
//clockwise
inline Transform rotate_y(const float theta)
{
    auto rad = deg2rad(theta);
    auto sin_theta = sin(rad);
    auto cos_theta = cos(rad);
    Matrix4x4 mat(/*col0*/ cos_theta, 0.0f, -sin_theta, 0.0f, /*col1*/ 0.0f, 1.0f, 0.0f, 0.0f, /*col2*/ sin_theta, 0.0f, cos_theta, 0.0f, /*col3*/ 0.0f, 0.0f, 0.0f, 1.0f);

    return Transform(mat, transpose(mat));
}
//clockwise
inline Transform rotate_z(const float theta)
{
    auto rad = deg2rad(theta);
    auto sin_theta = sin(rad);
    auto cos_theta = cos(rad);
    Matrix4x4 mat(/*col0*/ cos_theta, sin_theta, 0.0f, 0.0f, /*col1*/ -sin_theta, cos_theta, 0.0f, 0.0f, /*col2*/ 0.0f, 0.0f, 1.0f, 0.0f, /*col3*/ 0.0f, 0.0f, 0.0f, 1.0f);
    return Transform(mat, transpose(mat));
}

//http://ksuweb.kennesaw.edu/~plaval/math4490/rotgen.pdf
//clockwise

inline Transform rotate(const float theta, const Vector3f &axis)
{
    auto rad = deg2rad(theta);
    auto sin_theta = sin(rad);
    auto cos_theta = cos(rad);

    auto a = normalize(axis);
    auto t = (1.0f - cos_theta);

    Matrix4x4 mat;
    //col0
    mat[0] = t * a.x * a.x + cos_theta;
    mat[1] = t * a.x * a.y + sin_theta * a.z;
    mat[2] = t * a.x * a.z - sin_theta * a.y;

    //col1
    mat[4] = t * a.x * a.y - sin_theta * a.z;
    mat[5] = t * a.y * a.y + cos_theta;
    mat[6] = t * a.y * a.z + sin_theta * a.x;

    //col2
    mat[8] = t * a.x * a.z + sin_theta * a.y;
    mat[9] = t * a.y * a.z - sin_theta * a.x;
    mat[10] = t * a.z * a.z + cos_theta;

    return Transform(mat, transpose(mat));
}

inline Transform rotate(const float theta, const float axis_x, const float axis_y, const float axis_z)
{
    return rotate(theta, Vector3f(axis_x, axis_y, axis_z));
}


inline Transform look_at(const Point3f &o, const Point3f &target, const Vector3f &up)
{
    auto forward = normalize(target - o);
    auto right = cross(normalize(up), forward);
    auto newUp = cross(forward, right);

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
    cam2wrold.m[8] = forward.x;
    cam2wrold.m[9] = forward.y;
    cam2wrold.m[10] = forward.z;
    cam2wrold.m[11] = 0.0f;
    //pos
    cam2wrold.m[12] = o.x;
    cam2wrold.m[13] = o.y;
    cam2wrold.m[14] = o.z;
    cam2wrold.m[15] = 1.0f;
    return Transform(cam2wrold,transform_inverse_noscale(cam2wrold));
}

inline Transform look_at(float eye_x,float eye_y,float eye_z,float c_x,float c_y,float c_z,float up_x,float up_y,float up_z)
{
    return look_at(Point3f(eye_x,eye_y,eye_z),Point3f(c_x,c_y,c_z),Vector3f(up_x,up_y,up_z));
}

inline bool swap_handedness(const Transform &t)
{
    return (sub_matrix3x3_determinant(t.mat) < 0.0f);
}

inline Transform operator*(const Transform &t1, const Transform &t2) { return t1(t2); }

inline Transform orthographic(float near, float far)
{
    return scale(1.0f, 1.0f, 1.0f / (far - near)) * translate(Vector3f(0, 0, -near));
}

//fov (angle space)
inline Transform perspective(float fov,float n, float f)
{
    Matrix4x4 m = {
        1.0f, 0.0f, 0.0f, 0.0f,            //col0
        0.0f, 1.0f, 0.0f, 0.0f,            //col1
        0.0f, 0.0f, f / (f - n), 1.0f,     //col2
        0.0f, 0.0f, -f * n / (f - n), 0.0f //col3
    };

    float inv_tan_fov = 1.0f/std::tan(deg2rad(fov/2));
    return scale(inv_tan_fov,inv_tan_fov,1.0f) * Transform(m);
}

inline ref<Transform> ref_cast(const Transform& t)
{
    Transform* ptr = new Transform(t);
    return ref<Transform>(ptr);
}

extern const Transform IDENTITY;

NARUKAMI_END