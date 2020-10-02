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
#include "core/geometry.h"
#include "core/geometry.h"
#include "core/quaternion.h"
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
    inline Point3fPack operator()(const Point3fPack &p) const { return mat * p; }
    inline Vector3f operator()(const Vector3f &v) const { return mat * v; }
    inline Vector3fPack operator()(const Vector3fPack &v) const { return mat * v; }
    inline Normal3f operator()(const Normal3f &n) const { return transpose(inv_mat) * Vector3f(n); }
    Ray operator()(const Ray &ray) const;
    RayDifferential operator()(const RayDifferential &ray) const;
    Bounds3f operator()(const Bounds3f &b) const;
    Interaction operator()(const Interaction &i) const;
    SurfaceInteraction operator()(const SurfaceInteraction &i) const;
    inline Transform operator()(const Transform &t) const { return Transform(mat * t.mat, t.inv_mat * inv_mat); }

    void *operator new(size_t size);
    void operator delete(void *ptr);
};

inline std::ostream &operator<<(std::ostream &out, const Transform &t)
{
    out << t.mat;
    return out;
}

inline Point3f transform_4x4(const Transform &t, const Point3f &p)
{
    return mul_4x4(t.mat, p);
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
    Matrix4x4 inv_mat(1.0f / x, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f / y, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f / z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    return Transform(mat, inv_mat);
}

inline Transform scale(const Vector3f &vec)
{
    return scale(vec.x, vec.y, vec.z);
}

//counterclockwise
inline Transform rotate_x(const float theta)
{
    auto rad = deg2rad(theta);
    auto sin_theta = sin(rad);
    auto cos_theta = cos(rad);
    Matrix4x4 mat(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cos_theta, -sin_theta, 0.0f, 0.0f, sin_theta, cos_theta, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    //Matrix4x4 inv_mat(1.0f,0.0f,0.0f,0.0f, 0.0f,cos_theta,-sin_theta, 0.0f,0.0f,sin_theta, cos_theta,0.0f,0.0f, 0.0f,0.0f,1.0f);
    return Transform(mat, transpose(mat));
}
//counterclockwise
inline Transform rotate_y(const float theta)
{
    auto rad = deg2rad(theta);
    auto sin_theta = sin(rad);
    auto cos_theta = cos(rad);
    Matrix4x4 mat(/*col0*/ cos_theta, 0.0f, sin_theta, 0.0f, /*col1*/ 0.0f, 1.0f, 0.0f, 0.0f, /*col2*/ -sin_theta, 0.0f, cos_theta, 0.0f, /*col3*/ 0.0f, 0.0f, 0.0f, 1.0f);

    return Transform(mat, transpose(mat));
}
//counterclockwise
inline Transform rotate_z(const float theta)
{
    auto rad = deg2rad(theta);
    auto sin_theta = sin(rad);
    auto cos_theta = cos(rad);
    Matrix4x4 mat(/*col0*/ cos_theta, -sin_theta, 0.0f, 0.0f, /*col1*/ sin_theta, cos_theta, 0.0f, 0.0f, /*col2*/ 0.0f, 0.0f, 1.0f, 0.0f, /*col3*/ 0.0f, 0.0f, 0.0f, 1.0f);
    return Transform(mat, transpose(mat));
}

//http://ksuweb.kennesaw.edu/~plaval/math4490/rotgen.pdf 
//it's RGS
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
    //因为这个算法是RHS的，所以需要交换逆和原来矩阵的位置，逆矩阵才是我们需要的变换
    return Transform(transpose(mat),mat);
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
    return Transform(cam2wrold, transform_inverse_noscale(cam2wrold));
}

inline Transform look_at(float eye_x, float eye_y, float eye_z, float c_x, float c_y, float c_z, float up_x, float up_y, float up_z)
{
    return look_at(Point3f(eye_x, eye_y, eye_z), Point3f(c_x, c_y, c_z), Vector3f(up_x, up_y, up_z));
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
inline Transform perspective(float fov, float n, float f)
{
    Matrix4x4 m = {
        1.0f, 0.0f, 0.0f, 0.0f,            //col0
        0.0f, 1.0f, 0.0f, 0.0f,            //col1
        0.0f, 0.0f, f / (f - n), 1.0f,     //col2
        0.0f, 0.0f, -f * n / (f - n), 0.0f //col3
    };

    float inv_tan_fov = 1.0f / tan(deg2rad(fov / 2));
    return scale(inv_tan_fov, inv_tan_fov, 1.0f) * Transform(m);
}


class AnimatedTransform
{
private:
    float _start_time;
    float _end_time;
    shared<Transform> _t1;
    shared<Transform> _t2;
    Vector3f T[2];
    Quaternion R[2];
    Matrix4x4 S[2];

    bool _has_animation;
    bool _has_scale;
    bool _has_rotation;

public:
    AnimatedTransform(const shared<Transform>& t) : _start_time(0), _end_time(0), _t1(t), _t2(t)
    {
        _has_animation = false;
        _has_rotation = false;
        _has_scale = false;
    }
    AnimatedTransform(const shared<Transform>& t1, float start_time, const shared<Transform>& t2, float end_time) : _start_time(start_time), _end_time(end_time), _t1(t1), _t2(t2)
    {

        if (t1->mat == t2->mat)
        {
            _has_animation = false;
            _has_rotation = false;
            _has_scale = false;
        }
        else
        {
            _has_animation = true;
            polar_decompose(t1->mat, &T[0], &R[0], &S[0]);
            polar_decompose(t2->mat, &T[1], &R[1], &S[1]);
            _has_scale = (S[0] == S[1]) ? false : true;
            _has_rotation = (R[0] == R[1]) ? false : true;
        }

        if (dot(R[0], R[1]) < 0.0f)
        {
            R[1] = -R[1];
        }
    }

    void interpolate(float time, Transform *t) const
    {

        if (_has_animation)
        {
            if (time <= _start_time)
            {
                (*t) = (*_t1);
                return;
            }
            else if (time >= _end_time)
            {
                (*t) = (*_t2);
                return;
            }

            float dt = (time - _start_time) / (_end_time - _start_time);

            Vector3f trans = (1 - dt) * T[0] + dt * T[1];
            Matrix4x4 scale;
            if (_has_scale)
            {
                for (int i = 0; i < 3; ++i)
                {
                    for (int j = 0; j < 3; ++j)
                    {
                        scale.mn[i][j] = lerp(S[0].mn[i][j], S[1].mn[i][j], dt);
                    }
                }
            }

            Matrix4x4 trans_mat(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, trans.x, trans.y, trans.z, 1.0f);
            Matrix4x4 rotate_mat;
            if (_has_rotation)
            {
                rotate_mat = to_matrix(slerp(R[0], R[1], dt));
            }
            
            Matrix4x4 mat = trans_mat * rotate_mat * scale;
            *t = Transform(mat);
        }
        else
        {
            (*t) = (*_t1);
            return;
        }
    }

    Bounds3f motion_bounds(const Bounds3f &b) const
    {

        if (!_has_animation)
        {
            return (*_t1)(b);
        }
        else
        {
            const float step = (_end_time - _start_time) / 1024;

            Bounds3f ret;
            for (int i = 0; i < 1024; ++i)
            {
                float time = _start_time + i * step;
                Transform t;
                interpolate(time, &t);
                ret = _union(ret, t(b));
            }
            return ret;
        }
    }

    Bounds3f operator()(float time, const Bounds3f &b) const
    {
        Transform t;
        interpolate(time, &t);
        return t(b);
    }

    inline Point3f operator()(float time, const Point3f &p) const
    {
        Transform t;
        interpolate(time, &t);
        return t(p);
    }

    inline Point3fPack operator()(float time, const Point3fPack &p) const
    {
        Transform t;
        interpolate(time, &t);
        return t(p);
    }

    inline Vector3f operator()(float time, const Vector3f &v) const
    {
        Transform t;
        interpolate(time, &t);
        return t(v);
    }
    inline Vector3fPack operator()(float time, const Vector3fPack &v) const
    {
        Transform t;
        interpolate(time, &t);
        return t(v);
    }
    inline Normal3f operator()(float time, const Normal3f &n) const
    {
        Transform t;
        interpolate(time, &t);
        return t(n);
    }
    inline Ray operator()(float time, const Ray &ray) const
    {
        Transform t;
        interpolate(time, &t);
        return t(ray);
    }
    Interaction operator()(float time, const Interaction &i) const;
    SurfaceInteraction operator()(float time, const SurfaceInteraction &i) const;

    Bounds3f operator()(const Bounds3f &b) const
    {
        return motion_bounds(b);
    }

    bool has_animation() const
    {
        return _has_animation;
    }

    void *operator new(size_t size);
    void operator delete(void *ptr);
};

NARUKAMI_END