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
        enum class Type
        {
            MESH,
            ACCELERATER,
        };
    private:
        Type _type;
    public:
        Primitive(const Type& type):_type(type){};
        Type type() const {return _type;}
        virtual Bounds3f bounds() const = 0;
        virtual bool trace_ray(MemoryArena &arena, const Ray &ray,Interaction* interaction) const = 0;
        virtual bool trace_ray(const Ray &ray) const = 0;
    };

    std::vector<shared<Primitive>> concat(const std::vector<shared<Primitive>>& a,const std::vector<shared<Primitive>>& b);

    class MeshPrimitive:public Primitive
    {
        private:
            shared<TriangleMesh> _mesh;
        public:
            MeshPrimitive(const shared<TriangleMesh>& mesh):Primitive(Type::MESH),_mesh(mesh){}
            Bounds3f bounds() const override {return _mesh->bounds();}
            const Transform& object_to_world() const  {return _mesh->object_to_world();}
            const Transform& world_to_object() const  {return _mesh->world_to_object();}
            bool trace_ray(MemoryArena &arena, const Ray &ray,Interaction* interaction) const override;
            bool trace_ray(const Ray &ray) const override;
            

            const shared<TriangleMesh> mesh() const {return _mesh;}

            void * operator new(size_t size);
            void  operator delete(void * ptr);
    };
    
    std::vector<shared<MeshPrimitive>> create_mesh_primitives(const std::vector<shared<TriangleMesh>>&);


    struct MeshPrimitiveInfo4p{
        Triangle4p triangle;
        
        //*** 128 bit
        uint32_t offset;
        uint32_t pad0,pad1,pad2;
        //***
    };

    std::vector<MeshPrimitiveInfo4p> pack_mesh_primitives(const std::vector<shared<MeshPrimitive>> &triangles, uint32_t start, uint32_t count);
NARUKAMI_END