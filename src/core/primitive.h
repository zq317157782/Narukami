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
        private:
            const MeshManager* _mm;
        public:
        const MeshTriangle& triangle() const{return _mm->get_mesh_triangle_ref(triangle_index);}
        size_t triangle_index;
        const AreaLight* area_light;
        const LightMaterial* light_material;
        Primitive()=default;
        Primitive(const MeshManager* mm,const size_t triangle):_mm(mm),triangle_index(triangle),area_light(nullptr),light_material(nullptr){}
        Primitive(const MeshManager* mm,const size_t triangle,const AreaLight*area_light):_mm(mm),triangle_index(triangle),area_light(area_light),light_material(nullptr){}
        Primitive(const MeshManager* mm,const size_t triangle,const LightMaterial*light_material):_mm(mm),triangle_index(triangle),light_material(light_material),area_light(nullptr){}
    };

    inline Bounds3f get_world_bounds(const Primitive& primitive) {return primitive.triangle().get_world_bounds();}
    inline const Transform& get_object_to_world(const Primitive& primitive){return primitive.triangle().object_to_world();}
    inline const Transform& get_world_to_object(const Primitive& primitive){return primitive.triangle().world_to_object();}
    //inline Spectrum Le(const Primitive& primitive){ ()}

    std::vector<Primitive> create_primitives(const MeshManager&,size_t start,size_t end);
    std::vector<Primitive> create_primitives(const MeshManager&,size_t start,size_t end,const AreaLight* area_light);
    std::vector<Primitive> create_primitives(const MeshManager&,size_t start,size_t end,const LightMaterial* light_material);
    std::vector<Primitive> _union(const std::vector<Primitive>& a,const std::vector<Primitive>& b);

    struct SoAPrimitiveInfo{
        SoATriangle triangle;
        
        //*** 128 bit
        uint32_t offset;
        uint32_t pad0,pad1,pad2;
        //***
    };

    std::vector<SoAPrimitiveInfo> SoA_pack(const std::vector<Primitive> &triangles, uint32_t start, uint32_t count);
NARUKAMI_END