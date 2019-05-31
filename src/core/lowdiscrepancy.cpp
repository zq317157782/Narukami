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
    uint32_t sobol_multi_generator_matrix(uint32_t x,const uint32_t* M){
        uint32_t v=0;
        for (size_t i = 0; x!=0; x>>=1,++i)
        {
            if(x&0x1){
                v = v^M[i];
            }
        }
        return v;
    }


    Point2f get_sobol02_sample(const uint32_t idx){
        auto x = sobol_multi_generator_matrix(idx+1,&REVERSED_SOBOL02_GENERATOR_MATRIX[0][0])* 0x1p-32f;
        auto y = sobol_multi_generator_matrix(idx+1,&REVERSED_SOBOL02_GENERATOR_MATRIX[1][0])* 0x1p-32f;
        return Point2f(x,y);
    }

NARUKAMI_END