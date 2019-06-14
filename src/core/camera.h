
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
            Film* film;
        public:
            const Transform camera2world;
            inline Camera(const Transform&  camera2world,Film * film):camera2world(camera2world),film(film){}
            inline virtual float generate_normalized_ray(const CameraSample& sample,Ray* ray) const=0;   
    };


    class ProjectiveCamera:public Camera{
         protected:
            Transform camera2screen;
            Transform screen2raster;
            Transform raster2screen;
            Transform raster2camera;
         public:
            ProjectiveCamera(const Transform&  camera2world,const Transform&  camera2screen,const Bounds2f& screen_windows,Film * film):Camera(camera2world,film),camera2screen(camera2screen){ 
                screen2raster = scale(static_cast<float>(film->resolution.x),static_cast<float>(film->resolution.y),1.0f)*scale(1.0f/(screen_windows.max_point.x-screen_windows.min_point.x),1.0f/(screen_windows.min_point.y-screen_windows.max_point.y),1.0f)*translate(Vector3f(-screen_windows.min_point.x,-screen_windows.max_point.y,0.0f));
                raster2screen = inverse(screen2raster);
                raster2camera=inverse(camera2screen)/*screen2camera*/*raster2screen;
            }
    };
NARUKAMI_END