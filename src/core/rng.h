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
NARUKAMI_BEGIN

//random number generator 
//use xoroshiro128+
//http://xoshiro.di.unimi.it/xoroshiro128plus.c

static inline uint64_t rotl(const uint64_t x, int k) {
	return (x << k) | (x >> (64 - k));
}

#define DEFAULT_STATE 0x853c49e6748fea9bULL


class RNG{
private:
    uint64_t _s[2];//state


    inline uint64_t splitmix64(uint64_t seed)const{
        uint64_t z = (seed += UINT64_C(0x9E3779B97F4A7C15));
        z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
        z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
        return z ^ (z >> 31);
    }

    inline uint64_t next(){
        const uint64_t s0 = _s[0];
	    uint64_t s1 = _s[1];
	    const uint64_t result = s0 + s1;

	    s1 ^= s0;
	    _s[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16); // a, b
	    _s[1] = rotl(s1, 37); // c

	    return result;
    }
public:
    inline RNG(const uint64_t seed=DEFAULT_STATE){
        _s[0]=splitmix64(seed);
        _s[1]=0;
    }

    inline void set_seed(const uint64_t seed){
        _s[0]=splitmix64(seed);
        _s[1]=0;
    }

    inline uint32_t next_uint32(){
        uint64_t rand_num_64=next();
        uint32_t rand_num_32=(uint32_t)(rand_num_64>>32);
        return rand_num_32;
    }

    inline float next_float(){
        //constant from pbrt
        return min(ONE_MINUS_EPSILON,next_uint32()* 2.3283064365386963e-10f);
    }

    inline void state(uint64_t* s0,uint64_t* s1){
        assert(s0!=nullptr);
        assert(s1!=nullptr);
        (*s0) = _s[0];
        (*s1) = _s[1];
    }
    
};

NARUKAMI_END