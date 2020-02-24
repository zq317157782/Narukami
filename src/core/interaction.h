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
class Interaction{
public:
    Point3f p;
    Normal3f n;
    float hit_t;
    const Primitive * primitive;
public:
    Interaction() = default;
    Interaction(const Point3f& p):p(p){}
    Interaction(const Point3f& p,const Normal3f& n):p(p),n(n){}
};

FINLINE bool is_surface_interaction(const Interaction& interaction){
     return dot(interaction.n,interaction.n)!=0;
}

class SurfaceInteraction:public Interaction{
public:
     
};

inline const Transform& get_object_to_world(const SurfaceInteraction& interaction)
{
    return get_object_to_world(*interaction.primitive);
}

inline const Transform& get_world_to_object(const SurfaceInteraction& interaction)
{
    return get_world_to_object(*interaction.primitive);
}

Spectrum Le(const SurfaceInteraction& interaction,const Vector3f& wi);

NARUKAMI_END