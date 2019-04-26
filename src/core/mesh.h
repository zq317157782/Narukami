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
NARUKAMI_BEGIN
    struct MeshData{
        std::unique_ptr<Point3f[]> vertices;
        std::unique_ptr<Normal3f[]> normals;
        std::unique_ptr<Point2f[]> uvs;
        std::unique_ptr<uint32_t[]> indices;
        
        MeshData(const Transform& object2wrold,int triangle_num,const uint32_t *indices,int vertex_num,const Point3f *vertices,const Normal3f *normals=nullptr,const Point2f *uvs=nullptr);
    };

    struct MeshTriangle{
        const Transform * object2world,*world2object;
        std::shared_ptr<MeshData> mesh;
        uint32_t index[3];

        MeshTriangle(const Transform* object2world,const Transform* world2object,std::shared_ptr<MeshData> mesh,const uint32_t  idx[3]):object2world(object2world),world2object(world2object),mesh(mesh){
            memcpy(index,idx,sizeof(uint32_t)*3);
        }

        Point3f& operator[](const int i){ assert(i>=0&&i<2); return mesh->vertices[index[i]]; }
        const Point3f& operator[](const int i)const { assert(i>=0&&i<2); return mesh->vertices[index[i]];}
    };

    FINLINE bool intersect(const Ray& ray,const MeshTriangle& triangle,float* t,Point2f* uv){
        auto v0 = triangle[0];
        auto e1 = triangle[1]-v0;
        auto e2 = triangle[2]-v0;
        return intersect(ray.o,ray.d,ray.t_max,v0,e1,e2,t,uv);
    }

    std::vector<SoATriangle> cast2SoA(const std::vector<std::shared_ptr<MeshTriangle>>& triangles,uint32_t start,uint32_t count);

    std::vector<std::shared_ptr<MeshTriangle>> CreateMeshTriangles(const Transform* object2wrold,const Transform* world2object,int triangle_num,const uint32_t *indices,int vertex_num,const Point3f *vertices,const Normal3f *normals=nullptr,const Point2f *uvs=nullptr);

NARUKAMI_END

