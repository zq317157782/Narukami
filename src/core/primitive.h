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
#include "core/hairstrands.h"
#include "core/geometry.h"
#include "core/spectrum.h"

NARUKAMI_BEGIN

struct PrimitiveHitPoint
{   
    float hit_t;
    Point2f param_uv;
    int compact_offset;
    int triangle_index; //Quad用来标记是哪个三角被射中了
};

class Primitive
{
    virtual Bounds3f bounds() const = 0;
};

std::vector<shared<Primitive>> concat(const std::vector<shared<Primitive>> &a, const std::vector<shared<Primitive>> &b);

class MeshPrimitive : public Primitive
{
private:
    shared<Mesh> _mesh;
    uint32_t _segment;
    uint32_t _face;

public:
    MeshPrimitive(const shared<Mesh> &mesh, uint32_t segment, uint32_t face) : Primitive(), _mesh(mesh), _segment(segment), _face(face) {}
    Bounds3f bounds() const override { return _mesh->get_face_bounds(_segment, _face); }
    const Transform &object_to_world() const { return _mesh->object_to_world(); }
    const Transform &world_to_object() const { return _mesh->world_to_object(); }
    Point3f get_vertex(uint32_t vertex) const { return _mesh->get_vertex(_segment, _face, vertex); }
    Point2f get_texcoord(const Point2f &u) const { return _mesh->get_texcoord(_segment, _face, u); }
    Point2f get_texcoord(uint32_t vertex) const { return _mesh->get_texcoord(_segment, _face, vertex); }

    void *operator new(size_t size);
    void operator delete(void *ptr);
};

std::vector<shared<MeshPrimitive>> create_mesh_primitives(const shared<Mesh> &mesh);

struct CompactMeshPrimitive
{
    TrianglePack triangle; //128 byte
};

bool intersect(RayPack &soa_ray, const CompactMeshPrimitive &compact_primitive, PrimitiveHitPoint* hit_point);
bool intersect(RayPack &soa_ray, const CompactMeshPrimitive &compact_primitive);
void setup_interaction(const CompactMeshPrimitive &, const shared<MeshPrimitive> &, const Ray &, const PrimitiveHitPoint &, SurfaceInteraction *);
std::vector<CompactMeshPrimitive> pack_compact_primitives(const std::vector<shared<MeshPrimitive>> &triangles, uint32_t start, uint32_t count, std::vector<uint32_t> *offsets);

class HairSegmentPrimitive : public Primitive
{
private:
    shared<HairStrands> _hairstrands;
    uint32_t _strand;
    uint32_t _segment;

public:
    HairSegmentPrimitive(const shared<HairStrands> &hairstrands, uint32_t strand, uint32_t segment) : Primitive(), _hairstrands(hairstrands), _strand(strand), _segment(segment) {}
    Bounds3f bounds() const override { return _hairstrands->bounds(_strand, _segment); }
    const Transform &object_to_world() const { return _hairstrands->object_to_world(); }
    const Transform &world_to_object() const { return _hairstrands->world_to_object(); }

    Point3f get_start_vertex() const { return _hairstrands->get_start_vertex(_strand, _segment); }
    Point3f get_end_vertex() const { return _hairstrands->get_end_vertex(_strand, _segment); }
    float get_start_thickness() const { return _hairstrands->get_start_thickness(_strand, _segment); }
    float get_end_thickness() const { return _hairstrands->get_end_thickness(_strand, _segment); }
};

std::vector<shared<HairSegmentPrimitive>> create_hair_segment_primitives(const shared<HairStrands> &hairstrands);

struct CompactHairSegmentPrimitive
{
    Point3fPack p0, p1; //96 byte
    float4 w0, w1;      //128byte
};

bool intersect(RayPack &soa_ray, const CompactHairSegmentPrimitive &compact_primitive, PrimitiveHitPoint* hit_point);
bool intersect(RayPack &soa_ray, const CompactHairSegmentPrimitive &compact_primitive);
void setup_interaction(const CompactHairSegmentPrimitive &, const shared<HairSegmentPrimitive> &, const Ray &, const PrimitiveHitPoint &, SurfaceInteraction *);
std::vector<CompactHairSegmentPrimitive> pack_compact_primitives(const std::vector<shared<HairSegmentPrimitive>> &segments, uint32_t start, uint32_t count, std::vector<uint32_t> *offsets);

NARUKAMI_END