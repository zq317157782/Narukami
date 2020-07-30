
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
           
            shared<Film> film;
        public:
            const shared<AnimatedTransform> camera_to_world;
            const float shutter_open;
            const float shutter_end;
            inline Camera(const shared<AnimatedTransform>&  camera_to_world,float shutter_open,float shutter_end,shared<Film> film):camera_to_world(camera_to_world),shutter_open(shutter_open),shutter_end(shutter_end),film(std::move(film)){}
            inline virtual float generate_normalized_ray(const CameraSample& sample,Ray* ray) const=0;
            inline shared<Film> get_film() const {return film;}
    };


    class ProjectiveCamera:public Camera{
         protected:
            shared<Transform> _camera_to_screen;
            shared<Transform> _screen_to_raster;
            shared<Transform> _raster_to_screen;
            shared<Transform> _raster_to_camera;
         public:
            ProjectiveCamera(const shared<AnimatedTransform>&  camera_to_world,float shutter_open,float shutter_end,const shared<Transform>&  _camera_to_screen,const Bounds2f& screen_windows,shared<Film> film):Camera(camera_to_world,shutter_open,shutter_end,film),_camera_to_screen(_camera_to_screen){ 
                _screen_to_raster = make_shared(scale(static_cast<float>(film->resolution.x),static_cast<float>(film->resolution.y),1.0f)*scale(1.0f/(screen_windows.max_point.x-screen_windows.min_point.x),1.0f/(screen_windows.min_point.y-screen_windows.max_point.y),1.0f)*translate(Vector3f(-screen_windows.min_point.x,-screen_windows.max_point.y,0.0f)));
                _raster_to_screen = make_shared(inverse(*_screen_to_raster));
                _raster_to_camera=make_shared(inverse(*_camera_to_screen)/*screen2camera*/*(*_raster_to_screen));
            }
    };
NARUKAMI_END