#include "benchmark.h"
#include <xmmintrin.h>
#include "math.h"

static void BM_common_rcp(benchmark::State& state){
  float ret=0;
  for (auto _ : state){
      float v=state.range(0);
      benchmark::DoNotOptimize(ret+=1.0f/v);
  }
}
BENCHMARK(BM_common_rcp)->Range(8,8<<10);


// float fast_rcp(float x){
//     const __m128 a = _mm_set_ss(x);
//     const __m128 r = _mm_rcp_ss(a);
//     return _mm_cvtss_f32(_mm_mul_ss(r,_mm_sub_ss(_mm_set_ss(2.0f), _mm_mul_ss(r, a))));
// }

static void BM_fast_rcp(benchmark::State& state){
    float ret=0;
    for (auto _ : state){
        float v=state.range(0);
         benchmark::DoNotOptimize(ret+=rcp(v));
    }
}

BENCHMARK(BM_fast_rcp)->Range(8,8<<10);


BENCHMARK_MAIN();