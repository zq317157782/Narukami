#include "core/narukami.h"
#include "core/imageio.h"
using namespace narukami;
int main(){
    int i=0;
    float data[128*128*3];
    for(int h=0;h<128;++h){
        for(int w=0;w<128;++w){
            data[i]=w/(127.0f);
            data[i+1]=h/(127.0f);
            data[i+2]=0;
            i+=3;
        }
    }
    narukami::write_image_to_file("texture_space.png",data,128,128);
}