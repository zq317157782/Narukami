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
#include <vector>
NARUKAMI_BEGIN
    class MeshData{
        public:
            std::vector<Point3f> vertices;
            std::vector<Normal3f> normals;
            std::vector<Point2f> uvs;
            std::vector<uint32_t> indices;
        
        MeshData(const Transform& object2wrold,const std::vector<uint32_t>& indices,const std::vector<Point3f>& vertices,const std::vector<Normal3f>&normals,const std::vector<Point2f>&uvs);
        MeshData(const Transform& object2wrold,const std::vector<uint32_t>& indices,const std::vector<Point3f>& vertices);
        MeshData(const Transform& object2wrold,const std::vector<uint32_t>& indices,const std::vector<Point3f>& vertices,const std::vector<Normal3f>&normals);
        MeshData(const Transform& object2wrold,const std::vector<uint32_t>& indices,const std::vector<Point3f>& vertices,const std::vector<Point2f>&uvs);
    };

    class MeshTriangle{
        private:
            const Transform * _object2world,*_world2object;
            std::shared_ptr<MeshData> _mesh;
            uint32_t _index[3];
        public:
        MeshTriangle(const Transform* object2world,const Transform* world2object,std::shared_ptr<MeshData> mesh,const uint32_t  idx[3]):_object2world(object2world),_world2object(world2object),_mesh(mesh){
            memcpy(_index,idx,sizeof(uint32_t)*3);
        }

        //inline Point3f& operator[](const int i){ assert(i>=0&&i<2); return _mesh->vertices[_index[i]]; }
        inline const Point3f& operator[](const int i)const { assert(i>=0&&i<2); return _mesh->vertices[_index[i]];}
        inline  Point2f get_vertex_uv(const int i) const{ assert(i>=0&&i<2); if(_mesh->uvs.size()>0){ return _mesh->uvs[_index[i]]; } return Point2f(0);}
        inline  Point2f sample_uv(const Point2f& u) const{ if(_mesh->uvs.size()>0){ return _mesh->uvs[_index[0]]*(1.0f-u.x-u.y)+_mesh->uvs[_index[1]]*u.x+_mesh->uvs[_index[2]]*u.y; } return Point2f(0); }
        friend inline  std::ostream &operator<<(std::ostream &out, const MeshTriangle &v) { out << '(' << v._mesh->vertices[v._index[0]] << ',' << v._mesh->vertices[v._index[1]] << ',' << v._mesh->vertices[v._index[2]] << ')'; return out; }
    };


    

    inline bool intersect(const Ray& ray,const MeshTriangle& triangle,float* t,Point2f* uv){
        auto v0 = triangle[0];
        auto e1 = triangle[1]-v0;
        auto e2 = triangle[2]-v0;
        return intersect(ray.o,ray.d,ray.t_max,v0,e1,e2,t,uv);
    }

    std::vector<SoATriangle> cast2SoA(const std::vector<MeshTriangle>& triangles,uint32_t start,uint32_t count);

    std::vector<MeshTriangle> create_mesh_triangles(const Transform* object2wrold,const Transform* world2object,const std::vector<uint32_t>& indices,const std::vector<Point3f>& vertices,const std::vector<Normal3f>&normals,const std::vector<Point2f>&uvs);


NARUKAMI_END

