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

void * MeshPrimitive::operator new(size_t size)
{
    return g_mesh_primitive_pool.alloc();
}

void  MeshPrimitive::operator delete(void * ptr)
{
    g_mesh_primitive_pool.dealloc(reinterpret_cast<MeshPrimitive*>(ptr));
}

std::vector<ref<MeshPrimitive>> create_mesh_primitives(const std::vector<ref<TriangleMesh>>& meshs){
    std::vector<ref<MeshPrimitive>> primitives;
    for (uint32_t i = 0; i < meshs.size(); ++i)
    {
        primitives.push_back(ref<MeshPrimitive>(new MeshPrimitive(meshs[i])));
    }
    
    return primitives;
}

std::vector<SoAPrimitiveInfo> pack_mesh_primitives(const std::vector<ref<MeshPrimitive>> &triangles, uint32_t start, uint32_t count)
{
    assert(count > 0);
    assert((start + count) <= triangles.size());

    uint32_t soa_count = (uint32_t)(count-1)/ SSE_FLOAT_COUNT + 1;

    std::vector<Point3f> v0_array;
    std::vector<Vector3f> e1_array;
    std::vector<Vector3f> e2_array;

    for (uint32_t i = 0; i < soa_count * SSE_FLOAT_COUNT; ++i)
    {
        if (i < count)
        {
            auto m = triangles[start + i]->mesh();
            auto v0 = (*m)[0];
            auto e1 = (*m)[1] - v0;
            auto e2 = (*m)[2] - v0;

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
    std::vector<SoAPrimitiveInfo> soa_primitives;

    for (uint32_t i = 0; i < soa_count; ++i)
    {
        SoAPrimitiveInfo primitive;
        primitive.triangle.v0 = load(&v0_array[i * SSE_FLOAT_COUNT]);
        primitive.triangle.e1 = load(&e1_array[i * SSE_FLOAT_COUNT]);
        primitive.triangle.e2 = load(&e2_array[i * SSE_FLOAT_COUNT]);
        primitive.offset = start + i * SSE_FLOAT_COUNT;
        soa_primitives.push_back(primitive);
    }

    return soa_primitives;
}

std::vector<ref<Primitive>> concat(const std::vector<ref<Primitive>>& a,const std::vector<ref<Primitive>>& b)
{
    std::vector<ref<Primitive>> c;
    c.insert(c.end(),a.begin(),a.end());
    c.insert(c.end(),b.begin(),b.end());
    return c;
}

NARUKAMI_END