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
template <typename T>
class Vector3{
public:
    T x,y,z;
public:
    FINLINE Vector3():x(0),y(0),z(0){
    }

    FINLINE explicit Vector3(const float a):x(a),y(a),z(a){
        assert(!isnan(a));
    }

    FINLINE Vector3(const T& a,const T& b,const T& c):x(a),y(b),z(c){
        assert(!isnan(a));
        assert(!isnan(b));
        assert(!isnan(c));
    }
    //just for checking assert for debug
#ifdef NARUKAMI_DEBUG
    FINLINE Vector3(const Vector3& v1){
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        assert(!isnan(v1.z));
        x=v1.x;
        y=v1.y;
        z=v1.z;
    }

    FINLINE Vector3& operator=(const Vector3& v1){
        assert(!isnan(v1.x));
        assert(!isnan(v1.y));
        assert(!isnan(v1.z));
        x=v1.x;
        y=v1.y;
        z=v1.z;
        return (*this);
    }
#endif
    
    FINLINE Vector3 operator+(const Vector3& v1) const{
        Vector3 v;
        v.x=x+v1.x;
        v.y=y+v1.y;
        v.z=z+v1.z;
        return v;
    }

    FINLINE Vector3 operator-(const Vector3& v1) const{
        Vector3 v;
        v.x=x-v1.x;
        v.y=y-v1.y;
        v.z=z-v1.z;
        return v;
    }

    //compenont wise
    FINLINE Vector3 operator*(const Vector3& v1) const{
        Vector3 v;
        v.x=x*v1.x;
        v.y=y*v1.y;
        v.z=z*v1.z;
        return v;
    }

    FINLINE Vector3 operator*(const T& f) const{
        Vector3 v;
        v.x=x*f;
        v.y=y*f;
        v.z=z*f;
        return v;
    }
    
    FINLINE Vector3 operator/(const T& f) const{
        assert(f!=0);
        Vector3 v;
        v.x=x/f;
        v.y=y/f;
        v.z=z/f;
        return v;
    }
    
    FINLINE Vector3& operator+=(const Vector3& v1){
        x+=v1.x;
        y+=v1.y;
        z+=v1.z;
        return (*this);
    }

    FINLINE Vector3& operator-=(const Vector3& v1){
        x-=v1.x;
        y-=v1.y;
        z-=v1.z;
        return (*this);
    }

    FINLINE Vector3& operator*=(const Vector3& v1){
        x*=v1.x;
        y*=v1.y;
        z*=v1.z;
        return (*this);
    }

    FINLINE Vector3& operator*=(const T& f){
        x*=f;
        y*=f;
        z*=f;
        return (*this);
    }

    FINLINE Vector3& operator/=(const T& f){
        assert(f!=0);
        x/=f;
        y/=f;
        z/=f;
        return (*this);
    }

    FINLINE bool operator==(const Vector3& v1) const{
        if((x==v1.x)&&(y==v1.y)&&(z==v1.z)){
            return true;
        }
        return false;
    }

    FINLINE bool operator!=(const Vector3& v1) const{
        if((x!=v1.x)||(y!=v1.y)||(z!=v1.z)){
            return true;
        }
        return false;
    }

    FINLINE friend std::ostream& operator<<(std::ostream& out,const Vector3& v){
        out<<'('<<v.x<<','<<v.y<<','<<v.z<<')';
        return out;
    }
};

typedef Vector3<float> Vector3f;

NARUKAMI_END