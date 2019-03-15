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
    return narukami::Vector3f(r.x,r.y,r.z);
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

BENCHMARK_MAIN();