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

Ray Transform::operator()(const Ray &ray) const
{
    Ray r(ray);
    r.o = (*this)(ray.o);
    r.d = (*this)(ray.d);
    return r;
}

Bounds3f Transform::operator()(const Bounds3f &b) const
{
    auto b0 = _union((*this)(end_point0(b)), (*this)(end_point1(b)));
    b0 = _union(b0, (*this)(end_point2(b)));
    b0 = _union(b0, (*this)(end_point3(b)));
    b0 = _union(b0, (*this)(end_point4(b)));
    b0 = _union(b0, (*this)(end_point5(b)));
    b0 = _union(b0, (*this)(end_point6(b)));
    b0 = _union(b0, (*this)(end_point7(b)));
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

const Transform IDENTITY = identity();
NARUKAMI_END