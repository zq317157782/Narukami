
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
    class OrthographicCamera:public ProjectiveCamera{
        private:

        public:
            inline OrthographicCamera(const shared<AnimatedTransform>&  camera_to_world,float shutter_open,float shutter_end,const Bounds2f& screen_windows, std::shared_ptr<Film> film):ProjectiveCamera(camera_to_world,shutter_open,shutter_end,make_shared(orthographic(0.0f,1.0f)),screen_windows,std::move(film)){}
            
            
            inline virtual float generate_normalized_ray(const CameraSample& sample,Ray* ray) const override
            {
                auto pCamera=transform_4x4(*_raster_to_camera,Point3f(sample.pFilm.x,sample.pFilm.y,0));
                float shutter_time = shutter_open + sample.time * (shutter_end - shutter_open);
                Ray ray_cam(pCamera,Vector3f(0,0,1),shutter_time);
                ray_cam.time = shutter_time;
                Transform c2w;
                camera_to_world->interpolate(shutter_time,&c2w);
                (*ray)=c2w(ray_cam);
                return 1.0f;
            }  
    };
NARUKAMI_END

