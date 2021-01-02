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
#include "core/file.h"
#include "tiny_obj_loader.h"
#include "tinyply.h"
NARUKAMI_BEGIN

enum class MeshFileFormat
{
    OBJ,
    PLY
};

template <MeshFileFormat format>
shared<Mesh> load_mesh(const shared<Transform> &object2world, const shared<Transform> &world2object, const char *file_name)
{
    return shared<Mesh>();
}

template <>
shared<Mesh> load_mesh<MeshFileFormat::OBJ>(const shared<Transform> &object2world, const shared<Transform> &world2object, const char *file_name)
{
    //第一阶段:载入模型的数据，并且判断数据是否载入正确
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file_name, "", true);
    if (!warn.empty())
    {
        std::cout << warn << std::endl;
    }
    if (!err.empty())
    {
        std::cerr << err << std::endl;
        exit(-1);
    }
    if (!ret)
    {
        std::cerr << "fail to load \"" << file_name << "\"" << std::endl;
        return shared<Mesh>();
    }

    //第二阶段:转换到渲染器的Mesh格式

    std::vector<Point3f> positions;
    for (size_t i = 0; i < attrib.vertices.size(); i += 3)
    {
        positions.push_back(Point3f(attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2]));
    }
    std::vector<Normal3f> normals;
    for (size_t i = 0; i < attrib.normals.size(); i += 3)
    {
        normals.push_back(Normal3f(attrib.normals[i], attrib.normals[i + 1], attrib.normals[i + 2]));
    }
    std::vector<Point2f> uvs;
    for (size_t i = 0; i < attrib.texcoords.size(); i += 2)
    {
        uvs.push_back(Point2f(attrib.texcoords[i], attrib.texcoords[i + 1]));
    }

    std::vector<MeshSegment> segments;
    for (size_t s = 0; s < shapes.size(); s++)
    {
        std::vector<MeshFace> faces;
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            size_t fv = shapes[s].mesh.num_face_vertices[f];
            assert(fv == 3); //目前只支持TriangleMesh

            uint32_t vi[3];
            uint32_t ni[3];
            uint32_t ti[3];
            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++)
            {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                vi[v] = idx.vertex_index;
                ni[v] = idx.normal_index;
                ti[v] = idx.texcoord_index;
            }
            index_offset += fv;
            faces.push_back(MeshFace(vi, ni, ti));
        }

        MeshSegment segment(faces);
        segments.push_back(segment);
    }
    shared<Mesh> mesh = std::make_shared<Mesh>(object2world, world2object, positions, normals, uvs, segments);
    return mesh;
}

template <>
shared<Mesh> load_mesh<MeshFileFormat::PLY>(const shared<Transform> &object2world, const shared<Transform> &world2object,const char *file_name)
{

    std::unique_ptr<std::istream> file_stream;
    std::vector<uint8_t> byte_buffer;
    byte_buffer = read_file_binary(file_name);
    file_stream.reset(new memory_stream((char *)byte_buffer.data(), byte_buffer.size()));

    tinyply::PlyFile file;
    file.parse_header(*file_stream);
    std::shared_ptr<tinyply::PlyData> ply_vertices, ply_normals, ply_texcoords,ply_faces;
    try
    {
        ply_vertices = file.request_properties_from_element("vertex", {"x", "y", "z"});
    }
    catch (const std::exception &e)
    {
        NARUKAMI_WARNING("tinyply exception %s", e.what())
    }
    try
    {
        ply_normals = file.request_properties_from_element("vertex", {"nx", "ny", "nz"});
    }
    catch (const std::exception &e)
    {
        NARUKAMI_WARNING("tinyply exception %s", e.what())
    }
    try
    {
        ply_texcoords = file.request_properties_from_element("vertex", {"u", "v"});
    }
    catch (const std::exception &e)
    {
        NARUKAMI_WARNING("tinyply exception %s", e.what())
    }
    try
    {
        ply_faces = file.request_properties_from_element("face", {"vertex_indices"}, 3);
    }
    catch (const std::exception &e)
    {
        NARUKAMI_WARNING("tinyply exception %s", e.what())
    }
    file.read(*file_stream);

    std::vector<Point3f> positions(ply_vertices->count);
    memcpy(positions.data(),ply_vertices->buffer.get(), ply_vertices->buffer.size_bytes());
    std::vector<Normal3f> normals;
    if(ply_normals)
    {
        normals.resize(ply_normals->count);
        memcpy(normals.data(),ply_normals->buffer.get(), ply_normals->buffer.size_bytes());
    }
    
    std::vector<Point2f> texcoords;
    if(ply_texcoords)
    {
        texcoords.resize(ply_normals->count);
        memcpy(texcoords.data(),ply_texcoords->buffer.get(), ply_texcoords->buffer.size_bytes());
    }

    std::vector<uint32_t> faces_array(ply_faces->count * 3);
    memcpy(faces_array.data(),ply_faces->buffer.get(), ply_faces->buffer.size_bytes());

    std::vector<MeshFace> faces;
    for(int i=0;i<ply_faces->count;++i)
    {
        uint32_t vi[3];
        memcpy(vi,&faces_array[i * 3], 3 * sizeof(uint32_t));
        faces.push_back(vi);
    }
    std::vector<MeshSegment> segments;
    MeshSegment segment(faces);
    segments.push_back(segment);
    shared<Mesh> mesh = std::make_shared<Mesh>(object2world, world2object, positions, normals, texcoords, segments);
    return mesh;
}
NARUKAMI_END