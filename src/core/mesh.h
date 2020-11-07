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
#include "core/transform.h"
#include "core/stat.h"
#include "core/geometry.h"
#include "core/memory.h"
#include <vector>
NARUKAMI_BEGIN
STAT_COUNTER("mesh/total vertex count", mesh_total_vertex_count)
STAT_COUNTER("mesh/total segment count", mesh_total_segment_count)
STAT_COUNTER("mesh/total face count", mesh_total_face_count)
struct MeshFace
{
    uint32_t vertex_index[3];
    uint32_t normal_index[3];
    uint32_t texcoord_index[3];
    MeshFace() = default;
    MeshFace(const uint32_t vi[3], const uint32_t ni[3], const uint32_t ti[3])
    {
        memcpy_t(vertex_index, vi, 3);
        memcpy_t(normal_index, ni, 3);
        memcpy_t(texcoord_index, ti, 3);
    }

    MeshFace(const uint32_t vi[3])
    {
        memcpy_t(vertex_index, vi, 3);
        memcpy_t(normal_index, vi, 3);
        memcpy_t(texcoord_index, vi, 3);
    }
};
struct MeshSegment
{
    std::vector<MeshFace> faces;
    MeshSegment() = default;
    MeshSegment(const std::vector<MeshFace> &fs)
    {
        assert(fs.size() > 0);
        faces.resize(fs.size());
        memcpy_t(&faces[0], &fs[0], fs.size());
    }
};
class Mesh
{
private:
    shared<Transform> _object2world;
    shared<Transform> _world2object;

private:
    std::vector<Point3f> _positions;
    std::vector<Normal3f> _normals;
    std::vector<Point2f> _texcoords;
    std::vector<MeshSegment> _segments;

public:
    Mesh(const shared<Transform> &object2world, const shared<Transform> &world2object, const std::vector<Point3f> &positions, const std::vector<Normal3f> &normals, const std::vector<Point2f> &texcoords, const std::vector<MeshSegment> &segments) : _object2world(object2world), _world2object(world2object)
    {

        assert(positions.size() > 0);

        _positions.resize(positions.size());
        for (int i = 0; i < positions.size(); ++i)
        {
            _positions[i] = (*object2world)(positions[i]);
        }

        if (normals.size() > 0)
        {
            _normals.resize(normals.size());
            memcpy_t(&_normals[0], &normals[0], normals.size());
        }

        if (texcoords.size() > 0)
        {
            _texcoords.resize(texcoords.size());
            memcpy_t(&_texcoords[0], &texcoords[0], texcoords.size());
        }

        for (auto &&s : segments)
        {
            _segments.push_back(s);
        }
        /********************STAT************************/

        STAT_INCREASE_COUNTER(mesh_total_vertex_count, _positions.size())
        STAT_INCREASE_COUNTER(mesh_total_segment_count, _segments.size())
        for (int s = 0; s < _segments.size(); ++s)
        {
            STAT_INCREASE_COUNTER(mesh_total_face_count, _segments[s].faces.size())
        }
    }

    ~Mesh()
    {
        STAT_DECREASE_COUNTER(mesh_total_vertex_count, _positions.size())
        STAT_DECREASE_COUNTER(mesh_total_segment_count, _segments.size())
        for (int s = 0; s < _segments.size(); ++s)
        {
            STAT_DECREASE_COUNTER(mesh_total_face_count, _segments[s].faces.size());
        }
    }

    inline Point3f get_vertex(uint32_t segment, uint32_t face, uint32_t vertex) const
    {
        assert(vertex >= 0 && vertex <= 2);
        assert(segment < _segments.size());
        assert(face < _segments[segment].faces.size());
        uint32_t idx = _segments[segment].faces[face].vertex_index[vertex];
        return _positions[idx];
    }
    inline Point3f get_vertex(uint32_t segment, uint32_t face, Point2f &barycentric) const
    {
        assert(segment < _segments.size());
        assert(face < _segments[segment].faces.size());
        const Point2f &u = barycentric;
        MeshFace mf = _segments[segment].faces[face];
        Point3f p0 = _positions[mf.vertex_index[0]];
        Point3f p1 = _positions[mf.vertex_index[1]];
        Point3f p2 = _positions[mf.vertex_index[2]];
        return bycerp(p0, p1, p2, u.x, u.y);
    }
    inline Point2f get_texcoord(uint32_t segment, uint32_t face, uint32_t vertex) const
    {
        assert(vertex >= 0 && vertex <= 2);
        assert(segment < _segments.size());
        assert(face < _segments[segment].faces.size());
        if (_texcoords.size() > 0)
        {
            uint32_t idx = _segments[segment].faces[face].texcoord_index[vertex];
            return _texcoords[idx];
        }
        else
            return Point2f(0.0f, 0.0f);
    }

    inline Point2f get_texcoord(uint32_t segment, uint32_t face, const Point2f &barycentric) const
    {
        assert(segment < _segments.size());
        assert(face < _segments[segment].faces.size());

        if (_texcoords.size() > 0)
        {
            const Point2f &u = barycentric;
            MeshFace mf = _segments[segment].faces[face];
            Point2f t0 = _texcoords[mf.texcoord_index[0]];
            Point2f t1 = _texcoords[mf.texcoord_index[1]];
            Point2f t2 = _texcoords[mf.texcoord_index[2]];
            return t0 * (1.0f - u.x - u.y) + t1 * u.x + t2 * u.y;
        }
        else
            return Point2f(0.0f, 0.0f);
    }

    inline Bounds3f get_face_bounds(uint32_t segment, uint32_t face) const
    {
        assert(segment < _segments.size());
        assert(face < _segments[segment].faces.size());
        MeshFace mf = _segments[segment].faces[face];

        Point3f v0 = _positions[mf.vertex_index[0]];
        Point3f v1 = _positions[mf.vertex_index[1]];
        Point3f v2 = _positions[mf.vertex_index[2]];
        return _union(_union(v0, v1), v2);
    }

    inline const Transform &object_to_world() const { return *_object2world; }
    inline const Transform &world_to_object() const { return *_world2object; }

    inline size_t get_segment_count() const { return _segments.size(); }
    inline size_t get_face_count(uint32_t segment) const { return _segments[segment].faces.size(); }

    inline friend std::ostream &operator<<(std::ostream &out, const Mesh &mesh)
    {
        out << "[ vertex num:" << mesh._positions.size() << " normal num:" << mesh._normals.size() << " texcoord num:" << mesh._texcoords.size() << " segment num:" << mesh._segments.size() << " ]";
        return out;
    }

    void *operator new(size_t size);
    void operator delete(void *ptr);
};

shared<Mesh> create_plane(const shared<Transform> &object2world, const shared<Transform> &world2object, const float width, const float height);
// std::vector<shared<TriangleMesh>> create_disk(const shared<Transform> &object2worldobject2wrold, const shared<Transform> &object2worldworld2object, float radius, const uint32_t vertex_density);

NARUKAMI_END
