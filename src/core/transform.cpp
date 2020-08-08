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

#include "core/transform.h"
#include "core/interaction.h"
NARUKAMI_BEGIN

MemoryPool<Transform, SSE_LINE_SIZE> g_transform_pool(256);

void *Transform::operator new(size_t size)
{
    return g_transform_pool.alloc();
}

void Transform::operator delete(void *ptr)
{
    g_transform_pool.dealloc(reinterpret_cast<Transform *>(ptr));
}

MemoryPool<AnimatedTransform> g_animated_transform_pool(256);

void *AnimatedTransform::operator new(size_t size)
{
    return g_animated_transform_pool.alloc();
}

void AnimatedTransform::operator delete(void *ptr)
{
    g_animated_transform_pool.dealloc(reinterpret_cast<AnimatedTransform *>(ptr));
}

Ray Transform::operator()(const Ray &ray) const
{
    Ray r(ray);
    r.o = (*this)(ray.o);
    r.d = (*this)(ray.d);
    return r;
}

Bounds3f Transform::operator()(const Bounds3f &b) const
{
    auto b0 = _union((*this)(corner(b, 0)), (*this)(corner(b, 1)));
    b0 = _union(b0, (*this)(corner(b, 2)));
    b0 = _union(b0, (*this)(corner(b, 3)));
    b0 = _union(b0, (*this)(corner(b, 4)));
    b0 = _union(b0, (*this)(corner(b, 5)));
    b0 = _union(b0, (*this)(corner(b, 6)));
    b0 = _union(b0, (*this)(corner(b, 7)));
    return b0;
}

Interaction Transform::operator()(const Interaction &i) const
{
    Interaction new_i;
    new_i.p = (*this)(i.p);
    new_i.n = (*this)(i.n);
    new_i.primitive = i.primitive;
    new_i.hit_t = i.hit_t;
    return new_i;
};

Interaction AnimatedTransform::operator()(float time, const Interaction &i) const
{
    Transform t;
    interpolate(time, &t);
    return t(i);
}

NARUKAMI_END