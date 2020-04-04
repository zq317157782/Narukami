#include "core/mesh.h"
#include "core/memory.h"
#include <vector>

NARUKAMI_BEGIN

MemoryPool<VertexData>   g_vertex_data_pool(256);
MemoryPool<TriangleMesh> g_triangle_mesh_pool(4095);

void * VertexData::operator new(size_t size)
{
    return g_vertex_data_pool.alloc();
}

void  VertexData::operator delete(void * ptr)
{
    g_vertex_data_pool.dealloc(reinterpret_cast<VertexData*>(ptr));
}

void * TriangleMesh::operator new(size_t size)
{
    return g_triangle_mesh_pool.alloc();
}

void  TriangleMesh::operator delete(void * ptr)
{
    g_triangle_mesh_pool.dealloc(reinterpret_cast<TriangleMesh*>(ptr));
}

void append(std::vector<std::shared_ptr<TriangleMesh>> &A, const std::vector<std::shared_ptr<TriangleMesh>> &B)
{
    A.insert(A.end(), B.begin(), B.end());
}

std::vector<std::shared_ptr<TriangleMesh>> concat(const std::vector<std::shared_ptr<TriangleMesh>> &A, const std::vector<std::shared_ptr<TriangleMesh>> &B)
{
    std::vector<std::shared_ptr<TriangleMesh>> C;
    C.insert(C.end(), A.begin(), A.end());
    C.insert(C.end(), B.begin(), B.end());
    return C;
}

//TODO SSE alignas
std::vector<SoATriangle> SoA_pack(const std::vector<std::shared_ptr<TriangleMesh>> &meshs)
{
    uint32_t start = 0;
    uint32_t end = meshs.size();
    uint32_t count = end - start;
    assert(count > 0);

    uint32_t soa_count = (uint32_t)(count - 1) / SSE_FLOAT_COUNT + 1;

    std::vector<Point3f> v0_array;
    std::vector<Vector3f> e1_array;
    std::vector<Vector3f> e2_array;

    for (uint32_t i = 0; i < soa_count * SSE_FLOAT_COUNT; ++i)
    {
        if (i < count)
        {
            auto m = meshs[i];
            auto v0 = (*m)[0];
            auto e1 = (*m)[1] - v0;
            auto e2 = (*m)[2] - v0;

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

std::vector<std::shared_ptr<TriangleMesh>> create_mesh_triangles(const Transform *object2world, const Transform *world2object, const std::vector<uint32_t> &indices, const std::vector<Point3f> &positions, const std::vector<Normal3f> &normals, const std::vector<Point2f> &uvs)
{
    auto vertex_data = std::shared_ptr<VertexData>(new VertexData());
    for(uint32_t i = 0;i<positions.size();++i)
    {
        vertex_data->positions.push_back((*object2world)(positions[i]));
    }
    for(uint32_t i = 0;i<normals.size();++i)
    {
        vertex_data->normals.push_back((*object2world)(normals[i]));
    }

    vertex_data->uvs = uvs;
    
    std::vector<std::shared_ptr<TriangleMesh>> meshs;
   
    uint32_t mesh_size = indices.size() / 3;
    for (uint32_t m = 0; m < mesh_size; ++m)
    {
        uint32_t index[3];
        index[0] = indices[m * 3];
        index[1] = indices[m * 3 + 1];
        index[2] = indices[m * 3 + 2];
        auto triangle_mesh = std::shared_ptr<TriangleMesh>(new TriangleMesh(object2world,world2object,vertex_data,index));
        meshs.push_back(triangle_mesh);
    }
    return meshs;
}

std::vector<std::shared_ptr<TriangleMesh>> create_plane(const Transform *object2wrold, const Transform *world2object, const float width, const float height)
{
    float hw = width * 0.5f;
    float hh = height * 0.5f;
    std::vector<Point3f> vertices = {Point3f(hw, hh, 0), Point3f(hw, -hh, 0), Point3f(-hw, -hh, 0), Point3f(-hw, hh, 0)};
    std::vector<Normal3f> normals = {Normal3f(0, 0, 1), Normal3f(0, 0, 1), Normal3f(0, 0, 1), Normal3f(0, 0, 1)};
    std::vector<Point2f> uvs = {Point2f(0, 1), Point2f(0, 0), Point2f(1, 0), Point2f(1, 1)};
    std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

    return create_mesh_triangles(object2wrold, world2object, indices, vertices, normals, uvs);
}

std::vector<std::shared_ptr<TriangleMesh>> create_disk(const Transform *object2wrold, const Transform *world2object, float radius, const uint32_t vertex_density)
{
    assert(radius > 0);
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
            indices.push_back(i - 1);
            indices.push_back(0);
            indices.push_back(i);
        }
        theta += theta_step;
    }

    //last piece
    indices.push_back(vertex_density);
    indices.push_back(0);
    indices.push_back(1);

    return create_mesh_triangles(object2wrold, world2object, indices, vertices, normals, uvs);
}

NARUKAMI_END