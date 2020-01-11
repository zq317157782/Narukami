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
rect light local space
x axis  = width
y axis  = height
z aixs  = front face
*/
NARUKAMI_BEGIN
class RectLight : public AreaLight
{
private:
    Spectrum _radiance;
    bool _two_side;
    float _width, _height;

public:
    RectLight(const Transform &light_to_world, const Spectrum &L, bool two_side, const float w, const float h, const size_t sample_count = 4) : AreaLight(light_to_world, sample_count,w*h), _radiance(L), _two_side(two_side), _width(w), _height(h){
    }

    Spectrum sample_Li(const Interaction &interaction, const Point2f &u, Vector3f *wi, float *pdf, VisibilityTester *tester) override
    {

        Point3f local_light_position((u.x - 0.5f) * _width, (u.y - 0.5f) * _height, 0);
        auto light_position = _light_to_world(local_light_position);
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
        Spectrum power = area() * PI * _radiance;
        power=_two_side?power*2.0f:power;
        return power;
    }

    friend std::vector<MeshTriangle> load_mesh(const RectLight &rectlight)
    {
        std::vector<std::vector<MeshTriangle>> triangles;
        float hw = rectlight._width * 0.5f;
        float hh = rectlight._height * 0.5f;
        std::vector<Point3f> vertices = {Point3f(hw, hh, 0), Point3f(hw, -hh, 0), Point3f(-hw, -hh, 0), Point3f(-hw, hh, 0)};
        std::vector<Normal3f> normals = {Normal3f(0, 0, 1), Normal3f(0, 0, 1), Normal3f(0, 0, 1), Normal3f(0, 0, 1)};
        std::vector<Point2f> uvs = {Point2f(0, 1), Point2f(0, 0), Point2f(1, 0), Point2f(1, 1)};
        std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

        return create_mesh_triangles(&rectlight._light_to_world, &rectlight._world_to_light, indices, vertices, normals, uvs);
    }
};

NARUKAMI_END