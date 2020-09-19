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
#include "core/simd.h"
NARUKAMI_BEGIN
//---MATRIX4X4 BEGIN---
struct SSE_ALIGNAS Matrix4x4
{
public:
    union {
        float m[16];
        float mn[4][4];
        float4 col[4];
    };
public:
    inline Matrix4x4()
    {
        //col 0
        m[0] = 1.0f;
        m[1] = 0.0f;
        m[2] = 0.0f;
        m[3] = 0.0f;
        //col 1
        m[4] = 0.0f;
        m[5] = 1.0f;
        m[6] = 0.0f;
        m[7] = 0.0f;
        //col 2
        m[8] = 0.0f;
        m[9] = 0.0f;
        m[10] = 1.0f;
        m[11] = 0.0f;
        //col 3
        m[12] = 0.0f;
        m[13] = 0.0f;
        m[14] = 0.0f;
        m[15] = 1.0f;
    }
    inline Matrix4x4(const float m0, const float m1, const float m2, const float m3, const float m4, const float m5, const float m6, const float m7, const float m8, const float m9, const float m10, const float m11, const float m12, const float m13, const float m14, const float m15)
    {
        //col 0
        m[0] = m0;
        m[1] = m1;
        m[2] = m2;
        m[3] = m3;
        //col 1
        m[4] = m4;
        m[5] = m5;
        m[6] = m6;
        m[7] = m7;
        //col 2
        m[8] = m8;
        m[9] = m9;
        m[10] = m10;
        m[11] = m11;
        //col 3
        m[12] = m12;
        m[13] = m13;
        m[14] = m14;
        m[15] = m15;
    }

    inline Matrix4x4(const float *v)
    {
        //col 0
        m[0] = v[0];
        m[1] = v[1];
        m[2] = v[2];
        m[3] = v[3];
        //col 1
        m[4] = v[4];
        m[5] = v[5];
        m[6] = v[6];
        m[7] = v[7];
        //col 2
        m[8] = v[8];
        m[9] = v[9];
        m[10] = v[10];
        m[11] = v[11];
        //col 3
        m[12] = v[12];
        m[13] = v[13];
        m[14] = v[14];
        m[15] = v[15];
    }
    inline Matrix4x4(const float4 &col0, const float4 &col1, const float4 &col2, const float4 &col3)
    {
        col[0] = col0;
        col[1] = col1;
        col[2] = col2;
        col[3] = col3;
    }

    inline const float &operator[](const int idx) const
    {
        assert(idx >= 0 && idx < 16);
        return m[idx];
    }
    inline float &operator[](const int idx)
    {
        assert(idx >= 0 && idx < 16);
        return m[idx];
    }

    inline float element(int row,int col) const
    {
        return mn[col][row];
    }
};

inline void store(const Matrix4x4 &m, float *array)
{
    memcpy(array, m.m, 16 * sizeof(float));
}

inline std::ostream &operator<<(std::ostream &out, const Matrix4x4 &v)
{
    out << '(';
    for (int i = 0; i < 16; ++i)
    {
        out << v.m[i];
        out << (((i + 1) == 16) ? ')' : ',');
    }
    return out;
}

inline bool operator==(const Matrix4x4 &A, const Matrix4x4 &B)
{

    auto bool_i0 = A.col[0] == B.col[0];
    auto bool_i1 = A.col[1] == B.col[1];
    auto bool_i2 = A.col[2] == B.col[2];
    auto bool_i3 = A.col[3] == B.col[3];

    return all((bool_i0 & bool_i1) & (bool_i2 & bool_i3));
}

inline bool operator!=(const Matrix4x4 &A, const Matrix4x4 &B)
{

    auto bool_i0 = A.col[0] == B.col[0];
    auto bool_i1 = A.col[1] == B.col[1];
    auto bool_i2 = A.col[2] == B.col[2];
    auto bool_i3 = A.col[3] == B.col[3];

    return not_all((bool_i0 & bool_i1) & (bool_i2 & bool_i3));
}

inline Matrix4x4 operator+(const Matrix4x4 &A, const Matrix4x4 &B)
{
    Matrix4x4 r;
    r.col[0] = A.col[0] + B.col[0];
    r.col[1] = A.col[1] + B.col[1];
    r.col[2] = A.col[2] + B.col[2];
    r.col[3] = A.col[3] + B.col[3];
    return r;
}

inline Matrix4x4 operator*(const Matrix4x4 &A, const Matrix4x4 &B)
{

    Matrix4x4 ret;

    float4 r = A.col[0] * B.m[0];
    r += A.col[1] * B.m[1];
    r += A.col[2] * B.m[2];
    r += A.col[3] * B.m[3];
    ret.col[0] = r.xyzw;

    r = A.col[0] * B.m[4];
    r += A.col[1] * B.m[5];
    r += A.col[2] * B.m[6];
    r += A.col[3] * B.m[7];
    ret.col[1] = r.xyzw;

    r = A.col[0] * B.m[8];
    r += A.col[1] * B.m[9];
    r += A.col[2] * B.m[10];
    r += A.col[3] * B.m[11];
    ret.col[2] = r.xyzw;

    r = A.col[0] * B.m[12];
    r += A.col[1] * B.m[13];
    r += A.col[2] * B.m[14];
    r += A.col[3] * B.m[15];
    ret.col[3] = r.xyzw;

    return ret;
}

//https://lxjk.github.io/2017/09/03/Fast-4x4-Matrix-Inverse-with-SSE-SIMD-Explained.html#_appendix_2
inline Matrix4x4 transform_inverse_noscale(const Matrix4x4 &mat)
{
    Matrix4x4 r;
    //transpose left-upper 3x3
    __m128 t0 = shuffle<0, 1, 0, 1>(mat.col[0], mat.col[1]); // 00,10,01,11
    __m128 t1 = shuffle<2, 3, 2, 3>(mat.col[0], mat.col[1]); // 20,30,21,31
    // mat.col[2] : 02,12,22,32
    r.col[0] = shuffle<0, 2, 0, 3>(t0, mat.col[2]); //00,01,02,32(=0)
    r.col[1] = shuffle<1, 3, 1, 3>(t0, mat.col[2]); //10,11,12,32(=0)
    r.col[2] = shuffle<0, 2, 2, 3>(t1, mat.col[2]); //20,21,22,32(=0)

    //translate
    r.col[3] = _mm_mul_ps(r.col[0], swizzle<0>(mat.col[3]));
    r.col[3] = _mm_add_ps(r.col[3], _mm_mul_ps(r.col[1], swizzle<1>(mat.col[3])));
    r.col[3] = _mm_add_ps(r.col[3], _mm_mul_ps(r.col[2], swizzle<2>(mat.col[3])));
    r.col[3] = _mm_sub_ps(_mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f), r.col[3]);

    return r;
}

inline Matrix4x4 transform_inverse(const Matrix4x4 &mat)
{
    Matrix4x4 r;
    //transpose left-upper 3x3
    __m128 t0 = shuffle<0, 1, 0, 1>(mat.col[0], mat.col[1]); // 00,10,01,11
    __m128 t1 = shuffle<2, 3, 2, 3>(mat.col[0], mat.col[1]); // 20,30,21,31
    // mat.col[2] : 02,12,22,32
    r.col[0] = shuffle<0, 2, 0, 3>(t0, mat.col[2]); //00,01,02,32(=0)
    r.col[1] = shuffle<1, 3, 1, 3>(t0, mat.col[2]); //10,11,12,32(=0)
    r.col[2] = shuffle<0, 2, 2, 3>(t1, mat.col[2]); //20,21,22,32(=0)

    //calculate sqr size
    __m128 sqrl = _mm_mul_ps(r.col[0], r.col[0]);
    sqrl = _mm_add_ps(sqrl, _mm_mul_ps(r.col[1], r.col[1]));
    sqrl = _mm_add_ps(sqrl, _mm_mul_ps(r.col[2], r.col[2]));

    //here need avoid divide by zero?
    //but I will remove it here,and add (0,0,0,1) to it
    sqrl = _mm_add_ps(sqrl, _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f));

    __m128 rsqrl = _mm_div_ps(_mm_set1_ps(1.0f), sqrl);

    r.col[0] = _mm_mul_ps(r.col[0], rsqrl);
    r.col[1] = _mm_mul_ps(r.col[1], rsqrl);
    r.col[2] = _mm_mul_ps(r.col[2], rsqrl);

    //translate
    r.col[3] = _mm_mul_ps(r.col[0], swizzle<0>(mat.col[3]));
    r.col[3] = _mm_add_ps(r.col[3], _mm_mul_ps(r.col[1], swizzle<1>(mat.col[3])));
    r.col[3] = _mm_add_ps(r.col[3], _mm_mul_ps(r.col[2], swizzle<2>(mat.col[3])));
    r.col[3] = _mm_sub_ps(_mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f), r.col[3]);

    return r;
}

static inline __m128 adj2x2(const __m128 m) { return _mm_mul_ps(swizzle<3, 2, 1, 0>(m), _mm_set_ps(1.0f, -1.0f, -1.0f, 1.0f)); }
static inline __m128 mul2x2(const __m128 m0, const __m128 m1) { return _mm_add_ps(_mm_mul_ps(swizzle<0, 1, 0, 1>(m0), swizzle<0, 0, 2, 2>(m1)), _mm_mul_ps(swizzle<2, 3, 2, 3>(m0), swizzle<1, 1, 3, 3>(m1))); }
static inline __m128 det2x2(const __m128 m) { return _mm_sub_ps(_mm_mul_ps(swizzle<0, 0, 0, 0>(m) /*a*/, swizzle<3, 3, 3, 3>(m) /*d*/), _mm_mul_ps(swizzle<1, 1, 1, 1>(m) /*c*/, swizzle<2, 2, 2, 2>(m) /*b*/)); }

//Blockwise inversion
//https://en.wikipedia.org/wiki/Invertible_matrix#Blockwise_inversion
inline Matrix4x4 blockwise_inverse(const Matrix4x4 &mat)
{
    Matrix4x4 r;
    //extract 4 sub-matrix2x2
    // | A B |
    // | C D |
    __m128 A = shuffle<0, 1, 0, 1>(mat.col[0], mat.col[1]);
    __m128 C = shuffle<2, 3, 2, 3>(mat.col[0], mat.col[1]);
    __m128 B = shuffle<0, 1, 0, 1>(mat.col[2], mat.col[3]);
    __m128 D = shuffle<2, 3, 2, 3>(mat.col[2], mat.col[3]);

    //2x2 det
    // h->  |D| |B| |C| |A| <-l
    // __m128 det=_mm_sub_ps(
    // _mm_mul_ps(shuffle<0,2,0,2>(mat.col[0],mat.col[2])/*a*/,shuffle<1,3,1,3>(mat.col[1],mat.col[3])/*d*/),
    // _mm_mul_ps(shuffle<1,3,1,3>(mat.col[0],mat.col[2])/*c*/,shuffle<0,2,0,2>(mat.col[1],mat.col[3])/*b*/)
    // );

    //2x2 det
    __m128 detA = det2x2(A);
    // __m128 detB = swizzle<2>(det);
    // __m128 detC = swizzle<1>(det);
    // __m128 detD = swizzle<3>(det);

    __m128 invA = _mm_div_ps(adj2x2(A), detA);
    __m128 D_CinvAB = _mm_sub_ps(D, mul2x2(mul2x2(C, invA), B));
    __m128 det_D_CinvAB = _mm_sub_ps(
        _mm_mul_ps(swizzle<0, 0, 0, 0>(D_CinvAB) /*a*/, swizzle<3, 3, 3, 3>(D_CinvAB) /*d*/),
        _mm_mul_ps(swizzle<1, 1, 1, 1>(D_CinvAB) /*c*/, swizzle<2, 2, 2, 2>(D_CinvAB) /*b*/));

    __m128 DD = _mm_div_ps(adj2x2(D_CinvAB), det_D_CinvAB);
    __m128 CinvA = _mm_mul_ps(C, invA);
    __m128 invAB = _mm_mul_ps(invA, B);
    __m128 invABinv_D_CinvAB = _mm_mul_ps(invAB, DD);
    __m128 BB = _mm_sub_ps(_mm_set1_ps(0.0f), invABinv_D_CinvAB);
    __m128 AA = _mm_add_ps(invA, _mm_mul_ps(invABinv_D_CinvAB, CinvA));
    __m128 CC = _mm_sub_ps(_mm_set1_ps(0.0f), _mm_mul_ps(DD, CinvA));

    r.col[0] = shuffle<0, 1, 0, 1>(AA, CC);
    r.col[1] = shuffle<2, 3, 2, 3>(AA, CC);
    r.col[2] = shuffle<0, 1, 0, 1>(BB, DD);
    r.col[3] = shuffle<2, 3, 2, 3>(BB, DD);

    return r;
}

inline Matrix4x4 transpose(const Matrix4x4 &mat)
{
    Matrix4x4 r(mat);
    _MM_TRANSPOSE4_PS(r.col[0], r.col[1], r.col[2], r.col[3]);
    return r;
}

//TODO need optimal
inline Matrix4x4 minor(const Matrix4x4 &mat)
{
    float4 m0 = swizzle<1, 0, 0, 0>(mat.col[1]) * swizzle<2, 2, 1, 1>(mat.col[2]) * swizzle<3, 3, 3, 2>(mat.col[3]);
    m0 = m0 + swizzle<2, 2, 1, 1>(mat.col[1]) * swizzle<3, 3, 3, 2>(mat.col[2]) * swizzle<1, 0, 0, 0>(mat.col[3]);
    m0 = m0 + swizzle<3, 3, 3, 2>(mat.col[1]) * swizzle<1, 0, 0, 0>(mat.col[2]) * swizzle<2, 2, 1, 1>(mat.col[3]);
    m0 = m0 - swizzle<3, 3, 3, 2>(mat.col[1]) * swizzle<2, 2, 1, 1>(mat.col[2]) * swizzle<1, 0, 0, 0>(mat.col[3]);
    m0 = m0 - swizzle<2, 2, 1, 1>(mat.col[1]) * swizzle<1, 0, 0, 0>(mat.col[2]) * swizzle<3, 3, 3, 2>(mat.col[3]);
    m0 = m0 - swizzle<1, 2, 1, 1>(mat.col[1]) * swizzle<3, 3, 3, 2>(mat.col[2]) * swizzle<2, 0, 0, 0>(mat.col[3]);

    float4 m1 = swizzle<1, 0, 0, 0>(mat.col[0]) * swizzle<2, 2, 1, 1>(mat.col[2]) * swizzle<3, 3, 3, 2>(mat.col[3]);
    m1 = m1 + swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<3, 3, 3, 2>(mat.col[2]) * swizzle<1, 0, 0, 0>(mat.col[3]);
    m1 = m1 + swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<1, 0, 0, 0>(mat.col[2]) * swizzle<2, 2, 1, 1>(mat.col[3]);
    m1 = m1 - swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<2, 2, 1, 1>(mat.col[2]) * swizzle<1, 0, 0, 0>(mat.col[3]);
    m1 = m1 - swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<1, 0, 0, 0>(mat.col[2]) * swizzle<3, 3, 3, 2>(mat.col[3]);
    m1 = m1 - swizzle<1, 2, 1, 1>(mat.col[0]) * swizzle<3, 3, 3, 2>(mat.col[2]) * swizzle<2, 0, 0, 0>(mat.col[3]);

    float4 m2 = swizzle<1, 0, 0, 0>(mat.col[0]) * swizzle<2, 2, 1, 1>(mat.col[1]) * swizzle<3, 3, 3, 2>(mat.col[3]);
    m2 = m2 + swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<3, 3, 3, 2>(mat.col[1]) * swizzle<1, 0, 0, 0>(mat.col[3]);
    m2 = m2 + swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<1, 0, 0, 0>(mat.col[1]) * swizzle<2, 2, 1, 1>(mat.col[3]);
    m2 = m2 - swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<2, 2, 1, 1>(mat.col[1]) * swizzle<1, 0, 0, 0>(mat.col[3]);
    m2 = m2 - swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<1, 0, 0, 0>(mat.col[1]) * swizzle<3, 3, 3, 2>(mat.col[3]);
    m2 = m2 - swizzle<1, 2, 1, 1>(mat.col[0]) * swizzle<3, 3, 3, 2>(mat.col[1]) * swizzle<2, 0, 0, 0>(mat.col[3]);

    float4 m3 = swizzle<1, 0, 0, 0>(mat.col[0]) * swizzle<2, 2, 1, 1>(mat.col[1]) * swizzle<3, 3, 3, 2>(mat.col[2]);
    m3 = m3 + swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<3, 3, 3, 2>(mat.col[1]) * swizzle<1, 0, 0, 0>(mat.col[2]);
    m3 = m3 + swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<1, 0, 0, 0>(mat.col[1]) * swizzle<2, 2, 1, 1>(mat.col[2]);
    m3 = m3 - swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<2, 2, 1, 1>(mat.col[1]) * swizzle<1, 0, 0, 0>(mat.col[2]);
    m3 = m3 - swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<1, 0, 0, 0>(mat.col[1]) * swizzle<3, 3, 3, 2>(mat.col[2]);
    m3 = m3 - swizzle<1, 2, 1, 1>(mat.col[0]) * swizzle<3, 3, 3, 2>(mat.col[1]) * swizzle<2, 0, 0, 0>(mat.col[2]);
    return Matrix4x4(m0, m1, m2, m3);
}

inline Matrix4x4 cofactor(const Matrix4x4 &mat)
{
    float4 m0 = swizzle<1, 0, 0, 0>(mat.col[1]) * swizzle<2, 3, 1, 2>(mat.col[2]) * swizzle<3, 2, 3, 1>(mat.col[3]);
    m0 = m0 + swizzle<2, 2, 1, 1>(mat.col[1]) * swizzle<3, 0, 3, 0>(mat.col[2]) * swizzle<1, 3, 0, 2>(mat.col[3]);
    m0 = m0 + swizzle<3, 3, 3, 2>(mat.col[1]) * swizzle<1, 2, 0, 1>(mat.col[2]) * swizzle<2, 0, 1, 0>(mat.col[3]);
    m0 = m0 - swizzle<3, 3, 3, 2>(mat.col[1]) * swizzle<2, 0, 1, 0>(mat.col[2]) * swizzle<1, 2, 0, 1>(mat.col[3]);
    m0 = m0 - swizzle<2, 2, 1, 1>(mat.col[1]) * swizzle<1, 3, 0, 2>(mat.col[2]) * swizzle<3, 0, 3, 0>(mat.col[3]);
    m0 = m0 - swizzle<1, 0, 1, 0>(mat.col[1]) * swizzle<3, 2, 3, 1>(mat.col[2]) * swizzle<2, 3, 0, 2>(mat.col[3]);

    float4 m1 = swizzle<1, 0, 0, 0>(mat.col[0]) * swizzle<3, 2, 3, 1>(mat.col[2]) * swizzle<2, 3, 1, 2>(mat.col[3]);
    m1 = m1 + swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<1, 3, 0, 2>(mat.col[2]) * swizzle<3, 0, 3, 0>(mat.col[3]);
    m1 = m1 + swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<2, 0, 1, 0>(mat.col[2]) * swizzle<1, 2, 0, 1>(mat.col[3]);
    m1 = m1 - swizzle<1, 0, 0, 0>(mat.col[0]) * swizzle<2, 3, 1, 2>(mat.col[2]) * swizzle<3, 2, 3, 1>(mat.col[3]);
    m1 = m1 - swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<3, 0, 3, 0>(mat.col[2]) * swizzle<1, 3, 0, 2>(mat.col[3]);
    m1 = m1 - swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<1, 2, 0, 1>(mat.col[2]) * swizzle<2, 0, 1, 0>(mat.col[3]);

    float4 m2 = swizzle<1, 0, 0, 0>(mat.col[0]) * swizzle<2, 3, 1, 2>(mat.col[1]) * swizzle<3, 2, 3, 1>(mat.col[3]);
    m2 = m2 + swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<3, 0, 3, 0>(mat.col[1]) * swizzle<1, 3, 0, 2>(mat.col[3]);
    m2 = m2 + swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<1, 2, 0, 1>(mat.col[1]) * swizzle<2, 0, 1, 0>(mat.col[3]);
    m2 = m2 - swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<2, 0, 1, 0>(mat.col[1]) * swizzle<1, 2, 0, 1>(mat.col[3]);
    m2 = m2 - swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<1, 3, 0, 2>(mat.col[1]) * swizzle<3, 0, 3, 0>(mat.col[3]);
    m2 = m2 - swizzle<1, 0, 1, 0>(mat.col[0]) * swizzle<3, 2, 3, 1>(mat.col[1]) * swizzle<2, 3, 0, 2>(mat.col[3]);

    float4 m3 = swizzle<1, 0, 0, 0>(mat.col[0]) * swizzle<3, 2, 3, 1>(mat.col[1]) * swizzle<2, 3, 1, 2>(mat.col[2]);
    m3 = m3 + swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<1, 3, 0, 2>(mat.col[1]) * swizzle<3, 0, 3, 0>(mat.col[2]);
    m3 = m3 + swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<2, 0, 1, 0>(mat.col[1]) * swizzle<1, 2, 0, 1>(mat.col[2]);
    m3 = m3 - swizzle<1, 0, 0, 0>(mat.col[0]) * swizzle<2, 3, 1, 2>(mat.col[1]) * swizzle<3, 2, 3, 1>(mat.col[2]);
    m3 = m3 - swizzle<2, 2, 1, 1>(mat.col[0]) * swizzle<3, 0, 3, 0>(mat.col[1]) * swizzle<1, 3, 0, 2>(mat.col[2]);
    m3 = m3 - swizzle<3, 3, 3, 2>(mat.col[0]) * swizzle<1, 2, 0, 1>(mat.col[1]) * swizzle<2, 0, 1, 0>(mat.col[2]);
    return Matrix4x4(m0, m1, m2, m3);
}

inline float determinant(const Matrix4x4 &mat)
{
    auto cofactor_mat = cofactor(mat);
    return reduce_add(float4(mat.col[0]) * float4(cofactor_mat.col[0]));
}

inline float sub_matrix3x3_determinant(const Matrix4x4 &mat)
{
    auto temp = mat.col[0] * swizzle<1, 2, 0, 3>(mat.col[1]) * swizzle<2, 0, 1, 3>(mat.col[2]) - swizzle<2, 1, 0, 3>(mat.col[0]) * swizzle<1, 0, 2, 3>(mat.col[1]) * swizzle<0, 2, 1, 3>(mat.col[1]);
    float4 a = temp;
    float4 b = swizzle<1>(temp);
    float4 c = swizzle<2>(temp);
    return _mm_cvtss_f32(a + b + c);
}

//Cramer's rule
inline Matrix4x4 inverse(const Matrix4x4 &mat)
{
    auto tran_mat = transpose(mat);
    auto cofactor_mat = cofactor(tran_mat);
    auto det = vreduce_add(float4(tran_mat.col[0]) * float4(cofactor_mat.col[0]));
    auto inv_det = rcp(det);
    cofactor_mat.col[0] = cofactor_mat.col[0] * inv_det;
    cofactor_mat.col[1] = cofactor_mat.col[1] * inv_det;
    cofactor_mat.col[2] = cofactor_mat.col[2] * inv_det;
    cofactor_mat.col[3] = cofactor_mat.col[3] * inv_det;
    return cofactor_mat;
}

NARUKAMI_END