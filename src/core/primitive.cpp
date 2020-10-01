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
#include "core/primitive.h"
#include "core/memory.h"
NARUKAMI_BEGIN

MemoryPool<MeshPrimitive> g_mesh_primitive_pool(4096);

void *MeshPrimitive::operator new(size_t size)
{
    return g_mesh_primitive_pool.alloc();
}

void MeshPrimitive::operator delete(void *ptr)
{
    g_mesh_primitive_pool.dealloc(reinterpret_cast<MeshPrimitive *>(ptr));
}

std::vector<shared<MeshPrimitive>> create_mesh_primitives(const shared<Mesh> &mesh)
{

    std::vector<shared<MeshPrimitive>> primitives;

    for (uint32_t s = 0; s < mesh->get_segment_count(); ++s)
    {
        for (int f = 0; f < mesh->get_face_count(s); ++f)
        {
             primitives.push_back(shared<MeshPrimitive>(new MeshPrimitive(mesh,s,f)));
        }
    }

    return primitives;
}

std::vector<CompactMeshPrimitive> pack_mesh_primitives(const std::vector<shared<MeshPrimitive>> &triangles, uint32_t start, uint32_t count)
{
    assert(count > 0);
    assert((start + count) <= triangles.size());

    uint32_t soa_count = (uint32_t)(count - 1) / SSE_FLOAT_COUNT + 1;

    std::vector<Point3f> v0_array;
    std::vector<Vector3f> e1_array;
    std::vector<Vector3f> e2_array;

    for (uint32_t i = 0; i < soa_count * SSE_FLOAT_COUNT; ++i)
    {
        if (i < count)
        {
            auto m = triangles[start + i];
            auto v0 = m->get_vertex(0);
            auto e1 = m->get_vertex(1) - v0;
            auto e2 = m->get_vertex(2) - v0;

            v0_array.push_back(v0);
            e1_array.push_back(e1);
            e2_array.push_back(e2);
        }
        else
        {
            v0_array.push_back(Point3f());
            e1_array.push_back(Vector3f());
            e2_array.push_back(Vector3f());
        }
    }
    std::vector<CompactMeshPrimitive> soa_primitives;

    for (uint32_t i = 0; i < soa_count; ++i)
    {
        CompactMeshPrimitive primitive;
        primitive.triangle.v0 = load(&v0_array[i * SSE_FLOAT_COUNT]);
        primitive.triangle.e1 = load(&e1_array[i * SSE_FLOAT_COUNT]);
        primitive.triangle.e2 = load(&e2_array[i * SSE_FLOAT_COUNT]);
        primitive.offset = start + i * SSE_FLOAT_COUNT;
        soa_primitives.push_back(primitive);
    }

    return soa_primitives;
}

std::vector<shared<Primitive>> concat(const std::vector<shared<Primitive>> &a, const std::vector<shared<Primitive>> &b)
{
    std::vector<shared<Primitive>> c;
    c.insert(c.end(), a.begin(), a.end());
    c.insert(c.end(), b.begin(), b.end());
    return c;
}

NARUKAMI_END