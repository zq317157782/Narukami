
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
#include "core/camera.h"
NARUKAMI_BEGIN
class PerspectiveCamera : public ProjectiveCamera
{
private:
public:
    inline PerspectiveCamera(const shared<AnimatedTransform> &camera_to_world, float shutter_open, float shutter_end, const Bounds2f &screen_windows, const float fov, std::shared_ptr<Film> film) : ProjectiveCamera(camera_to_world, shutter_open, shutter_end, make_shared(perspective(fov, 1e-2f, 1000.0f)), screen_windows, std::move(film)) {}

    inline virtual float generate_normalized_ray(const CameraSample &sample, Ray *ray) const override
    {
        Point3f point_raster(sample.pFilm.x, sample.pFilm.y, 0);
        Point3f point_camera = transform_4x4(*_raster_to_camera, point_raster);
        float shutter_time = shutter_open + sample.time * (shutter_end - shutter_open);
        Ray ray_cam(Point3f(0, 0, 0), normalize(Vector3f(point_camera)));
        ray_cam.time = shutter_time;
        Transform c2w;
        camera_to_world->interpolate(shutter_time, &c2w);
        (*ray) = c2w(ray_cam);
        return 1.0f;
    }

    inline virtual float generate_normalized_ray_differential(const CameraSample &sample, RayDifferential *ray) const override
    {
        Point3f point_raster(sample.pFilm.x, sample.pFilm.y, 0);
        Point3f point_camera = transform_4x4(*_raster_to_camera, point_raster);
        float shutter_time = shutter_open + sample.time * (shutter_end - shutter_open);
        Transform c2w;
        camera_to_world->interpolate(shutter_time, &c2w);
        
        RayDifferential ray_cam(Point3f(0, 0, 0), normalize(Vector3f(point_camera)));
        ray_cam.time = shutter_time;

        // x offset eye ray
        {
            ray_cam.ox = Point3f(0, 0, 0);
            Point3f point_raster_x(sample.pFilm.x + 1.0f, sample.pFilm.y, 0);
            Point3f point_camera_x = transform_4x4(*_raster_to_camera, point_raster_x);
            ray_cam.dx = normalize(Vector3f(point_camera_x));
        }

        // y offset eye ray
        {
            ray_cam.oy = Point3f(0, 0, 0);
            Point3f point_raster_y(sample.pFilm.x , sample.pFilm.y + 1.0f, 0);
            Point3f point_camera_y = transform_4x4(*_raster_to_camera, point_raster_y);
            ray_cam.dy = normalize(Vector3f(point_camera_y));
        }

        (*ray) = c2w(ray_cam);
        return 1.0f;
    }
};
NARUKAMI_END
