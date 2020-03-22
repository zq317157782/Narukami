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
#include "tiny_obj_loader.h"
NARUKAMI_BEGIN

struct Vertex
{
    Point3f vertex;
    Normal3f normal;
    Point2f uv;
    bool has_normal;
    bool has_uv;

    Vertex(const float v_x, const float v_y, const float v_z, const float n_x, const float n_y, const float n_z, float uv_x, float uv_y, const bool has_normal = true, const bool has_uv = true) : vertex(v_x, v_y, v_z), normal(n_x, n_y, n_z), uv(uv_x, uv_y), has_normal(has_normal), has_uv(has_uv) {}
};

inline bool operator==(const Vertex &v0, const Vertex &v1)
{
    if (v0.vertex == v1.vertex)
    {
        if (v0.normal == v1.normal)
        {
            if (v0.uv == v1.uv)
            {
                return true;
            }
        }
    }
    return false;
}

//load mesh from obj file
//use syoyo's tinyobjloader
std::vector<std::vector<TriangleMesh>> load_mesh_triangles_list_from_obj(const Transform *object2wrold, const Transform *world2object, const char *obj_file_name, const char *base_path = nullptr)
{
    std::vector<std::vector<TriangleMesh>> meshs_list;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, obj_file_name, base_path, true);
    if (!warn.empty())
    {
        std::cout << warn << std::endl;
        return meshs_list;
    }

    if (!err.empty())
    {
        std::cerr << err << std::endl;
        return meshs_list;
    }

    if (!ret)
    {
        std::cerr << "fail to load \"" << obj_file_name << "\"" << std::endl;
        return meshs_list;
    }

    

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++)
    {
        std::vector<Vertex> vertices;
        std::vector<Vertex>::iterator it;
        std::vector<uint32_t> indices;
        bool has_normal = attrib.normals.size() > 0 ? true : false;
        bool has_uv = attrib.texcoords.size() > 0 ? true : false;

        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            size_t fv = shapes[s].mesh.num_face_vertices[f];

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++)
            {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
                tinyobj::real_t nx = 0;
                tinyobj::real_t ny = 0;
                tinyobj::real_t nz = 0;
                if (has_normal)
                {
                    nx = attrib.normals[3 * idx.normal_index + 0];
                    ny = attrib.normals[3 * idx.normal_index + 1];
                    nz = attrib.normals[3 * idx.normal_index + 2];
                }
                tinyobj::real_t tx = 0;
                tinyobj::real_t ty = 0;
                if (has_uv)
                {
                    tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                    ty = attrib.texcoords[2 * idx.texcoord_index + 1];
                }

                Vertex vertex(vx, vy, vz, nx, ny, nz, tx, ty, has_normal, has_uv);
                it = find(vertices.begin(), vertices.end(), vertex);
                if (it == vertices.end())
                {
                    indices.push_back(vertices.size());
                    vertices.push_back(vertex);
                }
                else
                {
                    indices.push_back(distance(vertices.begin(), it));
                }
            }
            index_offset += fv;

            // per-face material
            shapes[s].mesh.material_ids[f];
        }

        auto vertex_array = std::vector<Point3f>();
        auto normal_array = std::vector<Normal3f>();
        auto uv_array = std::vector<Point2f>();
        for (auto &&v : vertices)
        {
            vertex_array.push_back(v.vertex);
            normal_array.push_back(v.normal);
            uv_array.push_back(v.uv);
        }
        meshs_list.push_back(create_mesh_triangles(object2wrold, world2object, indices, vertex_array, normal_array, uv_array));
    }

    return meshs_list;
}

std::vector<TriangleMesh> load_mesh_triangles_from_obj(const Transform *object2wrold, const Transform *world2object, const char *obj_file_name, const char *base_path = nullptr)
{
    std::vector<TriangleMesh> meshs;
    auto list = load_mesh_triangles_list_from_obj(object2wrold, world2object, obj_file_name, base_path);
    std::pair<size_t, size_t> range;
    for (size_t i = 0; i < list.size(); ++i)
    {
        meshs.insert(meshs.end(),list[i].begin(),list[i].end());
    }
    return meshs;
}

//  std::vector<MeshTriangle> _union(std::vector<std::vector<MeshTriangle>>& triangles)
//  {
//      std::vector<MeshTriangle> all_triangles;
//      for(int i=0;i<triangles.size();++i)
//      {
//          all_triangles.insert(all_triangles.end(),all_triangles.begin(),all_triangles.end());
//      }
//      return all_triangles;
//  }
NARUKAMI_END