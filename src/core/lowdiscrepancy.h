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

#include "narukami.h"

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


    template<> FINLINE float radical_inverse_u32<2>(uint32_t x){
        return reverse_bits_u32(x) * 0x1p-32;
    }

NARUKAMI_END