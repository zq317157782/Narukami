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
    RGBA8,
    sRGBA8,
    RGBA32
};

inline const char *pixel_format_name(const PixelFormat &pf)
{
    switch (pf)
    {
    case PixelFormat::RGBA8:
        return "RGBA8";
        break;
    case PixelFormat::sRGBA8:
        return "sRGBA8";
        break;
    case PixelFormat::RGBA32:
        return "RGBA32";
        break;
    }
}
inline std::ostream &operator<<(std::ostream &out, const PixelFormat &pf)
{
    switch (pf)
    {
    case PixelFormat::RGBA8:
        out << "RGBA8";
        break;
    case PixelFormat::sRGBA8:
        out << "sRGBA8";
        break;
    case PixelFormat::RGBA32:
        out << "RGB32";
        break;
    }
    return out;
}

inline int num_channel(const PixelFormat &pf)
{
    switch (pf)
    {
    case PixelFormat::RGBA8:
    case PixelFormat::sRGBA8:
    case PixelFormat::RGBA32:
        return 4;
        break;
    }
}

inline int num_byte(const PixelFormat &pf)
{
    switch (pf)
    {
    case PixelFormat::RGBA8:
    case PixelFormat::sRGBA8:
        return 4;
        break;
    case PixelFormat::RGBA32:
        return 16;
        break;
    }
}

inline bool is_srgb_space(const PixelFormat &pf)
{
    switch (pf)
    {
    case PixelFormat::sRGBA8:
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

class Image
{
private:
    std::vector<uint8_t> _data;
    Point2i _resolution;
    PixelFormat _pixel_format;
    PixelState _pixel_state;
    float get_float_data(const uint8_t* address) const;
    void get_linear_data(float*,const uint8_t* address) const;
    const uint8_t* address(const Point2i &idx) const;
public:
    Image(uint8_t *data, const Point2i &resolution, const PixelFormat &pf);
    Image(const char *file_name, const PixelFormat &pf = PixelFormat::sRGBA8);
    void write_to_png(const char *file_name, const PixelFormat &output_format = PixelFormat::sRGBA8);
    int total_bytes() const;
    int total_pixels() const;
    const Point2i &resolution() const { return _resolution; }
    RGBA texel(const Point2i &uv) const;
    //static std::vector<shared<Image>> generate_mipmap(const shared<Image> &image);
};
NARUKAMI_END