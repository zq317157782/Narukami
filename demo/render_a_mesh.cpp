#include "core/narukami.h"
#include "core/geometry.h"
#include "core/mesh.h"
#include "core/imageio.h"
#include "core/film.h"
#include "core/spectrum.h"
#include "core/sampler.h"
#include "core/transform.h"
#include "cameras/orthographic.h"
#include "core/meshloader.h"
#include "core/integrator.h"
#include "core/scene.h"
#include "core/stat.h"
#include "core/light.h"
#include "lights/point.h"
#include "lights/rect.h"
using namespace narukami;
int main(){
    auto sampler = std::make_shared<Sampler>(128);
    auto film = std::make_shared<Film>(Point2i(256,256),Bounds2f(Point2f(0,0),Point2f(1,1)));
    auto camera = std::make_shared<OrthographicCamera>(Transform(),Bounds2f{{0,0},{1,1}},film);
    
    auto transform = translate(Vector3f(0.5f, 0.5f, 1.0f))*scale(0.2f,0.2f,0.2f)*rotate(90,Vector3f(0,1,0));
    auto inv_transform = translate(Vector3f(-0.5, -0.5, -1))*scale(-0.2,-0.2,-0.2)*rotate(-90,Vector3f(0,1,0));
    auto triangles=load_mesh_triangles_from_obj(&transform,&inv_transform,"bunny.obj",".");

    auto transform2 = translate(Vector3f(1.0f,0.5f, 1.0f))*scale(0.2f,0.2f,0.2f);
    auto inv_transform2 = translate(Vector3f(1.0f,-0.5, -1))*scale(-0.2,-0.2,-0.2);
    auto triangles2=load_mesh_triangles_from_obj(&transform2,&inv_transform2,"bunny.obj",".");

    //for(int i=0;i<100;++i)
    triangles[0].insert(triangles[0].end(),triangles2[0].begin(),triangles2[0].end());
  


    auto light_transform = translate(Vector3f(0.0f, 0.0f, 0.0f));

    //auto point_light = std::make_shared<PointLight>(light_transform,Spectrum(1,1,1));
    auto point_light = std::make_shared<RectLight>(light_transform,Spectrum(1,1,1),1,2,2);
    std::vector<std::shared_ptr<Light>> lights;
    lights.push_back(point_light);
    Scene scene(triangles[0],lights);
    Integrator integrator(camera,sampler);
    integrator.render(scene);
    report_thread_statistics();
    print_statistics(std::cout);

    
   // auto soa_triangles = cast_to_SoA_structure(triangles[0], 0, triangles[0].size());

    // for(int y=0;y<128;++y){
    //      for(int x=0;x<128;++x){
    //          sampler.start_pixel(Point2i(x,y));
    //          do{
    //             auto cameraSample = sampler.get_camera_sample(Point2i(x,y));
    //             //std::cout<<cameraSample.pFilm<<std::endl;
    //             Ray ray;
    //             camera.generate_normalized_ray(cameraSample,&ray);
    //             SoARay soa_ray(ray);
    //             //std::cout<<ray<<std::endl;;
    //             Point2f uv;
    //             float t=0;
    //             int index;
    //             int triangles_index=-1;
    //             bool is_hit = false; 
                
    //             for(size_t i=0;i<soa_triangles.size();++i){
                   
    //                 is_hit = intersect(soa_ray, soa_triangles[i], &t, &uv, &index);
    //                 if(is_hit){
    //                     soa_ray.t_max=float4(t);
    //                     triangles_index=i*4+index;
                       
    //                 }
    //             }

    //             if (triangles_index!=-1)
    //             {
    //                 film->add_sample(cameraSample.pFilm,Spectrum(min(t,1.0f),min(t,1.0f),min(t,1.0f)),1);                    
    //             }else{
    //                 film->add_sample(cameraSample.pFilm,Spectrum(0,0,0),1);     
    //             }
    //          }while(sampler.start_next_sample());
            
    //      }
    // }
    film->write_to_file("mesh_32ssp.png");

}


// #include "core/narukami.h"
// #include "core/geometry.h"
// #include "core/transform.h"
// #include "core/mesh.h"
// #include "core/imageio.h"
// #include "core/sampler.h"
// #include "core/meshloader.h"
// using namespace narukami;
// int main()
// {

    

//     Sampler sampler(16);

  
//     auto transform = scale(4,4,4)*translate(Vector3f(0.5, 0, 1));
//     auto inv_transform = scale(-0.25f,-0.25f,-0.25f)*translate(Vector3f(-0.5, 0, 1));
//     auto triangles=load_mesh_triangles_from_obj(&transform,&inv_transform,"bunny.obj",".");
//     auto soa_triangles = cast_to_SoA_structure(triangles[0], 0, triangles[0].size());

//     std::vector<uint8_t> image;
//     float data[128 * 128 * 3];
//     for (int y = 0; y < 128; ++y)
//     {
//         for (int x = 0; x < 128; ++x)
//         {
//             data[(y*128+x)* 3] = 0;
//             data[(y*128+x)* 3 + 1] = 0;
//             sampler.start_pixel(Point2i(x, y));
//             do
//             {
//                 auto sample = sampler.get_2D();
//                 SoARay ray(Point3f((x+sample.x)/128.0f, (y+sample.y)/128.0f, 0), Vector3f(0, 0, 1));
//                 Point2f uv;
//                 float t=0;
//                 int index;
//                 int triangles_index=-1;
//                 bool b = false;
                
//                 for(size_t i=0;i<soa_triangles.size();++i){
                   
//                     b = intersect(ray, soa_triangles[i], &t, &uv, &index);
                    
//                     if(b){
//                         ray.t_max=float4(t);
//                         triangles_index=i*4+index;
//                         //std::cout<<t<<std::endl;
//                     }
//                 }

//                 if (triangles_index!=-1)
//                 {
//                     uv = triangles2[0][triangles_index]->sample_uv(uv);
//                     data[(y*128+x)* 3] += uv[0];
//                     data[(y*128+x)* 3 + 1] += uv[1];
//                     data[(y*128+x)* 3] += min(t,1.0f);
//                     data[(y*128+x)* 3 + 1] += min(t,1.0f);

                    
//                 }
//                 // else
//                 // {
//                 //     std::cout << "miss " << ray.o << std::endl;
//                 // }

//             } while (sampler.start_next_sample());
//             data[(y*128+x)* 3] /= sampler.get_spp();
//             data[(y*128+x)* 3 + 1] /= sampler.get_spp();
//             data[(y*128+x)* 3 + 2] = 0;
//         }
//     }

//     narukami::write_image_to_file("mesh.png", data, 128, 128);
//     // //unsigned error = lodepng::encode(, image, 128, 128);
// }