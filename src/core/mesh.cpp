#include "core/mesh.h"
#include "core/memory.h"
#include <vector>

NARUKAMI_BEGIN

MeshData::MeshData(const Transform &object2wrold, const std::vector<uint32_t> &indices, const std::vector<Point3f> &vertices, const std::vector<Normal3f> &normals, const std::vector<Point2f> &uvs)
{
    for (auto &&i : indices)
    {
        this->indices.push_back(i);
    }
    for (auto &&i : vertices)
    {
        this->vertices.push_back(object2wrold(i));
    }
    for (auto &&i : normals)
    {
        this->normals.push_back(object2wrold(i));
    }
    for (auto &&i : uvs)
    {
        this->uvs.push_back(i);
    }
}

MeshData::MeshData(const Transform &object2wrold, const std::vector<uint32_t> &indices, const std::vector<Point3f> &vertices)
{
    for (auto &&i : indices)
    {
        this->indices.push_back(i);
    }
    for (auto &&i : vertices)
    {
        this->vertices.push_back(object2wrold(i));
    }
}

MeshData::MeshData(const Transform &object2wrold, const std::vector<uint32_t> &indices, const std::vector<Point3f> &vertices, const std::vector<Normal3f> &normals)
{
    for (auto &&i : indices)
    {
        this->indices.push_back(i);
    }
    for (auto &&i : vertices)
    {
        this->vertices.push_back(object2wrold(i));
    }
    for (auto &&i : normals)
    {
        this->normals.push_back(object2wrold(i));
    }
}

MeshData::MeshData(const Transform &object2wrold, const std::vector<uint32_t> &indices, const std::vector<Point3f> &vertices, const std::vector<Point2f> &uvs)
{
    for (auto &&i : indices)
    {
        this->indices.push_back(i);
    }
    for (auto &&i : vertices)
    {
        this->vertices.push_back(object2wrold(i));
    }
    for (auto &&i : uvs)
    {
        this->uvs.push_back(i);
    }
}

//TODO SSE alignas
std::vector<SoATriangle> cast2SoA(const std::vector<MeshTriangle> &triangles, uint32_t start, uint32_t count)
{
    assert(count > 0);
    assert((start + count) <= triangles.size());

    size_t soa_count = (uint32_t)(count-1) / SSE_FLOAT_COUNT + 1;

    std::vector<Point3f> v0_array;
    std::vector<Vector3f> e1_array;
    std::vector<Vector3f> e2_array;

    for (size_t i = 0; i < soa_count * SSE_FLOAT_COUNT; ++i)
    {
        if (i < count)
        {
            auto v0 = triangles[start + i][0];
            auto e1 = triangles[start + i][1] - v0;
            auto e2 = triangles[start + i][2] - v0;

            v0_array.push_back(v0);
            e1_array.push_back(e1);
            e2_array.push_back(e2);
        }
        else
        {
            v0_array.push_back(Point3f(0,0,0));
            e1_array.push_back(Vector3f(0,0,0));
            e2_array.push_back(Vector3f(0,0,0));
        }
    }
    std::vector<SoATriangle> soa_triangles;

    for (size_t i = 0; i < soa_count; ++i)
    {
        SoATriangle triangle;
        triangle.v0 = load(&v0_array[i * SSE_FLOAT_COUNT]);
        triangle.e1 = load(&e1_array[i * SSE_FLOAT_COUNT]);
        triangle.e2 = load(&e2_array[i * SSE_FLOAT_COUNT]);
        soa_triangles.push_back(triangle);
    }

    return soa_triangles;
}

std::vector<MeshTriangle> create_mesh_triangles(const Transform *object2wrold, const Transform *world2object, const std::vector<uint32_t> &indices, const std::vector<Point3f> &vertices, const std::vector<Normal3f> &normals, const std::vector<Point2f> &uvs)
{
    std::shared_ptr<MeshData> mesh_data = std::make_shared<MeshData>(*object2wrold, indices, vertices, normals, uvs);
    std::vector<MeshTriangle> triangles;
    auto triangle_num = indices.size() / 3;
    for (size_t i = 0; i < triangle_num; ++i)
    {
        triangles.push_back(MeshTriangle(object2wrold, world2object, mesh_data, &indices[3 * i]));
    }
    return triangles;
}

NARUKAMI_END