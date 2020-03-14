#include "core/mesh.h"
#include "core/memory.h"
#include <vector>

NARUKAMI_BEGIN

MeshData& MeshData::add_vertices(const std::vector<Point3f> &vertices)
{
    for (auto &&i : vertices)
    {
        this->vertices.push_back(i);
    }
    return (*this);
}
MeshData& MeshData::add_normals( const std::vector<Normal3f> &normals)
{
    for (auto &&i : normals)
    {
        this->normals.push_back(i);
    }
    return (*this);
}
MeshData& MeshData::add_uvs(const std::vector<Point2f> &uvs)
{
    for (auto &&i : uvs)
    {
        this->uvs.push_back(i);
    }
    return (*this);
}
MeshData& MeshData::add_indices(const std::vector<uint32_t> &indices)
{
    for (auto &&i : indices)
    {
        this->indices.push_back(i);
    }
    return (*this);
}

MeshData& MeshData::add_transform_vertices(const Transform &trans,const std::vector<Point3f> &vertices)
{
    for (auto &&i : vertices)
    {
        this->vertices.push_back(trans(i));
    }
    return (*this);
}

MeshData& MeshData::add_transform_normals(const Transform &trans,const std::vector<Normal3f> &normals)
{
    for (auto &&i : normals)
    {
        this->normals.push_back(trans(i));
    }
    return (*this);
}

void MeshTriangle::init(const Transform* object2world,const Transform* world2object,const MeshManager* mm,uint32_t mesh_data,const uint32_t idx[3])
{
    _object2world = object2world;
    _world2object = world2object;
    _mm = mm;
    _mesh_data_index = mesh_data;
    memcpy(_index,idx,sizeof(uint32_t)*3);
}

const MeshData& MeshTriangle::mesh() const{return _mm->get_mesh_data_ref(_mesh_data_index);}

 MeshData& MeshManager::add_get_mesh_data_ref(uint32_t& index)
 {
     STAT_INCREASE_COUNTER(MeshData_count,1)
     index=_mesh_datas.size();
    _mesh_datas.emplace_back();
    return _mesh_datas.back();
 }

 MeshTriangle& MeshManager::add_get_mesh_triangle_ref(uint32_t& index)
 {
    STAT_INCREASE_COUNTER(MeshTriangle_count,1)
    index=_mesh_triangles.size();
    _mesh_triangles.emplace_back();
    return _mesh_triangles.back();
 }

//TODO SSE alignas
std::vector<SoATriangle> SoA_pack(const MeshManager& manager,const std::pair<uint32_t,uint32_t>& range)
{
    uint32_t start = range.first;
    uint32_t end = range.second;
    uint32_t count = end - start;
    assert(count > 0);
    assert((start + count) <= manager.mesh_triange_size());

    uint32_t soa_count = (uint32_t)(count - 1) / SSE_FLOAT_COUNT + 1;

    std::vector<Point3f> v0_array;
    std::vector<Vector3f> e1_array;
    std::vector<Vector3f> e2_array;

    for (uint32_t i = 0; i < soa_count * SSE_FLOAT_COUNT; ++i)
    {
        if (i < count)
        {
            auto v0 = manager.get_mesh_triangle_ref(start + i)[0];
            auto e1 = manager.get_mesh_triangle_ref(start + i)[1] - v0;
            auto e2 = manager.get_mesh_triangle_ref(start + i)[2] - v0;

            v0_array.push_back(v0);
            e1_array.push_back(e1);
            e2_array.push_back(e2);
        }
        else
        {
            v0_array.push_back(Point3f(0, 0, 0));
            e1_array.push_back(Vector3f(0, 0, 0));
            e2_array.push_back(Vector3f(0, 0, 0));
        }
    }
    std::vector<SoATriangle> soa_triangles;

    for (uint32_t i = 0; i < soa_count; ++i)
    {
        SoATriangle triangle;
        triangle.v0 = load(&v0_array[i * SSE_FLOAT_COUNT]);
        triangle.e1 = load(&e1_array[i * SSE_FLOAT_COUNT]);
        triangle.e2 = load(&e2_array[i * SSE_FLOAT_COUNT]);
        soa_triangles.push_back(triangle);
    }

    return soa_triangles;
}

std::pair<uint32_t,uint32_t> create_mesh_triangles(const Transform *object2wrold, const Transform *world2object, const std::vector<uint32_t> &indices, const std::vector<Point3f> &vertices, const std::vector<Normal3f> &normals, const std::vector<Point2f> &uvs,MeshManager& manager)
{
    
    uint32_t mesh_index;
    MeshData& mesh_data = manager.add_get_mesh_data_ref(mesh_index);
    mesh_data.add_transform_vertices(*object2wrold,vertices)
             .add_indices(indices)
             .add_transform_normals(*object2wrold,normals)
             .add_uvs(uvs);

    auto triangle_num = indices.size() / 3;
    uint32_t start_index = manager.mesh_triange_size();
    for (uint32_t i = 0; i < triangle_num; ++i)
    {
        uint32_t dmp;
        MeshTriangle& mesh_triangle = manager.add_get_mesh_triangle_ref(dmp);
        mesh_triangle.init(object2wrold, world2object,&manager, mesh_index, &indices[3 * i]);
    }
    uint32_t end_index = manager.mesh_triange_size();
    return std::pair<uint32_t,uint32_t>(start_index,end_index);
}



std::pair<uint32_t,uint32_t>  create_plane(const Transform *object2wrold, const Transform *world2object, const float width, const float height,MeshManager& manager)
{
    std::vector<std::vector<MeshTriangle>> triangles;
    float hw = width * 0.5f;
    float hh = height * 0.5f;
    std::vector<Point3f> vertices = {Point3f(hw, hh, 0), Point3f(hw, -hh, 0), Point3f(-hw, -hh, 0), Point3f(-hw, hh, 0)};
    std::vector<Normal3f> normals = {Normal3f(0, 0, 1), Normal3f(0, 0, 1), Normal3f(0, 0, 1), Normal3f(0, 0, 1)};
    std::vector<Point2f> uvs = {Point2f(0, 1), Point2f(0, 0), Point2f(1, 0), Point2f(1, 1)};
    std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

    return create_mesh_triangles(object2wrold, world2object, indices, vertices, normals, uvs,manager);
}

std::pair<uint32_t,uint32_t> create_disk(const Transform *object2wrold, const Transform *world2object,float radius, const uint32_t vertex_density,MeshManager& manager)
{
        assert(radius > 0);
        std::vector<std::vector<MeshTriangle>> triangles;
        std::vector<Point3f> vertices = {Point3f(0, 0, 0)};
        std::vector<Normal3f> normals = {Normal3f(0, 0, 1)};
        std::vector<Point2f> uvs = {Point2f(0, 0)};
        std::vector<uint32_t> indices;
        float theta_step = 2.0f * PI / vertex_density;
        float theta = 0;
        for (uint32_t i = 1; i <= vertex_density; i++)
        {
            Point3f v(cos(theta) * radius, sin(theta) * radius, 0);
            vertices.push_back(v);
            uvs.push_back(Point2f(0, 0));
            if (i > 1)
            {
                indices.push_back(i-1);
                indices.push_back(0);
                indices.push_back(i);
            }
            theta += theta_step;
        }

        //last piece
        indices.push_back(vertex_density);
        indices.push_back(0);
        indices.push_back(1);

        return create_mesh_triangles(object2wrold, world2object, indices, vertices, normals, uvs,manager);
    }

NARUKAMI_END