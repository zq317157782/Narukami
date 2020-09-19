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


/**
 * 描述每个MeshPrimitive的额外信息
*/
struct BVHMeshPrimitiveInfo
{
    uint32_t prim_index;
    Bounds3f bounds;
    Point3f centroid;
    BVHMeshPrimitiveInfo() = default;
    BVHMeshPrimitiveInfo(const shared<MeshPrimitive> &p, uint32_t index) : prim_index(index), bounds(p->bounds()), centroid((p->bounds().min_point + p->bounds().max_point) * 0.5f) {}
};

/**
 * 构建BVH的中间节点
*/
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

/**
 * 记录构建QBVH时候的塌陷信息
*/
struct QBVHCollapseNode
{
    const BVHBuildNode *data[4];
    QBVHCollapseNode *childrens[4];
    uint32_t axis0, axis1, axis2;
};

/**
 * QBVH节点
 * 128 byte
*/

struct SSE_ALIGNAS QBVHNode
{
    Bounds3f4p bounds;
    uint32_t childrens[4];
    uint32_t axis0, axis1, axis2;
    uint32_t depth;
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

inline void init_QBVH_node(QBVHNode *node, uint32_t depth,const QBVHCollapseNode *cn)
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

    node->depth = depth;
}

/*SAH分割策略使用的Bucket信息*/
struct BucketInfo
{
    Bounds3f bounds;
    uint32_t count = 0;
};

//MeshBLAS ONLY
STAT_COUNTER("accelerator/primitive num", primitive_count)
STAT_MEMORY_COUNTER("accelerator/primitive memory", primitive_memory_cost)
STAT_PERCENT("accelerator/blas SoA utilization ratio", SoA_utilization_ratio_num, SoA_utilization_ratio_denom)
STAT_COUNTER("accelerator/intersect triangle num", intersect_triangle_num)
//TLAS ONLY
STAT_COUNTER("accelerator/blas instance num", blas_instance_num)
// GENERL
STAT_MEMORY_COUNTER("accelerator/QBVH node memory", QBVH_node_memory_cost)
STAT_PERCENT("accelerator/ratio of travel QBVH's four subnode(25%:just one subnode is visited. 50%:two subnodes are  visited and so on.) ", ordered_traversal_num, ordered_traversal_denom)

class ProgressReporter;

class BLAS
{
public:
    BLAS(){}
    virtual bool intersect(MemoryArena &arena, const Ray &ray,SurfaceInteraction* interaction) const = 0;
    virtual bool intersect(const Ray &ray) const = 0;
    virtual Bounds3f bounds() const = 0;
};

class MeshBLAS:public BLAS
{
private:
    std::vector<shared<MeshPrimitive>> _primitives;
    std::vector<MeshPrimitiveInfo4p> _soa_primitive_infos;
    std::vector<QBVHNode> _nodes;
    Bounds3f _bounds;

    BVHBuildNode *build(MemoryArena &arena, uint32_t start, uint32_t end, std::vector<BVHMeshPrimitiveInfo> &primitive_infos, std::vector<shared<MeshPrimitive>> &ordered, uint32_t *total);
    void build_soa_primitive_info(BVHBuildNode *node);
    
    shared<MeshPrimitive> get_mesh_primitive(int soa_primitive_id,int soa_offset) const
    {
        auto primitive_offset = _soa_primitive_infos[soa_primitive_id].offset + soa_offset;
        return _primitives[primitive_offset];
    }
public:
    MeshBLAS(const std::vector<shared<MeshPrimitive>> &primitives);
    bool intersect(MemoryArena &arena, const Ray &ray,SurfaceInteraction* interaction) const override;
    bool intersect(const Ray &ray) const override;
    Bounds3f bounds() const override{ return _bounds; }

//以下函数主要用于可视化
public:
    std::vector<QBVHNode> get_nodes_by_depth(uint32_t depth) const;
    const std::vector<shared<MeshPrimitive>>& get_mesh_primitives() const {return _primitives;} 

    void *operator new(size_t size);
    void operator delete(void *ptr);
};

class BLASInstance:public BLAS
{
private:
    shared<BLAS> _blas;
    const shared<AnimatedTransform> _blas_to_world;
    Bounds3f _bounds;

public:
    BLASInstance(const shared<AnimatedTransform>& blas_to_world,const shared<BLAS> &blas) : _blas_to_world(blas_to_world), _blas(blas) { _bounds = (*_blas_to_world)(_blas->bounds()); };

    bool intersect(MemoryArena &arena, const Ray &ray,SurfaceInteraction* interaction) const override
    {
        Transform w2b;
        Transform b2w;
        {
            _blas_to_world->interpolate(ray.time,&b2w);
            w2b = inverse(b2w);
        }

        auto blas_ray = w2b(ray);
        bool has_hit = _blas->intersect(arena, blas_ray,interaction);
        (*interaction) = b2w(*interaction);
        ray.t_max = blas_ray.t_max;
        return has_hit;
    }

    bool intersect(const Ray &ray) const override
    {
        Transform w2b;
        Transform b2w;
        {
            _blas_to_world->interpolate(ray.time,&b2w);
            w2b = inverse(b2w);
        }
        auto blas_ray = w2b(ray);
        bool has_hit = _blas->intersect(blas_ray);
        ray.t_max = blas_ray.t_max;
        return has_hit;
    }
    Bounds3f bounds() const override{ return _bounds; }

    void *operator new(size_t size);
    void operator delete(void *ptr);
};

struct BLASInstanceInfo
{
    uint32_t instance_index;
    Bounds3f bounds;
    Point3f centroid;
    BLASInstanceInfo() = default;
    BLASInstanceInfo(const shared<BLASInstance> &instance, uint32_t index) : instance_index(index), bounds(instance->bounds()), centroid((instance->bounds().min_point + instance->bounds().max_point) * 0.5f) {}
};

struct BLASInstanceInfo4p
{
    Bounds3f4p bounds;
    uint32_t offset;
};

class TLAS
{
private:
    std::vector<shared<BLASInstance>> _instances;
    std::vector<BLASInstanceInfo4p> _soa_instance_infos;
    std::vector<QBVHNode> _nodes;
    Bounds3f _bounds;
    

    BVHBuildNode *build(MemoryArena &arena, uint32_t start, uint32_t end, std::vector<BLASInstanceInfo> &instance_infos, std::vector<shared<BLASInstance>> &ordered, uint32_t *total);
    void build_soa_instance_info(BVHBuildNode *node);

public:
    TLAS(const std::vector<shared<BLASInstance>> &instance);
    bool intersect(MemoryArena &arena, const Ray &ray, SurfaceInteraction *interaction) const;
    bool intersect(const Ray &ray) const;
    Bounds3f bounds() const { return _bounds; }

    void *operator new(size_t size);
    void operator delete(void *ptr);
};

NARUKAMI_END
