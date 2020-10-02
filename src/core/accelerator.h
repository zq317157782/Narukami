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

constexpr uint32_t BLAS_ELEMENT_NUM_PER_LEAF = 64;
constexpr int BLAS_SAH_BUCKET_NUM = 12;

constexpr uint32_t ACCELERATOR_ELEMENT_NUM_PER_LEAF = 64;
constexpr int ACCELERATOR_SAH_BUCKET_NUM = 12;

/**
 * 描述每个MeshPrimitive的额外信息
*/
struct BVHMeshPrimitiveState
{
    uint32_t prim_index;
    Bounds3f bounds;
    Point3f centroid;
    BVHMeshPrimitiveState() = default;
    BVHMeshPrimitiveState(const shared<MeshPrimitive> &p, uint32_t index) : prim_index(index), bounds(p->bounds()), centroid((p->bounds().min_point + p->bounds().max_point) * 0.5f) {}
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
    Bounds3fPack bounds;
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

inline void init_QBVH_node(QBVHNode *node, uint32_t depth, const QBVHCollapseNode *cn)
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

    node->bounds = Bounds3fPack(bounds);

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

template <typename T>
Bounds3f get_max_bounds(std::vector<T> &infos, uint32_t start, uint32_t end)
{
    Bounds3f max_bounds;
    for (uint32_t i = start; i < end; i++)
    {
        max_bounds = _union(max_bounds, infos[i].bounds);
    }
    return max_bounds;
}

QBVHCollapseNode *collapse(MemoryArena &arena, const BVHBuildNode *subtree_root, uint32_t *total);
uint32_t flatten(std::vector<QBVHNode> &nodes, uint32_t depth, const QBVHCollapseNode *c_node, uint32_t *offset);
void get_traversal_orders(const QBVHNode &node, const Vector3f &dir, uint32_t orders[4]);

class ProgressReporter;

class BLAS
{
public:
    BLAS() {}
    virtual bool intersect(MemoryArena &arena, const Ray &ray, SurfaceInteraction *interaction) const = 0;
    virtual bool intersect(const Ray &ray) const = 0;
    virtual Bounds3f bounds() const = 0;
};

template <class PrimitiveType>
struct BVHPrimitiveState
{
    uint32_t prim_index;
    Bounds3f bounds;
    Point3f centroid;
    BVHPrimitiveState() = default;
    BVHPrimitiveState(const shared<PrimitiveType> &p, uint32_t index) : prim_index(index), bounds(p->bounds()), centroid((p->bounds().min_point + p->bounds().max_point) * 0.5f) {}
};
template <class PrimitiveType, class CompactPrimitiveType>
class CompactBLAS : public BLAS
{
private:
    std::vector<shared<PrimitiveType>> _primitives;
    std::vector<CompactPrimitiveType> _compact_primitives;
    std::vector<QBVHNode> _nodes;
    Bounds3f _bounds;
    BVHBuildNode *build(MemoryArena &arena, uint32_t start, uint32_t end, std::vector<BVHPrimitiveState<PrimitiveType>> &primitive_states, std::vector<shared<PrimitiveType>> &ordered, uint32_t *total);
    void build_compact_primitives(BVHBuildNode *node);
    shared<PrimitiveType> get_primitive(int compact_primitive_id, int compact_primitive_offset) const
    {
        auto primitive_offset = _compact_primitives[compact_primitive_id].offset + compact_primitive_offset;
        return _primitives[primitive_offset];
    }

public:
    CompactBLAS(const std::vector<shared<PrimitiveType>> &primitives);
    bool intersect(MemoryArena &arena, const Ray &ray, SurfaceInteraction *interaction) const override;
    bool intersect(const Ray &ray) const override;
    Bounds3f bounds() const override { return _bounds; }
};

template <class PrimitiveType, class CompactPrimitiveType>
CompactBLAS<PrimitiveType, CompactPrimitiveType>::CompactBLAS(const std::vector<shared<PrimitiveType>> &primitives) : _primitives(primitives)
{
    STAT_INCREASE_COUNTER(primitive_count, _primitives.size())
    std::vector<BVHPrimitiveState<PrimitiveType>> primitive_states(_primitives.size());
    for (uint32_t i = 0; i < _primitives.size(); ++i)
    {
        primitive_states[i] = BVHPrimitiveState<PrimitiveType>(_primitives[i], i);
    }
    //获取所有Primitive的Bounds
    _bounds = get_max_bounds(primitive_states, 0, static_cast<uint32_t>(primitive_states.size()));
    MemoryArena arena;
    std::vector<shared<PrimitiveType>> _ordered_primitives;
    uint32_t total_build_node_num = 0;
    uint32_t total_collapse_node_num = 0;
    //1.build
    auto build_root = build(arena, 0, static_cast<uint32_t>(primitive_states.size()), primitive_states, _ordered_primitives, &total_build_node_num);
    _primitives = _ordered_primitives;
    //2.collapse
    auto collapse_root = collapse(arena, build_root, &total_collapse_node_num);
    _nodes.resize(total_collapse_node_num);
    //3.compact
    build_compact_primitives(build_root);
    //4.flatten
    uint32_t offset = 0;
    flatten(_nodes, 0, collapse_root, &offset);
    STAT_INCREASE_MEMORY_COUNTER(primitive_memory_cost, sizeof(Primitive) * _primitives.size())
    STAT_INCREASE_MEMORY_COUNTER(QBVH_node_memory_cost, sizeof(QBVHNode) * total_collapse_node_num)
}

template <class PrimitiveType, class CompactPrimitiveType>
BVHBuildNode *CompactBLAS<PrimitiveType, CompactPrimitiveType>::build(MemoryArena &arena, uint32_t start, uint32_t end, std::vector<BVHPrimitiveState<PrimitiveType>> &primitive_states, std::vector<shared<PrimitiveType>> &ordered, uint32_t *total)
{
    auto node = arena.alloc<BVHBuildNode>(1);
    (*total)++;
    Bounds3f max_bounds = get_max_bounds(primitive_states, start, end);

    uint32_t num = end - start;
    if (num <= BLAS_ELEMENT_NUM_PER_LEAF)
    {
        uint32_t offset = static_cast<uint32_t>(ordered.size());
        for (uint32_t i = start; i < end; i++)
        {
            ordered.push_back(_primitives[primitive_states[i].prim_index]);
        }
        init_leaf(node, offset, num, max_bounds);
    }
    else
    {
         Bounds3f centroid_bounds;
        for (uint32_t i = start; i < end; i++)
        {
            centroid_bounds = _union(centroid_bounds, primitive_states[i].centroid);
        }

        auto dim = max_extent(centroid_bounds);

        if (centroid_bounds.min_point[dim] == centroid_bounds.max_point[dim])
        {
            //degenerate
            auto mid = (start + end) / 2;
            std::nth_element(&primitive_states[start], &primitive_states[mid], &primitive_states[end - 1] + 1, [dim](const BVHPrimitiveState<PrimitiveType> &p0, const BVHPrimitiveState<PrimitiveType> &p1) { return p0.centroid[dim] < p1.centroid[dim]; });
            init_interior(node, build(arena, start, mid, primitive_states, ordered, total), build(arena, mid, end, primitive_states, ordered, total), dim);
        }
        else
        {
            //SAH
            BucketInfo bucket_infos[BLAS_SAH_BUCKET_NUM];

            for (uint32_t i = start; i < end; ++i)
            {
                auto bucket_index = static_cast<int>(BLAS_SAH_BUCKET_NUM * offset(centroid_bounds, primitive_states[i].centroid)[dim]);
                bucket_index = min(bucket_index, BLAS_SAH_BUCKET_NUM - 1);
                bucket_infos[bucket_index].bounds = _union(bucket_infos[bucket_index].bounds, primitive_states[i].bounds);
                bucket_infos[bucket_index].count++;
            }

            //compute cost function
            float costs[BLAS_SAH_BUCKET_NUM - 1];
            for (uint32_t i = 0; i < BLAS_SAH_BUCKET_NUM - 1; i++)
            {
                Bounds3f b0, b1;
                uint32_t count0 = 0, count1 = 0;
                for (uint32_t j = 0; j <= i; j++)
                {
                    b0 = _union(b0, bucket_infos[j].bounds);
                    count0 += bucket_infos[j].count;
                }
                for (uint32_t j = i + 1; j < BLAS_SAH_BUCKET_NUM; j++)
                {
                    b1 = _union(b1, bucket_infos[j].bounds);
                    count1 += bucket_infos[j].count;
                }

                costs[i] = 0.125f + (count0 * surface_area(b0) + count1 * surface_area(b1)) / surface_area(max_bounds);
            }

            float min_cost = costs[0];
            int min_cost_bucket_index = 0;
            for (uint32_t i = 1; i < BLAS_SAH_BUCKET_NUM - 1; i++)
            {
                if (costs[i] < min_cost)
                {
                    min_cost = costs[i];
                    min_cost_bucket_index = i;
                }
            }

            auto mid_ptr = std::partition(&primitive_states[start], &primitive_states[end - 1] + 1, [=](const BVHPrimitiveState<PrimitiveType> &pi) {
                auto bucket_index = static_cast<int>(BLAS_SAH_BUCKET_NUM * offset(centroid_bounds.min_point[dim], centroid_bounds.max_point[dim], pi.centroid[dim]));
                bucket_index = min(bucket_index, BLAS_SAH_BUCKET_NUM - 1);
                return bucket_index <= min_cost_bucket_index;
            });
            auto mid = static_cast<uint32_t>(mid_ptr - &primitive_states[0]);
            init_interior(node, build(arena, start, mid, primitive_states, ordered, total), build(arena, mid, end, primitive_states, ordered, total), dim);
        }
    }
    return node;
}
template <class PrimitiveType, class CompactPrimitiveType>
void CompactBLAS<PrimitiveType, CompactPrimitiveType>::build_compact_primitives(BVHBuildNode *node)
{

    if (is_leaf(node))
    {
        STAT_INCREASE_COUNTER_CONDITION(SoA_utilization_ratio_num, 1, (node->num % 4) == 1)
        STAT_INCREASE_COUNTER_CONDITION(SoA_utilization_ratio_num, 2, (node->num % 4) == 2)
        STAT_INCREASE_COUNTER_CONDITION(SoA_utilization_ratio_num, 3, (node->num % 4) == 3)
        STAT_INCREASE_COUNTER_CONDITION(SoA_utilization_ratio_num, 4, (node->num % 4) == 0)

        auto primitive_states = pack_mesh_primitives(_primitives, node->offset, node->num);
        node->num = static_cast<uint32_t>(primitive_states.size());
        node->offset = static_cast<uint32_t>(_compact_primitives.size());
        _compact_primitives.insert(_compact_primitives.end(), primitive_states.begin(), primitive_states.end());

        STAT_INCREASE_COUNTER(SoA_utilization_ratio_num, (static_cast<uint32_t>(primitive_states.size()) - 1) * 4)
        STAT_INCREASE_COUNTER(SoA_utilization_ratio_denom, static_cast<uint32_t>(primitive_states.size()) * 4)
    }
    //codition-> the interior node must has two child node
    if (node->childrens[0] && node->childrens[1])
    {
        build_compact_primitives(node->childrens[0]);
        build_compact_primitives(node->childrens[1]);
    }
}

template <class PrimitiveType, class CompactPrimitiveType>
bool CompactBLAS<PrimitiveType, CompactPrimitiveType>::intersect(MemoryArena &arena, const Ray &ray, SurfaceInteraction *interaction) const
{
    std::stack<std::pair<const QBVHNode *, float>> node_stack;
    RayPack soa_ray(ray.o, ray.d, ray.t_max);
    int is_positive[3] = {ray.d[0] >= 0 ? 1 : 0, ray.d[1] >= 0 ? 1 : 0, ray.d[2] >= 0 ? 1 : 0};
    node_stack.push({&_nodes[0], 0.0f});

    bool has_hit = false;

    uint32_t compact_offset;
    uint32_t compact_idx;
    Point2f param_uv;

    while (!node_stack.empty())
    {
        if (node_stack.top().second > ray.t_max)
        {
            node_stack.pop();
            continue;
        }

        auto node = node_stack.top().first;
        node_stack.pop();
        float4 box_t;
        auto box_hits = narukami::intersect(soa_ray.o, robust_rcp(soa_ray.d), float4(0), float4(soa_ray.t_max), is_positive, node->bounds, &box_t);

        bool push_child[4] = {false, false, false, false};
        uint32_t orders[4];
        get_traversal_orders((*node), ray.d, orders);

        for (uint32_t i = 0; i < 4; ++i)
        {
            uint32_t index = orders[i];
            STAT_INCREASE_COUNTER(ordered_traversal_denom, 1)
            if (box_hits[index] && box_t[index] < ray.t_max)
            {
                STAT_INCREASE_COUNTER(ordered_traversal_num, 1)
                if (is_leaf(node->childrens[index]))
                {
                    auto offset = leaf_offset(node->childrens[index]);
                    auto num = leaf_num(node->childrens[index]);
                    for (uint32_t j = offset; j < offset + num; ++j)
                    {
                        float hit_t = INFINITE;
                        Point2f temp_param_uv;
                        int temp_compact_offset;
                        auto is_hit = narukami::intersect(soa_ray,_compact_primitives[j],&hit_t, &temp_param_uv, &temp_compact_offset);//narukami::intersect(soa_ray, _compact_primitives[j].triangle, &hit_t, &temp_param_uv, &temp_compact_offset);
                        STAT_INCREASE_COUNTER(intersect_triangle_num, 1)

                        if (is_hit && hit_t < ray.t_max)
                        {
                            {
                                has_hit = true;
                            }
                            //更新射线的t_max
                            {
                                soa_ray.t_max = float4(hit_t);
                                ray.t_max = hit_t;
                            }
                            {
                                compact_offset = temp_compact_offset;
                                compact_idx = j;
                                param_uv = temp_param_uv;
                            }
                        }
                    }
                }
                else
                {
                    push_child[index] = true;
                }
            }
        }

        for (uint32_t i = 0; i < 4; i++)
        {
            uint32_t index = orders[i];
            if (push_child[index])
            {
                node_stack.push({&_nodes[node->childrens[index]], box_t[index]});
            }
        }
    }

    if (has_hit)
    {
        setup_interaction(_compact_primitives[compact_idx],get_primitive(compact_idx, compact_offset),compact_offset,ray,param_uv,interaction);
    }
    return has_hit;
}
template <class PrimitiveType, class CompactPrimitiveType>
bool CompactBLAS<PrimitiveType, CompactPrimitiveType>::intersect(const Ray &ray) const
{
    std::stack<std::pair<const QBVHNode *, float>> node_stack;
    RayPack soa_ray(ray);
    int is_positive[3] = {ray.d[0] >= 0 ? 1 : 0, ray.d[1] >= 0 ? 1 : 0, ray.d[2] >= 0 ? 1 : 0};
    node_stack.push({&_nodes[0], 0.0f});
    while (!node_stack.empty())
    {
        auto node = node_stack.top().first;
        node_stack.pop();
        float4 box_t;
        auto box_hits = narukami::intersect(soa_ray.o, robust_rcp(soa_ray.d), float4(0), float4(soa_ray.t_max), is_positive, node->bounds, &box_t);

        bool push_child[4] = {false, false, false, false};
        uint32_t orders[4];
        get_traversal_orders((*node), ray.d, orders);

        for (uint32_t i = 0; i < 4; ++i)
        {
            uint32_t index = orders[i];
            STAT_INCREASE_COUNTER(ordered_traversal_denom, 1)
            if (box_hits[index])
            {
                STAT_INCREASE_COUNTER(ordered_traversal_num, 1)
                if (is_leaf(node->childrens[index]))
                {
                    auto offset = leaf_offset(node->childrens[index]);
                    auto num = leaf_num(node->childrens[index]);
                    for (uint32_t j = offset; j < offset + num; ++j)
                    {
                        auto is_hit = narukami::intersect(soa_ray, _compact_primitives[j]);
                        if (is_hit)
                        {
                            return true;
                        }
                    }
                }
                else
                {
                    push_child[index] = true;
                }
            }
        }

        for (uint32_t i = 0; i < 4; i++)
        {
            uint32_t index = orders[i];
            if (push_child[index])
            {
                node_stack.push({&_nodes[node->childrens[index]], box_t[index]});
            }
        }
    }
    return false;
}
class BLASInstance : public BLAS
{
private:
    shared<BLAS> _blas;
    const shared<AnimatedTransform> _blas_to_world;
    Bounds3f _bounds;

public:
    BLASInstance(const shared<AnimatedTransform> &blas_to_world, const shared<BLAS> &blas) : _blas_to_world(blas_to_world), _blas(blas) { _bounds = (*_blas_to_world)(_blas->bounds()); };

    bool intersect(MemoryArena &arena, const Ray &ray, SurfaceInteraction *interaction) const override
    {
        Transform w2b;
        Transform b2w;
        {
            _blas_to_world->interpolate(ray.time, &b2w);
            w2b = inverse(b2w);
        }

        auto blas_ray = w2b(ray);
        bool has_hit = _blas->intersect(arena, blas_ray, interaction);
        (*interaction) = b2w(*interaction);
        ray.t_max = blas_ray.t_max;
        return has_hit;
    }

    bool intersect(const Ray &ray) const override
    {
        Transform w2b;
        Transform b2w;
        {
            _blas_to_world->interpolate(ray.time, &b2w);
            w2b = inverse(b2w);
        }
        auto blas_ray = w2b(ray);
        bool has_hit = _blas->intersect(blas_ray);
        ray.t_max = blas_ray.t_max;
        return has_hit;
    }
    Bounds3f bounds() const override { return _bounds; }

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

struct CompactBLASInstance
{
    Bounds3fPack bounds;
    uint32_t offset;
};

class TLAS
{
private:
    std::vector<shared<BLASInstance>> _instances;
    std::vector<CompactBLASInstance> _compact_instances;
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
