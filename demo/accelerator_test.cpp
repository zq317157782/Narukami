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
#include "core/primitive.h"
#include "core/accelerator.h"
using namespace narukami;
int main(){
    // auto sampler = std::make_shared<Sampler>(1);
    // auto film = std::make_shared<Film>(Point2i(128,128),Bounds2f(Point2f(0,0),Point2f(1,1)));
    // SSEAllocator<OrthographicCamera> alloc_camera;
    // auto camera = std::allocate_shared<OrthographicCamera>(alloc_camera,Transform(),Bounds2f{{0,0},{1,1}},film);
    
    // auto transform = translate(Vector3f(0.5f, 0.0f, 1.0f))*scale(0.1f,0.1f,0.1f);
    // auto inv_transform = translate(Vector3f(-0.5, 0, -1))*scale(-0.25,-0.25,-0.25);
    // auto triangles=load_mesh_triangles_from_obj(&transform,&inv_transform,"bunny.obj",".");
    
    // auto primitives=create_primitives(triangles[0]);
    // std::cout<<primitives.size();
    // Accelerator acc(primitives);
    // acc.intersect(Ray(Point3f(0,0,0),Vector3f(0,0,1)));
    // Scene scene(triangles[0]);
    // Integrator integrator(camera,sampler);
    // integrator.render(scene);
    // film->write_to_file("mesh_32ssp.png");

}

