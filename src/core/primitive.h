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
#include "core/mesh.h"
#include "core/geometry.h"
#include "core/spectrum.h"

NARUKAMI_BEGIN
    class Primitive{
        public:
        MeshTriangle triangle;
        const AreaLight* area_light;

        Primitive() = default;
        Primitive(const Primitive&) = default;
        Primitive(Primitive&&) = default;
        Primitive& operator=(const Primitive&) = default;
        Primitive& operator=(Primitive&&) = default;
        ~Primitive()=default;
        Primitive(MeshTriangle _triangle):triangle(std::move(_triangle)),area_light(nullptr){}
        Primitive(MeshTriangle _triangle,const AreaLight*area_light):triangle(std::move(_triangle)),area_light(area_light){}
    };

    inline Bounds3f get_world_bounds(const Primitive& primitive) {return primitive.triangle.get_world_bounds();}
    //inline Spectrum Le(const Primitive& primitive){ ()}

    std::vector<Primitive> create_primitives(const std::vector<MeshTriangle>& triangles,const AreaLight* area_light = nullptr);

    struct SoAPrimitiveInfo{
        SoATriangle triangle;
        
        //*** 128 bit
        uint32_t offset;
        uint32_t pad0,pad1,pad2;
        //***
    };

    std::vector<SoAPrimitiveInfo> cast_to_SoA_structure(const std::vector<Primitive> &triangles, uint32_t start, uint32_t count);
NARUKAMI_END