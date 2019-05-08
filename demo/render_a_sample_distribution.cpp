#include "core/narukami.h"
#include "core/rng.h"
#include "core/film.h"
#include <algorithm>
#include <fstream>
using namespace narukami;

static const float OneMinusEpsilon = 0.99999994;


#define PCG32_DEFAULT_STATE 0x853c49e6748fea9bULL
#define PCG32_DEFAULT_STREAM 0xda3e39cb94b95bdbULL
#define PCG32_MULT 0x5851f42d4c957f2dULL
class RNG_pbrt {
  public:
    // RNG Public Methods
    RNG_pbrt();
    RNG_pbrt(uint64_t sequenceIndex) { SetSequence(sequenceIndex); }
    void SetSequence(uint64_t sequenceIndex);
    uint32_t UniformUInt32();
    uint32_t UniformUInt32(uint32_t b) {
        uint32_t threshold = (~b + 1u) % b;
        while (true) {
            uint32_t r = UniformUInt32();
            if (r >= threshold) return r % b;
        }
    }
    float UniformFloat() {
        //return std::min(OneMinusEpsilon, Float(UniformUInt32() * 0x1p-32f));
		return std::min(OneMinusEpsilon, float(UniformUInt32() * 2.3283064365386963e-10f));
    }
    template <typename Iterator>
    void Shuffle(Iterator begin, Iterator end) {
        for (Iterator it = end - 1; it > begin; --it)
            std::iter_swap(it,
                           begin + UniformUInt32((uint32_t)(it - begin + 1)));
    }
    void Advance(int64_t idelta) {
        uint64_t cur_mult = PCG32_MULT, cur_plus = _inc, acc_mult = 1u,
                 acc_plus = 0u, delta = (uint64_t)idelta;
        while (delta > 0) {
            if (delta & 1) {
                acc_mult *= cur_mult;
                acc_plus = acc_plus * cur_mult + cur_plus;
            }
            cur_plus = (cur_mult + 1) * cur_plus;
            cur_mult *= cur_mult;
            delta /= 2;
        }
        _state = acc_mult * _state + acc_plus;
    }
    int64_t operator-(const RNG_pbrt &other) const {
       
        uint64_t cur_mult = PCG32_MULT, cur_plus = _inc, cur_state = other._state,
                 the_bit = 1u, distance = 0u;
        while (_state != cur_state) {
            if ((_state & the_bit) != (cur_state & the_bit)) {
                cur_state = cur_state * cur_mult + cur_plus;
                distance |= the_bit;
            }
           
            the_bit <<= 1;
            cur_plus = (cur_mult + 1ULL) * cur_plus;
            cur_mult *= cur_mult;
        }
        return (int64_t)distance;
    }

  private:
    uint64_t _state, _inc;
};

inline RNG_pbrt::RNG_pbrt() : _state(PCG32_DEFAULT_STATE), _inc(PCG32_DEFAULT_STREAM) {}
inline void RNG_pbrt::SetSequence(uint64_t initseq) {
    _state = 0u;
    _inc = (initseq << 1u) | 1u;
    UniformUInt32();
    _state += PCG32_DEFAULT_STATE;
    UniformUInt32();
}

inline uint32_t RNG_pbrt::UniformUInt32() {
    uint64_t oldstate = _state;
    _state = oldstate * PCG32_MULT + _inc;
    uint32_t xorshifted = (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);
    uint32_t rot = (uint32_t)(oldstate >> 59u);
    return (xorshifted >> rot) | (xorshifted << ((~rot + 1u) & 31));
}



int main(){
        RNG rng(1);
        RNG rng2(2);
        RNG rng3(3);
        RNG rng4(4);
        RNG rng5(5);
        Film film(Point2i(128,128),Bounds2f(Point2f(0,0),Point2f(1,1)));
        for(int i=0;i<100;++i){
             film.add_sample(Point2f(rng.next_float()*127,rng2.next_float()*127),Spectrum(rng3.next_float(),rng4.next_float(),rng5.next_float()),1);
        }
        std::cout<<"output:sample_distribution.png"<<std::endl;
        film.write_to_file("sample_distribution.png");

        RNG_pbrt rng_pbrt(1);
        RNG_pbrt rng_pbrt2(2);
        RNG_pbrt rng_pbrt3(3);
        RNG_pbrt rng_pbrt4(4);
        RNG_pbrt rng_pbrt5(5);
        Film film2(Point2i(128,128),Bounds2f(Point2f(0,0),Point2f(1,1)));
        for(int i=0;i<100;++i){
             film2.add_sample(Point2f(rng_pbrt.UniformFloat()*127,rng_pbrt2.UniformFloat()*127),Spectrum(rng_pbrt3.UniformFloat(),rng_pbrt4.UniformFloat(),rng_pbrt5.UniformFloat()),1);
        }
        std::cout<<"output:sample_distribution_pbrt.png"<<std::endl;
        film2.write_to_file("sample_distribution_pbrt.png");
}