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
#include "core/math.h"

NARUKAMI_BEGIN

class Vector3f{
public:
    float x,y,z;
public:
    Vector3f():x(0),y(0),z(0){
    }

    explicit Vector3f(const float a):x(a),y(a),z(a){
        assert(!isnan(a));
    }

    Vector3f(const float a,const float b,const float c):x(a),y(b),z(c){
        assert(!isnan(a));
        assert(!isnan(b));
        assert(!isnan(c));
    }
    //just for checking assert for debug
#ifdef NARUKAMI_DEBUG
    Vector3f(const Vector3f& v1){
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        assert(!isnan(v1.z));
        x=v1.x;
        y=v1.y;
        z=v1.z;
    }

    Vector3f& operator=(const Vector3f& v1){
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        assert(!isnan(v1.z));
        x=v1.x;
        y=v1.y;
        z=v1.z;
        return (*this);
    }
#endif
    
    Vector3f operator+(const Vector3f& v1) const{
        Vector3f v;
        v.x=x+v1.x;
        v.y=y+v1.y;
        v.z=z+v1.z;
        return v;
    }

    Vector3f operator-(const Vector3f& v1) const{
        Vector3f v;
        v.x=x-v1.x;
        v.y=y-v1.y;
        v.z=z-v1.z;
        return v;
    }

    //compenont wise
    Vector3f operator*(const Vector3f& v1) const{
        Vector3f v;
        v.x=x*v1.x;
        v.y=y*v1.y;
        v.z=z*v1.z;
        return v;
    }

    Vector3f operator*(const float f) const{
        Vector3f v;
        v.x=x*f;
        v.y=y*f;
        v.z=z*f;
        return v;
    }
    
    Vector3f operator/(const float f) const{
        assert(f!=0);
        Vector3f v;
        v.x=x/f;
        v.y=y/f;
        v.z=z/f;
        return v;
    }
    
    Vector3f& operator+=(const Vector3f& v1){
        x+=v1.x;
        y+=v1.y;
        z+=v1.z;
        return (*this);
    }

    Vector3f& operator-=(const Vector3f& v1){
        x-=v1.x;
        y-=v1.y;
        z-=v1.z;
        return (*this);
    }

    Vector3f& operator*=(const Vector3f& v1){
        x*=v1.x;
        y*=v1.y;
        z*=v1.z;
        return (*this);
    }

    Vector3f& operator*=(const float f){
        x*=f;
        y*=f;
        z*=f;
        return (*this);
    }

    Vector3f& operator/=(const float f){
        assert(f!=0);
        x/=f;
        y/=f;
        z/=f;
        return (*this);
    }

    bool operator==(const Vector3f& v1) const{
        if((x==v1.x)&&(y==v1.y)&&(z==v1.z)){
            return true;
        }
        return false;
    }
};

NARUKAMI_END