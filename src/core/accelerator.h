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
#include "core/primitive.h"
#include "core/memory.h"
#include "core/geometry.h"
#include "core/interaction.h"
#include "core/stat.h"
#include <vector>
#include <stack>
#include <algorithm>
NARUKAMI_BEGIN

constexpr uint32_t BLAS_ELEMENT_NUM_PER_LEAF = 64;
constexpr int BLAS_SAH_BUCKET_NUM = 12;

constexpr uint32_t ACCELERATOR_ELEMENT_NUM_PER_LEAF = 64;
constexpr int ACCELERATOR_SAH_BUCKET_NUM = 12;

struct BVHMeshPrimitiveInfo
{
    uint32_t prim_index;
    Bounds3f bounds;
    Point3f centroid;
    BVHMeshPrimitiveInfo() = default;
    BVHMeshPrimitiveInfo(const ref<MeshPrimitive> &p, uint32_t index) : prim_index(index), bounds(p->bounds()), centroid((p->bounds().min_point + p->bounds().max_point) * 0.5f) {}
};

struct BVHBuildNode
{
    Bounds3f bounds;
    BVHBuildNode *childrens[2];
    uint32_t split_axis;
    uint32_t offset, num;
};

inline void init_leaf(BVHBuildNode *node, const uint32_t offset, const uint32_t num, const Bounds3f &bounds)
{
    node->bounds = bounds;
    node->offset = offset;
    node->num = num;
    node->childrens[0] = nullptr;
    node->childrens[1] = nullptr;
}

inline void init_interior(BVHBuildNode *node, BVHBuildNode *c0, BVHBuildNode *c1, const uint32_t split_axis)
{
    node->bounds = _union(c0->bounds, c1->bounds);
    node->childrens[0] = c0;
    node->childrens[1] = c1;
    node->split_axis = split_axis;
}

inline bool is_leaf(const BVHBuildNode *node)
{
    if (node->childrens[0] == nullptr && node->childrens[1] == nullptr)
    {
        return true;
    }
    return false;
}

struct QBVHCollapseNode
{
    const BVHBuildNode *data[4];
    QBVHCollapseNode *childrens[4];
    uint32_t axis0, axis1, axis2;
};

//128 bytes
struct SSE_ALIGNAS QBVHNode
{
    Bounds3f4p bounds;
    uint32_t childrens[4];
    uint32_t axis0, axis1, axis2;
    uint32_t fill;
};

inline uint32_t leaf(const uint32_t offset, const uint32_t num)
{
    auto bits = 0x80000000;                      //set flag for leaf 1 bits
    bits = (((offset & 0x7FFFFFF) << 4) | bits); //set offset 27 bits
    bits = (((num - 1) & 0XF) | bits);           //set number 4 bits
    return bits;
}

inline uint32_t empty_leaf()
{
    return 0XFFFFFFFF;
}
inline bool leaf_is_empty(const uint32_t bits)
{
    return (bits & 0XFFFFFFFF) == 0XFFFFFFFF;
}

inline uint32_t interior(const uint32_t bits)
{
    return (bits & 0x7FFFFFFF);
}

inline bool is_leaf(const uint32_t bits)
{
    return bits & 0x80000000;
}
inline uint32_t leaf_offset(const uint32_t bits)
{
    return (bits >> 4) & 0x7FFFFFF;
}
inline uint32_t leaf_num(const uint32_t bits)
{
    return ((bits)&0xF) + 1;
}

inline void init_QBVH_node(QBVHNode *node, const QBVHCollapseNode *cn)
{
    Bounds3f bounds[4];

    bounds[0] = cn->data[0]->bounds;
    if (cn->data[1] != nullptr)
    {
        bounds[1] = cn->data[1]->bounds;
    }
    else
    {
        bounds[1] = Bounds3f();
    }

    if (cn->data[2] != nullptr)
    {
        bounds[2] = cn->data[2]->bounds;
    }
    else
    {
        bounds[2] = Bounds3f();
    }

    if (cn->data[3] != nullptr)
    {
        bounds[3] = cn->data[3]->bounds;
    }
    else
    {
        bounds[3] = Bounds3f();
    }

    node->bounds = Bounds3f4p(bounds);

    if (is_leaf(cn->data[0]))
    {
        node->childrens[0] = leaf(cn->data[0]->offset, cn->data[0]->num);
    }
    if (cn->data[1] == nullptr)
    {
        node->childrens[1] = empty_leaf();
    }
    else if (is_leaf(cn->data[1]))
    {
        node->childrens[1] = leaf(cn->data[1]->offset, cn->data[1]->num);
    }
    if (cn->data[2] == nullptr)
    {
        node->childrens[2] = empty_leaf();
    }
    else if (is_leaf(cn->data[2]))
    {
        node->childrens[2] = leaf(cn->data[2]->offset, cn->data[2]->num);
    }
    if (cn->data[3] == nullptr)
    {
        node->childrens[3] = empty_leaf();
    }
    else if (is_leaf(cn->data[3]))
    {
        node->childrens[3] = leaf(cn->data[3]->offset, cn->data[3]->num);
    }
}

struct BucketInfo
{
    Bounds3f bounds;
    uint32_t count = 0;
};

//BLAS ONLY
STAT_COUNTER("accelerator/MeshPrimitive's num", MeshPrimitiveInfo_count)
STAT_COUNTER("accelerator/MeshPrimitiveInfo4p's num", MeshPrimitiveInfo4p_count)
STAT_MEMORY_COUNTER("accelerator/MeshPrimitive's memory", Primitive_memory_cost)
STAT_MEMORY_COUNTER("accelerator/MeshPrimitiveInfo4p's memory", MeshPrimitiveInfo4p_memory_cost)
STAT_PERCENT("accelerator/MeshPrimitiveInfo4p(1)'s ratio", SoAPrimitiveInfo_num_1_4, SoAPrimitiveInfo_denom_1_4)
STAT_PERCENT("accelerator/MeshPrimitiveInfo4p(2)'s ratio", SoAPrimitiveInfo_num_2_4, SoAPrimitiveInfo_denom_2_4)
STAT_PERCENT("accelerator/MeshPrimitiveInfo4p(3)'s ratio", SoAPrimitiveInfo_num_3_4, SoAPrimitiveInfo_denom_3_4)
STAT_PERCENT("accelerator/MeshPrimitiveInfo4p(4)'s ratio", SoAPrimitiveInfo_num_4_4, SoAPrimitiveInfo_denom_4_4)
//TLAS ONLY
STAT_COUNTER("accelerator/BLASInstance's num", BLASInstance_count)
STAT_COUNTER("accelerator/BLASInstanceInfo4p's num", BLASInstanceInfo4p_count)
STAT_PERCENT("accelerator/BLASInstanceInfo4p(1)'s ratio", BLASInstanceInfo4p_num_1_4, BLASInstanceInfo4p_denom_1_4)
STAT_PERCENT("accelerator/BLASInstanceInfo4p(2)'s ratio", BLASInstanceInfo4p_num_2_4, BLASInstanceInfo4p_denom_2_4)
STAT_PERCENT("accelerator/BLASInstanceInfo4p(3)'s ratio", BLASInstanceInfo4p_num_3_4, BLASInstanceInfo4p_denom_3_4)
STAT_PERCENT("accelerator/BLASInstanceInfo4p(4)'s ratio", BLASInstanceInfo4p_num_4_4, BLASInstanceInfo4p_denom_4_4)

// GENERL
STAT_MEMORY_COUNTER("accelerator/QBVH node's memory", QBVH_node_memory_cost)
STAT_PERCENT("accelerator/ratio of travel QBVH's four subnode(25%:just one subnode is visited. 50%:two subnodes are  visited and so on.) ", ordered_traversal_num, ordered_traversal_denom)

class ProgressReporter;

struct Payload
{
    bool is_hit;
    float closest_hit_t;
    int instance_index;
    int primitive_index;
    int triangle_index;
    Point2f triangle_uv;
    Vector3f blas_ray_direction;
    Payload() : is_hit(false), closest_hit_t(INFINITE),instance_index(0),primitive_index(0), triangle_index(0), triangle_uv(Point2f(0.0f)), blas_ray_direction(Vector3f(0.0f)) {}
    bool is_closer(float t) {return t < closest_hit_t;}
};

class BLAS
{
private:
    std::vector<ref<MeshPrimitive>> _primitives;
    std::vector<MeshPrimitiveInfo4p> _soa_primitive_infos;
    std::vector<QBVHNode> _nodes;

    Bounds3f _bounds;

    BVHBuildNode *build(MemoryArena &arena, uint32_t start, uint32_t end, std::vector<BVHMeshPrimitiveInfo> &primitive_infos, std::vector<ref<MeshPrimitive>> &ordered, uint32_t *total);
    void build_soa_primitive_info(BVHBuildNode *node);

public:
    BLAS(const std::vector<ref<MeshPrimitive>> &primitives);
    bool closet_hit(MemoryArena &arena, const Ray &ray,Payload* payload) const;
    void fill_interaction(const Payload* result, Interaction *interaction) const;
    bool anyhit(const Ray &ray) const;

    Bounds3f bounds() const { return _bounds; }
};

class BLASInstance
{
private:
    ref<BLAS> _blas;
    const Transform *_world_to_blas;
    const Transform *_blas_to_world;
    Bounds3f _bounds;

public:
    BLASInstance(const Transform *blas_to_world, const Transform *world_to_blas, const ref<BLAS> &blas) : _blas_to_world(blas_to_world), _world_to_blas(world_to_blas), _blas(blas) { _bounds = (*_blas_to_world)(_blas->bounds()); };

    bool closet_hit(MemoryArena &arena, const Ray &ray,Payload* payload) const
    {
        auto blas_ray = (*_world_to_blas)(ray);
        bool has_hit = _blas->closet_hit(arena, blas_ray,payload);
        return has_hit;
    }

    void fill_interaction(const Payload* result, Interaction *interaction) const
    {
        _blas->fill_interaction(result, interaction);
        //从BLAS空间，转换到世界空间
        (*interaction) = (*_blas_to_world)(*interaction);
    }


    bool anyhit(const Ray &ray) const
    {
        auto blas_ray = (*_world_to_blas)(ray);
        bool ret = _blas->anyhit(blas_ray);
        if (ret)
        {
            ray.t_max = blas_ray.t_max;
        }
        return ret;
    }
    Bounds3f bounds() const { return _bounds; }
};

struct BLASInstanceInfo
{
    uint32_t instance_index;
    Bounds3f bounds;
    Point3f centroid;
    BLASInstanceInfo() = default;
    BLASInstanceInfo(const ref<BLASInstance> &instance, uint32_t index) : instance_index(index), bounds(instance->bounds()), centroid((instance->bounds().min_point + instance->bounds().max_point) * 0.5f) {}
};

struct BLASInstanceInfo4p
{
    Bounds3f4p bounds;
    uint32_t offset;
};

class TLAS
{
private:
    std::vector<ref<BLASInstance>> _instances;
    std::vector<BLASInstanceInfo4p> _soa_instance_infos;
    std::vector<QBVHNode> _nodes;

    BVHBuildNode *build(MemoryArena &arena, uint32_t start, uint32_t end, std::vector<BLASInstanceInfo> &instance_infos, std::vector<ref<BLASInstance>> &ordered, uint32_t *total);
    void build_soa_instance_info(BVHBuildNode *node);

public:
    TLAS(const std::vector<ref<BLASInstance>> &instance);
    bool closet_hit(MemoryArena &arena, const Ray &ray, Interaction *interaction) const;
    bool anyhit(const Ray &ray) const;
};

NARUKAMI_END
