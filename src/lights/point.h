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

#include "core/light.h"

NARUKAMI_BEGIN
class PointLight : public Light
{
private:
    Spectrum _I; //radiant intensity
public:
    PointLight(const Transform *light_to_world,const Transform *world_to_light, const Spectrum &L) : Light(light_to_world,world_to_light,1), _I(L) {}
    Spectrum sample_Li(const Interaction &interaction, const Point2f &u, Vector3f *wi, float *pdf, VisibilityTester *tester) override
    {
        auto light_position = (*_light_to_world)(Point3f(0.0f, 0.0f, 0.0f));
        (*wi) = normalize(light_position - interaction.p);
        if (pdf)
        {
            (*pdf) = 1;
        }
        if (tester)
        {
            (*tester) = VisibilityTester(interaction, Interaction(light_position));
        }

        return _I / sqrlen(interaction.p - light_position);
    }

    Spectrum power() const override
    {
        return _I * 4.0f * PI;
    }
};
NARUKAMI_END