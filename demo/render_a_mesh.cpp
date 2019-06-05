#include "core/narukami.h"
#include "core/geometry.h"
#include "core/transform.h"
#include "core/mesh.h"
#include "core/imageio.h"
#include "core/sampler.h"
using namespace narukami;
int main()
{

    Sampler sampler(16);

    Point3f vertices[4] = {Point3f(0, 1, 1), Point3f(0, 0, 1), Point3f(1, 0, 1), Point3f(1, 1, 1)};
    Point2f uvs[4] = {Point2f(0, 1), Point2f(0, 0), Point2f(1, 0), Point2f(1, 1)};
    uint32_t indices[6] = {0, 1, 3, 1, 2, 3};
    auto transform = translate(Vector3f(0, 0, 0));
    auto transform2 = translate(Vector3f(0, 0, 0));
    auto triangles = create_mesh_triangles(&transform, &transform2, 2, indices, 4, vertices, nullptr, uvs);

    auto soa_triangles = cast2SoA(triangles, 0, 2);
    std::cout << soa_triangles[0];

    std::vector<uint8_t> image;
    float data[128 * 128 * 3];
    for (int y = 0; y < 128; ++y)
    {
        for (int x = 0; x < 128; ++x)
        {
            data[(y*128+x)* 3] = 0;
            data[(y*128+x)* 3 + 1] = 0;
            sampler.start_pixel(Point2i(0, 0));
            do
            {
                auto sample = sampler.get_2D();
                SoARay ray(Point3f((x+sample.x)/128.0f, (y+sample.y)/128.0f, 0), Vector3f(0, 0, 1));
                Point2f uv;
                float t;
                int index;
                bool b = intersect(ray, soa_triangles[0], &t, &uv, &index);
                if (b)
                {
                    uv = triangles[index]->sample_uv(uv);
                    data[(y*128+x)* 3] += uv[0];
                    data[(y*128+x)* 3 + 1] += uv[1];
                }
                else
                {
                    std::cout << "miss " << ray.o << std::endl;
                }

            } while (sampler.start_next_sample());
            data[(y*128+x)* 3] /= sampler.get_spp();
            data[(y*128+x)* 3 + 1] /= sampler.get_spp();
            data[(y*128+x)* 3 + 2] = 0;
        }
    }

    narukami::write_image_to_file("mesh.png", data, 128, 128);
    //unsigned error = lodepng::encode(, image, 128, 128);
}