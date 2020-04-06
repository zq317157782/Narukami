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
    class Primitive
    {
    public:
        ref<TriangleMesh> mesh;
        const AreaLight* area_light;
        const LightMaterial* light_material;
        Primitive() = default;
        Primitive(const ref<TriangleMesh>& mesh):mesh(mesh),area_light(nullptr),light_material(nullptr){}
        Primitive(const ref<TriangleMesh>& mesh,const AreaLight*area_light):mesh(mesh),area_light(area_light),light_material(nullptr){}
        Primitive(const ref<TriangleMesh>& mesh,const LightMaterial*light_material):mesh(mesh),light_material(light_material),area_light(nullptr){}

        void * operator new(size_t size);
        void  operator delete(void * ptr);
    };

    inline Bounds3f get_world_bounds(const Primitive& primitive) {return primitive.mesh->get_world_bounds();}
    inline const Transform& get_object_to_world(const Primitive& primitive){return primitive.mesh->object_to_world();}
    inline const Transform& get_world_to_object(const Primitive& primitive){return primitive.mesh->world_to_object();}
   

    std::vector<ref<Primitive>> create_primitives(const std::vector<ref<TriangleMesh>>&);
    std::vector<ref<Primitive>> concat(const std::vector<ref<Primitive>>& a,const std::vector<ref<Primitive>>& b);

    struct SoAPrimitiveInfo{
        SoATriangle triangle;
        
        //*** 128 bit
        uint32_t offset;
        uint32_t pad0,pad1,pad2;
        //***
    };

    std::vector<SoAPrimitiveInfo> SoA_pack(const std::vector<ref<Primitive>> &triangles, uint32_t start, uint32_t count);
NARUKAMI_END