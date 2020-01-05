#include "core/narukami.h"
#include "core/imageio.h"
#include "core/film.h"
#include "core/spectrum.h"
using namespace narukami;
int main(){
    float data[128*128*3];
    Film film(Point2i(128,128),Bounds2f(Point2f(0,0),Point2f(1,1)));
    for(int h=0;h<128;++h){
        for(int w=0;w<128;++w){
            film.add_sample(Point2f(w,h),Spectrum(w/(127.0f),h/(127.0f),0),1);
        }
    }
    std::cout<<"output:texture_space.png"<<std::endl;
    film.write_to_file("texture_space.png");

    //narukami::write_image_to_file("texture_space.png",data,128,128);
}