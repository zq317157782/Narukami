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
#include "core/accelerator.h"
#include "core/progressreporter.h"
NARUKAMI_BEGIN

MemoryPool<MeshBLAS> g_mesh_blas_pool(4096);

void * MeshBLAS::operator new(size_t size)
{
    return g_mesh_blas_pool.alloc();
}

void  MeshBLAS::operator delete(void * ptr)
{
    g_mesh_blas_pool.dealloc(reinterpret_cast<MeshBLAS*>(ptr));
}

MemoryPool<BLASInstance> g_blas_instance_pool(4096);
void * BLASInstance::operator new(size_t size)
{
    return g_blas_instance_pool.alloc();
}

void  BLASInstance::operator delete(void * ptr)
{
    g_blas_instance_pool.dealloc(reinterpret_cast<BLASInstance*>(ptr));
}

MemoryPool<TLAS> g_tlas_pool(1);
void * TLAS::operator new(size_t size)
{
    return g_tlas_pool.alloc();
}

void  TLAS::operator delete(void * ptr)
{
    g_tlas_pool.dealloc(reinterpret_cast<TLAS*>(ptr));
}

constexpr uint32_t BLAS_ELEMENT_NUM_PER_LEAF = 64;
constexpr int BLAS_SAH_BUCKET_NUM = 12;

constexpr uint32_t ACCELERATOR_ELEMENT_NUM_PER_LEAF = 64;
constexpr int ACCELERATOR_SAH_BUCKET_NUM = 12;

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

QBVHCollapseNode *collapse(MemoryArena &arena, const BVHBuildNode *subtree_root, uint32_t *total)
{
    auto node = arena.alloc<QBVHCollapseNode>(1);
    (*total)++;
    node->childrens[0] = nullptr;
    node->childrens[1] = nullptr;
    node->childrens[2] = nullptr;
    node->childrens[3] = nullptr;
    if (is_leaf(subtree_root))
    {
        node->data[0] = subtree_root;
        node->data[1] = nullptr;
        node->data[2] = nullptr;
        node->data[3] = nullptr;
        node->axis0 = 0;
        node->axis1 = 0;
        node->axis2 = 0;
        return node;
    }

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

    node->axis0 = subtree_root->split_axis;
    node->axis1 = subtree_root->childrens[0]->split_axis;
    node->axis2 = subtree_root->childrens[1]->split_axis;
    return node;
}

uint32_t flatten(std::vector<QBVHNode> &nodes, uint32_t depth, const QBVHCollapseNode *c_node, uint32_t *offset)
{
    auto cur_offset = (*offset);
    (*offset)++;
    init_QBVH_node(&nodes[cur_offset], depth, c_node);

    if (c_node->childrens[0] != nullptr)
    {
        nodes[cur_offset].childrens[0] = flatten(nodes, depth + 1, c_node->childrens[0], offset);
    }
    if (c_node->childrens[1] != nullptr)
    {
        nodes[cur_offset].childrens[1] = flatten(nodes, depth + 1, c_node->childrens[1], offset);
    }
    if (c_node->childrens[2] != nullptr)
    {
        nodes[cur_offset].childrens[2] = flatten(nodes, depth + 1, c_node->childrens[2], offset);
    }
    if (c_node->childrens[3] != nullptr)
    {
        nodes[cur_offset].childrens[3] = flatten(nodes, depth + 1, c_node->childrens[3], offset);
    }

    //set QBVH's split axis
    nodes[cur_offset].axis0 = c_node->axis0;
    nodes[cur_offset].axis1 = c_node->axis1;
    nodes[cur_offset].axis2 = c_node->axis2;

    return cur_offset;
}

MeshBLAS::MeshBLAS(const std::vector<shared<MeshPrimitive>> &primitives) : _primitives(primitives)
{
    STAT_INCREASE_COUNTER(primitive_count, _primitives.size())
    std::vector<BVHMeshPrimitiveInfo> primitive_infos(_primitives.size());
    for (uint32_t i = 0; i < _primitives.size(); ++i)
    {
        primitive_infos[i] = BVHMeshPrimitiveInfo(_primitives[i], i);
    }

    //获取所有MeshPrimitive的Bounds
    _bounds = get_max_bounds(primitive_infos, 0, static_cast<uint32_t>(primitive_infos.size()));

    MemoryArena arena;
    std::vector<shared<MeshPrimitive>> _ordered_primitives;
    uint32_t total_build_node_num = 0;
    uint32_t total_collapse_node_num = 0;

    auto build_root = build(arena, 0, static_cast<uint32_t>(primitive_infos.size()), primitive_infos, _ordered_primitives, &total_build_node_num);

    _primitives = _ordered_primitives;
    auto collapse_root = collapse(arena, build_root, &total_collapse_node_num);

    _nodes.resize(total_collapse_node_num);
    build_soa_primitive_info(build_root);
    uint32_t offset = 0;

    flatten(_nodes, 0, collapse_root, &offset);

    STAT_INCREASE_MEMORY_COUNTER(primitive_memory_cost, sizeof(Primitive) * _primitives.size())
    STAT_INCREASE_MEMORY_COUNTER(QBVH_node_memory_cost, sizeof(QBVHNode) * total_collapse_node_num)
}

BVHBuildNode *MeshBLAS::build(MemoryArena &arena, uint32_t start, uint32_t end, std::vector<BVHMeshPrimitiveInfo> &primitive_infos, std::vector<shared<MeshPrimitive>> &ordered, uint32_t *total)
{
    auto node = arena.alloc<BVHBuildNode>(1);
    (*total)++;
    Bounds3f max_bounds = get_max_bounds(primitive_infos, start, end);

    uint32_t num = end - start;
    if (num <= BLAS_ELEMENT_NUM_PER_LEAF)
    {
        uint32_t offset = static_cast<uint32_t>(ordered.size());
        for (uint32_t i = start; i < end; i++)
        {
            ordered.push_back(_primitives[primitive_infos[i].prim_index]);
        }
        init_leaf(node, offset, num, max_bounds);
    }
    else
    {
        Bounds3f centroid_bounds;
        for (uint32_t i = start; i < end; i++)
        {
            centroid_bounds = _union(centroid_bounds, primitive_infos[i].centroid);
        }

        auto dim = max_extent(centroid_bounds);

        if (centroid_bounds.min_point[dim] == centroid_bounds.max_point[dim])
        {
            //degenerate
            auto mid = (start + end) / 2;
            std::nth_element(&primitive_infos[start], &primitive_infos[mid], &primitive_infos[end - 1] + 1, [dim](const BVHMeshPrimitiveInfo &p0, const BVHMeshPrimitiveInfo &p1) { return p0.centroid[dim] < p1.centroid[dim]; });
            init_interior(node, build(arena, start, mid, primitive_infos, ordered, total), build(arena, mid, end, primitive_infos, ordered, total), dim);
        }
        // else if (num <= 2 * BLAS_ELEMENT_NUM_PER_LEAF)
        // {
        //     auto mid = start + BLAS_ELEMENT_NUM_PER_LEAF;
        //     std::nth_element(&primitive_infos[start], &primitive_infos[mid], &primitive_infos[end - 1] + 1, [dim](const BVHMeshPrimitiveInfo &p0, const BVHMeshPrimitiveInfo &p1) { return p0.centroid[dim] < p1.centroid[dim]; });
        //     init_interior(node, build(arena, start, mid, primitive_infos, ordered, total), build(arena, mid, end, primitive_infos, ordered, total), dim);
        // }
        //else{
        //     auto mid = start+2*BLAS_ELEMENT_NUM_PER_LEAF;
        //     std::nth_element(&primitive_infos[start], &primitive_infos[mid], &primitive_infos[end - 1] + 1, [dim](const BVHMeshPrimitiveInfo &p0, const BVHMeshPrimitiveInfo &p1) { return p0.centroid[dim] < p1.centroid[dim]; });
        //     init_interior(node, build(arena, start, mid, primitive_infos, ordered, total), build(arena, mid, end, primitive_infos, ordered, total), dim);
        // }
        else
        {
            //SAH
            BucketInfo bucket_infos[BLAS_SAH_BUCKET_NUM];

            for (uint32_t i = start; i < end; ++i)
            {
                auto bucket_index = static_cast<int>(BLAS_SAH_BUCKET_NUM * offset(centroid_bounds, primitive_infos[i].centroid)[dim]);
                bucket_index = min(bucket_index, BLAS_SAH_BUCKET_NUM - 1);
                bucket_infos[bucket_index].bounds = _union(bucket_infos[bucket_index].bounds, primitive_infos[i].bounds);
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

            // auto mid_point = (centroid_bounds.max_point[dim] + centroid_bounds.min_point[dim]) / 2;
            // auto mid_ptr = std::partition(&primitive_infos[start], &primitive_infos[end - 1] + 1, [dim, mid_point](const BVHMeshPrimitiveInfo &pi) { return pi.centroid[dim] < mid_point; });
            auto mid_ptr = std::partition(&primitive_infos[start], &primitive_infos[end - 1] + 1, [=](const BVHMeshPrimitiveInfo &pi) {
                auto bucket_index = static_cast<int>(BLAS_SAH_BUCKET_NUM * offset(centroid_bounds.min_point[dim], centroid_bounds.max_point[dim], pi.centroid[dim]));
                bucket_index = min(bucket_index, BLAS_SAH_BUCKET_NUM - 1);
                return bucket_index <= min_cost_bucket_index;
            });
            auto mid = static_cast<uint32_t>(mid_ptr - &primitive_infos[0]);
            init_interior(node, build(arena, start, mid, primitive_infos, ordered, total), build(arena, mid, end, primitive_infos, ordered, total), dim);
        }
    }
    return node;
}

void MeshBLAS::build_soa_primitive_info(BVHBuildNode *node)
{

    if (is_leaf(node))
    {
        STAT_INCREASE_COUNTER_CONDITION(SoA_utilization_ratio_num, 1, (node->num % 4) == 1)
        STAT_INCREASE_COUNTER_CONDITION(SoA_utilization_ratio_num, 2, (node->num % 4) == 2)
        STAT_INCREASE_COUNTER_CONDITION(SoA_utilization_ratio_num, 3, (node->num % 4) == 3)
        STAT_INCREASE_COUNTER_CONDITION(SoA_utilization_ratio_num, 4, (node->num % 4) == 0)
       

        auto primitive_infos = pack_mesh_primitives(_primitives, node->offset, node->num);
        node->num = static_cast<uint32_t>(primitive_infos.size());
        node->offset = static_cast<uint32_t>(_soa_primitive_infos.size());
        _soa_primitive_infos.insert(_soa_primitive_infos.end(), primitive_infos.begin(), primitive_infos.end());

        STAT_INCREASE_COUNTER(SoA_utilization_ratio_num,(static_cast<uint32_t>(primitive_infos.size()) - 1) * 4)
        STAT_INCREASE_COUNTER(SoA_utilization_ratio_denom, static_cast<uint32_t>(primitive_infos.size()) * 4)



    }
    //codition-> the interior node must has two child node
    if (node->childrens[0] && node->childrens[1])
    {
        build_soa_primitive_info(node->childrens[0]);
        build_soa_primitive_info(node->childrens[1]);
    }
}

void get_traversal_orders(const QBVHNode &node, const Vector3f &dir, uint32_t orders[4])
{
    orders[0] = 0;
    orders[1] = 1;
    orders[2] = 2;
    orders[3] = 3;

    if (dir[node.axis1] <= 0)
    {
        std::swap(orders[0], orders[1]);
    }
    if (dir[node.axis2] <= 0)
    {
        std::swap(orders[2], orders[3]);
    }
    if (dir[node.axis0] <= 0)
    {
        std::swap(orders[0], orders[2]);
        std::swap(orders[1], orders[3]);
    }
}

bool MeshBLAS::intersect(MemoryArena &arena, const Ray &ray, SurfaceInteraction *interaction) const
{
    std::stack<std::pair<const QBVHNode *, float>> node_stack;
    SoARay soa_ray(ray.o, ray.d, ray.t_max);
    int is_positive[3] = {ray.d[0] >= 0 ? 1 : 0, ray.d[1] >= 0 ? 1 : 0, ray.d[2] >= 0 ? 1 : 0};
    node_stack.push({&_nodes[0], 0.0f});

    bool has_hit = false;

    uint32_t sub_soa_idx;
    uint32_t soa_idx;
    Point2f barycentric;

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
                        Point2f temp_barycentric;
                        int triangle_index;
                        auto is_hit = narukami::intersect(soa_ray, _soa_primitive_infos[j].triangle, &hit_t, &temp_barycentric, &triangle_index);
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
                                sub_soa_idx = triangle_index;
                                soa_idx = j;
                                barycentric = temp_barycentric;
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
        auto triangle = _soa_primitive_infos[soa_idx].triangle[sub_soa_idx];
        auto primitive = get_mesh_primitive(soa_idx,sub_soa_idx);
        //交点和法线
        interaction->p = get_vertex(triangle, barycentric);
        interaction->n = hemisphere_flip(get_normalized_normal(triangle), -ray.d);
        //dpdu,dpdv
        Vector3f dpdu,dpdv;

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
        if(det == 0)
        {
            coordinate_system(interaction->n,&dpdu,&dpdv);
        }
        else
        {
            //求2x2矩阵的逆

            dpdu = duv20.y * dp10 - duv10.y * dp20;
            dpdv = -duv20.x * dp10 + duv10.x * dp20;
            dpdu/=det;
            dpdv/=det;
        }

        interaction->dpdu = dpdu;
        interaction->dpdv = dpdv;

        interaction->uv = primitive->get_texcoord(barycentric);
    }
    return has_hit;
}

bool MeshBLAS::intersect(const Ray &ray) const
{
    std::stack<std::pair<const QBVHNode *, float>> node_stack;
    SoARay soa_ray(ray);
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
                        auto is_hit = narukami::intersect(soa_ray, _soa_primitive_infos[j].triangle);
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

std::vector<QBVHNode> MeshBLAS::get_nodes_by_depth(uint32_t depth) const
{
    std::vector<QBVHNode> ret;
    for (auto &node : _nodes)
    {
        if (node.depth == depth)
        {
            ret.push_back(node);
        }
    }
    return ret;
}

TLAS::TLAS(const std::vector<shared<BLASInstance>> &instance_list) :_instances(instance_list)
{
    STAT_INCREASE_COUNTER(blas_instance_num, instance_list.size())
    std::vector<BLASInstanceInfo> instance_infos(instance_list.size());
    for (uint32_t i = 0; i < instance_list.size(); ++i)
    {
        instance_infos[i] = BLASInstanceInfo(instance_list[i], i);
    }

    _bounds = get_max_bounds(instance_infos, 0, static_cast<uint32_t>(instance_infos.size()));

    MemoryArena arena;
    std::vector<shared<BLASInstance>> _ordered_instance_list;
    uint32_t total_build_node_num = 0;
    uint32_t total_collapse_node_num = 0;

    auto build_root = build(arena, 0, static_cast<uint32_t>(instance_infos.size()), instance_infos, _ordered_instance_list, &total_build_node_num);

    _instances = _ordered_instance_list;
    auto collapse_root = collapse(arena, build_root, &total_collapse_node_num);
    _nodes.resize(total_collapse_node_num);

    build_soa_instance_info(build_root);
    uint32_t offset = 0;

    flatten(_nodes, 0, collapse_root, &offset);

    STAT_INCREASE_MEMORY_COUNTER(QBVH_node_memory_cost, sizeof(QBVHNode) * total_collapse_node_num)
}

BVHBuildNode *TLAS::build(MemoryArena &arena, uint32_t start, uint32_t end, std::vector<BLASInstanceInfo> &instance_infos, std::vector<shared<BLASInstance>> &ordered, uint32_t *total)
{
    auto node = arena.alloc<BVHBuildNode>(1);
    (*total)++;
    Bounds3f max_bounds = get_max_bounds(instance_infos, start, end);

    uint32_t num = end - start;

    if (num <= ACCELERATOR_ELEMENT_NUM_PER_LEAF)
    {
        //初始化叶子节点
        uint32_t offset = static_cast<uint32_t>(ordered.size());
        for (uint32_t i = start; i < end; i++)
        {
            ordered.push_back(_instances[instance_infos[i].instance_index]);
        }
        init_leaf(node, offset, num, max_bounds);
    }
    else
    {
        Bounds3f centroid_bounds;
        for (uint32_t i = start; i < end; i++)
        {
            centroid_bounds = _union(centroid_bounds, instance_infos[i].centroid);
        }

        auto dim = max_extent(centroid_bounds);

        if (centroid_bounds.min_point[dim] == centroid_bounds.max_point[dim])
        {
            //degenerate
            auto mid = (start + end) / 2;
            std::nth_element(&instance_infos[start], &instance_infos[mid], &instance_infos[end - 1] + 1, [dim](const BLASInstanceInfo &p0, const BLASInstanceInfo &p1) { return p0.centroid[dim] < p1.centroid[dim]; });
            init_interior(node, build(arena, start, mid, instance_infos, ordered, total), build(arena, mid, end, instance_infos, ordered, total), dim);
        }
        else if (num <= 2 * BLAS_ELEMENT_NUM_PER_LEAF)
        {
            auto mid = start + ACCELERATOR_ELEMENT_NUM_PER_LEAF;
            std::nth_element(&instance_infos[start], &instance_infos[mid], &instance_infos[end - 1] + 1, [dim](const BLASInstanceInfo &p0, const BLASInstanceInfo &p1) { return p0.centroid[dim] < p1.centroid[dim]; });
            init_interior(node, build(arena, start, mid, instance_infos, ordered, total), build(arena, mid, end, instance_infos, ordered, total), dim);
        }
        else
        {
            //SAH
            BucketInfo bucket_infos[ACCELERATOR_SAH_BUCKET_NUM];

            for (uint32_t i = start; i < end; ++i)
            {
                auto bucket_index = static_cast<int>(ACCELERATOR_SAH_BUCKET_NUM * offset(centroid_bounds, instance_infos[i].centroid)[dim]);
                bucket_index = min(bucket_index, ACCELERATOR_SAH_BUCKET_NUM - 1);
                bucket_infos[bucket_index].bounds = _union(bucket_infos[bucket_index].bounds, instance_infos[i].bounds);
                bucket_infos[bucket_index].count++;
            }

            //compute cost function
            float costs[ACCELERATOR_SAH_BUCKET_NUM - 1];
            for (uint32_t i = 0; i < ACCELERATOR_SAH_BUCKET_NUM - 1; i++)
            {
                Bounds3f b0, b1;
                uint32_t count0 = 0, count1 = 0;
                for (uint32_t j = 0; j <= i; j++)
                {
                    b0 = _union(b0, bucket_infos[j].bounds);
                    count0 += bucket_infos[j].count;
                }
                for (uint32_t j = i + 1; j < ACCELERATOR_SAH_BUCKET_NUM; j++)
                {
                    b1 = _union(b1, bucket_infos[j].bounds);
                    count1 += bucket_infos[j].count;
                }

                costs[i] = 0.125f + (count0 * surface_area(b0) + count1 * surface_area(b1)) / surface_area(max_bounds);
            }

            float min_cost = costs[0];
            int min_cost_bucket_index = 0;
            for (uint32_t i = 1; i < ACCELERATOR_SAH_BUCKET_NUM - 1; i++)
            {
                if (costs[i] < min_cost)
                {
                    min_cost = costs[i];
                    min_cost_bucket_index = i;
                }
            }

            auto mid_ptr = std::partition(&instance_infos[start], &instance_infos[end - 1] + 1, [=](const BLASInstanceInfo &pi) {
                auto bucket_index = static_cast<int>(ACCELERATOR_SAH_BUCKET_NUM * offset(centroid_bounds.min_point[dim], centroid_bounds.max_point[dim], pi.centroid[dim]));
                bucket_index = min(bucket_index, ACCELERATOR_SAH_BUCKET_NUM - 1);
                return bucket_index <= min_cost_bucket_index;
            });
            auto mid = static_cast<uint32_t>(mid_ptr - &instance_infos[0]);
            init_interior(node, build(arena, start, mid, instance_infos, ordered, total), build(arena, mid, end, instance_infos, ordered, total), dim);
        }
    }
    return node;
}

std::vector<BLASInstanceInfo4p> pack_instances(const std::vector<shared<BLASInstance>> &instance_list, uint32_t start, uint32_t count)
{
    assert(count > 0);
    assert((start + count) <= instance_list.size());

    uint32_t soa_count = (uint32_t)(count - 1) / SSE_FLOAT_COUNT + 1;

    std::vector<Bounds3f> bounds_array;

    for (uint32_t i = 0; i < soa_count * SSE_FLOAT_COUNT; ++i)
    {
        if (i < count)
        {
            bounds_array.push_back(instance_list[start + i]->bounds());
        }
        else
        {
            bounds_array.push_back(Bounds3f());
        }
    }
    std::vector<BLASInstanceInfo4p> soa_instance_info;

    for (uint32_t i = 0; i < soa_count; ++i)
    {
        BLASInstanceInfo4p instance;
        instance.bounds = load(&bounds_array[i * SSE_FLOAT_COUNT]);

        instance.offset = start + i * SSE_FLOAT_COUNT;
        soa_instance_info.push_back(instance);
    }

    return soa_instance_info;
}

void TLAS::build_soa_instance_info(BVHBuildNode *node)
{

    if (is_leaf(node))
    {
        auto instance_infos = pack_instances(_instances, node->offset, node->num);
        node->num = static_cast<uint32_t>(instance_infos.size());
        node->offset = static_cast<uint32_t>(_soa_instance_infos.size());
        _soa_instance_infos.insert(_soa_instance_infos.end(), instance_infos.begin(), instance_infos.end());
    }
    //codition-> the interior node must has two child node
    if (node->childrens[0] && node->childrens[1])
    {
        build_soa_instance_info(node->childrens[0]);
        build_soa_instance_info(node->childrens[1]);
    }
}

bool TLAS::intersect(MemoryArena &arena, const Ray &ray, SurfaceInteraction *interaction) const
{
    std::stack<std::pair<const QBVHNode *, float>> node_stack;
    SoARay soa_ray(ray.o, ray.d, ray.t_max);

    int is_positive[3] = {ray.d[0] >= 0 ? 1 : 0, ray.d[1] >= 0 ? 1 : 0, ray.d[2] >= 0 ? 1 : 0};
    node_stack.push({&_nodes[0], 0.0f});

    bool tlas_has_hit = false;
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

                        auto leaf_box_hits = narukami::intersect(soa_ray.o, robust_rcp(soa_ray.d), float4(0), float4(soa_ray.t_max), is_positive, _soa_instance_infos[j].bounds);

                        for (uint32_t k = 0; k < 4; k++)
                        {
                            if (leaf_box_hits[k])
                            {
                                auto instance_offset = _soa_instance_infos[j].offset + k;
                                auto blas_instance = _instances[instance_offset];

                                bool has_hit = blas_instance->intersect(arena, ray, interaction);

                                if (has_hit)
                                {
                                    {
                                        tlas_has_hit = true;
                                    }
                                    {
                                        //这里不需要更新ray的t_max,因为已经在blas中更新过了
                                        soa_ray.t_max = float4(ray.t_max);
                                    }
                                }
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

    return tlas_has_hit;
}

bool TLAS::intersect(const Ray &ray) const
{
    std::stack<std::pair<const QBVHNode *, float>> node_stack;
    SoARay soa_ray(ray);
    int is_positive[3] = {ray.d[0] >= 0 ? 1 : 0, ray.d[1] >= 0 ? 1 : 0, ray.d[2] >= 0 ? 1 : 0};
    node_stack.push({&_nodes[0], 0.0f});
    Point2f uv;
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

                        auto leaf_box_hits = narukami::intersect(soa_ray.o, robust_rcp(soa_ray.d), float4(0), float4(soa_ray.t_max), is_positive, _soa_instance_infos[j].bounds);

                        for (uint32_t k = 0; k < 4; k++)
                        {
                            if (leaf_box_hits[k])
                            {
                                auto instance_offset = _soa_instance_infos[j].offset + k;
                                auto blas_instance = _instances[instance_offset];
                                bool is_hit = blas_instance->intersect(ray);
                                if (is_hit)
                                {
                                    return true;
                                }
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
    return false;
}

NARUKAMI_END