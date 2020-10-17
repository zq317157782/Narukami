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
#include "core/hairstrands.h"
#include "cyHairFile.h"
NARUKAMI_BEGIN
enum class HairStrandsFileFormat
{
    HAIR
};

template <HairStrandsFileFormat format>
shared<HairStrands> load_hairstrands(const shared<Transform> &object2world, const shared<Transform> &world2object, const char *file_name,float thickness_scale)
{
    return shared<HairStrands>();
}

template <>
shared<HairStrands> load_hairstrands<HairStrandsFileFormat::HAIR>(const shared<Transform> &object2world, const shared<Transform> &world2object, const char *file_name,float thickness_scale)
{
    // Load the hair model
    cyHairFile hairfile;
    int result = hairfile.LoadFromFile(file_name);

    int hair_count = hairfile.GetHeader().hair_count;
    const unsigned short *cy_segments = hairfile.GetSegmentsArray();
    float *cy_points = hairfile.GetPointsArray();
    float *cy_thickness = hairfile.GetThicknessArray();
    if (!cy_points)
    {
        NARUKAMI_ERROR("hairstrands has vertex count %d", hair_count)
        exit(-1);
    }
    if (!cy_thickness)
    {
        NARUKAMI_WARNING("hairstrands has not vertex thickness data, so use default thickness %f with scale %f", hairfile.GetHeader().d_thickness,thickness_scale)
    }

   
    std::vector<StrandCurve> strands;
    uint32_t global_point_index = 0;
    for (int i = 0; i < hair_count; ++i)
    {
        uint32_t point_count = cy_segments[i] + 1;
        std::vector<Point3f> points;
        std::vector<float> thickness_array;
        for (uint32_t p = 0; p < point_count; ++p, ++global_point_index)
        {
            {
                Point3f point(cy_points[global_point_index * 3], cy_points[global_point_index * 3 + 1], cy_points[global_point_index * 3 + 2]);
                points.push_back((*object2world)(point));
            }
            {
                float thickness = 0;
                if (cy_thickness)
                {
                    thickness = cy_thickness[global_point_index];
                }
                else
                {
                   thickness = hairfile.GetHeader().d_thickness;
                }
                thickness_array.push_back(thickness * thickness_scale);
            }
        }
        StrandCurve strand(points, thickness_array);
        strands.push_back(strand);
    }

    return shared<HairStrands>(new HairStrands(object2world, world2object, strands));
}

NARUKAMI_END