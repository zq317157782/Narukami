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
#include "core/geometry.h"
#include "core/spectrum.h"
NARUKAMI_BEGIN
enum class PixelFormat
{
    R8,
    RGB8,
    sRGB8,
    RGBA8,
    sRGBA8,
    RGBA32,
    sRGBA32
};

inline const char *pixel_format_name(const PixelFormat &pf)
{
    switch (pf)
    {
    case PixelFormat::R8:
        return "R8";
        break;
    case PixelFormat::sRGB8:
        return "sRGB8";
        break;
    case PixelFormat::RGB8:
        return "RGB8";
        break;
    case PixelFormat::RGBA8:
        return "RGBA8";
        break;
    case PixelFormat::sRGBA8:
        return "sRGBA8";
        break;
    case PixelFormat::RGBA32:
        return "RGBA32";
        break;
    case PixelFormat::sRGBA32:
        return "sRGBA32";
        break;
    }
    return "unknown";
}

inline int num_channel(const PixelFormat &pf)
{
    switch (pf)
    {
    case PixelFormat::R8:
        return 1;
        break;
    case PixelFormat::RGB8:
    case PixelFormat::sRGB8:
        return 3;
        break;
    case PixelFormat::RGBA8:
    case PixelFormat::sRGBA8:
    case PixelFormat::RGBA32:
    case PixelFormat::sRGBA32:
        return 4;
        break;
    }
    return 4;
}

inline int num_byte(const PixelFormat &pf)
{
    switch (pf)
    {
    case PixelFormat::R8:
        return 1;
        break;
    case PixelFormat::RGB8:
    case PixelFormat::sRGB8:
        return 3;
        break;
    case PixelFormat::RGBA8:
    case PixelFormat::sRGBA8:
        return 4;
        break;
    case PixelFormat::RGBA32:
    case PixelFormat::sRGBA32:
        return 16;
        break;
    }
    return 4;
}

inline bool is_srgb_space(const PixelFormat &pf)
{
    switch (pf)
    {
    case PixelFormat::sRGB8:
    case PixelFormat::sRGBA8:
    case PixelFormat::sRGBA32:
        return true;
    default:
        return false;
    }
}

inline bool has_alpha(const PixelFormat &pf)
{
    switch (pf)
    {
    case PixelFormat::RGBA8:
    case PixelFormat::sRGBA8:
    case PixelFormat::RGBA32:
    case PixelFormat::sRGBA32:
        return true;
    default:
        return false;
    }
}

struct PixelState
{
    PixelState(const PixelFormat &pf)
    {
        channel_num = num_channel(pf);
        byte_num = num_byte(pf);
        byte_per_channel = byte_num / channel_num;
        is_srgb = is_srgb_space(pf);
    }
    int channel_num;
    int byte_num;
    int byte_per_channel;
    bool is_srgb;
};

struct FLinearColor
{
    float r, g, b, a;
    FLinearColor(float rr, float gg, float bb, float aa) : r(rr), g(gg), b(bb), a(aa) {}
    inline float operator[](const int idx) const
    {
        assert(idx >= 0 && idx < 4);
        return (&r)[idx];
    }
    inline float &operator[](const int idx)
    {
        assert(idx >= 0 && idx < 4);
        return (&r)[idx];
    }
};

class Image
{
private:
    std::vector<uint8_t> _data;
    Point2i _resolution;
    PixelFormat _pixel_format;
    PixelState _pixel_state;
    float get_data(const uint8_t *address) const;
    void set_data(uint8_t *address, float value);
    void get_linear_data(const uint8_t *address, float *) const;
    void set_linear_data(uint8_t *address, const float *);
    uint8_t *address(const Point2i &idx);
    const uint8_t *address(const Point2i &idx) const;

public:
    Image(uint8_t *data, const Point2i &resolution, const PixelFormat &pf);
    Image(const char *file_name, const PixelFormat &pf = PixelFormat::sRGBA8);
    void write_to_png(const char *file_name, const PixelFormat &output_format = PixelFormat::sRGBA8);
    int total_bytes() const;
    int total_channels() const;
    int total_pixels() const;
    const Point2i &resolution() const { return _resolution; }
    FLinearColor get_texel(const Point2i &p) const;
    void set_texel(const Point2i &p, const FLinearColor &rgba);
    //static std::vector<shared<Image>> generate_mipmap(const shared<Image> &image);
};
NARUKAMI_END