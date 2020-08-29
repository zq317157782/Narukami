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
#include "core/primitive.h"
#include "core/spectrum.h"
NARUKAMI_BEGIN

//basic interaction
class Interaction
{
public:
    float hit_t;
    Point3f p;
    Normal3f n;
    Point2f uv;
public:
    Interaction():hit_t(INFINITE){};
    Interaction(const Point3f& p):hit_t(INFINITE),p(p){}
    Interaction(const Point3f& p,const Normal3f& n):hit_t(INFINITE),p(p),n(n){}
};

FINLINE bool is_closer(const Interaction& i,float t) {return t < i.hit_t;}

FINLINE bool is_surface_interaction(const Interaction& interaction){
     return dot(interaction.n,interaction.n)!=0;
}

class SurfaceInteraction:public Interaction{};


Color Le(const SurfaceInteraction& interaction,const Vector3f& wi);


// _p0 is the start point
// _p1 is the end point
class VisibilityTester
{
private:
    Interaction _p0, _p1;

public:
    VisibilityTester() = default;
    VisibilityTester(const Interaction &p0, const Interaction &p1) : _p0(p0), _p1(p1) {}
    bool unoccluded(const Scene &scene) const;
};


NARUKAMI_END