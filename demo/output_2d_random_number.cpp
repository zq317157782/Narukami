#include "core/narukami.h"
#include "core/rng.h"
#include <fstream>
using namespace narukami;
int main(){
        RNG rng;
        RNG rng2(12345678UL);
        std::ofstream outfile;
        outfile.open("2d_random_number.dat");
        for(int i=0;i<100;++i){
            outfile << rng.next_float()<<" 0.5"<<std::endl;
        }
        outfile.close();
}