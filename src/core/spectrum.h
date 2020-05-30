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
#include "core/constant.h"
#include "core/math.h"
NARUKAMI_BEGIN
    struct Color
    {
        float r,g,b;
        inline Color():r(0.0f),g(0.0f),b(0.0f){}
        inline Color(const float r,const float g,const float b):r(r),g(g),b(b){assert(!isnan(r));assert(!isnan(g));assert(!isnan(b)); }
        inline float operator[](const int idx) const{ assert(idx>=0&&idx<3); return (&r)[idx]; }
        inline float& operator[](const int idx){ assert(idx>=0&&idx<3); return (&r)[idx]; }
    };
    inline  std::ostream &operator<<(std::ostream &out, const Color &L) { out << '(' << L.r << ',' << L.g << ',' << L.b << ')'; return out; }
    inline bool operator==(const Color& a,const Color& b){ if((a.r==b.r)&&(a.g==b.g)&&(a.b==b.b)){ return true; } return false; }
    inline bool operator!=(const Color& a,const Color& b){ if((a.r!=b.r)||(a.g!=b.g)||(a.b!=b.b)){ return true; } return false; }
    
    inline Color operator+(const Color& L1,const Color& L2){ Color L; L.r = L1.r+L2.r; L.g = L1.g+L2.g; L.b = L1.b+L2.b; return L; }
    inline Color operator*(const Color& L1,const Color& L2){ Color L; L.r = L1.r*L2.r; L.g = L1.g*L2.g; L.b = L1.b*L2.b; return L; }
    inline Color operator*(const Color& L1,const float f){assert(!isnan(f));Color L; L.r = L1.r*f; L.g = L1.g*f; L.b = L1.b*f; return L; }
    inline Color operator*(const float f,const Color& L1){return L1 * f;}
    inline Color operator/(const Color& L1,const float f){assert(!isnan(f));Color L; L.r = L1.r/f; L.g = L1.g/f; L.b = L1.b/f; return L; }

    inline bool is_black(const Color& L){return L.r==0&&L.g==0&&L.b==0;}


  
NARUKAMI_END