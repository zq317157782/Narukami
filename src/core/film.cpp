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
#include "core/film.h"
#include "core/memory.h"
#include "core/imageio.h"
NARUKAMI_BEGIN

FilmTile::FilmTile(const Bounds2i &pixel_bounds, const float *filter_lut, const float filter_radius) : _pixel_bounds(pixel_bounds), _filter_lut(filter_lut), _filter_radius(filter_radius), _inv_filter_radius(1.0f / filter_radius)
{
    _tile_pixels = std::unique_ptr<TilePixel[]>(new TilePixel[area(_pixel_bounds)]);
}

void FilmTile::add_sample(const Point2f &pos, const Spectrum &l, const float weight) const
{

    //calculate bounds
    auto dp = pos - Vector2f(0.5f, 0.5f);
    Point2i p0 = static_cast<Point2i>(ceil(dp - _filter_radius));
    Point2i p1 = static_cast<Point2i>(floor(dp + _filter_radius)) + Point2i(1, 1);

    p0 = max(p0, _pixel_bounds.min_point);
    p1 = min(p1, _pixel_bounds.max_point);
    for (int x = p0.x; x < p1.x; ++x)
    {
        int idx_x = min(static_cast<int>(abs(x - dp.x) * _inv_filter_radius * NARUKAMI_FILM_FILTER_LUT_WIDTH), NARUKAMI_FILM_FILTER_LUT_WIDTH - 1);
        float filter_weight_x = _filter_lut[idx_x];
        for (int y = p0.y; y < p1.y; ++y)
        {
            int idx_y = min(static_cast<int>(abs(y - dp.y) * _inv_filter_radius * NARUKAMI_FILM_FILTER_LUT_WIDTH), NARUKAMI_FILM_FILTER_LUT_WIDTH - 1);
            float filter_weight = _filter_lut[idx_y] * filter_weight_x;
            TilePixel &pixel = get_tile_pixel(Point2i(x, y));
            {
                pixel.intensity += l * weight * filter_weight;
            }
            pixel.weight += filter_weight;
        }
    }
}

TilePixel &FilmTile::get_tile_pixel(const Point2i &p) const
{
    assert(inside_exclusive(p, _pixel_bounds));
    auto width = _pixel_bounds[1].x - _pixel_bounds[0].x;
    auto x = p.x - _pixel_bounds[0].x;
    auto y = p.y - _pixel_bounds[0].y;
    return _tile_pixels[y * width + x];
}

Film::Film(const Point2i &resolution, const Bounds2f &cropped_rect, float const filter_radius, const float gaussian_alpha) : resolution(resolution), _filter_radius(filter_radius), _inv_filter_radius(1.0f / filter_radius), _gaussian_alpha(gaussian_alpha), _gaussian_exp(exp(-gaussian_alpha * filter_radius * filter_radius))
{
    Point2i bounds_min_p = Point2i((int)ceil(resolution.x * cropped_rect.min_point.x), (int)ceil(resolution.y * cropped_rect.min_point.y));
    Point2i bounds_max_p = Point2i((int)ceil(resolution.x * cropped_rect.max_point.x), (int)ceil(resolution.y * cropped_rect.max_point.y));
    this->_cropped_pixel_bounds = Bounds2i(bounds_min_p, bounds_max_p);
    _pixels = std::unique_ptr<Pixel[]>(new Pixel[area(_cropped_pixel_bounds)]);

    //init filter LUT
    for (int i = 0; i < NARUKAMI_FILM_FILTER_LUT_WIDTH; ++i)
    {
        float x = (i + 0.5f) / NARUKAMI_FILM_FILTER_LUT_WIDTH;
        _filter_lut[i] = gaussian_1D(x);
    }
}

Pixel &Film::get_pixel(const Point2i &p) const
{
    assert(inside_exclusive(p, _cropped_pixel_bounds));
    auto width = _cropped_pixel_bounds[1].x - _cropped_pixel_bounds[0].x;
    auto x = p.x - _cropped_pixel_bounds[0].x;
    auto y = p.y - _cropped_pixel_bounds[0].y;
    return _pixels[y * width + x];
}

float Film::gaussian_1D(float x) const
{
    return max(0.0f, exp(-_gaussian_alpha * x * x) - _gaussian_exp);
}

shared<narukami::Image> Film::get_image() const
{
    std::vector<float> data;
    for (int y = _cropped_pixel_bounds[0].y; y < _cropped_pixel_bounds[1].y; ++y)
    {
        for (int x = _cropped_pixel_bounds[0].x; x < _cropped_pixel_bounds[1].x; ++x)
        {
            const Pixel &pixel = get_pixel(Point2i(x, y));
            float inv_w = rcp(pixel.weight);
            if (EXPECT_NOT_TAKEN(pixel.weight == 0.0f))
            {
                inv_w = 1.0f;
            }

            RGB rgb(pixel.intensity * inv_w);
            data.push_back(rgb[0]);
            data.push_back(rgb[1]);
            data.push_back(rgb[2]);
            data.push_back(1.0f);
        }
    }
    narukami::Image image(reinterpret_cast<uint8_t *>(&data[0]), resolution, PixelFormat::RGBA32);
    return std::make_shared<narukami::Image>(image);
}

// void Film::write_to_file(const char *file_name) const
// {
//     std::vector<float> data;
//     for (int y = _cropped_pixel_bounds[0].y; y < _cropped_pixel_bounds[1].y; ++y)
//     {
//         for (int x = _cropped_pixel_bounds[0].x; x < _cropped_pixel_bounds[1].x; ++x)
//         {
//             const Pixel &pixel = get_pixel(Point2i(x, y));
//             float inv_w = rcp(pixel.weight);
//             if (EXPECT_NOT_TAKEN(pixel.weight == 0.0f))
//             {
//                 inv_w = 1.0f;
//             }

//             RGB rgb(pixel.intensity * inv_w);
//             data.push_back(rgb[0]);
//             data.push_back(rgb[1]);
//             data.push_back(rgb[2]);
//         }
//     }
//     write_image_to_file(file_name, &data[0], resolution.x, resolution.y);
// }

void Film::add_sample(const Point2f &pos, const Spectrum &l, const float weight) const
{

    //calculate bounds
    auto dp = pos - Vector2f(0.5f, 0.5f);
    Point2i p0 = (Point2i)ceil(dp - _filter_radius);
    Point2i p1 = (Point2i)floor(dp + _filter_radius) + Point2i(1, 1);

    p0 = max(p0, _cropped_pixel_bounds.min_point);
    p1 = min(p1, _cropped_pixel_bounds.max_point);
    for (int x = p0.x; x < p1.x; ++x)
    {
        int idx_x = min(static_cast<int>(abs(x - dp.x) * _inv_filter_radius * NARUKAMI_FILM_FILTER_LUT_WIDTH), NARUKAMI_FILM_FILTER_LUT_WIDTH - 1);
        float filter_weight_x = _filter_lut[idx_x];
        for (int y = p0.y; y < p1.y; ++y)
        {
            int idx_y = min(static_cast<int>(abs(y - dp.y) * _inv_filter_radius * NARUKAMI_FILM_FILTER_LUT_WIDTH), NARUKAMI_FILM_FILTER_LUT_WIDTH - 1);
            float filter_weight = _filter_lut[idx_y] * filter_weight_x;
            Pixel &pixel = get_pixel(Point2i(x, y));
            {
                pixel.intensity += l * weight * filter_weight;
            }
            pixel.weight += filter_weight;
        }
    }
}

std::unique_ptr<FilmTile> Film::get_film_tile(const Bounds2i &sample_bounds) const
{
    const Vector2f half_bounds(0.5f, 0.5f);
    auto float_bounds = static_cast<Bounds2f>(sample_bounds);
    Point2i p0 = static_cast<Point2i>(ceil(float_bounds.min_point - half_bounds - _filter_radius));
    Point2i p1 = static_cast<Point2i>(floor(float_bounds.max_point - half_bounds + _filter_radius)) + Point2i(1, 1);
    auto pixel_bound = intersect(Bounds2i(p0, p1), _cropped_pixel_bounds);
    return make_unique<FilmTile>(pixel_bound, _filter_lut, _filter_radius);
}

void Film::merge_film_tile(std::unique_ptr<FilmTile> tile)
{
    //lock for thread
    std::lock_guard<std::mutex> lock(_mutex);

    for (auto p : tile->_pixel_bounds)
    {
        const auto tile_pixel = tile->get_tile_pixel(p);
        Pixel &merge_pixel = get_pixel(p);
        merge_pixel.intensity = merge_pixel.intensity + tile_pixel.intensity;
        merge_pixel.weight = merge_pixel.weight + tile_pixel.weight;
    }
}

NARUKAMI_END
