#include "core/narukami.h"
#include "core/sampler.h"
#include <fstream>
using namespace narukami;
int main(){
        Sampler sampler;
        sampler.request_2d_array();
        sampler.commit();
        sampler.start_pixel(Point2i(0,0));
        std::ofstream outfile;
        outfile.open("sampler.dat");
        Point2f array[32];
        sampler.get_2D_array(32,array);
        for (size_t i = 0; i < 32; i++)
        {
            outfile << array[i].x<<" "<<array[i].y<<std::endl;
        }
        
        // for(int i=0;i<32;++i){
        //     auto p = sampler.get_2D();
        //     outfile << p.x<<" "<<p.y<<std::endl;
        //     sampler.start_next_sample();
        // }
        outfile.close();
}