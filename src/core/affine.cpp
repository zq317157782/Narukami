
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
#include "core/affine.h"
#include "core/quaternion.h"
NARUKAMI_BEGIN
void polar_decompose(const Matrix4x4 &m, Vector3f *T, Quaternion *Q, Matrix4x4 *S)
{
    T->x = m.mn[3][0];
    T->y = m.mn[3][1];
    T->z = m.mn[3][2];

    Matrix4x4 M = m;
    for (int i = 0; i < 3; ++i)
    {
        M.mn[3][i] = M.mn[i][3] = 0.f;
    }
    M.mn[3][3] = 1.0f;
    //polar decompose
    int count = 0;
    float norm = 0;
    Matrix4x4 R = M;
    do
    {
        Matrix4x4 Rnext;
        Matrix4x4 Rit = inverse(transpose(R));
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                Rnext.mn[i][j] = 0.5f * (R.mn[i][j] + Rit.mn[i][j]);
            }
        }
        norm = 0;
        for (int i = 0; i < 3; ++i)
        {
            float n = abs(R.mn[0][i] - Rnext.mn[0][i]) +
                      abs(R.mn[1][i] - Rnext.mn[1][i]) +
                      abs(R.mn[2][i] - Rnext.mn[2][i]);
            norm = max(norm, n);
        }

        R = Rnext;
    } while (++count < 100 && norm > 0.001f);

    *Q = Quaternion(R);

    *S = inverse(R) * M;
}
NARUKAMI_END