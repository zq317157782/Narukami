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
    // _p0 is the start point
    // _p1 is the end point
    class VisibilityTester{
        private:
            Interaction _p0,_p1;
        public:
            VisibilityTester() = default;
            VisibilityTester(const Interaction& p0,const Interaction& p1):_p0(p0),_p1(p1){}
            bool unoccluded(const Scene& scene) const;
    };

    class Light{
        protected:
            const Transform _light_to_world;
            const Transform _world_to_light;
        public:
            Light(const Transform& light_to_world):_light_to_world(light_to_world),_world_to_light(inverse(light_to_world)){}
            virtual Spectrum sample_Li(const Interaction& interaction,const Point2f& u,Vector3f* wi,float * pdf,VisibilityTester* tester) = 0;

    };
NARUKAMI_END