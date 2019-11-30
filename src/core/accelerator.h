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

constexpr uint32_t ACCELERATOR_TIRANGLE_NUM_PER_LEAF=64;
constexpr int ACCELERATOR_SAH_BUCKET_NUM = 12;

struct BVHPrimitiveInfo
{
    size_t prim_index;
    Bounds3f bounds;
    Point3f centroid;
    BVHPrimitiveInfo() = default;
    BVHPrimitiveInfo(const Primitive &p, size_t index) : prim_index(index), bounds(get_world_bounds(p)), centroid((get_world_bounds(p).min_point + get_world_bounds(p).max_point) * 0.5f) {}
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
struct  SSE_ALIGNAS QBVHNode
{
    SoABounds3f bounds;
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
    bounds[2] = cn->data[2]->bounds;
    if (cn->data[3] != nullptr)
    {
        bounds[3] = cn->data[3]->bounds;
    }
    else
    {
        bounds[3] = Bounds3f();
    }

    node->bounds = SoABounds3f(bounds);

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
    if (is_leaf(cn->data[2]))
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


struct BucketInfo{
    Bounds3f bounds;
    uint32_t count=0;
};


STAT_COUNTER("count of SoAPrimitiveInfo",SoAPrimitiveInfo_count)
STAT_COUNTER("count of SoAPrimitiveInfo without four triangle",SoAPrimitiveInfo_notfull_count)
STAT_MEMORY_COUNTER("memory cost for Primitive array",Primitive_memory_cost)
STAT_MEMORY_COUNTER("memory cost for SoAPrimitiveInfo array",SoAPrimitiveInfo_memory_cost)
STAT_MEMORY_COUNTER("memory cost for QBVH node array",QBVH_node_memory_cost)
class Accelerator
{
private:
    std::vector<Primitive> _primitives;
    std::vector<SoAPrimitiveInfo> _soa_primitive_infos;
    std::vector<QBVHNode> _nodes;

    BVHBuildNode *build(MemoryArena &arena, size_t start, size_t end, std::vector<BVHPrimitiveInfo> &primitive_infos, std::vector<Primitive> &ordered, uint32_t *total);
    void build_soa_primitive_info(BVHBuildNode *node);
    QBVHCollapseNode *collapse(MemoryArena &arena, const BVHBuildNode *subtree_root, uint32_t *total);
    uint32_t flatten(const QBVHCollapseNode *c_node, uint32_t *offset);
    void get_traversal_orders(const QBVHNode& node,const Vector3f& dir,uint32_t orders[4]) const;
public:
    Accelerator() = default;
    Accelerator(std::vector<Primitive> primitives);
    bool intersect(MemoryArena &arena,const Ray &ray,Interaction* interaction) const;
    bool collide(const Ray &ray) const;
};
NARUKAMI_END