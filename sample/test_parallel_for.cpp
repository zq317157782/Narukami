#include "core/narukami.h"
#include "core/parallel.h"
using namespace narukami;
int main(){
    parallel_for([](size_t i){std::cout<<i<<std::endl;},10,1);
    parallel_for_2D([](Point2i i){std::cout<<i<<std::endl;},Point2i(10,10));
    parallel_for_clean();
}