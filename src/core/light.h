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
#include "core/transform.h"
#include "core/spectrum.h"
#include "core/interaction.h"
NARUKAMI_BEGIN
class Light
{
protected:
    const Transform *_light_to_world;
    const Transform *_world_to_light;
public:
    Light(const Transform *light_to_world,const Transform *world_to_light) : _light_to_world(light_to_world), _world_to_light(world_to_light) {}
    virtual Spectrum sample_Li(const Interaction &interaction, const Point2f &u, Vector3f *wi, float *pdf, VisibilityTester *tester) = 0;
    virtual Spectrum power() const = 0;
};

class AreaLight : public Light
{
private:
    float _area;
public:
    AreaLight(const Transform *light_to_world,const Transform *world_to_light,const float area) : Light(light_to_world,world_to_light),_area(area) {}
    float area() const { return _area; }
    virtual Spectrum L(const Interaction &interaction, const Vector3f &wi) const = 0;
};

NARUKAMI_END