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
#include "core/spectrum.h"
#include "core/geometry.h"
#include "core/memory.h"
#include "core/imageio.h"
NARUKAMI_BEGIN

//16bytes
struct Pixel
{
   float rgb[3];
   float weight;

   Pixel(){
       rgb[0]=0.0f;
       rgb[1]=0.0f;
       rgb[2]=0.0f;
       weight=0.0f;
   }
};

class Film{
    private:
        
        const Point2i _resolution;
        std::unique_ptr<Pixel[]> _pixels;
        
        const Pixel& get_pixel(int x,int y) const{
            return _pixels[y*_resolution.x+x];
        }
    public:
        Film(const Point2i& resolution):_resolution(resolution){
            _pixels=std::unique_ptr<Pixel[]>(static_cast<Pixel*>(alloc_aligned<NARUKAMI_CACHE_LINE>(_resolution.x*_resolution.y*sizeof(Pixel))));
        }

        void write_to_file(const char* file_name) const{
            std::vector<float> data;
            for(size_t y=0;y<_resolution.y;++y){
                for(size_t x=0;x<_resolution.x;++x){
                    const auto pixel=get_pixel(x,y);
                    float inv_w=rcp(pixel.weight);
                    data.push_back(pixel.rgb[0]*inv_w);
                    data.push_back(pixel.rgb[1]*inv_w);
                    data.push_back(pixel.rgb[2]*inv_w);
                }
            }
            write_image_to_file(file_name,&data[0],_resolution.x,_resolution.y);
        }

};


NARUKAMI_END