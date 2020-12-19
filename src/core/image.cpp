
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
#include "core/image.h"
#include "lodepng.h"
#include <functional>
NARUKAMI_BEGIN

Image::Image(uint8_t *data, const Point2i &resolution, const PixelFormat &pf) : _resolution(resolution), _pixel_format(pf), _pixel_state(pf)
{
    _data.resize(total_bytes());
    memcpy(&_data[0], data, total_bytes());
}

namespace png
{

    //     LCT_GREY = 0,       /*greyscale: 1,2,4,8,16 bit*/
    //     LCT_RGB = 2,        /*RGB: 8,16 bit*/
    //     LCT_PALETTE = 3,    /*palette: 1,2,4,8 bit*/
    //     LCT_GREY_ALPHA = 4, /*greyscale with alpha: 8,16 bit*/
    //     LCT_RGBA = 6        /*RGB with alpha: 8,16 bit*/
    LodePNGColorType inner_format(const PixelFormat &pf)
    {
        switch (pf)
        {
        case PixelFormat::RGBA8:
        case PixelFormat::sRGBA8:
        case PixelFormat::RGBA32:
            //case PixelFormat::SRGBA8:
            return LCT_RGBA;
            break;
        }
    }

    int bitdepth(const PixelFormat &pf)
    {
        switch (pf)
        {
        case PixelFormat::RGBA8:
        case PixelFormat::sRGBA8:
        case PixelFormat::RGBA32: //TODO 16bit is better
            return 8;
            break;
        }
    }

    PixelFormat safe_format(const PixelFormat &pf)
    {
        switch (pf)
        {
        case PixelFormat::RGBA32:
            return PixelFormat::RGBA8;
            break;
        default:
            return pf;
        }
    }
} // namespace png

int Image::total_bytes() const
{
    return num_byte(_pixel_format) * _resolution[0] * _resolution[1];
}

int Image::total_pixels() const
{
    return num_channel(_pixel_format) * _resolution[0] * _resolution[1];
}

float srgb_to_linear(float c)
{
    if (c <= 0.0404482362771082f)
    {
        return c / 12.92f;
    }
    else
    {
        return pow((c + 0.055f) / 1.055f, 2.4f);
    }
}

float linear_to_srgb(float c)
{
    if (c > 0.0031308f)
    {
        return 1.055f * (pow(c, (1.0f / 2.4f))) - 0.055f;
    }
    else
    {
        return 12.92f * c;
    }
}

void convert_byte_to_byte(std::vector<uint8_t> &out, const std::vector<uint8_t> &in, int count, const std::function<float(float)> &encode)
{
    out.resize(count);
    const uint8_t *input = &in[0];
    for (int i = 0; i < count; ++i)
    {
        float v = clamp(input[i] / 255.f, 0.0f, 1.0f);
        v = encode(v);
        out[i] = static_cast<uint8_t>(clamp(v, 0.0f, 1.0f) * 255);
    }
}

void convert_byte_to_float(std::vector<uint8_t> &out, const std::vector<uint8_t> &in, int count, const std::function<float(float)> &encode)
{
    out.resize(count * 4);
    const uint8_t *input = &in[0];
    float *output = reinterpret_cast<float *>(&out[0]);
    for (int i = 0; i < count; ++i)
    {
        float v = clamp(input[i] / 255.f, 0.0f, 1.0f);
        v = encode(v);
        output[i] = v;
    }
}

void convert_float_to_byte(std::vector<uint8_t> &out, const std::vector<uint8_t> &in, int count, const std::function<float(float)> &encode)
{
    out.resize(count);
    const float *input = reinterpret_cast<const float *>(&in[0]);
    for (int i = 0; i < count; ++i)
    {
        float v = input[i];
        v = encode(v);
        out[i] = static_cast<uint8_t>(clamp(v, 0.0f, 1.0f) * 255);
    }
}

void convert_float_to_float(std::vector<uint8_t> &out, const std::vector<uint8_t> &in, int count, const std::function<float(float)> &encode)
{
    out.resize(count * 4);
    const float *input = reinterpret_cast<const float *>(&in[0]);
    float *output = reinterpret_cast<float *>(&out[0]);
    for (int i = 0; i < count; ++i)
    {
        float v = input[i];
        v = encode(v);
        output[i] = v;
    }
}

void convert(std::vector<uint8_t> &out, const std::vector<uint8_t> &in, const PixelFormat &out_format, const PixelFormat &in_format, int count)
{
    std::vector<uint8_t> data;
    bool in_srgb = is_srgb_space(in_format);
    bool out_srgb = is_srgb_space(out_format);

    int in_byte_per_channel = num_byte(in_format) / num_channel(in_format);
    int out_byte_per_channel = num_byte(out_format) / num_channel(out_format);

    std::function<float(float)> encoder;
    if ((in_srgb && out_srgb) || (!in_srgb && !out_srgb))
    {
        encoder = [](float a) { return a; };
    }
    else if (in_srgb && !out_srgb)
    {
        encoder = srgb_to_linear;
    }
    else
    {
        encoder = linear_to_srgb;
    }

    std::function<void(std::vector<uint8_t> & out, const std::vector<uint8_t> &in, int count, const std::function<float(float)> &encode)> converter;
    if (in_byte_per_channel == 1)
    {
        if (out_byte_per_channel == 1)
        {
            converter = convert_byte_to_byte;
        }
        else if (out_byte_per_channel == 4)
        {
            converter = convert_byte_to_float;
        }
    }
    else if (in_byte_per_channel == 4)
    {
        if (out_byte_per_channel == 1)
        {
            converter = convert_float_to_byte;
        }
        else if (out_byte_per_channel == 4)
        {
            converter = convert_float_to_float;
        }
    }

    converter(out, in, count, encoder);
}

Image::Image(const char *file_name, const PixelFormat &pf) : _pixel_state(pf)
{
    //读取后缀
    const char *file_ext;
    const char *ptr, c = '.';
    ptr = strrchr(file_name, c);
    auto pos = ptr - file_name;
    file_ext = &file_name[pos + 1];

    if (0 == strcmp(file_ext, "png") || 0 == strcmp(file_ext, "PNG"))
    {
        _pixel_format = png::safe_format(pf);
        _pixel_state = PixelState(_pixel_format);

        LodePNGColorType PNG_format = png::inner_format(_pixel_format);
        int bitdepth = png::bitdepth(_pixel_format);

        std::vector<uint8_t> raw_data;
        uint32_t width, height;
        uint32_t error = lodepng::decode(raw_data, width, height, file_name, PNG_format, bitdepth);
        if (error)
        {
            NARUKAMI_ERROR("error code %d %s", error, lodepng_error_text(error));
            exit(-1);
        }
        _resolution[0] = width;
        _resolution[1] = height;
        convert(_data, raw_data, _pixel_format, _pixel_format, total_pixels());
    }
}

void Image::write_to_png(const char *file_name, const PixelFormat &output_format)
{
    NARUKAMI_LOG("write image from [%dx%d %s] to [%dx%d %s png] ", _resolution[0], _resolution[1], pixel_format_name(_pixel_format), _resolution[0], _resolution[1], pixel_format_name(png::safe_format(output_format)));
    std::vector<uint8_t> output;
    int width = _resolution[0];
    int height = _resolution[1];
    int num_c = num_channel(_pixel_format);
    auto png_format = png::inner_format(_pixel_format);
    auto png_bitdepth = png::bitdepth(_pixel_format);
    convert(output, _data, png::safe_format(output_format), _pixel_format, total_pixels());

    unsigned error = lodepng::encode(file_name, output, width, height, png_format, png_bitdepth);
    if (error)
    {
        NARUKAMI_ERROR("error code %d %s", error, lodepng_error_text(error));
        exit(-1);
    }
}

float Image::get_float_data(const uint8_t *address) const
{
    float data = .0f;
    if (_pixel_state.byte_per_channel == 1)
    {
        data = (*address) / 255.f;
    }
    else if (_pixel_state.byte_per_channel == 4)
    {
        auto raw_ptr_f = reinterpret_cast<const float *>(address);
        data = (*raw_ptr_f);
    }
    return data;
}

void Image::get_linear_data(float *texel, const uint8_t *address) const
{
    for (int i = 0; i < _pixel_state.channel_num; ++i)
    {
        auto new_address = address + i * _pixel_state.byte_per_channel;
        float data = get_float_data(new_address);
        if (_pixel_state.is_srgb && i != 3)
        {
            //alpha 不需要decode
            data = srgb_to_linear(data);
        }
        texel[i] = data;
    }
}

const uint8_t *Image::address(const Point2i &idx) const
{
    int width = _resolution.x;
    return _data.data() + (width * idx.y + idx.x) * _pixel_state.byte_num;
}

RGBA Image::texel(const Point2i &idx) const
{
    assert(idx.x >= 0 && idx.x < _resolution.x);
    assert(idx.y >= 0 && idx.y < _resolution.y);
    float data[4];
    get_linear_data(data, address(idx));
    return RGBA(data[0], data[1], data[2], data[3]);
}

// //使用Bilinear 来生成Mipmap
// std::vector<shared<Image>> Image::generate_mipmap(const shared<Image> &image)
// {
//     auto res = image->resolution();
//     int width = res.x;
//     int height = res.y;
//     int max_lod = log2(max(width, height));
//     std::vector<shared<Image>> mipmap;
//     return mipmap;
// }

NARUKAMI_END