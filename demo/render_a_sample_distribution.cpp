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
    Sampler sampler(16);
    Film film(Point2i(128,128),Bounds2f(Point2f(0,0),Point2f(1,1)));
    OrthographicCamera camera(Transform(),{{0,0},{1,1}},&film);
    
     sampler.start_pixel(Point2i(0,0));
     do{
          auto cameraSample = sampler.get_camera_sample(Point2i(0,0));
          cameraSample.pFilm.x=cameraSample.pFilm.x*128;
          cameraSample.pFilm.y=cameraSample.pFilm.y*128;
          film.add_sample(cameraSample.pFilm,Spectrum(1,1,1),1);
     }while(sampler.start_next_sample());
     film.write_to_file("sobol.png");

}