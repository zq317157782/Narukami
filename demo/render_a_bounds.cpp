#include "core/narukami.h"
#include "core/geometry.h"
#include "core/mesh.h"
#include "core/imageio.h"
#include "core/film.h"
#include "core/spectrum.h"
#include "core/sampler.h"
using namespace narukami;
int main(){
    Sampler sampler(1024);
    Film film(Point2i(128,128),Bounds2f(Point2f(0,0),Point2f(1,1)));
    SoABounds3f bound(SoAPoint3f(Point3f(0.1f,0.1f,1.0f),Point3f(0.1f,0.6f,1.0f),Point3f(0.6f,0.1f,1.0f),Point3f(0.6f,0.6f,1.0f)),SoAPoint3f(Point3f(0.4f,0.4f,1.0f),Point3f(0.4f,0.9f,1.0f),Point3f(0.9f,0.4f,1.0f),Point3f(0.9f,0.9f,1.0f)));
    int isPositive[3]={1,1,1};
    for(int y=0;y<128;++y){
         for(int x=0;x<128;++x){
             sampler.start_pixel(Point2i(x,y));
             do{
                auto sample = sampler.get_2D();
                if(sample.x>=1 || sample.y>=1){
                    std::cout<<"卧槽"<<sample<<std::endl;
                }
                float px=x+sample.x;
                float py=y+sample.y;
                if(collide(SoAPoint3f(px/128.0f,py/128.0f,0),SoAVector3f(INFINITE,INFINITE,1),float4(0),float4(1),isPositive,bound)){
                    film.add_sample(Point2f(px,py),Spectrum(1,1,1),1);
                }
             }while(sampler.start_next_sample());
            
         }
    }
    film.write_to_file("bounds.png");

}