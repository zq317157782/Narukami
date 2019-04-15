
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
#include "core/platform.h"
#include "core/narukami.h"
NARUKAMI_BEGIN
static   MAYBE_UNUSED const float MIN_RCP_INPUT = 1E-18f;
static   MAYBE_UNUSED constexpr float EPSION = std::numeric_limits<float>::epsilon();
struct TrueType {
    FINLINE operator bool() const{return true;}
} ;
extern MAYBE_UNUSED const TrueType True;

struct FalseType {
    FINLINE operator bool() const{return true;}
} ;
extern MAYBE_UNUSED const FalseType False;

struct ZeroType {
    FINLINE operator float() const{return 0.0f;}
    FINLINE operator int() const{return 0;}
} ;
extern MAYBE_UNUSED const ZeroType Zero;


struct OneType {
    FINLINE operator float() const{return 1.0f;}
    FINLINE operator int() const{return 1;}
} ;
extern MAYBE_UNUSED const OneType One;

struct NegOneType {
    FINLINE operator float() const{return -1.0f;}
    FINLINE operator int() const{return -1;}
} ;
extern MAYBE_UNUSED const NegOneType NegOne;

struct InfiniteType{
    FINLINE operator float() const{return std::numeric_limits<float>::infinity();}
};
extern MAYBE_UNUSED const InfiniteType Infinite;
NARUKAMI_END