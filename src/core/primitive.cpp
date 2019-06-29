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
#include "core/primitive.h"
NARUKAMI_BEGIN
std::vector<Primitive> create_primitives(const std::vector<MeshTriangle>& triangles){
    std::vector<Primitive> primitives(triangles.size());
    for (size_t i = 0; i < triangles.size(); ++i)
    {
        primitives[i]=Primitive(triangles[i]);
    }
    
    return primitives;
}


std::vector<SoATriangle> cast2SoA(const std::vector<Primitive> &triangles, uint32_t start, uint32_t count)
{
    assert(count > 0);
    assert((start + count) <= triangles.size());

    size_t soa_count = (uint32_t)count / SSE_FLOAT_COUNT + 1;

    std::vector<Point3f> v0_array;
    std::vector<Vector3f> e1_array;
    std::vector<Vector3f> e2_array;

    for (size_t i = 0; i < soa_count * SSE_FLOAT_COUNT; ++i)
    {
        if (i < count)
        {
            auto v0 = triangles[start + i].triangle[0];
            auto e1 = triangles[start + i].triangle[1] - v0;
            auto e2 = triangles[start + i].triangle[2] - v0;

            v0_array.push_back(v0);
            e1_array.push_back(e1);
            e2_array.push_back(e2);
        }
        else
        {
            v0_array.push_back(Point3f());
            e1_array.push_back(Vector3f());
            e2_array.push_back(Vector3f());
        }
    }
    std::vector<SoATriangle> soa_triangles;

    for (size_t i = 0; i < soa_count; ++i)
    {
        SoATriangle triangle;
        triangle.v0 = load(&v0_array[i * SSE_FLOAT_COUNT]);
        triangle.e1 = load(&e1_array[i * SSE_FLOAT_COUNT]);
        triangle.e2 = load(&e2_array[i * SSE_FLOAT_COUNT]);
        soa_triangles.push_back(triangle);
    }

    return soa_triangles;
}

NARUKAMI_END