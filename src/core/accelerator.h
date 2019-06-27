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
#include <vector>
#include <algorithm>
NARUKAMI_BEGIN

struct BVHPrimitiveInfo{
    size_t prim_index;
    Bounds3f bounds;
    Point3f  centroid;
    BVHPrimitiveInfo() = default;
    BVHPrimitiveInfo(const Primitive& p,size_t index):prim_index(index),bounds(p.get_bounds()),centroid((p.get_bounds().min_point+p.get_bounds().max_point)*0.5f){}
};

struct BVHBuildNode{
    Bounds3f bounds;
    BVHBuildNode* childrens[2];
    uint32_t split_axis;
    uint32_t offset, num;
};

inline void init_leaf(BVHBuildNode* node,const uint32_t offset,const uint32_t num,const Bounds3f& bounds){
    node->bounds=bounds;
    node->offset=offset;
    node->num=num;
} 

inline void init_interior(BVHBuildNode* node,BVHBuildNode* c0,BVHBuildNode* c1,const uint32_t split_axis){
    node->bounds=_union(c0->bounds,c1->bounds);
    node->childrens[0]=c0;
    node->childrens[1]=c1;
    node->split_axis=split_axis;
}

class Accelerator{
    private:
        std::vector<Primitive> _primitives;

        BVHBuildNode* build(MemoryArena& arena,size_t start,size_t end,std::vector<BVHPrimitiveInfo>& primitive_infos,std::vector<Primitive>& ordered,uint32_t* total){
            auto node=arena.alloc<BVHBuildNode>(1);
            (*total)++;
            Bounds3f max_bounds;
            for (size_t i = start; i < end; i++)
            {
                max_bounds=_union(max_bounds,primitive_infos[i].bounds);
            }
            uint32_t num = end - start;
            if (num<64){
                auto offset=ordered.size();
                for (size_t i = start; i < end; i++)
                {
                   ordered.push_back(_primitives[primitive_infos[i].prim_index]);
                }
                init_leaf(node,offset,num,max_bounds);
            }
            else{
                Bounds3f centroid_bounds;
                for (size_t i = start; i < end; i++)
                {
                    centroid_bounds=_union(centroid_bounds,primitive_infos[i].centroid);
                }
                 
                auto dim=max_extent(centroid_bounds);

                if(centroid_bounds.min_point[dim]==centroid_bounds.max_point[dim]){
                    auto mid = (start+end)/2;
                    std::nth_element(&primitive_infos[start],&primitive_infos[mid],&primitive_infos[end-1]+1,[dim](const BVHPrimitiveInfo& p0,const BVHPrimitiveInfo& p1){return p0.centroid[dim]<p1.centroid[dim];});
                    init_interior(node,build(arena,start,mid,primitive_infos,ordered,total),build(arena,mid,end,primitive_infos,ordered,total),dim);
                }else{
                    auto mid_point=(centroid_bounds.max_point[dim]+centroid_bounds.min_point[dim])/2;
                    auto mid_ptr=std::partition(&primitive_infos[start],&primitive_infos[end-1]+1,[dim,mid_point](const BVHPrimitiveInfo& pi){return pi.centroid[dim]<mid_point;});
                    auto mid = static_cast<uint32_t>(mid_ptr-&primitive_infos[0]);
                    init_interior(node,build(arena,start,mid,primitive_infos,ordered,total),build(arena,mid,end,primitive_infos,ordered,total),dim);
                }
            }
            return node;
        }
    public:
    Accelerator(std::vector<Primitive> primitives):_primitives(std::move(primitives)){
    std::vector<BVHPrimitiveInfo> primitive_infos(_primitives.size());
       for (size_t i = 0; i < _primitives.size(); ++i)
       {
           primitive_infos[i]=BVHPrimitiveInfo(_primitives[i],i);
       }
       
       MemoryArena arena;
       std::vector<Primitive> _ordered_primitives;
       uint32_t total_node_num=0;
       auto build_root = build(arena,0,primitive_infos.size(),primitive_infos,_ordered_primitives,&total_node_num);
    }
};
NARUKAMI_END