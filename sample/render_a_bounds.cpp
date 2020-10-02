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
    Sampler sampler(1024);
    auto film = std::make_shared<Film>(Point2i(128,128),Bounds2f(Point2f(0,0),Point2f(1,1)));
    shared<AnimatedTransform> t(new AnimatedTransform(make_shared(Transform()),0,make_shared(Transform()),0));
    OrthographicCamera camera(t,0,0,{{0,0},{1,1}},film);
    Bounds3fPack bound(Point3fPack(Point3f(0.1f,0.1f,0.1f),Point3f(0.1f,0.6f,0.1f),Point3f(0.6f,0.1f,0.1f),Point3f(0.6f,0.6f,0.1f)),Point3fPack(Point3f(0.4f,0.4f,1.0f),Point3f(0.4f,0.9f,1.0f),Point3f(0.9f,0.4f,1.0f),Point3f(0.9f,0.9f,1.0f)));
    Bounds3f bound2(Point3f(0.1f,0.1f,0.1f),Point3f(0.9f,0.9f,0.9f));
    int isPositive[3]={1,1,1};
    for(int y=0;y<128;++y){
         for(int x=0;x<128;++x){
             sampler.start_pixel(Point2i(x,y));
             do{
                auto cameraSample = sampler.get_camera_sample(Point2i(x,y));
                Ray ray;
                camera.generate_normalized_ray(cameraSample,&ray);
                if(intersect(ray.o,Vector3f(1.0f/ray.d.x,1.0f/ray.d.y,1.0f/ray.d.z),0,1,isPositive,bound2)){
                    film->add_sample(cameraSample.pFilm,Color(1,1,1),1);
                }
             }while(sampler.start_next_sample());
            
         }
    }
    film->write_to_file("bounds.png");

}