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
    enum class PrimitiveType
    {
        MESH
    };
    class Primitive
    {
    private:
        PrimitiveType _type;
    public:
       
        Primitive(const PrimitiveType& type):_type(type){};
        PrimitiveType type() const {return _type;}
        virtual Bounds3f world_bounds() const = 0;
        virtual const Transform& object_to_world() const = 0;
        virtual const Transform& world_to_object() const = 0;
    };

    std::vector<ref<Primitive>> concat(const std::vector<ref<Primitive>>& a,const std::vector<ref<Primitive>>& b);

    class MeshPrimitive:public Primitive
    {
        private:
            ref<TriangleMesh> _mesh;
        public:
            MeshPrimitive(const ref<TriangleMesh>& mesh):Primitive(PrimitiveType::MESH),_mesh(mesh){}
            virtual Bounds3f world_bounds() const override {return _mesh->world_bounds();}
            virtual const Transform& object_to_world() const override {return _mesh->object_to_world();}
            virtual const Transform& world_to_object() const override {return _mesh->world_to_object();}

            const ref<TriangleMesh> mesh() const {return _mesh;}

            void * operator new(size_t size);
            void  operator delete(void * ptr);
    };
    
    std::vector<ref<MeshPrimitive>> create_mesh_primitives(const std::vector<ref<TriangleMesh>>&);


    struct MeshPrimitiveInfo4p{
        Triangle4p triangle;
        
        //*** 128 bit
        uint32_t offset;
        uint32_t pad0,pad1,pad2;
        //***
    };

    std::vector<MeshPrimitiveInfo4p> pack_mesh_primitives(const std::vector<ref<MeshPrimitive>> &triangles, uint32_t start, uint32_t count);
NARUKAMI_END