#include "benchmark.h"
#include <xmmintrin.h>
#include "core/math/math.h"

static void BM_common_rcp(benchmark::State &state)
{
    float ret = 0;
    float x = 100.0f;
    for (auto _ : state)
    {   
        x++;
        benchmark::DoNotOptimize(ret += 1.0f /x);
    }
}
BENCHMARK(BM_common_rcp);

static void BM_SSE_rcp(benchmark::State &state)
{
    float ret = 0;
    float x = 100.0f;
    for (auto _ : state)
    {
        x++;
        benchmark::DoNotOptimize(ret += narukami::rcp(x));
    }
}

BENCHMARK(BM_SSE_rcp);

static void BM_common_rsqrt(benchmark::State &state)
{
    float ret = 0;
    float x = 100.0f;
    for (auto _ : state)
    {
        x++;
        benchmark::DoNotOptimize(ret += 1.0f / std::sqrt(x));
    }
}
BENCHMARK(BM_common_rsqrt);

static void BM_SSE_rsqrt(benchmark::State &state)
{
    float ret = 0;
    float x = 100.0f;
    for (auto _ : state)
    {
        x++;
        benchmark::DoNotOptimize(ret += narukami::rsqrt(x));
    }
}
BENCHMARK(BM_SSE_rsqrt);

static void BM_fast_rsqrt(benchmark::State &state)
{
    float ret = 0;
    float x = 100.0f;
    for (auto _ : state)
    {
        x++;
        benchmark::DoNotOptimize(ret += narukami::fast_rsqrt(x));
    }
}
BENCHMARK(BM_fast_rsqrt);

#include "core/math/vector3.h"
template <typename T>
FINLINE narukami::Vector3f normalize_v1(const narukami::Vector3<T> &v1)
{
    float l = narukami::length(v1);
    return v1 / l;
}

template <typename T>
FINLINE narukami::Vector3f normalize_v2(const narukami::Vector3<T> &v1)
{
    float inv_l = narukami::rsqrt(dot(v1, v1));
    return v1 * inv_l;
}

template <typename T>
FINLINE narukami::Vector3f normalize_v3(const narukami::Vector3<T> &v1)
{
    const __m128 x = _mm_set_ps(1.0f, v1.z, v1.y, v1.x);
    const __m128 s = _mm_mul_ps(x, x);
    const __m128 t = _mm_add_ss(s, _mm_movehl_ps(s, s));
    const __m128 pa = _mm_max_ss(_mm_add_ss(t, _mm_shuffle_ps(t, t, 1)), _mm_set_ss(1.0e-30f));
    const __m128 r = _mm_rsqrt_ss(pa);
    // one more iteration
    return v1 * _mm_cvtss_f32(_mm_mul_ss(r, _mm_add_ss(_mm_set_ss(1.5f),
                                                       _mm_mul_ss(_mm_mul_ss(pa, _mm_set_ss(-0.5f)), _mm_mul_ss(r, r)))));
}

template <typename T>
FINLINE narukami::Vector3f normalize_v4(const narukami::Vector3<T> &a)
{
    const __m128 pa = _mm_max_ss(_mm_set_ss(a.x * a.x + a.y * a.y + a.z * a.z), _mm_set_ss(1.0e-30f));
    const __m128 r = _mm_rsqrt_ss(pa);
    // one more iteration
    const float d = _mm_cvtss_f32(_mm_mul_ss(r, _mm_add_ss(_mm_set_ss(1.5f),
                                                           _mm_mul_ss(_mm_mul_ss(pa, _mm_set_ss(-0.5f)), _mm_mul_ss(r, r)))));
    return a * d;
}

static void BM_normalize_v1(benchmark::State &state)
{
    narukami::Vector3f v1(0, 0, 0);
    srand(0);
    float x = rand();
    float y = rand();
    float z = rand();
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(v1 += normalize_v1(narukami::Vector3f(x, y, z)));
    }
}
BENCHMARK(BM_normalize_v1);

static void BM_normalize_v2(benchmark::State &state)
{
    narukami::Vector3f v1(0, 0, 0);
    srand(0);
    float x = rand();
    float y = rand();
    float z = rand();
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(v1 += normalize_v2(narukami::Vector3f(x, y, z)));
    }
}
BENCHMARK(BM_normalize_v2);

static void BM_normalize_v3(benchmark::State &state)
{
    narukami::Vector3f v1(0, 0, 0);
    srand(0);
    float x = rand();
    float y = rand();
    float z = rand();
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(v1 += normalize_v3(narukami::Vector3f(x, y, z)));
    }
}
BENCHMARK(BM_normalize_v3);

static void BM_normalize_v4(benchmark::State &state)
{
    narukami::Vector3f v1(0, 0, 0);
    srand(0);
    float x = rand();
    float y = rand();
    float z = rand();
    for (auto _ : state)
    {

        benchmark::DoNotOptimize(v1 += normalize_v4(narukami::Vector3f(x, y, z)));
    }
}
BENCHMARK(BM_normalize_v4);

static void BM_cross_Vector3f(benchmark::State &state)
{
    narukami::Vector3f v1(1, 0, 0);
    srand(0);
    float x = rand();
    float y = rand();
    float z = rand();
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(v1=cross(narukami::Vector3f(x, y, z),v1));
    }
}
BENCHMARK(BM_cross_Vector3f);

static void BM_cross_SSEVector3f(benchmark::State &state)
{
    narukami::SSEVector3f v1(1, 0, 0);
    srand(0);
    float x = rand();
    float y = rand();
    float z = rand();
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(v1=cross(narukami::SSEVector3f(x, y, z),v1));
    }
}
BENCHMARK(BM_cross_SSEVector3f);


static void BM_sum_v1(benchmark::State &state)
{
    __m128 a = _mm_set_ps(1,2,3,4);
    float b=0;
    for (auto _ : state)
    {
         benchmark::DoNotOptimize(b=b+narukami::sum(a));
    }
}
BENCHMARK(BM_sum_v1);

FINLINE float sum_v2(const __m128 a){
    float v[4];
    _mm_store_ps(v,a);
    return v[0]+v[1]+v[2]+v[3];
}
static void BM_sum_v2(benchmark::State &state)
{
    __m128 a = _mm_set_ps(1,2,3,4);
    float b=0;
    for (auto _ : state)
    {
         benchmark::DoNotOptimize(b=b+sum_v2(a));
    }
}
BENCHMARK(BM_sum_v2);

#include "core/math/matrix4x4.h"


FINLINE narukami::Vector3f matMul_v1(const narukami::Matrix4x4& M,const narukami::Vector3f& v){
     __m128 xyzw = _mm_set_ps(narukami::Zero,v.z,v.y,v.x);
     
     narukami::float4 r=narukami::float4(M.mVec[0])*narukami::float4(v.x);
     r+=narukami::float4(M.mVec[1])*narukami::float4(v.y);
     r+=narukami::float4(M.mVec[2])*narukami::float4(v.z);

    //float w=sum(_mm_add_ps(M.mVec[3],xyzw));
    float rr[4];
    _mm_store_ps(rr,r.xyzw);
    return narukami::Vector3f(rr[0],rr[1],rr[2]);
}

FINLINE narukami::Vector3f matMul_v2(const narukami::Matrix4x4& M,const narukami::Vector3f& v){
    float x =  M.m[0]*v.x+M.m[4]*v.y+M.m[8]*v.z;
    float y =  M.m[1]*v.x+M.m[5]*v.y+M.m[9]*v.z;
    float z =  M.m[2]*v.x+M.m[6]*v.y+M.m[10]*v.z;

    return narukami::Vector3f(x,y,z);
}

static void BM_matrix_mul_v1(benchmark::State &state)
{
    narukami::Matrix4x4 M;
    narukami::Vector3f v;
    float b=0;
    for (auto _ : state)
    {
         benchmark::DoNotOptimize(v=matMul_v1(M,v));
    }
}
BENCHMARK(BM_matrix_mul_v1);

static void BM_matrix_mul_v2(benchmark::State &state)
{
    narukami::Matrix4x4 M;
    narukami::Vector3f v;
    float b=0;
    for (auto _ : state)
    {
         benchmark::DoNotOptimize(v=matMul_v2(M,v));
    }
}
BENCHMARK(BM_matrix_mul_v2);



FINLINE narukami::Matrix4x4 matMul_v2(const narukami::Matrix4x4& A,const narukami::Matrix4x4& B){
    narukami::Matrix4x4 ret;
    for(int r=0;r<4;++r){
        for(int c=0;c<4;++c){
            ret[c*4+r]=A.m[c*4+r]*B.m[c*4+r]+A.m[(c+1)*4+r]*B.m[c*4+(r+1)]+A.m[(c+2)*4+r]*B.m[c*4+(r+2)]+A.m[(c+3)*4+r]*B.m[c*4+(r+3)];
        }
    }   
    return ret;
}

static void BM_matrix_mul_mm_v1(benchmark::State &state)
{
    narukami::Matrix4x4 M;
    narukami::Matrix4x4 M2;
    float b=0;
    for (auto _ : state)
    {
         benchmark::DoNotOptimize(M=M*M2);
    }
}
BENCHMARK(BM_matrix_mul_mm_v1);

static void BM_matrix_mul_mm_v2(benchmark::State &state)
{
    narukami::Matrix4x4 M;
    narukami::Matrix4x4 M2;
    float b=0;
    for (auto _ : state)
    {
         benchmark::DoNotOptimize(M=matMul_v2(M,M2));
    }
}
BENCHMARK(BM_matrix_mul_mm_v2);


FINLINE bool matEqual_sse(const narukami::Matrix4x4& A,const narukami::Matrix4x4& B){
     __m128 mask0=_mm_and_ps(_mm_cmpeq_ps(A.mVec[0], B.mVec[0]),_mm_cmpeq_ps(A.mVec[1], B.mVec[1]));
    __m128 mask1=_mm_and_ps(_mm_cmpeq_ps(A.mVec[2], B.mVec[2]),_mm_cmpeq_ps(A.mVec[3], B.mVec[3]));
    return (_mm_movemask_ps(_mm_and_ps(mask0,mask1))&15)==15;
}

FINLINE bool matEqual_scaler(const narukami::Matrix4x4& A,const narukami::Matrix4x4& B){
    if((A.m[0]==B.m[0])&&(A.m[1]==B.m[1])&&(A.m[2]==B.m[2])&&(A.m[3]==B.m[3])&&(A.m[4]==B.m[4])
    &&(A.m[5]==B.m[5])&&(A.m[6]==B.m[6])&&(A.m[7]==B.m[7])&&(A.m[8]==B.m[8])&&(A.m[9]==B.m[9])
    &&(A.m[10]==B.m[10])&&(A.m[11]==B.m[11])&&(A.m[12]==B.m[12])&&(A.m[13]==B.m[13])
    &&(A.m[14]==B.m[14])&&(A.m[15]==B.m[15])){
        return true;
    }
    return false;
}

static void BM_matrix_equal_sse(benchmark::State &state)
{
    narukami::Matrix4x4 M;
    narukami::Matrix4x4 M2;
    bool a;
    for (auto _ : state)
    {
         benchmark::DoNotOptimize(a+=matEqual_sse(M,M2));
    }
}
BENCHMARK(BM_matrix_equal_sse);

static void BM_matrix_equal_scaler(benchmark::State &state)
{
    narukami::Matrix4x4 M;
    narukami::Matrix4x4 M2;
    bool a;
    for (auto _ : state)
    {
         benchmark::DoNotOptimize(a+=matEqual_scaler(M,M2));
    }
}
BENCHMARK(BM_matrix_equal_scaler);


static void BM_matrix_transform_inverse(benchmark::State &state)
{
    narukami::Matrix4x4 M;
    narukami::Matrix4x4 M2;
    bool a;
    for (auto _ : state)
    {
         benchmark::DoNotOptimize(M2=narukami::transform_inverse(M));
    }
}
BENCHMARK(BM_matrix_transform_inverse);

static void BM_matrix_inverse(benchmark::State &state)
{
    narukami::Matrix4x4 M;
    narukami::Matrix4x4 M2;
    bool a;
    for (auto _ : state)
    {
         benchmark::DoNotOptimize(M2=narukami::inverse(M));
    }
}
BENCHMARK(BM_matrix_inverse);


narukami::Matrix4x4 pbrt_inverse(const narukami::Matrix4x4 &m) {
	int indxc[4], indxr[4];
	int ipiv[4] = { 0, 0, 0, 0 };
	float minv[4][4];
	memcpy(minv, m.m, 4 * 4 * sizeof(float));
	for (int i = 0; i < 4; i++) {
		int irow = 0, icol = 0;
		float big = 0.f;
		// Choose pivot
		for (int j = 0; j < 4; j++) {
			if (ipiv[j] != 1) {
				for (int k = 0; k < 4; k++) {
					if (ipiv[k] == 0) {
						if (std::abs(minv[j][k]) >= big) {
							big = float(std::abs(minv[j][k]));
							irow = j;
							icol = k;
						}
					}
				}
			}
		}
		++ipiv[icol];
		// Swap rows _irow_ and _icol_ for pivot
		if (irow != icol) {
			for (int k = 0; k < 4; ++k)
				std::swap(minv[irow][k], minv[icol][k]);
		}
		indxr[i] = irow;
		indxc[i] = icol;
		

		// Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
		float pivinv = 1. / minv[icol][icol];
		minv[icol][icol] = 1.;
		for (int j = 0; j < 4; j++)
			minv[icol][j] *= pivinv;

		// Subtract this row from others to zero out their columns
		for (int j = 0; j < 4; j++) {
			if (j != icol) {
				float save = minv[j][icol];
				minv[j][icol] = 0;
				for (int k = 0; k < 4; k++)
					minv[j][k] -= minv[icol][k] * save;
			}
		}
	}
	// Swap columns to reflect permutation
	for (int j = 3; j >= 0; j--) {
		if (indxr[j] != indxc[j]) {
			for (int k = 0; k < 4; k++)
				std::swap(minv[k][indxr[j]], minv[k][indxc[j]]);
		}
	}
	return narukami::Matrix4x4((float*)minv);
}

static void BM_matrix_pbrt(benchmark::State &state)
{
    narukami::Matrix4x4 M;
    narukami::Matrix4x4 M2;
    bool a;
    for (auto _ : state)
    {
         benchmark::DoNotOptimize(M2=pbrt_inverse(M));
    }
}
BENCHMARK(BM_matrix_pbrt);

BENCHMARK_MAIN();