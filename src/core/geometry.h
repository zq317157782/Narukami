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
#include "core/simd.h"
#include <vector>
NARUKAMI_BEGIN

struct Ray
{
    Point3f o;
    Vector3f d;
    mutable float t_max;

    inline Ray() : o(Point3f(0, 0, 0)), d(Vector3f(0, 0, 1)), t_max(INFINITE) {}
    inline Ray(const Point3f &o, const Vector3f &d, const float t_max = INFINITE) : o(o), d(d), t_max(t_max) {}
};
inline std::ostream &operator<<(std::ostream &out, const Ray &ray)
{
    out << "[o:" << ray.o << " d:" << ray.d << " t:" << ray.t_max << "]";
    return out;
}

// Normal points outward for rays exiting the surface, else is flipped
// "Realtime Ray Tracing Gems" chapter 6
inline Ray offset_ray(const Ray &ray, const Normal3f &n)
{
    constexpr float origin = 1.0f / 32.0f;
    constexpr float int_scale = 256.0f;
    constexpr float float_scale = 1.0f / 65536.0f;
    Point3i of_i(static_cast<int>(int_scale * n.x), static_cast<int>(int_scale * n.y), static_cast<int>(int_scale * n.z));
    Point3f p_i(cast_i2f(cast_f2i(ray.o.x) + ((ray.o.x < 0) ? -of_i.x : of_i.x)),
                cast_i2f(cast_f2i(ray.o.y) + ((ray.o.y < 0) ? -of_i.y : of_i.y)),
                cast_i2f(cast_f2i(ray.o.z) + ((ray.o.z < 0) ? -of_i.z : of_i.z)));
    Point3f o_offseted(fabsf(ray.o.x) < origin ? ray.o.x + float_scale * n.x : p_i.x,
                       fabsf(ray.o.y) < origin ? ray.o.y + float_scale * n.y : p_i.y,
                       fabsf(ray.o.z) < origin ? ray.o.z + float_scale * n.z : p_i.z);
    float epsion = length(o_offseted - ray.o);
    return Ray(o_offseted,ray.d,max(0.0f,ray.t_max-epsion));
}

struct SSE_ALIGNAS SoARay
{
    SoAPoint3f o;
    SoAVector3f d;
    mutable float4 t_max;

    inline SoARay(const Point3f &o, const Vector3f &d, const float t_max = INFINITE) : o(SoAPoint3f(o)), d(SoAVector3f(d)), t_max(t_max) {}
    inline explicit SoARay(const Ray &ray) : o(ray.o), d(ray.d), t_max(ray.t_max) {}
};
inline std::ostream &operator<<(std::ostream &out, const SoARay &ray)
{
    out << "[o:" << ray.o << " d:" << ray.d << " t:" << float4(ray.t_max) << "]";
    return out;
}

struct Triangle
{
    Point3f v0;
    Vector3f e1;
    Vector3f e2;
};

inline Normal3f get_unnormalized_normal(const Triangle &tri)
{
    return cross(tri.e1, tri.e2);
}

inline Normal3f get_normalized_normal(const Triangle &tri)
{
    return normalize(cross(tri.e1, tri.e2));
}

inline Point3f barycentric_interpolate_position(const Triangle &tri, const Point2f &uv)
{
    return tri.v0 + tri.e1 * uv.x + tri.e2 * uv.y;
}


inline bool is_degraded(const Triangle &tri)
{
    float inner_product = dot(tri.e1,tri.e2);
    //TODO can simper?
    if(inner_product==0||inner_product==1||inner_product==-1)
    {
        return true;
    }
    return false;
}

inline float area(const Triangle &tri)
{
    return length(cross(tri.e1,tri.e2))/2;
}

inline std::ostream &operator<<(std::ostream &out, const Triangle &triangle)
{
    out << "[v0:" << triangle.v0 << " e1:" << triangle.e1 << " e2:" << triangle.e2 << "]";
    return out;
}

//v0(128*3)+e1(128*3)+e2(128*3)
struct SSE_ALIGNAS SoATriangle
{
    SoAPoint3f v0;
    SoAVector3f e1;
    SoAVector3f e2;

    Triangle operator[](const uint32_t idx) const
    {
        assert(idx >= 0 && idx < SSE_FLOAT_COUNT);
        Triangle triangle;
        triangle.v0 = v0[idx];
        triangle.e1 = e1[idx];
        triangle.e2 = e2[idx];
        return triangle;
    }
};

inline std::ostream &operator<<(std::ostream &out, const SoATriangle &triangle)
{
    out << "[v0:" << triangle.v0 << " e1:" << triangle.e1 << " e2:" << triangle.e2 << "]";
    return out;
}

template <typename T>
struct Bounds2
{
    Point2<T> min_point;
    Point2<T> max_point;
    inline const Point2<T> &operator[](const int idx) const
    {
        assert(idx >= 0 && idx < 2);
        return (&min_point)[idx];
    }
    inline Point2<T> &operator[](const int idx)
    {
        assert(idx >= 0 && idx < 2);
        return (&min_point)[idx];
    }

    inline Bounds2()
    {
        T min_value = std::numeric_limits<T>::lowest();
        T max_value = std::numeric_limits<T>::max();
        min_point = Point2<T>(max_value, max_value);
        max_point = Point2<T>(min_value, min_value);
    }

    inline Bounds2(const Point2<T> &p0, const Point2<T> &p1)
    {
        min_point = min(p0, p1);
        max_point = max(p0, p1);
    }

    template <typename U>
    inline explicit Bounds2(const Bounds2<U> &bounds)
    {
        min_point = Point2<T>(bounds.min_point);
        max_point = Point2<T>(bounds.max_point);
    }
};
template <typename T>
inline std::ostream &operator<<(std::ostream &out, const Bounds2<T> &box)
{
    out << "[min point:" << box.min_point << " max point:" << box.max_point << "]";
    return out;
}

template <typename T>
inline T area(const Bounds2<T> &bounds)
{
    T w = bounds.max_point.x - bounds.min_point.x;
    T h = bounds.max_point.y - bounds.min_point.y;
    return w * h;
}

template <typename T>
inline Vector2<T> diagonal(const Bounds2<T> &bounds)
{
    T w = bounds.max_point.x - bounds.min_point.x;
    T h = bounds.max_point.y - bounds.min_point.y;
    return Vector2<T>(w,h);
}


template <typename T>
inline bool inside_exclusive(const Point2<T> &p, const Bounds2<T> &bounds)
{
    bool cond_x = p.x >= bounds.min_point.x && p.x < bounds.max_point.x;
    bool cond_y = p.y >= bounds.min_point.y && p.y < bounds.max_point.y;
    return (cond_x && cond_y);
}

template <typename T>
inline Bounds2<T> intersect(const Bounds2<T> &b0,const Bounds2<T> &b1) 
{ 
    return Bounds2<T>(max(b0.min_point,b1.min_point),min(b0.max_point,b1.max_point));
}

template <typename T>
inline T width(const Bounds2<T> &bounds) { return bounds.max_point.x - bounds.min_point.x; }
template <typename T>
inline T height(const Bounds2<T> &bounds) { return bounds.max_point.y - bounds.min_point.y; }

typedef Bounds2<float> Bounds2f;
typedef Bounds2<int> Bounds2i;

// from PBRT
class Bounds2iIterator : public std::forward_iterator_tag
{
public:
    inline Bounds2iIterator(const Bounds2i &b, const Point2i &pt) : _p(pt), _bounds(&b)
    {
    }
    inline Bounds2iIterator operator++()
    {
        advance();
        return *this;
    }
    inline Bounds2iIterator operator++(int)
    {
        Bounds2iIterator old = *this;
        advance();
        return old;
    }
    inline bool operator==(const Bounds2iIterator &bi) const
    {
        return _p == bi._p && _bounds == bi._bounds;
    }
    inline bool operator!=(const Bounds2iIterator &bi) const
    {
        return _p != bi._p || _bounds != bi._bounds;
    }

    inline Point2i operator*() const
    {
        return _p;
    }

private:
    inline void advance()
    {
        ++_p.x;
        if (_p.x == _bounds->max_point.x)
        {
            _p.x = _bounds->min_point.x;
            ++_p.y;
        }
    }
    Point2i _p;
    const Bounds2i *_bounds;
};

inline Bounds2iIterator begin(const Bounds2i &b)
{
    return Bounds2iIterator(b, b.min_point);
}

inline Bounds2iIterator end(const Bounds2i &b)
{
    Point2i pEnd(b.min_point.x, b.max_point.y);
    if (b.min_point.x >= b.max_point.x || b.min_point.y >= b.max_point.y)
        pEnd = b.min_point;
    return Bounds2iIterator(b, pEnd);
}

template <typename T>
struct Bounds3
{
    Point3<T> min_point;
    Point3<T> max_point;
    inline const Point3<T> &operator[](const int idx) const
    {
        assert(idx >= 0 && idx < 2);
        return (&min_point)[idx];
    }

    inline Bounds3()
    {
        T min_value = std::numeric_limits<T>::lowest();
        T max_value = std::numeric_limits<T>::max();
        min_point = Point3<T>(max_value, max_value, max_value);
        max_point = Point3<T>(min_value, min_value, min_value);
    }

    inline Bounds3(const Point3<T> &p0, const Point3<T> &p1)
    {
        min_point = min(p0, p1);
        max_point = max(p0, p1);
    }
};

template <typename T>
inline std::ostream &operator<<(std::ostream &out, const Bounds3<T> &box)
{
    out << "[min point:" << box.min_point << " max point:" << box.max_point << "]";
    return out;
}

typedef Bounds3<float> Bounds3f;
typedef Bounds3<int> Bounds3i;

template <typename T>
inline bool operator==(const Bounds3<T> &b0, const Bounds3<T> &b1)
{
    return (b0.min_point == b1.min_point) && (b0.max_point == b1.max_point);
}
template <typename T>
inline bool operator!=(const Bounds3<T> &b0, const Bounds3<T> &b1)
{
    return (b0.min_point != b1.min_point) || (b0.max_point != b1.max_point);
}

template <typename T>
inline Bounds3<T> _union(const Point3<T> &p0, const Point3<T> &p1)
{
    return Bounds3<T>(p0, p1);
}
template <typename T>
inline Bounds3<T> _union(const Bounds3<T> &b0, const Point3<T> &p1)
{
    return Bounds3<T>(min(b0.min_point, p1), max(b0.max_point, p1));
}
template <typename T>
inline Bounds3<T> _union(const Point3<T> &p1, const Bounds3<T> &b0)
{
    return _union(b0, p1);
}
template <typename T>
inline Bounds3<T> _union(const Bounds3<T> &b0, const Bounds3<T> &b1)
{
    return _union(min(b0.min_point, b1.min_point), max(b0.max_point, b1.max_point));
}

template <typename T>
inline Bounds3<T> intersect(const Bounds3<T> &b0,const Bounds3<T> &b1) 
{ 
    return Bounds3<T>(max(b0.min_point,b1.min_point),min(b0.max_point,b1.max_point));
}

template <typename T>
inline int max_extent(const Bounds3<T> &b0)
{
    Vector3<T> v = b0.max_point - b0.min_point;
    if (v[0] > v[1] && v[0] > v[2])
    {
        return 0;
    }
    else if (v[1] > v[2])
    {
        return 1;
    }
    else
    {
        return 2;
    }
}

template <typename T>
inline Vector3f offset(const Bounds3<T> &b0, const Point3<T> &p0)
{
    return static_cast<Vector3f>(p0 - b0.min_point) / static_cast<Vector3f>(b0.max_point - b0.min_point);
}

template <typename T>
inline T surface_area(const Bounds3<T> &bounds)
{
    T w = bounds.max_point.x - bounds.min_point.x;
    T h = bounds.max_point.y - bounds.min_point.y;
    T d = bounds.max_point.z - bounds.min_point.z;

    return (w * h + w * d + d * h) * 2;
}

struct SSE_ALIGNAS SoABounds3f
{
    SoAPoint3f min_point;
    SoAPoint3f max_point;

    inline const SoAPoint3f &operator[](const int idx) const
    {
        assert(idx >= 0 && idx < 2);
        return (&min_point)[idx];
    }

    inline SoABounds3f()
    {
        min_point = SoAPoint3f(MAX, MAX, MAX);
        max_point = SoAPoint3f(LOWEST, LOWEST, LOWEST);
    }

    inline SoABounds3f(const SoAPoint3f &p0, const SoAPoint3f &p1)
    {
        min_point = min(p0, p1);
        max_point = max(p0, p1);
    }

    inline SoABounds3f(const Bounds3f bounds[4])
    {
        min_point = SoAPoint3f(bounds[0].min_point, bounds[1].min_point, bounds[2].min_point, bounds[3].min_point);
        max_point = SoAPoint3f(bounds[0].max_point, bounds[1].max_point, bounds[2].max_point, bounds[3].max_point);
    }
};

inline std::ostream &operator<<(std::ostream &out, const SoABounds3f &box)
{
    out << "[min point:" << box.min_point << " max point:" << box.max_point << "]";
    return out;
}

//Tomas Moll https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
FINLINE bool intersect(const Point3f &ray_o, const Vector3f &ray_d, const float ray_t_max, const Point3f &v0, const Vector3f &e1, const Vector3f &e2, float *tt = nullptr, Point2f *uv = nullptr)
{
    auto O = ray_o;
    auto D = ray_d;

    auto V0 = v0;
    auto E1 = e1;
    auto E2 = e2;

    auto T = O - V0;
    auto P = cross(D, E2);
    auto Q = cross(T, E1);

    auto P_dot_E1 = dot(P, E1);

    if (P_dot_E1 <= EPSION && P_dot_E1 >= -EPSION)
    {
        return false;
    }

    auto P_dot_T = dot(P, T);
    auto Q_dot_D = dot(Q, D);

    auto inv_P_dot_E1 = 1.0f / P_dot_E1;
    auto u = P_dot_T * inv_P_dot_E1;
    auto v = Q_dot_D * inv_P_dot_E1;

    if (EXPECT_TAKEN(!(u >= 0.0f && v >= 0.0f && (u + v) <= 1.0f)))
    {
        return false;
    }
    float t = dot(Q, E2) * inv_P_dot_E1;
    if (t >= 0 && t <= ray_t_max)
    {
        if (tt)
        {
            (*tt) = t;
        }
        if (uv)
        {
            uv->x = u;
            uv->y = v;
        }
        return true;
    }
    return false;
}

//Tomas Moll https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
inline bool intersect(const Ray &ray, const Triangle &triangle, float *t = nullptr, Point2f *uv = nullptr)
{
    return intersect(ray.o, ray.d, ray.t_max, triangle.v0, triangle.e1, triangle.e2, t, uv);
}

FINLINE int check(const bool4 &mask, const float4 &t_results, const float4 &u_results, const float4 &v_results, float *t, Point2f *uv)
{
    float min_t = INFINITE;
    auto valid_mask = movemask(mask);
    int idx = -1;
    for (int x = valid_mask, i = 0; x != 0; x >>= 1, ++i)
    {
        if ((x & 0x1) && min_t > t_results[i])
        {
            min_t = t_results[i];
            idx = i;
        }
    }

    if (idx != -1)
    {
        (*t) = min_t;
        uv->x = u_results[idx];
        uv->y = v_results[idx];
    }

    return idx;
}

inline bool4 intersect(const SoARay &ray, const SoATriangle &triangle, float4 *t_results, float4 *u_results, float4 *v_results, bool4 mask = SSE_MASK_TRUE)
{
    auto O = ray.o;
    auto D = ray.d;
    auto V0 = triangle.v0;
    auto E1 = triangle.e1;
    auto E2 = triangle.e2;

    auto T = O - V0;

    auto P = cross(D, E2);
    auto Q = cross(T, E1);

    auto P_dot_E1 = dot(P, E1);

    auto P_dot_T = dot(P, T);
    auto Q_dot_D = dot(Q, D);

    float4 zero = _mm_setzero_ps();
    float4 one = float4(1.0f);

    float4 inv_P_dot_E1 = one / float4(P_dot_E1);

    float4 u = float4(P_dot_T) * float4(inv_P_dot_E1);
    float4 v = float4(Q_dot_D) * float4(inv_P_dot_E1);

    //check det
    bool4 mask_det = (float4(P_dot_E1) >= float4(EPSION));
    mask_det = mask_det | (float4(P_dot_E1) <= float4(-EPSION));

    mask = mask & mask_det;

    //check u/v
    mask = mask & (u >= zero);
    mask = mask & (v >= zero);
    mask = mask & ((u + v) <= one);
    if (EXPECT_TAKEN(none(mask)))
    {
        return mask;
    }

    auto Q_dot_E2 = dot(Q, E2);
    float4 t = float4(Q_dot_E2) * inv_P_dot_E1;
    //check t
    mask = mask & (t <= float4(ray.t_max));
    mask = mask & (t >= zero);

    if (EXPECT_TAKEN(t_results))
    {
        (*t_results) = t;
    }

    if (EXPECT_TAKEN(u_results))
    {
        (*t_results) = u;
    }

    if (EXPECT_TAKEN(v_results))
    {
        (*t_results) = v;
    }

    return mask;
}

//Tomas Moll https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
inline bool intersect(const SoARay &ray, const SoATriangle &triangle, float *t_result , Point2f *uv , int *index, bool4 mask = SSE_MASK_TRUE)
{
    auto O = ray.o;
    auto D = ray.d;
    auto V0 = triangle.v0;
    auto E1 = triangle.e1;
    auto E2 = triangle.e2;

    auto T = O - V0;

    auto P = cross(D, E2);
    auto Q = cross(T, E1);

    auto P_dot_E1 = dot(P, E1);

    auto P_dot_T = dot(P, T);
    auto Q_dot_D = dot(Q, D);

    float4 zero = _mm_setzero_ps();
    float4 one = float4(1.0f);

    float4 inv_P_dot_E1 = one / float4(P_dot_E1);

    float4 u = float4(P_dot_T) * float4(inv_P_dot_E1);
    float4 v = float4(Q_dot_D) * float4(inv_P_dot_E1);

    //check det
    bool4 mask_det = (float4(P_dot_E1) >= float4(EPSION));
    mask_det = mask_det | (float4(P_dot_E1) <= float4(-EPSION));

    mask = mask & mask_det;

    //check u/v
    mask = mask & (u >= zero);
    mask = mask & (v >= zero);
    mask = mask & ((u + v) <= one);
    if (EXPECT_TAKEN(none(mask)))
    {
        return false;
    }
    auto Q_dot_E2 = dot(Q, E2);
    float4 t = float4(Q_dot_E2) * inv_P_dot_E1;
    //check t
    mask = mask & (t <= float4(ray.t_max));
    mask = mask & (t >= zero);

    if (EXPECT_TAKEN(none(mask)))
    {
        return false;
    }

    if (EXPECT_TAKEN(t_result))
    {
        float min_t = INFINITE;
        auto valid_mask = movemask(mask);
        int idx = -1;
        for (int x = valid_mask, i = 0; x != 0; x >>= 1, ++i)
        {
            if ((x & 0x1) && min_t > t[i])
            {
                min_t = t[i];
                idx = i;
            }
        }

        if (EXPECT_NOT_TAKEN(idx != -1))
        {
            (*t_result) = min_t;
            if (EXPECT_TAKEN(index))
            {
                (*index) = idx;
            }
            if (EXPECT_TAKEN(uv))
            {
                uv->x = u[idx];
                uv->y = v[idx];
            }
        }
    }
    return true;
}

inline bool intersect(const SoARay &ray, const SoATriangle &triangle, bool4 mask = SSE_MASK_TRUE)
{
    auto O = ray.o;
    auto D = ray.d;
    auto V0 = triangle.v0;
    auto E1 = triangle.e1;
    auto E2 = triangle.e2;

    auto T = O - V0;

    auto P = cross(D, E2);
    auto Q = cross(T, E1);

    auto P_dot_E1 = dot(P, E1);

    auto P_dot_T = dot(P, T);
    auto Q_dot_D = dot(Q, D);

    float4 zero = _mm_setzero_ps();
    float4 one = float4(1.0f);

    float4 inv_P_dot_E1 = one / float4(P_dot_E1);

    float4 u = float4(P_dot_T) * float4(inv_P_dot_E1);
    float4 v = float4(Q_dot_D) * float4(inv_P_dot_E1);

    //check det
    bool4 mask_det = (float4(P_dot_E1) >= float4(EPSION));
    mask_det = mask_det | (float4(P_dot_E1) <= float4(-EPSION));

    mask = mask & mask_det;

    //check u/v
    mask = mask & (u >= zero);
    mask = mask & (v >= zero);
    mask = mask & ((u + v) <= one);
    if (EXPECT_TAKEN(none(mask)))
    {
        return false;
    }
    auto Q_dot_E2 = dot(Q, E2);
    float4 t = float4(Q_dot_E2) * inv_P_dot_E1;
    //check t
    mask = mask & (t <= float4(ray.t_max));
    mask = mask & (t >= zero);

    if (EXPECT_TAKEN(none(mask)))
    {
        return false;
    }
    return true;
}

//https://www.slideshare.net/ssuser2848d3/qbv
//single ray with four box
inline bool intersect(const Point3f &o, const Vector3f &inv_d, float t_min, float t_max, const int isPositive[3], const Bounds3f &box)
{
    //x
    t_min = max((box[1 - isPositive[0]].x - o.x) * inv_d.x, t_min);
    t_max = min((box[isPositive[0]].x - o.x) * inv_d.x, t_max);
    //y
    t_min = max((box[1 - isPositive[1]].y - o.y) * inv_d.y, t_min);
    t_max = min((box[isPositive[1]].y - o.y) * inv_d.y, t_max);
    //z
    t_min = max((box[1 - isPositive[2]].z - o.z) * inv_d.z, t_min);
    t_max = min((box[isPositive[2]].z - o.z) * inv_d.z, t_max);

    return t_min <= t_max;
}

inline bool4 intersect(const SoAPoint3f &o, const SoAVector3f &inv_d, float4 t_min, float4 t_max, const int isPositive[3], const SoABounds3f &box)
{
    // x
    t_min = max((box[1 - isPositive[0]].xxxx - o.xxxx) * inv_d.xxxx, t_min);
    t_max = min((box[isPositive[0]].xxxx - o.xxxx) * inv_d.xxxx, t_max);

    //y
    t_min = max((box[1 - isPositive[1]].yyyy - o.yyyy) * inv_d.yyyy, t_min);
    t_max = min((box[isPositive[1]].yyyy - o.yyyy) * inv_d.yyyy, t_max);

    //z
    t_min = max((box[1 - isPositive[2]].zzzz - o.zzzz) * inv_d.zzzz, t_min);
    t_max = min((box[isPositive[2]].zzzz - o.zzzz) * inv_d.zzzz, t_max);

    //check
    return t_min <= t_max;
}

inline bool4 intersect(const SoAPoint3f &o, const SoAVector3f &inv_d, float4 t_min, float4 t_max, const int isPositive[3], const SoABounds3f &box, float4 *t)
{
    // x
    t_min = max((box[1 - isPositive[0]].xxxx - o.xxxx) * inv_d.xxxx, t_min);
    t_max = min((box[isPositive[0]].xxxx - o.xxxx) * inv_d.xxxx, t_max);

    //y
    t_min = max((box[1 - isPositive[1]].yyyy - o.yyyy) * inv_d.yyyy, t_min);
    t_max = min((box[isPositive[1]].yyyy - o.yyyy) * inv_d.yyyy, t_max);

    //z
    t_min = max((box[1 - isPositive[2]].zzzz - o.zzzz) * inv_d.zzzz, t_min);
    t_max = min((box[isPositive[2]].zzzz - o.zzzz) * inv_d.zzzz, t_max);

    (*t) = t_min;

    //check
    return t_min <= t_max;
}
NARUKAMI_END
