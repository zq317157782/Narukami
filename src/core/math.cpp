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
#include "math.h"

NARUKAMI_BEGIN
uint32_t reverse_bits_u32(uint32_t x){ 
    x=(x<<16)|(x>>16);
    x=((x&0x00ff00ff)<<8)|((x&0xff00ff00)>>8);
    x=((x&0x0f0f0f0f)<<4)|((x&0xf0f0f0f0)>>4);
    x=((x&0x33333333)<<2)|((x&0xcccccccc)>>2);
    x=((x&0x55555555)<<1)|((x&0xaaaaaaaa)>>1);
    return x;
}

NARUKAMI_END