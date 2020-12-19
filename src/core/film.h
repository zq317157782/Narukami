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
#include "core/image.h"
#include <mutex>
NARUKAMI_BEGIN

#ifndef NARUKAMI_FILM_FILTER_LUT_WIDTH
#define NARUKAMI_FILM_FILTER_LUT_WIDTH 32
#endif

struct TilePixel
{
   Spectrum intensity;
   float weight;

   TilePixel(){
       intensity=Spectrum();
       weight=0.0f;
   }
};

class FilmTile
{
    public:
        
    private:
        const Bounds2i _pixel_bounds;
        const float *_filter_lut;
        const float _filter_radius;
        const float _inv_filter_radius;
        std::unique_ptr<TilePixel[]> _tile_pixels;
    public:
        FilmTile( const Bounds2i& pixel_bounds,const float* filter_lut,const float filter_radius);
        friend class Film;
        void add_sample(const Point2f& pos,const Spectrum& l,const float weight) const;
        TilePixel& get_tile_pixel(const Point2i& p) const;
        inline  Bounds2i get_pixel_bounds() const{
            return _pixel_bounds;
        }
};

struct Pixel
{
   Spectrum intensity;
   float weight;

   Pixel(){
       intensity=Spectrum();
       weight=0.0f;
   }
};

class Film{
    public:
        const Point2i resolution;
       
    private:
        std::unique_ptr<Pixel[]> _pixels;
        Bounds2i _cropped_pixel_bounds;
        
        Pixel& get_pixel(const Point2i& p) const;
        //from PBRT
        float gaussian_1D(float x) const;
        float _filter_lut[NARUKAMI_FILM_FILTER_LUT_WIDTH]; 
        const float _gaussian_exp;
        const float _gaussian_alpha;
        const float _filter_radius;
        const float _inv_filter_radius;

        std::mutex _mutex;
    public:
        Film(const Point2i& resolution,const Bounds2f& cropped_rect,float const filter_radius=1.0f, float gaussian_alpha=1.0f);
        inline  Bounds2i get_cropped_pixel_bounds() const{
            return _cropped_pixel_bounds;
        }
        inline  Bounds2i get_sample_bounds() const{
            Point2f min_point = floor(Point2f(_cropped_pixel_bounds.min_point)+Vector2f(0.5f,0.5f)-_filter_radius);
            Point2f max_point = ceil(Point2f(_cropped_pixel_bounds.max_point)-Vector2f(0.5f,0.5f)+_filter_radius);
            return Bounds2i(Bounds2f(min_point,max_point));
        }

        shared<narukami::Image> get_image() const;
        void add_sample(const Point2f& pos,const Spectrum& l,const float weight) const;

        std::unique_ptr<FilmTile> get_film_tile(const Bounds2i& sample_bounds) const;
        void merge_film_tile(std::unique_ptr<FilmTile> tile);

        friend inline  std::ostream &operator<<(std::ostream &out, const Film &film) {
            out<<"[resolution:"<<film.resolution<<" cropped pixel bounds:"<<film._cropped_pixel_bounds<<"]";
            return out;
        } 
};



NARUKAMI_END