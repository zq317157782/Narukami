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
#include <vector>
NARUKAMI_BEGIN
    class MeshData{
        public:
            std::vector<Point3f> vertices;
            std::vector<Normal3f> normals;
            std::vector<Point2f> uvs;
            std::vector<uint32_t> indices;
        public:
            MeshData& add_vertices(const std::vector<Point3f> &vertices);
            MeshData& add_normals( const std::vector<Normal3f> &normals);
            MeshData& add_uvs(const std::vector<Point2f> &uvs);
            MeshData& add_indices(const std::vector<uint32_t> &indices);

            MeshData& add_transform_vertices(const Transform &trans,const std::vector<Point3f> &vertices);
            MeshData& add_transform_normals(const Transform &trans,const std::vector<Normal3f> &normals);
        public:
            friend class MeshTriangle;
    };
    class MeshManager;
    class MeshTriangle{
        private:
            const Transform * _object2world,*_world2object;
            const MeshManager* _mm;
            uint32_t _mesh_data_id;
            uint32_t _index[3];
        const MeshData& mesh() const;
        public:
        MeshTriangle() = default;
        MeshTriangle(const MeshTriangle&) = default;
        MeshTriangle(MeshTriangle&&) = default;
        MeshTriangle& operator=(const MeshTriangle&) = default;
        MeshTriangle& operator=(MeshTriangle&&) = default;
        ~MeshTriangle()=default;


        void init(const Transform* object2world,const Transform* world2object,const MeshManager* mm,uint32_t mesh_data,const uint32_t idx[3]);

        //inline Point3f& operator[](const int i){ assert(i>=0&&i<2); return mesh().vertices[_index[i]]; }
        inline const Point3f& operator[](const int i)const { assert(i>=0&&i<=2); return mesh().vertices[_index[i]];}
        inline  Point2f get_vertex_uv(const int i) const{ assert(i>=0&&i<=2); if(mesh().uvs.size()>0){ return mesh().uvs[_index[i]]; } return Point2f(0);}
        inline  Point2f sample_uv(const Point2f& u) const{ if(mesh().uvs.size()>0){ return mesh().uvs[_index[0]]*(1.0f-u.x-u.y)+mesh().uvs[_index[1]]*u.x+mesh().uvs[_index[2]]*u.y; } return Point2f(0); }
        inline  Bounds3f get_world_bounds() const{return _union(_union(mesh().vertices[_index[0]],mesh().vertices[_index[1]]),mesh().vertices[_index[2]]);}
        inline  const Transform& object_to_world() const {return *_object2world;}
        inline  const Transform& world_to_object() const {return *_world2object;}
        inline  Triangle triangle() const {
            Triangle triangle;
            triangle.v0 = (*this)[0];
            triangle.e1 = (*this)[1] - triangle.v0;
            triangle.e2 = (*this)[2] - triangle.v0;
            return triangle;
        }
        friend inline  std::ostream &operator<<(std::ostream &out, const MeshTriangle &v) { out << '(' << v.mesh().vertices[v._index[0]] << ',' << v.mesh().vertices[v._index[1]] << ',' << v.mesh().vertices[v._index[2]] << ')'; return out; }
    };
    
    inline bool intersect(const Ray& ray,const MeshTriangle& triangle,float* t,Point2f* uv){
        auto v0 = triangle[0];
        auto e1 = triangle[1]-v0;
        auto e2 = triangle[2]-v0;
        return intersect(ray.o,ray.d,ray.t_max,v0,e1,e2,t,uv);
    }
    STAT_COUNTER("mesh/MeshData's count",MeshData_count)
    STAT_COUNTER("mesh/MeshTriangle's count",MeshTriangle_count)
    class MeshManager
    {
    private:
        std::vector<MeshData> _mesh_datas;
        std::vector<MeshTriangle> _mesh_triangles;
    public:
        MeshData& add_get_mesh_data_ref(uint32_t& index);
        MeshTriangle& add_get_mesh_triangle_ref(uint32_t& index);

        uint32_t mesh_data_size() const{return _mesh_datas.size();}
        uint32_t mesh_triange_size() const{return _mesh_triangles.size();}

        const MeshData& get_mesh_data_ref(uint32_t index) const {return _mesh_datas[index];}
        const MeshTriangle& get_mesh_triangle_ref(uint32_t index) const {return _mesh_triangles[index];}
    };

    std::vector<SoATriangle> SoA_pack(const MeshManager&,const std::pair<uint32_t,uint32_t>& range);

    std::pair<uint32_t,uint32_t> create_mesh_triangles(const Transform* object2wrold,const Transform* world2object,const std::vector<uint32_t>& indices,const std::vector<Point3f>& vertices,const std::vector<Normal3f>&normals,const std::vector<Point2f>&uvs,MeshManager&);    
    std::pair<uint32_t,uint32_t> create_plane(const Transform* object2wrold,const Transform* world2object,const float width,const float height,MeshManager&);
    std::pair<uint32_t,uint32_t> create_disk(const Transform *object2wrold, const Transform *world2object,float radius, const uint32_t vertex_density,MeshManager&);
NARUKAMI_END

