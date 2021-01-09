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
#include "core/narukami.h"
#include "core/spectrum.h"
#include "core/cie.h"
#include "rgb2spec/rgb2spec.h"
#include <vector>

NARUKAMI_BEGIN

RGB2Spec * srgb_coeff;
static const float CIE_Y_integral = 106.856895f;

int partition_spd_by_wavelength(float *lambda, float *sample, int low, int high)
{
    float pivot = lambda[high]; //最后一个波长值作为快速排序的pivot;
    int i = low - 1;
    for (int j = low; j <= high - 1; ++j)
    {
        if (lambda[j] < pivot)
        {
            i++;
            std::swap(lambda[i], lambda[j]);
            std::swap(sample[i], sample[j]);
        }
    }
    std::swap(lambda[i + 1], lambda[high]);
    std::swap(sample[i + 1], sample[high]);

    return i + 1;
}

void quick_sort_spd_by_wavelength(float *lambda, float *sample, int low, int high)
{
    if (low < high)
    {
        int pi = partition_spd_by_wavelength(lambda, sample, low, high);
        quick_sort_spd_by_wavelength(lambda, sample, low, pi - 1);
        quick_sort_spd_by_wavelength(lambda, sample, pi + 1, high);
    }
}

bool is_sorted_spd_by_wavelength(const float *lambda, int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        if (lambda[i] >= lambda[i + 1])
        {
            return false;
        }
    }
    return true;
}

void sort_spd_by_wavelength(float *lambda, float *sample, int n)
{
    quick_sort_spd_by_wavelength(lambda, sample, 0, n - 1);
}

//计算两个波长之间的平均能量
float average_spd_sample(const float *lambda, const float *sample, int n, float lambda0, float lambda1)
{
    //处理edge case
    if (lambda1 <= lambda[0])
    {
        return sample[0];
    }
    else if (lambda0 >= lambda[n - 1])
    {
        return sample[n - 1];
    }
    else if (n == 1)
    {
        return sample[0];
    }

    float sum = 0.0f;
    //处理超出样本范围的数据
    if (lambda0 < lambda[0])
    {
        sum += sample[0] * (lambda[0] - lambda0);
    }
    if (lambda1 > lambda[n - 1])
    {
        sum += sample[n - 1] * (lambda1 - lambda[n - 1]);
    }
    //找到lambda0所在的lambda区间的索引
    //TODO 使用二分法加速
    int i = 0;
    while ((i + 1) < n)
    {
        if (lambda0 > lambda[i + 1])
        {
            i++;
        }
        else
        {
            break;
        }
    }

    auto interp = [lambda, sample](float w, int i) {
        return lerp(sample[i], sample[i + 1], w - lambda[i] / (lambda[i + 1] - lambda[i]));
    };

    //遍历所有区间，计算平均值
    for (; (i + 1) < n && lambda1 >= lambda[i]; i++)
    {
        float lambda_s = max(lambda0, lambda[i]);
        float lambda_e = min(lambda1, lambda[i + 1]);

        //取两个端点处的值的均值
        sum += (interp(lambda_s, i) + interp(lambda_e, i)) * 0.5f * (lambda_e - lambda_s);
    }

    return sum / (lambda1 - lambda0);
}

Spectrum from_sample_data(const float *lambda, float *sample, int n)
{
    if (!is_sorted_spd_by_wavelength(lambda, n))
    {
        std::vector<float> sorted_lambda(&lambda[0], &lambda[n]);
        std::vector<float> sorted_sample(&sample[0], &sample[n]);

        sort_spd_by_wavelength(&sorted_lambda[0], &sorted_sample[0], n);
        lambda = &sorted_lambda[0];
        sample = &sorted_sample[0];
    }
    Spectrum spd;
    for (int i = 0; i < SPD_SAMPLE_COUNT; ++i)
    {
        float lambda0 = lerp(WAVELENGTH_MIN, WAVELENGTH_MAX, float(i) / SPD_SAMPLE_COUNT);
        float lambda1 = lerp(WAVELENGTH_MIN, WAVELENGTH_MAX, float(i + 1) / SPD_SAMPLE_COUNT);
        spd[i] = average_spd_sample(lambda, sample, n, lambda0, lambda1);
    }
    return spd;
}

Spectrum Spectrum::X;
Spectrum Spectrum::Y;
Spectrum Spectrum::Z;
void Spectrum::init()
{
    for (int i = 0; i < SPD_SAMPLE_COUNT; ++i)
    {
#if 0
        float lambda0 = lerp(WAVELENGTH_MIN, WAVELENGTH_MAX, float(i) / float(SPD_SAMPLE_COUNT));
        float lambda1 = lerp(WAVELENGTH_MIN, WAVELENGTH_MAX, float(i + 1) / float(SPD_SAMPLE_COUNT));
        //这段代码使用的是插值的方式来算X,Y,Z的频谱样本,使用的是PBRT的方式
        X[i] = average_spd_sample(CIE_lambda, CIE_X, WAVELENGTH_COUNT, lambda0, lambda1);
        Y[i] = average_spd_sample(CIE_lambda, CIE_Y, WAVELENGTH_COUNT, lambda0, lambda1);
        Z[i] = average_spd_sample(CIE_lambda, CIE_Z, WAVELENGTH_COUNT, lambda0, lambda1);
#else
        //这里直接采样了cie1931中已经算好的样本
        X[i] = static_cast<float>(cie_x[i]);
        Y[i] = static_cast<float>(cie_y[i]);
        Z[i] = static_cast<float>(cie_z[i]);
#endif
    }
    {
        srgb_coeff = nullptr;
        srgb_coeff = rgb2spec_load("./srgb.coeff");
    }
}

void Spectrum::free()
{
    if(srgb_coeff)
    {
        rgb2spec_free(srgb_coeff);
    }
}

void from_spd_to_xyz(const Spectrum &spd, float xyz[3])
{
    float4 xxxx, yyyy, zzzz;
    for (int i = 0; i < SPD_SSE_SAMPLE_COUNT; ++i)
    {
        xxxx += spd.samples[i] * Spectrum::X.samples[i];
        yyyy += spd.samples[i] * Spectrum::Y.samples[i];
        zzzz += spd.samples[i] * Spectrum::Z.samples[i];
    }

    float x = vreduce_add(xxxx)[0];
    float y = vreduce_add(yyyy)[0];
    float z = vreduce_add(zzzz)[0];

    float scale = (WAVELENGTH_MAX - WAVELENGTH_MIN) / (CIE_Y_integral * SPD_SAMPLE_COUNT);
    xyz[0] = x * scale;
    xyz[1] = y * scale;
    xyz[2] = z * scale;
}

void from_xyz_to_srgb(const float xyz[3], float rgb[3])
{
    rgb[0] = 3.240479f * xyz[0] - 1.537150f * xyz[1] - 0.498535f * xyz[2];
    rgb[1] = -0.969256f * xyz[0] + 1.875991f * xyz[1] + 0.041556f * xyz[2];
    rgb[2] = 0.055648f * xyz[0] - 0.204043f * xyz[1] + 1.057311f * xyz[2];
}

void from_srgb_to_spd(float rgb[3],Spectrum &spd)
{
    float coeff[RGB2SPEC_N_COEFFS];
    rgb2spec_fetch(srgb_coeff,rgb,coeff);
   
    for(int i=0;i<SPD_SAMPLE_COUNT;++i)
    {
        float lambda = WAVELENGTH_MIN + i * (WAVELENGTH_MAX - WAVELENGTH_MIN)/(SPD_SAMPLE_COUNT - 1);
        spd[i] = rgb2spec_eval_precise(coeff, lambda);
    }
}

void blackbody(const float *lambda, int n, float T, float *Le)
{
    constexpr float c = 299792458.f;
    constexpr float h = 6.6260697e-34f;
    constexpr float kb = 1.3806488e-23f;
    for (int i = 0; i < n; ++i)
    {
        float l = lambda[i] * 1e-9f;
        float lambda5 = (l * l) * (l * l) * l;
        Le[i] = (2 * h * c * c) / (lambda5 * (exp((h * c) / (l * kb * T)) - 1.0f));
    }
}

void blackbody_normalized(const float *lambda, int n, float T, float *Le)
{
    blackbody(lambda, n, T, Le);
    float max_lambda = 2.8977721e-3f / T * 1e9f; //转换到nm
    float max_Le;
    blackbody(&max_lambda, 1, T, &max_Le);
    for (int i = 0; i < n; ++i)
    {
        Le[i] /= max_Le;
    }
}

Spectrum tungsten_lamp_2700k(float intensity)
{
    float Le[CIE_SAMPLES];
    blackbody_normalized(cie_lambda, CIE_SAMPLES, 2700, Le);
    return from_sample_data(cie_lambda, Le, CIE_SAMPLES) * intensity;
}

Spectrum tungsten_lamp_3000k(float intensity)
{
    float Le[CIE_SAMPLES];
    blackbody_normalized(cie_lambda, CIE_SAMPLES, 3000, Le);
    return from_sample_data(cie_lambda, Le, CIE_SAMPLES) * intensity;
}

NARUKAMI_END