#include "core/narukami.h"
#include "core/geometry.h"
#include "core/mesh.h"
#include "core/imageio.h"
#include "core/film.h"
#include "core/spectrum.h"
#include "core/sampler.h"
#include "core/transform.h"
#include "cameras/orthographic.h"
using namespace narukami;
int main(){
    Sampler sampler(2048);
    auto film_0 =std::make_shared<Film>(Point2i(512,512),Bounds2f(Point2f(0,0),Point2f(1,1)));
    auto film_1 =std::make_shared<Film>(Point2i(512,512),Bounds2f(Point2f(0,0),Point2f(1,1)));
    auto film_2 =std::make_shared<Film>(Point2i(512,512),Bounds2f(Point2f(0,0),Point2f(1,1)));
    auto film_3 =std::make_shared<Film>(Point2i(512,512),Bounds2f(Point2f(0,0),Point2f(1,1)));
    auto film_4 =std::make_shared<Film>(Point2i(512,512),Bounds2f(Point2f(0,0),Point2f(1,1)));
    
     sampler.start_pixel(Point2i(0,0));
     do{
          auto sample_0 = sampler.get_2D();
          film_0->add_sample(sample_0*512.0f,Spectrum(1,1,1),1);
          auto sample_1 = sampler.get_2D();
          film_1->add_sample(sample_1*512.0f,Spectrum(1,1,1),1);
          auto sample_2 = sampler.get_2D();
          film_2->add_sample(sample_2*512.0f,Spectrum(1,1,1),1);
          auto sample_3 = sampler.get_2D();
          film_3->add_sample(sample_3*512.0f,Spectrum(1,1,1),1);
          auto sample_4 = sampler.get_2D();
          film_4->add_sample(sample_4*512.0f,Spectrum(1,1,1),1);
     }while(sampler.start_next_sample());
    
     film_0->write_to_file("sample_set_0.png");
     film_1->write_to_file("sample_set_1.png");
     film_2->write_to_file("sample_set_2.png");
     film_3->write_to_file("sample_set_3.png");
     film_4->write_to_file("sample_set_4.png");

}