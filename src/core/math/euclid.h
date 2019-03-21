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
#include "core/math/math.h"
#include "core/math/vector3.h"
#include "core/math/point3.h"
NARUKAMI_BEGIN
template <typename T>
FINLINE Point3<T> operator+(const Point3<T> &p, const Vector3<T> &v) { Point3<T> rp; rp.x = p.x + v.x; rp.y = p.y + v.y; rp.z = p.z + v.z; return rp; }
template <typename T>
FINLINE Point3<T> operator-(const Point3<T> &p, const Vector3<T> &v) {return p+(-v);}
template <typename T>
FINLINE Vector3<T> operator-(const Point3<T> &p1, const Point3<T> &p2) { Vector3<T> v; p.x = p1.x - p2.x; v.y = p1.y - p2.y; v.z = p1.z - p2.z; return v; }


FINLINE SSEVector3f operator+(const SSEPoint3f&  p ,const SSEVector3f& v ){ return _mm_add_ps(p,v); }
FINLINE SSEVector3f operator-(const SSEPoint3f&  p ,const SSEVector3f& v ){ return _mm_sub_ps(p,v); }
FINLINE SSEVector3f operator-(const SSEPoint3f& p1,const SSEPoint3f& p2){ return _mm_sub_ps(p1,p2); }

NARUKAMI_END
