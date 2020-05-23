
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
            inline OrthographicCamera(const ref<Transform>&  camera_to_world,const Bounds2f& screen_windows, std::shared_ptr<Film> film):ProjectiveCamera(camera_to_world,ref_cast(orthographic(0.0f,1.0f)),screen_windows,std::move(film)){}
            
            
            inline virtual float generate_normalized_ray(const CameraSample& sample,Ray* ray) const override
            {
                auto pCamera=transform_h(*_raster_to_camera,Point3f(sample.pFilm.x,sample.pFilm.y,0));
                Ray rayCamera(pCamera,Vector3f(0,0,1));
                (*ray)=(*camera_to_world)(rayCamera);
                return 1.0f;
            }  
    };
NARUKAMI_END

