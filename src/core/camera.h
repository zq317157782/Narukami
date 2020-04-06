
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
#include "core/transform.h"
#include "core/film.h"
#include "core/sampler.h"
#include "core/geometry.h"
NARUKAMI_BEGIN
    class Camera{
        protected:
            ref<Film> film;
        public:
            const Transform camera_to_world;
            inline Camera(const Transform&  camera_to_world,ref<Film> film):camera_to_world(camera_to_world),film(std::move(film)){}
            inline virtual float generate_normalized_ray(const CameraSample& sample,Ray* ray) const=0;
            inline ref<Film> get_film() const {return film;}
    };


    class ProjectiveCamera:public Camera{
         protected:
            Transform _camera_to_screen;
            Transform _screen_to_raster;
            Transform _raster_to_screen;
            Transform _raster_to_camera;
         public:
            ProjectiveCamera(const Transform&  camera_to_world,const Transform&  _camera_to_screen,const Bounds2f& screen_windows,ref<Film> film):Camera(camera_to_world,film),_camera_to_screen(_camera_to_screen){ 
                _screen_to_raster = scale(static_cast<float>(film->resolution.x),static_cast<float>(film->resolution.y),1.0f)*scale(1.0f/(screen_windows.max_point.x-screen_windows.min_point.x),1.0f/(screen_windows.min_point.y-screen_windows.max_point.y),1.0f)*translate(Vector3f(-screen_windows.min_point.x,-screen_windows.max_point.y,0.0f));
                _raster_to_screen = inverse(_screen_to_raster);
                _raster_to_camera=inverse(_camera_to_screen)/*screen2camera*/*_raster_to_screen;
            }
    };
NARUKAMI_END