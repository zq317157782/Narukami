
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
#include "core/sampling.h"
#include "lodepng.h"
#include <functional>
NARUKAMI_BEGIN
Image::Image(uint8_t *data, const Point2i &resolution, const PixelFormat &pf) : _resolution(resolution), _pixel_format(pf), _pixel_state(pf)
{
    _data.resize(total_bytes());
    if (data)
    {
        memcpy(&_data[0], data, total_bytes());
    }
    else
    {
        memset(&_data[0], 0, total_bytes());
    }
}

namespace png
{

    //     LCT_GREY = 0,       /*greyscale: 1,2,4,8,16 bit*/
    //     LCT_RGB = 2,        /*RGB: 8,16 bit*/
    //     LCT_PALETTE = 3,    /*palette: 1,2,4,8 bit*/
    //     LCT_GREY_ALPHA = 4, /*greyscale with alpha: 8,16 bit*/
    //     LCT_RGBA = 6        /*RGB with alpha: 8,16 bit*/
    LodePNGColorType raw_format(const PixelFormat &pf)
    {
        switch (pf)
        {
        case PixelFormat::R8:
            return LCT_GREY;
        case PixelFormat::RGB8:
        case PixelFormat::sRGB8:
            return LCT_RGB;
            break;
        case PixelFormat::RGBA8:
        case PixelFormat::sRGBA8:
        case PixelFormat::RGBA32:
            return LCT_RGBA;
            break;
        }
        return LCT_RGBA;
    }

    int bit_depth(const PixelFormat &pf)
    {
        switch (pf)
        {
        case PixelFormat::R8:
        case PixelFormat::RGB8:
        case PixelFormat::sRGB8:
        case PixelFormat::RGBA8:
        case PixelFormat::sRGBA8:
        case PixelFormat::RGBA32: //TODO 16bit is better
            return 8;
            break;
        }
        return 8;
    }

    PixelFormat compatible_format(const PixelFormat &pf)
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
    return num_byte(_pixel_format) * total_pixels();
}

int Image::total_channels() const
{
    return num_channel(_pixel_format) * total_pixels();
}

int Image::total_pixels() const
{
    return _resolution[0] * _resolution[1];
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

bool is_alpha_channel(int32_t idx)
{
    return (idx % 4) == 3;
}

void convert_byte_to_byte(std::vector<uint8_t> &out, int out_channel_num, const std::vector<uint8_t> &in, int in_channel_num, int pixel_num, const std::function<float(float)> &encode)
{
    out.resize(pixel_num * out_channel_num);
    const uint8_t *input = &in[0];
    for (int pid = 0; pid < pixel_num; ++pid)
    {
        for (int cid = 0; cid < out_channel_num; ++cid)
        {
            int in_id = pid * in_channel_num + cid;
            int out_id = pid * out_channel_num + cid;

            float v = clamp(input[in_id] / 255.f, 0.0f, 1.0f);
            if (!is_alpha_channel(cid))
            {
                v = encode(v);
            }
            out[out_id] = static_cast<uint8_t>(clamp(v, 0.0f, 1.0f) * 255);
        }
    }
}

void convert_byte_to_float(std::vector<uint8_t> &out, int out_channel_num, const std::vector<uint8_t> &in, int in_channel_num, int pixel_num, const std::function<float(float)> &encode)
{
    out.resize(pixel_num * out_channel_num * 4);
    const uint8_t *input = &in[0];
    float *output = reinterpret_cast<float *>(&out[0]);
    for (int pid = 0; pid < pixel_num; ++pid)
    {
        for (int cid = 0; cid < out_channel_num; ++cid)
        {
            int in_id = pid * in_channel_num + cid;
            int out_id = pid * out_channel_num + cid;

            float v = clamp(input[in_id] / 255.f, 0.0f, 1.0f);
            if (!is_alpha_channel(cid))
            {
                v = encode(v);
            }
            output[out_id] = v;
        }
    }
}

void convert_float_to_byte(std::vector<uint8_t> &out, int out_channel_num, const std::vector<uint8_t> &in, int in_channel_num, int pixel_num, const std::function<float(float)> &encode)
{
    out.resize(pixel_num * out_channel_num);
    const float *input = reinterpret_cast<const float *>(&in[0]);
    for (int pid = 0; pid < pixel_num; ++pid)
    {
        for (int cid = 0; cid < out_channel_num; ++cid)
        {
            int in_id = pid * in_channel_num + cid;
            int out_id = pid * out_channel_num + cid;

            float v = input[in_id];
            if (!is_alpha_channel(cid))
            {
                v = encode(v);
            }
            out[out_id] = static_cast<uint8_t>(clamp(v, 0.0f, 1.0f) * 255);
        }
    }
}

void convert_float_to_float(std::vector<uint8_t> &out, int out_channel_num, const std::vector<uint8_t> &in, int in_channel_num, int pixel_num, const std::function<float(float)> &encode)
{
    out.resize(pixel_num * out_channel_num);
    const float *input = reinterpret_cast<const float *>(&in[0]);
    float *output = reinterpret_cast<float *>(&out[0]);
    for (int pid = 0; pid < pixel_num; ++pid)
    {
        for (int cid = 0; cid < out_channel_num; ++cid)
        {
            int in_id = pid * in_channel_num + cid;
            int out_id = pid * out_channel_num + cid;

            float v = input[in_id];
            if (!is_alpha_channel(cid))
            {
                v = encode(v);
            }
            output[out_id] = v;
        }
    }
}

void convert(std::vector<uint8_t> &out, const std::vector<uint8_t> &in, const PixelFormat &out_format, const PixelFormat &in_format, int pixel_num)
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

    std::function<void(std::vector<uint8_t> &, int, const std::vector<uint8_t> &, int, int, const std::function<float(float)> &encode)> converter;
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

    converter(out, num_channel(out_format), in, num_channel(in_format), pixel_num, encoder);
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
        _pixel_format = png::compatible_format(pf);
        _pixel_state = PixelState(_pixel_format);

        LodePNGColorType PNG_format = png::raw_format(_pixel_format);
        int bit_depth = png::bit_depth(_pixel_format);

        std::vector<uint8_t> raw_data;
        uint32_t width, height;
        uint32_t error = lodepng::decode(raw_data, width, height, file_name, PNG_format, bit_depth);
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
    PixelFormat compatible_output_format = png::compatible_format(output_format);
    NARUKAMI_LOG("write image from [%dx%d %s] to [%dx%d %s png] ", _resolution[0], _resolution[1], pixel_format_name(_pixel_format), _resolution[0], _resolution[1], pixel_format_name(compatible_output_format));
    std::vector<uint8_t> output;
    int width = _resolution[0];
    int height = _resolution[1];
    auto png_format = png::raw_format(compatible_output_format);
    auto png_bitdepth = png::bit_depth(compatible_output_format);
    convert(output, _data, compatible_output_format, _pixel_format, total_pixels());

    unsigned error = lodepng::encode(file_name, output, width, height, png_format, png_bitdepth);
    if (error)
    {
        NARUKAMI_ERROR("error code %d %s", error, lodepng_error_text(error));
        exit(-1);
    }
}

float Image::get_data(const uint8_t *address) const
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

void Image::set_data(uint8_t *address, float value)
{
    if (_pixel_state.byte_per_channel == 1)
    {
        (*address) = static_cast<uint8_t>(value * 255);
    }
    else if (_pixel_state.byte_per_channel == 4)
    {
        auto raw_ptr_f = reinterpret_cast<float *>(address);
        (*raw_ptr_f) = value;
    }
}

void Image::get_linear_data(const uint8_t *address, float *texel) const
{
    for (int i = 0; i < _pixel_state.channel_num; ++i)
    {
        auto new_address = address + i * _pixel_state.byte_per_channel;
        float data = get_data(new_address);
        if (_pixel_state.is_srgb && !is_alpha_channel(i))
        {
            //alpha 不需要decode
            data = srgb_to_linear(data);
        }
        texel[i] = data;
    }
}

void Image::set_linear_data(uint8_t *address, const float *texel)
{
    for (int i = 0; i < _pixel_state.channel_num; ++i)
    {
        auto new_address = address + i * _pixel_state.byte_per_channel;
        float data = texel[i];
        if (_pixel_state.is_srgb && !is_alpha_channel(i))
        {
            data = linear_to_srgb(data);
        }
        set_data(new_address, data);
    }
}

uint8_t *Image::address(const Point2i &idx)
{
    int width = _resolution.x;
    return &_data[0] + (width * idx.y + idx.x) * _pixel_state.byte_num;
}

const uint8_t *Image::address(const Point2i &idx) const
{
    int width = _resolution.x;
    return &_data[0] + (width * idx.y + idx.x) * _pixel_state.byte_num;
}

RGBA Image::get_texel(const Point2i &idx) const
{
    assert(idx.x >= 0 && idx.x < _resolution.x);
    assert(idx.y >= 0 && idx.y < _resolution.y);
    float data[4];
    get_linear_data(address(idx), data);
    return RGBA(data[0], data[1], data[2], data[3]);
}

void Image::set_texel(const Point2i &p, const RGBA &rgba)
{
    assert(idx.x >= 0 && idx.x < _resolution.x);
    assert(idx.y >= 0 && idx.y < _resolution.y);
    float data[4] = {rgba[0], rgba[1], rgba[2], rgba[3]};
    set_linear_data(address(p), data);
}

//from PBRT
struct ResampleWeight
{
    int32_t first_texel;
    float weights[4];
};

// unique<ResampleWeight[]> resample_weights(int32_t old_res, int32_t new_res)
// {
//     assert(new_res > old_res);
//     unique<ResampleWeight[]> wt(new ResampleWeight[new_res]);
//     float filter_width = 2.0f;
//     for (int i = 0; i < new_res; ++i)
//     {
//         //连续空间 old
//         float center = (i + 0.5f) * old_res / new_res;
//         wt[i].first_texel = static_cast<int>(floor(center - filter_width + 0.5f));
//         for (int j = 0; j < 4; ++j)
//         {
//             auto pos = wt[i].first_texel + j + 0.5f;
//             wt[i].weights[j] = gaussian(center - pos, filter_width, 1.0f);
//         }
//         //标准化
//         float inv_sum = 1.0f / ((wt[i].weights[0] + wt[i].weights[1]) + (wt[i].weights[2] + wt[i].weights[3]));
//         for (int j = 0; j < 4; ++j)
//         {
//             wt[i].weights[j] *= inv_sum;
//         }
//     }
//     return wt;
// }
// // //使用Bilinear 来生成Mipmap
// std::vector<shared<Image>> Image::generate_mipmap(const shared<Image> &image)
// {

//     auto res = image->resolution();
//     Point2i res_pow2 = Point2i(round_up_pow2(res.x), round_up_pow2(res.y));

//     if (res_pow2 != res)
//     {
//         //需要先从npt转化到pt
//         auto resample_weights_x = resample_weights(res.x,res_pow2.x);
//         auto resample_weights_y = resample_weights(res.y,res_pow2.y);
//     }

//     int last_width = res.x;
//     int last_height = res.y;
//     int max_mip = log2(max(last_width, last_height));
//     std::vector<shared<Image>> mipmap;
//     mipmap.push_back(image);
//     for (int mip = 1; mip < max_mip; ++mip)
//     {
//         float w0x = 0.0f, w1x = 0.0f, w2x = 0.0f;
//         // if ((last_width & 1) == 1)
//         // {
//         //     w0x =
//         // }
//     }

//     return mipmap;
// }

NARUKAMI_END