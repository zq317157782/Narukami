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
#include "core/geometry.h"
#include "core/rng.h"

NARUKAMI_BEGIN
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


    template<> inline float radical_inverse_u32<2>(uint32_t x){
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
    inline float scrambled_radical_inverse_u32_base2(uint32_t x,const uint32_t scramble){
         return (reverse_bits_u32(x)^scramble) * 0x1p-32f;
    }

    template<typename T>
    void shuffle(T* sampes,int count,int num_dim,RNG&rng)
    {
        for(int i=0;i<count;++i)
        {
            int other = i + rng.next_uint32(count-i);
            for(int d=0;d<num_dim;++d)
            {
                std::swap(sampes[num_dim * i + d],sampes[num_dim * other + d]);
            }
        }
    }

    Point2f sobol02(const uint32_t idx,const uint32_t scramble[2]);
    void sobol02(int sample_per_pixel, int pixel_num,Point2f *samples,RNG& rng);

    float van_der_corput(const uint32_t idx,uint32_t scramble);
    void van_der_corput(int sample_per_pixel, int pixel_num,float *samples, RNG& rng);
   
NARUKAMI_END