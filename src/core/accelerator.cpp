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
#include "accelerator.h"

NARUKAMI_BEGIN



Accelerator::Accelerator(std::vector<Primitive> primitives) : _primitives(std::move(primitives))
{
    std::vector<BVHPrimitiveInfo> primitive_infos(_primitives.size());
    for (size_t i = 0; i < _primitives.size(); ++i)
    {
        primitive_infos[i] = BVHPrimitiveInfo(_primitives[i], i);
    }

    MemoryArena arena;
    std::vector<Primitive> _ordered_primitives;
    uint32_t total_build_node_num = 0;
    uint32_t total_collapse_node_num = 0;

    auto build_root = build(arena, 0, primitive_infos.size(), primitive_infos, _ordered_primitives, &total_build_node_num);
    auto collapse_root = collapse(arena, build_root, &total_collapse_node_num);
    _primitives = _ordered_primitives;
    _nodes.resize(total_collapse_node_num);
    build_soa_triangles(build_root);

    uint32_t offset = 0;
    flatten(collapse_root, &offset);
}

BVHBuildNode *Accelerator::build(MemoryArena &arena, size_t start, size_t end, std::vector<BVHPrimitiveInfo> &primitive_infos, std::vector<Primitive> &ordered, uint32_t *total)
{
    auto node = arena.alloc<BVHBuildNode>(1);
    (*total)++;
    Bounds3f max_bounds;
    for (size_t i = start; i < end; i++)
    {
        max_bounds = _union(max_bounds, primitive_infos[i].bounds);
    }
    uint32_t num = end - start;
    if (num <= 64)
    {
        auto offset = ordered.size();
        for (size_t i = start; i < end; i++)
        {
            ordered.push_back(_primitives[primitive_infos[i].prim_index]);
        }
        init_leaf(node, offset, num, max_bounds);
    }
    else
    {
        Bounds3f centroid_bounds;
        for (size_t i = start; i < end; i++)
        {
            centroid_bounds = _union(centroid_bounds, primitive_infos[i].centroid);
        }

        auto dim = max_extent(centroid_bounds);

        if (centroid_bounds.min_point[dim] == centroid_bounds.max_point[dim])
        {
            auto mid = (start + end) / 2;
            std::nth_element(&primitive_infos[start], &primitive_infos[mid], &primitive_infos[end - 1] + 1, [dim](const BVHPrimitiveInfo &p0, const BVHPrimitiveInfo &p1) { return p0.centroid[dim] < p1.centroid[dim]; });
            init_interior(node, build(arena, start, mid, primitive_infos, ordered, total), build(arena, mid, end, primitive_infos, ordered, total), dim);
        }
        else
        {
            auto mid_point = (centroid_bounds.max_point[dim] + centroid_bounds.min_point[dim]) / 2;
            auto mid_ptr = std::partition(&primitive_infos[start], &primitive_infos[end - 1] + 1, [dim, mid_point](const BVHPrimitiveInfo &pi) { return pi.centroid[dim] < mid_point; });
            auto mid = static_cast<uint32_t>(mid_ptr - &primitive_infos[0]);
            init_interior(node, build(arena, start, mid, primitive_infos, ordered, total), build(arena, mid, end, primitive_infos, ordered, total), dim);
        }
    }
    return node;
}

void Accelerator::build_soa_triangles(BVHBuildNode *node)
{
    if (is_leaf(node))
    {
        auto tris = cast2SoA(_primitives, node->offset, node->num);
        node->num = tris.size();
        node->offset = _triangles.size();
        _triangles.insert(_triangles.end(), tris.begin(), tris.end());
    }
    if (node->childrens[0] && node->childrens[1])
    {
        build_soa_triangles(node->childrens[0]);
        build_soa_triangles(node->childrens[1]);
    }
}

QBVHCollapseNode *Accelerator::collapse(MemoryArena &arena, const BVHBuildNode *subtree_root, uint32_t *total)
{
    auto node = arena.alloc<QBVHCollapseNode>(1);
    (*total)++;
    node->childrens[0] = nullptr;
    node->childrens[1] = nullptr;
    node->childrens[2] = nullptr;
    node->childrens[3] = nullptr;
    if (is_leaf(subtree_root->childrens[0]))
    {
        node->data[0] = subtree_root->childrens[0];
        node->data[1] = nullptr;
    }
    else
    {
        node->data[0] = subtree_root->childrens[0]->childrens[0];
        node->data[1] = subtree_root->childrens[0]->childrens[1];
        if (!is_leaf(node->data[0]))
        {
            node->childrens[0] = collapse(arena, node->data[0], total);
        }
        if (!is_leaf(node->data[1]))
        {
            node->childrens[1] = collapse(arena, node->data[1], total);
        }
    }
    if (is_leaf(subtree_root->childrens[1]))
    {
        node->data[2] = subtree_root->childrens[1];
        node->data[3] = nullptr;
    }
    else
    {
        node->data[2] = subtree_root->childrens[1]->childrens[0];
        node->data[3] = subtree_root->childrens[1]->childrens[1];
        if (!is_leaf(node->data[2]))
        {
            node->childrens[2] = collapse(arena, node->data[2], total);
        }
        if (!is_leaf(node->data[3]))
        {
            node->childrens[3] = collapse(arena, node->data[3], total);
        }
    }

    return node;
}

uint32_t Accelerator::flatten(const QBVHCollapseNode *c_node, uint32_t *offset)
{
    auto cur_offset = (*offset);
    (*offset)++;
    init_QBVH_node(&_nodes[cur_offset], c_node);

    if (c_node->childrens[0] != nullptr)
    {
        _nodes[cur_offset].childrens[0] = flatten(c_node->childrens[0], offset);
    }
    if (c_node->childrens[1] != nullptr)
    {
        _nodes[cur_offset].childrens[1] = flatten(c_node->childrens[1], offset);
    }
    if (c_node->childrens[2] != nullptr)
    {
        _nodes[cur_offset].childrens[2] = flatten(c_node->childrens[2], offset);
    }
    if (c_node->childrens[3] != nullptr)
    {
        _nodes[cur_offset].childrens[3] = flatten(c_node->childrens[3], offset);
    }

    return cur_offset;
}

bool Accelerator::intersect(const Ray &ray) const
{
    std::stack<std::pair<QBVHNode, float>> node_stack;
    SoARay soa_ray(ray);
    int is_positive[3] = {ray.d[0] >= 0 ? 1 : 0, ray.d[1] >= 0 ? 1 : 0, ray.d[2] >= 0 ? 1 : 0};
    node_stack.push({_nodes[0], 0});
    float t = INFINITE;
    bool is_hit_ret = false;
    while (!node_stack.empty())
    {

        if (node_stack.top().second > t)
        {
            node_stack.pop();
            continue;
        }

        auto node = node_stack.top().first;
        node_stack.pop();
        float4 box_t;
        auto box_hits = narukami::intersect(soa_ray.o, robust_rcp(soa_ray.d), float4(0), float4(soa_ray.t_max), is_positive, node.bounds, &box_t);

        bool push_child[4] = {false, false, false, false};

        for (size_t i = 0; i < 4; ++i)
        {
            if (box_hits[i] && box_t[i] < t)
            {
                if (is_leaf(node.childrens[i]))
                {
                    auto offset = leaf_offset(node.childrens[i]);
                    auto num = leaf_num(node.childrens[i]);
                    for (size_t j = offset; j < offset + num; ++j)
                    {
                        Point2f uv;
                        int index;
                        auto is_hit = narukami::intersect(soa_ray, _triangles[j], &t, &uv, &index);
                        if (is_hit)
                        {
                            soa_ray.t_max = float4(t);
                            is_hit_ret = true;
                        }
                    }
                }
                else
                {
                    push_child[i] = true;
                }
            }
        }

        for (size_t i = 0; i < 4; i++)
        {
            if (push_child[i])
            {
                node_stack.push({_nodes[node.childrens[i]], box_t[i]});
            }
        }
    }
    return is_hit_ret;
}

NARUKAMI_END