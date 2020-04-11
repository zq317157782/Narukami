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
#include "core/transform.h"
#include "core/stat.h"
#include "core/geometry.h"
#include "core/memory.h"
#include <vector>
NARUKAMI_BEGIN
   
    struct VertexData
    {
        std::vector<Point3f>  positions;
        std::vector<Normal3f> normals;
        std::vector<Point2f>  uvs;

        void * operator new(size_t size);
        void  operator delete(void * ptr);
    };

    class TriangleMesh
    {
        private:
            const Transform * _object2world,*_world2object;
            ref<VertexData> _vertex_data;
            uint32_t _index[3];
        public:
        TriangleMesh(const Transform* object2world, const Transform* world2object,const ref<VertexData>& vertex_data,const uint32_t index[3]):_object2world(object2world), _world2object(world2object),_vertex_data(vertex_data){memcpy(_index,index,3*sizeof(uint32_t) );}

        //inline Point3f& operator[](const int i){ assert(i>=0&&i<2); return mesh().vertices[_index[i]]; }
        inline  Point3f operator[](const int i) const { assert(i>=0&&i<=2); return _vertex_data->positions[_index[i]];}
        inline  Point2f get_uv(const int i) const{ assert(i>=0&&i<=2);return _vertex_data->uvs[_index[i]];}
        inline  Point2f sample_uv(const Point2f& u) const { return _vertex_data->uvs[_index[0]]*(1.0f-u.x-u.y)+_vertex_data->uvs[_index[1]]*u.x + _vertex_data->uvs[_index[2]] * u.y;}
        inline  Bounds3f bounds() const{return _union(_union((*this)[0],(*this)[1]),(*this)[2]);}
        inline  const Transform& object_to_world() const {return *_object2world;}
        inline  const Transform& world_to_object() const {return *_world2object;}
        inline  Triangle geom_tri() const 
        {
            Triangle triangle;
            triangle.v0 = (*this)[0];
            triangle.e1 = (*this)[1] - triangle.v0;
            triangle.e2 = (*this)[2] - triangle.v0; 
            return triangle;
        }
        void * operator new(size_t size);
        void  operator delete(void * ptr);
        friend inline  std::ostream &operator<<(std::ostream &out, const TriangleMesh &v) { out << '(' << v[0] << ',' << v[1] << ',' << v[2] << ')'; return out; }
    };
    
    inline bool intersect(const Ray& ray,const TriangleMesh& triangle,float* t,Point2f* uv){
        auto v0 = triangle[0];
        auto e1 = triangle[1]-v0;
        auto e2 = triangle[2]-v0;
        return intersect(ray.o,ray.d,ray.t_max,v0,e1,e2,t,uv);
    }

    void append(std::vector<ref<TriangleMesh>>& A,const std::vector<ref<TriangleMesh>>& B);
    std::vector<ref<TriangleMesh>> concat(const std::vector<ref<TriangleMesh>>& A,const std::vector<ref<TriangleMesh>>& B);

    std::vector<ref<TriangleMesh>> create_mesh_triangles(const Transform* object2wrold,const Transform* world2object,const std::vector<uint32_t>& indices,const std::vector<Point3f>& vertices,const std::vector<Normal3f>&normals,const std::vector<Point2f>&uvs);    
    std::vector<ref<TriangleMesh>> create_plane(const Transform *object2wrold, const Transform *world2object, const float width, const float height);
    std::vector<ref<TriangleMesh>> create_disk(const Transform *object2wrold, const Transform *world2object,float radius, const uint32_t vertex_density);

    std::vector<Triangle4p> SoA_pack(const std::vector<ref<TriangleMesh>>&);

    
NARUKAMI_END

