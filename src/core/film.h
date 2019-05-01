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
       
        std::unique_ptr<Pixel[]> _pixels;
        Bounds2i _cropped_pixel_bounds;
        Pixel& get_pixel(const Point2i& p) const{
            assert(inside_exclusive(p,_cropped_pixel_bounds));
            auto width=_cropped_pixel_bounds[1].x-_cropped_pixel_bounds[0].x;
            auto x = p.x -_cropped_pixel_bounds[0].x;
            auto y = p.y -_cropped_pixel_bounds[0].y;
            return _pixels[y*width+x];
        }
    public:
        const Point2i resolution;
        
    public:
        Film(const Point2i& resolution,const Bounds2f& bounds):resolution(resolution){
            Point2i bounds_min_p=Point2i((int)ceil(resolution.x*bounds.min_point.x),(int)ceil(resolution.y*bounds.min_point.y));
            Point2i bounds_max_p=Point2i((int)ceil(resolution.x*bounds.max_point.x),(int)ceil(resolution.y*bounds.max_point.y));
            this->_cropped_pixel_bounds=Bounds2i(bounds_min_p,bounds_max_p);
             _pixels=std::unique_ptr<Pixel[]>(static_cast<Pixel*>(alloc_aligned<NARUKAMI_CACHE_LINE>(area(_cropped_pixel_bounds)*sizeof(Pixel))));
        }

        FINLINE const Bounds2i& cropped_pixel_bounds() const{
            return _cropped_pixel_bounds;
        }

        inline void write_to_file(const char* file_name) const{
            std::vector<float> data;
            for(int y=_cropped_pixel_bounds[0].y;y<_cropped_pixel_bounds[1].y;++y){
                for(int x=_cropped_pixel_bounds[0].x;x<_cropped_pixel_bounds[1].x;++x){
                    const Pixel& pixel=get_pixel(Point2i(x,y));
                    float inv_w=rcp(pixel.weight);
                    data.push_back(pixel.rgb[0]*inv_w);
                    data.push_back(pixel.rgb[1]*inv_w);
                    data.push_back(pixel.rgb[2]*inv_w);
                }
            }
            write_image_to_file(file_name,&data[0],resolution.x,resolution.y);
        }

        inline void add_sample(const Point2f& pos,const Spectrum& l,const float weight) const{
            Point2i p = Point2i(pos);
            Pixel& pixel = get_pixel(p);
            pixel.rgb[0] = pixel.rgb[0] + l.r*weight;
            pixel.rgb[1] = pixel.rgb[1] + l.g*weight;
            pixel.rgb[2] = pixel.rgb[2] + l.b*weight;
            pixel.weight = pixel.weight + weight;
        }

    
};


NARUKAMI_END