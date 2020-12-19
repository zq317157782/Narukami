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
#include "core/lowdiscrepancy.h"
NARUKAMI_BEGIN

constexpr uint32_t SOBOL02_GENERATOR_MATRIX[2][32]=
{
    { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 1073741824, 2147483648 },
    { 1, 3, 5, 15, 17, 51, 85, 255, 257, 771, 1285, 3855, 4369, 13107, 21845, 65535, 65537, 196611, 327685, 983055, 1114129, 3342387, 5570645, 16711935, 16843009, 50529027, 84215045, 252645135, 286331153, 858993459, 1431655765, 4294967295 }
};

constexpr uint32_t REVERSED_SOBOL02_GENERATOR_MATRIX[2][32]=
{
    { 0x80000000, 0x40000000, 0x20000000, 0x10000000, 0x8000000, 0x4000000, 0x2000000, 0x1000000, 0x800000, 0x400000, 0x200000, 0x100000, 0x80000, 0x40000, 0x20000, 0x10000, 0x8000, 0x4000, 0x2000, 0x1000, 0x800, 0x400, 0x200, 0x100, 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1 },
    { 0x80000000, 0xc0000000, 0xa0000000, 0xf0000000, 0x88000000, 0xcc000000, 0xaa000000, 0xff000000, 0x80800000, 0xc0c00000, 0xa0a00000, 0xf0f00000, 0x88880000, 0xcccc0000, 0xaaaa0000, 0xffff0000, 0x80008000, 0xc000c000, 0xa000a000, 0xf000f000, 0x88008800, 0xcc00cc00, 0xaa00aa00, 0xff00ff00, 0x80808080, 0xc0c0c0c0, 0xa0a0a0a0, 0xf0f0f0f0, 0x88888888, 0xcccccccc, 0xaaaaaaaa, 0xffffffff }
};

// constexpr uint32_t VAN_DER_CORPUT_GENERATOR_MATRIX[32]={

// };

constexpr uint32_t REVERSED_VAN_DER_CORPUT_GENERATOR_MATRIX[32]=
{
    0b10000000000000000000000000000000,
    0b1000000000000000000000000000000,
    0b100000000000000000000000000000,
    0b10000000000000000000000000000,
    0b1000000000000000000000000000,
    0b100000000000000000000000000,
    0b10000000000000000000000000,
    0b1000000000000000000000000,
    0b100000000000000000000000,
    0b10000000000000000000000,
    0b1000000000000000000000,
    0b100000000000000000000,
    0b10000000000000000000,
    0b1000000000000000000,
    0b100000000000000000,
    0b10000000000000000,
    0b1000000000000000,
    0b100000000000000,
    0b10000000000000,
    0b1000000000000,
    0b100000000000,
    0b10000000000,
    0b1000000000,
    0b100000000,
    0b10000000,
    0b1000000,
    0b100000,
    0b10000,
    0b1000,
    0b100,
    0b10,
    0b1
};

uint32_t multiply_generator(uint32_t x, const uint32_t* M)
{
    uint32_t v=0;
    for (size_t i = 0; x!=0; x>>=1, ++i)
    {
        if (x&0x1) {
            v = v^M[i];
        }
    }
    return v;
}

inline float sample_generator_matrix(const uint32_t idx, const uint32_t* M, const uint32_t scramble)
{
    return min((multiply_generator(idx+1, M)^scramble)* 0x1p-32f, ONE_MINUS_EPSILON);
}


inline uint32_t gray_code(const uint32_t n)
{
    return (n>>1)^n;
}

inline void gray_code_sample(const uint32_t *C, uint32_t n, uint32_t scramble, float *p)
{
    uint32_t v = scramble;
    for (uint32_t i = 0; i < n; ++i) {
        p[i] = min(v * 0x1p-32f, ONE_MINUS_EPSILON);  /* 1/2^32 */
        v ^= C[ctz(i + 1)];
    }
}

inline void gray_code_sample(const uint32_t *C0, const uint32_t *C1, uint32_t n, const uint32_t scramble[2], Point2f *p)
{
    uint32_t v[2] ={scramble[0],scramble[1]};
    for (uint32_t i = 0; i < n; ++i) {
        p[i][0] = min(v[0] * 0x1p-32f, ONE_MINUS_EPSILON);
        p[i][1] = min(v[1] * 0x1p-32f, ONE_MINUS_EPSILON);

        v[0] ^= C0[ctz(i + 1)];
        v[1] ^= C1[ctz(i + 1)];
    }
}

Point2f sobol02(const uint32_t idx, const uint32_t scramble[2])
{
    auto x = sample_generator_matrix(idx, REVERSED_SOBOL02_GENERATOR_MATRIX[0], scramble[0]);
    auto y = sample_generator_matrix(idx, REVERSED_SOBOL02_GENERATOR_MATRIX[1], scramble[1]);
    return Point2f(x, y);
}

void sobol02(int sample_per_pixel, int pixel_num,Point2f *samples, RNG& rng)
{
    uint32_t total_sample_num = sample_per_pixel * pixel_num;
    const uint32_t *C0 = REVERSED_SOBOL02_GENERATOR_MATRIX[0];
    const uint32_t *C1 = REVERSED_SOBOL02_GENERATOR_MATRIX[1];
    uint32_t scramble[2] = {rng.next_uint32(),rng.next_uint32()};
    gray_code_sample(C0, C1, total_sample_num, scramble, samples);
    for(int i=0;i<pixel_num;++i)
    {
        shuffle(samples + sample_per_pixel * i,sample_per_pixel,1,rng);
    }
    shuffle(samples,pixel_num,sample_per_pixel,rng);
}

float van_der_corput(const uint32_t idx, uint32_t scramble)
{
     return sample_generator_matrix(idx, REVERSED_VAN_DER_CORPUT_GENERATOR_MATRIX,scramble);
}

void van_der_corput(int sample_per_pixel, int pixel_num, float *samples, RNG& rng)
{
    uint32_t total_sample_num = sample_per_pixel * pixel_num;
    const uint32_t *C = REVERSED_VAN_DER_CORPUT_GENERATOR_MATRIX;
    uint32_t scramble = rng.next_uint32();
    gray_code_sample(C, total_sample_num, scramble , samples);

    for(int i=0;i<pixel_num;++i)
    {
        shuffle(samples + sample_per_pixel * i,sample_per_pixel,1,rng);
    }
    shuffle(samples,pixel_num,sample_per_pixel,rng);
    
}

NARUKAMI_END