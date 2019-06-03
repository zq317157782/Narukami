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
#include "core/affine.h"
#include "core/rng.h"
#include "core/lowdiscrepancy.h"
#include <vector>
NARUKAMI_BEGIN
    class Sampler{
        private:
            uint64_t _current_sample_index;
            Point2i  _current_pixel;
            std::vector<uint32_t> _scramble_1d;
            std::vector<uint32_t> _scramble_2d;
            uint32_t _sample_1d_offset;
            uint32_t _sample_2d_offset;

            const uint32_t _max_dim;
            RNG _rng;
        public:
            Sampler(const uint32_t max_dim=5):_max_dim(max_dim){
                _scramble_1d=std::vector<uint32_t>(_max_dim);
                _scramble_2d=std::vector<uint32_t>(_max_dim*2);

            }

            void start_pixel(const Point2i& p){
                _current_pixel=p;
                _current_sample_index=0;
                _sample_1d_offset = 0;
                _sample_2d_offset = 0;
                //generate all scramble number
                for (size_t i = 0; i < _max_dim; i++)
                {
                   _scramble_1d[i]=_rng.next_uint32();
                   _scramble_2d[i*2]=_rng.next_uint32();
                   _scramble_2d[i*2+1]=_rng.next_uint32();
                }
            }

            bool start_next_sample(){
                _current_sample_index++;
                _sample_1d_offset = 0;
                _sample_2d_offset = 0;
                return true;
            }


            Point2f get_2D(){
                if(EXPECT_TAKEN(_sample_2d_offset<_max_dim)){
                    auto sample= sample_scrambled_gray_code_sobol02(_current_sample_index,&_scramble_2d[_sample_2d_offset*2],&_scramble_2d[_sample_2d_offset*2+1]);
                    _sample_2d_offset++;
                    return sample;
                }
                else{
                    auto sample= Point2f(_rng.next_float(),_rng.next_float());
                    _sample_2d_offset++;
                    return sample;
                }
                 
            }
    };
NARUKAMI_END