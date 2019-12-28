#include "core/narukami.h"
#include "core/geometry.h"
#include "core/mesh.h"
#include "core/imageio.h"
#include "core/film.h"
#include "core/spectrum.h"
#include "core/sampler.h"
#include "core/transform.h"
#include "cameras/orthographic.h"
#include "cameras/perspective.h"
#include "core/meshloader.h"
#include "core/integrator.h"
#include "core/scene.h"
#include "core/stat.h"
#include "core/light.h"
#include "lights/point.h"
#include "lights/rect.h"
#include "lights/disk.h"
using namespace narukami;
int main(){

    auto sampler = std::make_shared<Sampler>(128);
    auto film = std::make_shared<Film>(Point2i(1920,1080),Bounds2f(Point2f(0,0),Point2f(1,1)));
    float aspect = 16.0f/9.0f;
    auto camera = std::make_shared<PerspectiveCamera>(Transform(),Bounds2f{{-2*aspect,-2},{2*aspect,2}},45,film);
    
    // //create mesh
    auto transform = translate(Vector3f(0, 0, 1.0f))*scale(0.2f,0.2f,0.2f)*rotate(90,Vector3f(0,1,0));
    auto inv_transform = inverse(transform);// translate(Vector3f(-0.5f, -0.5f, -1))*scale(0.2f,0.2f,0.2f)*rotate(-90,Vector3f(0,1,0));
    auto triangles=load_mesh_triangles_from_obj(&transform,&inv_transform,"bunny.obj",".");

    auto transform2 = translate(Vector3f(0.5f,0, 1.0f))*scale(0.2f,0.2f,0.2f);
    auto inv_transform2 = inverse(transform2);//translate(Vector3f(1.0f,-0.5f, -1))*scale(-0.2f,-0.2f,-0.2f);
    auto triangles2=load_mesh_triangles_from_obj(&transform2,&inv_transform2,"bunny.obj",".");

    auto transform3 = translate(0,-1,0)*rotate(90,1,0,0);
    auto inv_transform3= inverse(transform3);
    auto triangles3 = create_plane(&transform3,&inv_transform3,10,10);

    triangles = _union(triangles,triangles2);
    triangles = _union(triangles,triangles3);

    //create light 
    std::vector<std::shared_ptr<Light>> lights;
    
    // auto light_transform = translate(Vector3f(0.0f, 0.0f, 1.0f));//*rotate(45,0,1,0);
    // auto rect_light = std::make_shared<RectLight>(light_transform,Spectrum(10,10,10),false,1,2);
    // auto rectlight_triangles = load_mesh(*rect_light);
    // lights.push_back(rect_light);

    // auto light_transform2 = translate(Vector3f(0.0f, 1.0f, 1.0f));
    // auto point_light = std::make_shared<PointLight>(light_transform2,Spectrum(10,10,10));
    // lights.push_back(point_light);

    auto light_transform3 = translate(Vector3f(0.0f, 0.5f, 1.0f))*rotate(90,1,0,0);
    auto disk_light = std::make_shared<DiskLight>(light_transform3,Spectrum(10,10,10),false,0.2);
    auto disklight_triangles = load_mesh(*disk_light);
    lights.push_back(disk_light);

   

     //create primitive
    auto primitives = create_primitives(triangles);
    // auto light_primitives = create_primitives(rectlight_triangles,rect_light.get());
    // primitives = _union(primitives,light_primitives);
    auto light_primitives2 = create_primitives(disklight_triangles,disk_light.get());
    primitives = _union(primitives,light_primitives2);

    
    
    Scene scene(primitives,lights);
    Integrator integrator(camera,sampler);
    integrator.render(scene);
    report_thread_statistics();
    print_statistics(std::cout);

    
    film->write_to_file("demo_mesh.png");

}

