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
#include "core/math.h"
#include "core/affine.h"

NARUKAMI_BEGIN
    
    constexpr uint32_t SOBOL02_GENERATOR_MATRIX[2][32]={
        {1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072,262144,524288,1048576,2097152,4194304,8388608,16777216,33554432,67108864,134217728,268435456,536870912,1073741824,2147483648},
        {1,3,5,15,17,51,85,255,257,771,1285,3855,4369,13107,21845,65535,65537,196611,327685,983055,1114129,3342387,5570645,16711935,16843009,50529027,84215045,252645135,286331153,858993459,1431655765,4294967295}
    };

    constexpr uint32_t REVERSED_SOBOL02_GENERATOR_MATRIX[2][32]={
        {0x80000000, 0x40000000, 0x20000000, 0x10000000, 0x8000000, 0x4000000, 0x2000000, 0x1000000, 0x800000, 0x400000, 0x200000, 0x100000, 0x80000, 0x40000, 0x20000, 0x10000, 0x8000, 0x4000, 0x2000, 0x1000, 0x800, 0x400, 0x200, 0x100, 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1},
        {0x80000000, 0xc0000000, 0xa0000000, 0xf0000000, 0x88000000, 0xcc000000, 0xaa000000, 0xff000000, 0x80800000, 0xc0c00000, 0xa0a00000, 0xf0f00000, 0x88880000, 0xcccc0000, 0xaaaa0000, 0xffff0000, 0x80008000, 0xc000c000, 0xa000a000, 0xf000f000, 0x88008800, 0xcc00cc00, 0xaa00aa00, 0xff00ff00, 0x80808080, 0xc0c0c0c0, 0xa0a0a0a0, 0xf0f0f0f0, 0x88888888, 0xcccccccc, 0xaaaaaaaa, 0xffffffff}
    };

    // constexpr uint32_t VAN_DER_CORPUT_GENERATOR_MATRIX[32]={

    // };

    constexpr uint32_t REVERSED_VAN_DER_CORPUT_GENERATOR_MATRIX[32]={
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

    template<uint32_t base> float radical_inverse_u32(uint32_t x){
         uint32_t reverse = 0;
         float inv_base = 1.0f/base;
         float inv_baseN=1.0f;
         while(x){
             // int op
             uint32_t next =x/base;
             uint32_t digit = x - next*base;
             reverse = reverse*base + digit;
             x=next;
             //float op
             inv_baseN*=inv_base;
         }
        //combinition
        return min(reverse*inv_baseN,ONE_MINUS_EPSILON);
    }


    template<> FINLINE float radical_inverse_u32<2>(uint32_t x){
        return reverse_bits_u32(x) * 0x1p-32f;
    }

    template<uint32_t base> float scrambled_radical_inverse_u32(uint32_t x,const uint32_t*permution){
         assert(permution[base-1]<base);
         uint32_t reverse = 0;
         float inv_base = 1.0f/base;
         float inv_baseN=1.0f;
         while(x){
             // int op
             uint32_t next =x/base;
             uint32_t digit = x - next*base;
             reverse = reverse*base + permution[digit];
             x=next;
             //float op
             inv_baseN*=inv_base;
         }
        //combinition
        //geometry series => permutation[0]*base^-1 + permutation[0]*base^-2+permutation[0]*base^-3 +...
        //geometry series sum = ( permutation[0]*base^-1 )/( 1-base^-1 )
        return min((reverse+permution[0]*inv_base/(1-inv_base))*inv_baseN,ONE_MINUS_EPSILON);
    }
    

    //scramble after reverse 
    FINLINE float scrambled_radical_inverse_u32_base2(uint32_t x,const uint32_t scramble){
         return (reverse_bits_u32(x)^scramble) * 0x1p-32f;
    }

    uint32_t sobol_multi_generator_matrix(uint32_t x,const uint32_t* M);
    
    FINLINE float sample_sobol(const uint32_t idx,const uint32_t* M){
        return sobol_multi_generator_matrix(idx+1,M) * 0x1p-32f;
    }

    FINLINE float sample_scrambled_sobol(const uint32_t idx,const uint32_t* M,const uint32_t scramble){
        return (sobol_multi_generator_matrix(idx+1,M)^scramble)* 0x1p-32f;
    }


    Point2f sample_sobol02(const uint32_t idx);
    Point2f sample_scrambled_sobol02(const uint32_t idx,const uint32_t scramble_x,const uint32_t scramble_y);


    FINLINE uint32_t gray_code(const uint32_t n){
        return (n>>1)^n;
    }

    FINLINE Point2f sample_scrambled_gray_code_sobol02(const uint32_t idx,uint32_t* scramble_x,uint32_t* scramble_y){
        assert(scramble_x!=nullptr);
        assert(scramble_y!=nullptr);
        uint32_t col = count_trailing_zero(idx+1);
        (*scramble_x)=(*scramble_x)^REVERSED_SOBOL02_GENERATOR_MATRIX[0][col];
        (*scramble_y)=(*scramble_y)^REVERSED_SOBOL02_GENERATOR_MATRIX[1][col];

        float x = (*scramble_x)* 0x1p-32f;
        float y = (*scramble_y)* 0x1p-32f;

        return Point2f(x,y);
    }

    
    FINLINE float sample_scrambled_gray_code_van_der_corput(const uint32_t idx,uint32_t* scramble){
        assert(scramble!=nullptr);
        uint32_t col = count_trailing_zero(idx+1);
        (*scramble)=(*scramble)^REVERSED_VAN_DER_CORPUT_GENERATOR_MATRIX[col];
        return (*scramble) * 0x1p-32f;
    }

NARUKAMI_END