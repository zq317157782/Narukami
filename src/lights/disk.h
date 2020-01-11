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
#include "core/light.h"
#include "core/monte.h"

/*
disk light local space
z aixs  = front face
*/
NARUKAMI_BEGIN
class DiskLight : public AreaLight
{
private:
    Spectrum _radiance;
    bool _two_side;
    float _radius;

public:
    DiskLight(const Transform &light_to_world, const Spectrum &L, bool two_side, const float radius, const size_t sample_count = 4) : AreaLight(light_to_world, sample_count,2.0f * PI * radius * radius), _radiance(L), _two_side(two_side), _radius(radius)
    {

    }

    Spectrum sample_Li(const Interaction &interaction, const Point2f &u, Vector3f *wi, float *pdf, VisibilityTester *tester) override
    {

        auto sample = uniform_sample_disk(u);

        Point3f local_position(sample.x * _radius, sample.y * _radius, 0);
        auto light_position = _light_to_world(local_position);
        auto unnormalized_wi = light_position - interaction.p;
        auto distance_sqr = sqrlen(unnormalized_wi);
        (*wi) = normalize(unnormalized_wi);

        auto costheta = _world_to_light(-(*wi)).z;
        if (!_two_side && costheta <= 0.0f)
        {
            return Spectrum(0.0f, 0.0f, 0.0f);
        }

        if (pdf)
        {
            (*pdf) = to_solid_angle_measure_pdf(rcp(area()), distance_sqr, abs(costheta));
        }
        if (tester)
        {
            (*tester) = VisibilityTester(interaction, Interaction(light_position));
        }

        return _radiance;
    }

    Spectrum L(const Interaction &interaction, const Vector3f &wi) const override
    {
        if (!_two_side && _world_to_light(wi).z >= 0)
        {
            return Spectrum(0.0f, 0.0f, 0.0f);
        }
        return _radiance;
    }

    Spectrum power() const override
    {
        Spectrum flux = area() * PI * _radiance;
        flux = _two_side ? flux * 2.0f : flux;
        return flux;
    }

    friend std::vector<MeshTriangle> load_mesh(const DiskLight &disklight, const uint32_t vertex_rate = 32)
    {
        std::vector<std::vector<MeshTriangle>> triangles;
        std::vector<Point3f> vertices = {Point3f(0, 0, 0)};
        std::vector<Normal3f> normals = {Normal3f(0, 0, 1)};
        std::vector<Point2f> uvs = {Point2f(0, 0)};
        std::vector<uint32_t> indices;
        float radius = disklight._radius;
        float theta_step = 2.0f * PI / vertex_rate;
        float theta = 0;
        for (size_t i = 1; i <= vertex_rate; i++)
        {
            Point3f v(cos(theta) * radius, sin(theta) * radius, 0);
            vertices.push_back(v);
            uvs.push_back(Point2f(0, 0));
            if (i > 1)
            {
                indices.push_back(i-1);
                indices.push_back(0);
                indices.push_back(i);
            }
            theta += theta_step;
        }

        //last piece
        indices.push_back(vertex_rate);
        indices.push_back(0);
        indices.push_back(1);

        return create_mesh_triangles(&disklight._light_to_world, &disklight._world_to_light, indices, vertices, normals, uvs);
    }
};

NARUKAMI_END