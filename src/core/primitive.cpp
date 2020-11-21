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
#include "core/interaction.h"
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
            primitives.push_back(shared<MeshPrimitive>(new MeshPrimitive(mesh, s, f)));
        }
    }

    return primitives;
}

bool intersect(RayPack &soa_ray, const CompactMeshPrimitive &compact_primitive, PrimitiveHitPoint *hit_point)
{
    return intersect(soa_ray, compact_primitive.triangle, &hit_point->hit_t, &hit_point->param_uv, &hit_point->compact_offset);
}

bool intersect(RayPack &soa_ray, const CompactMeshPrimitive &compact_primitive)
{
    return intersect(soa_ray, compact_primitive.triangle);
}

void setup_interaction(const CompactMeshPrimitive &compact_primitive, const shared<MeshPrimitive> &primitive, const Ray &ray, const PrimitiveHitPoint &hit_point, SurfaceInteraction *interaction)
{
    auto triangle = compact_primitive.triangle[hit_point.compact_offset];
    //交点和法线
    interaction->p = get_vertex(triangle, hit_point.param_uv);
    interaction->n = hemisphere_flip(get_normalized_normal(triangle), -ray.d);
    //dpdu,dpdv
    Vector3f dpdu, dpdv;

    Vector3f dp10 = triangle.e1;
    Vector3f dp20 = triangle.e2;

    Point2f uv0 = primitive->get_texcoord(0);
    Point2f uv1 = primitive->get_texcoord(1);
    Point2f uv2 = primitive->get_texcoord(2);

    Vector2f duv10 = uv1 - uv0;
    Vector2f duv20 = uv2 - uv0;

    //使用 delta X 和 delta Y 计算导数
    //判断行列式 是否退化
    float det = duv10[0] * duv20[1] - duv20[0] * duv10[1];
    if (det == 0)
    {
        coordinate_system(interaction->n, &dpdu, &dpdv);
    }
    else
    {
        //求2x2矩阵的逆

        dpdu = duv20.y * dp10 - duv10.y * dp20;
        dpdv = -duv20.x * dp10 + duv10.x * dp20;
        dpdu /= det;
        dpdv /= det;
    }

    interaction->dpdu = dpdu;
    interaction->dpdv = dpdv;

    interaction->uv = primitive->get_texcoord(hit_point.param_uv);
}

std::vector<CompactMeshPrimitive> pack_compact_primitives(const std::vector<shared<MeshPrimitive>> &triangles, uint32_t start, uint32_t count, std::vector<uint32_t> *offsets)
{
    assert(count > 0);
    assert((start + count) <= triangles.size());

    uint32_t soa_count = (uint32_t)(count - 1) / SSE_WIDTH + 1;

    std::vector<Point3f> v0_array;
    std::vector<Vector3f> e1_array;
    std::vector<Vector3f> e2_array;

    for (uint32_t i = 0; i < soa_count * SSE_WIDTH; ++i)
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
        primitive.triangle.v0 = load(&v0_array[i * SSE_WIDTH]);
        primitive.triangle.e1 = load(&e1_array[i * SSE_WIDTH]);
        primitive.triangle.e2 = load(&e2_array[i * SSE_WIDTH]);
        //primitive.offset = start + i * SSE_WIDTH;
        soa_primitives.push_back(primitive);
        offsets->push_back(start + i * SSE_WIDTH);
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

std::vector<shared<HairSegmentPrimitive>> create_hair_segment_primitives(const shared<HairStrands> &hairstrands)
{
    std::vector<shared<HairSegmentPrimitive>> primitives;

    for (uint32_t s = 0; s < hairstrands->strands_count(); ++s)
    {
        for (int f = 0; f < hairstrands->segment_count(s); ++f)
        {
            primitives.push_back(shared<HairSegmentPrimitive>(new HairSegmentPrimitive(hairstrands, s, f)));
        }
        // if(s==100)
        // return primitives;
    }
    return primitives;
}
std::vector<CompactHairSegmentPrimitive> pack_compact_primitives(const std::vector<shared<HairSegmentPrimitive>> &segments, uint32_t start, uint32_t count, std::vector<uint32_t> *offsets)
{
    assert(count > 0);
    assert((start + count) <= segments.size());

    uint32_t soa_count = (uint32_t)(count - 1) / SSE_WIDTH + 1;

    std::vector<Point3f> p0_array;
    std::vector<Point3f> p1_array;
    std::vector<float> w0_array;
    std::vector<float> w1_array;

    for (uint32_t i = 0; i < soa_count * SSE_WIDTH; ++i)
    {
        if (i < count)
        {
            auto m = segments[start + i];
            auto p0 = m->get_start_vertex();
            auto p1 = m->get_end_vertex();
            auto w0 = m->get_start_thickness();
            auto w1 = m->get_end_thickness();

            p0_array.push_back(p0);
            p1_array.push_back(p1);
            w0_array.push_back(w0);
            w1_array.push_back(w1);
        }
        else
        {
            p0_array.push_back(Point3f());
            p1_array.push_back(Point3f());
            w0_array.push_back(0);
            w1_array.push_back(0);
        }
    }
    std::vector<CompactHairSegmentPrimitive> soa_primitives;

    for (uint32_t i = 0; i < soa_count; ++i)
    {
        CompactHairSegmentPrimitive primitive;
        primitive.p0 = load(&p0_array[i * SSE_WIDTH]);
        primitive.p1 = load(&p1_array[i * SSE_WIDTH]);
        primitive.w0 = load(&w0_array[i * SSE_WIDTH]);
        primitive.w1 = load(&w1_array[i * SSE_WIDTH]);
        soa_primitives.push_back(primitive);
        offsets->push_back(start + i * SSE_WIDTH);
    }

    return soa_primitives;
}

Quad construct_quadpack(const Ray &ray, const Point3f &p0, const Point3f &p1, float w0, float w1)
{
    auto T = p1 - p0;
    auto N = -ray.d;
    auto B = normalize(cross(N, T));
    auto hw0 = w0 * 0.5f;
    auto hw1 = w1 * 0.5f;
    Quad quad;
    quad.p0 = p0 - B * hw0;
    quad.p1 = p0 + B * hw0;
    quad.p2 = p1 - B * hw1;
    quad.p3 = p1 + B * hw1;
    return quad;
}

QuadPack construct_quadpack(const RayPack &ray, const CompactHairSegmentPrimitive &compact_primitive)
{
    auto T = compact_primitive.p1 - compact_primitive.p0;
    auto N = -ray.d;
    auto B = (cross(N, T));
    auto hw0 = compact_primitive.w0 * float4(0.5f);
    auto hw1 = compact_primitive.w1 * float4(0.5f);
    QuadPack quad;
    quad.p0 = compact_primitive.p0 - B * hw0;
    quad.p1 = compact_primitive.p0 + B * hw0;
    quad.p2 = compact_primitive.p1 - B * hw1;
    quad.p3 = compact_primitive.p1 + B * hw1;
    return quad;
}

//int *triangle_index, float *t_result, Point2f *uv, int *index, bool4 mask = SSE_MASK_TRUE
bool intersect(RayPack &soa_ray, const CompactHairSegmentPrimitive &compact_primitive, PrimitiveHitPoint *hit_point)
{
    QuadPack quad = construct_quadpack(soa_ray, compact_primitive);
   // std::cout << compact_primitive.p0 << " " << compact_primitive.p1 << " " << compact_primitive.w0 << " " << compact_primitive.w1 << std::endl;;
    return intersect(soa_ray, quad, &hit_point->triangle_index, &hit_point->hit_t, &hit_point->param_uv, &hit_point->compact_offset);
}

bool intersect(RayPack &soa_ray, const CompactHairSegmentPrimitive &compact_primitive)
{
    QuadPack quad = construct_quadpack(soa_ray, compact_primitive);
    return intersect(soa_ray, quad);
}
void setup_interaction(const CompactHairSegmentPrimitive &compact_primitive, const shared<HairSegmentPrimitive> &primitive, const Ray &ray, const PrimitiveHitPoint &hit_point, SurfaceInteraction *interaction)
{
    auto p0 = compact_primitive.p0[hit_point.compact_offset];
    auto p1 = compact_primitive.p1[hit_point.compact_offset];
    auto w0 = compact_primitive.w0[hit_point.compact_offset];
    auto w1 = compact_primitive.w1[hit_point.compact_offset];

    Quad quad = construct_quadpack(ray, p0, p1, w0, w1);
    Triangle triangle;
    if (hit_point.triangle_index == 0)
    {
        triangle.v0 = quad.p0;
        triangle.e1 = quad.p2 - quad.p0;
        triangle.e2 = quad.p3 - quad.p0;
    }
    else
    {
        triangle.v0 = quad.p0;
        triangle.e1 = quad.p3 - quad.p0;
        triangle.e2 = quad.p1 - quad.p0;
    }
    //交点和法线
    interaction->p = get_vertex(triangle, hit_point.param_uv);
    interaction->n = hemisphere_flip(get_normalized_normal(triangle), -ray.d);
    interaction->uv = hit_point.param_uv;
}

NARUKAMI_END