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
#include "core/geometry.h"
#include <vector>
NARUKAMI_BEGIN

STAT_COUNTER("hairstrands/total vertex count", hairstrands_total_vertex_count)
STAT_COUNTER("hairstrands/total strands count", hairstrands_total_strand_count)

struct StrandCurve
{
    std::vector<Point3f> points;
    std::vector<float> thickness;
    StrandCurve(const std::vector<Point3f> &points, const std::vector<float> thickness) : points(points),thickness(thickness){}
    //目前的做法不是非常的紧密
    //todo 有没有更加紧密的方式来包围
    Bounds3f bounds(uint32_t segment) const
    {
        assert(segment < points.size() - 1);
        Point3f p0 = points[segment];
        Point3f p1 = points[segment + 1];
        float w0 = thickness[segment];
        float w1 = thickness[segment + 1];
        Bounds3f bounds(p0, p1);
        bounds = expand(bounds, max(w0, w1));
        return bounds;
    }

    size_t segment_count() const { return points.size() - 1; }

    Point3f get_start_vertex(int s) const
    {
        return points[s];
    }

    Point3f get_end_vertex(int s) const
    {
        return points[s + 1];
    }

    float get_start_thickness(int s) const
    {
        return thickness[s];
    }

    float get_end_thickness(int s) const
    {
        return thickness[s + 1];
    }
};

class HairStrands
{
private:
    shared<Transform> _object2world;
    shared<Transform> _world2object;

private:
    std::vector<StrandCurve> _strands;

public:
    HairStrands(const shared<Transform> &object2world, const shared<Transform> &world2object, std::vector<StrandCurve> strands) : _strands(strands) 
    {
        STAT_INCREASE_COUNTER(hairstrands_total_strand_count,_strands.size())
        for(int i=0;i<_strands.size();++i)
        {
            STAT_INCREASE_COUNTER(hairstrands_total_vertex_count,_strands[i].segment_count() + 1)
        }
    }

    Bounds3f bounds(uint32_t strand, uint32_t segment) const
    {
        return _strands[strand].bounds(segment);
    }

    inline const Transform &object_to_world() const { return *_object2world; }
    inline const Transform &world_to_object() const { return *_world2object; }
    size_t strands_count() const { return _strands.size(); }
    size_t segment_count(uint32_t strand) const { return _strands[strand].segment_count(); }

    Point3f get_start_vertex(uint32_t strand, uint32_t segment)
    {
        return _strands[strand].get_start_vertex(segment);
    }

    Point3f get_end_vertex(uint32_t strand, uint32_t segment)
    {
        return _strands[strand].get_end_vertex(segment);
    }

    float get_start_thickness(uint32_t strand, uint32_t segment)
    {
        return _strands[strand].get_start_thickness(segment);
    }

    float get_end_thickness(uint32_t strand, uint32_t segment)
    {
        return _strands[strand].get_end_thickness(segment);
    }
};

NARUKAMI_END