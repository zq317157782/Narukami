#include "benchmark.h"
#include <xmmintrin.h>
#include "core/math.h"

static void BM_common_rcp(benchmark::State &state)
{
    float ret = 0;
    srand(0);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(ret += 1.0f / rand());
    }
}
BENCHMARK(BM_common_rcp);

static void BM_fast_rcp(benchmark::State &state)
{
    float ret = 0;
    srand(0);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(ret += narukami::rcp(rand()));
    }
}

BENCHMARK(BM_fast_rcp);

static void BM_common_rsqrt(benchmark::State &state)
{
    float ret = 0;
    srand(0);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(ret += 1.0f / std::sqrt(rand()));
    }
}
BENCHMARK(BM_common_rsqrt);

static void BM_fast_rsqrt(benchmark::State &state)
{
    float ret = 0;
    srand(0);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(ret += narukami::rsqrt(rand()));
    }
}
BENCHMARK(BM_fast_rsqrt);

#include "core/vector.h"
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

BENCHMARK_MAIN();