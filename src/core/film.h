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
NARUKAMI_BEGIN

//16bytes
struct Pixel
{
   float rgb[3]={0.0f,0.0f,0.0f};
   float weight;

   Pixel(){
       rgb[0]=0.0f;
       rgb[1]=0.0f;
       rgb[2]=0.0f;
       weight=0.0f;
   }
};

//use Mitchell Filter
class Film{
    public:
        const Point2i resolution;
        static constexpr float B =1.0f/3.0f;
        static constexpr float C =1.0f/3.0f;
        static constexpr int FILTER_LUT_WIDTH = 16;
        static constexpr float FILTER_RADIUS = 1.0f;
        static constexpr float FILTER_INVERSE_RADIUS = 1.0f;
    private:
        std::unique_ptr<Pixel[]> _pixels;
        Bounds2i _cropped_pixel_bounds;
        
        Pixel& get_pixel(const Point2i& p) const;
        //from PBRT
        float mitchell_1D(float x) const;
        float _filter_lut[FILTER_LUT_WIDTH];
    public:
        Film(const Point2i& resolution,const Bounds2f& cropped_pixel_bounds);
        FINLINE  Bounds2i cropped_pixel_bounds() const{
            return _cropped_pixel_bounds;
        }
        FINLINE  Bounds2i sample_bounds() const{
            Point2f min_point = floor(Point2f(_cropped_pixel_bounds.min_point)+Vector2f(0.5f,0.5f)-FILTER_RADIUS);
            Point2f max_point = ceil(Point2f(_cropped_pixel_bounds.max_point)-Vector2f(0.5f,0.5f)+FILTER_RADIUS);
            return Bounds2i(Bounds2f(min_point,max_point));
        }
        void write_to_file(const char* file_name) const;
        void add_sample(const Point2f& pos,const Spectrum& l,const float weight) const;
};


NARUKAMI_END