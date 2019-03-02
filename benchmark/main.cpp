#include "benchmark.h"
#include <xmmintrin.h>
#include "core/math.h"

static void BM_common_rcp(benchmark::State& state){
  float ret=0;
  for (auto _ : state){
      float v=state.range(0);
      benchmark::DoNotOptimize(ret+=1.0f/v);
  }
}
BENCHMARK(BM_common_rcp)->Range(8,8<<10);


static void BM_fast_rcp(benchmark::State& state){
    float ret=0;
    for (auto _ : state){
        float v=state.range(0);
         benchmark::DoNotOptimize(ret+=narukami::rcp(v));
    }
}

BENCHMARK(BM_fast_rcp)->Range(8,8<<10);

static void BM_common_rsqrt(benchmark::State& state){
    float ret=0;
    for (auto _ : state){
        float v=state.range(0);
         benchmark::DoNotOptimize(1.0f/std::sqrt(v));
    }
}
BENCHMARK(BM_common_rsqrt)->Range(8,8<<10);

static void BM_fast_rsqrt(benchmark::State& state){
    float ret=0;
    for (auto _ : state){
        float v=state.range(0);
         benchmark::DoNotOptimize(narukami::rsqrt(v));
    }
}
BENCHMARK(BM_fast_rsqrt)->Range(8,8<<10);


BENCHMARK_MAIN();