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
#include "core/parallel.h"
#include "lights/point.h"
#include "lights/rect.h"
#include "lights/disk.h"
using namespace narukami;
int main()
{
    auto camera_transform = translate(0, 0, -4)  * rotate(-1.5f,0,0,1);
    auto camera_transform2 = translate(0, 0, -4) * rotate( 1.5f,0,0,1);
    auto sampler = Sampler(32);
    auto film = std::make_shared<Film>(Point2i(1920, 1080), Bounds2f(Point2f(0, 0), Point2f(1, 1)));
    float aspect = 16.0f / 9.0f;

    

    auto camera = PerspectiveCamera(make_shared(AnimatedTransform(make_shared(camera_transform),0,make_shared(camera_transform2),1)),0,1,Bounds2f{{-1 * aspect, -1}, {1 * aspect, 1}}, 45, film);

    std::vector<shared<BLASInstance>> instance_list;

    Transform blas_to_wrold = translate(1.0f, 0, 0);
    Transform world_to_blas = inverse(blas_to_wrold);
    Transform blas_to_wrold2 = translate(-1.0f, 0, 0);
    Transform world_to_blas2 = inverse(blas_to_wrold2);
    
    {
        std::vector<std::shared_ptr<TriangleMesh>> meshs;
        {
            auto transform = make_shared(translate(Vector3f(0, 0, 0.0f)) * scale(0.2f, 0.2f, 0.2f) * rotate(90, Vector3f(0, 1, 0)));
            auto inv_transform = make_shared(inverse(*transform)); // translate(Vector3f(-0.5f, -0.5f, -1))*scale(0.2f,0.2f,0.2f)*rotate(-90,Vector3f(0,1,0));
            append(meshs, load_mesh_triangles_from_obj(transform, inv_transform, "bunny.obj", "."));
        }
        auto primitives = create_mesh_primitives(meshs);
        auto blas = shared<MeshBLAS>(new MeshBLAS(primitives));

        {
            
            auto instance = shared<BLASInstance>(new BLASInstance(&blas_to_wrold, &world_to_blas, blas));
            instance_list.push_back(instance);
        }

        {
          
            auto instance = shared<BLASInstance>(new BLASInstance(&blas_to_wrold2, &world_to_blas2, blas));
            instance_list.push_back(instance);
        }
    }
    
    auto t = identity();
    {
        std::vector<std::shared_ptr<TriangleMesh>> meshs;
        {

            auto transform = make_shared(translate(0, -1, 0) * rotate(90, 1, 0, 0));
            auto inv_transform = make_shared(inverse(*transform));
            append(meshs, create_plane(transform, inv_transform, 5, 5));
        }
        {
            auto transform = make_shared(translate(0, 1, 0) * rotate(90, 1, 0, 0));
            auto inv_transform = make_shared(inverse(*transform));
            append(meshs, create_plane(transform, inv_transform, 5, 5));
        }

        {
            auto transform = make_shared(translate(0, 0, 2.5f));
            auto inv_transform = make_shared(inverse(*transform));
            append(meshs, create_plane(transform, inv_transform, 5, 2));
        }

        {
            auto transform = make_shared(translate(2.5f, 0, 0) * rotate(90, 0, 1, 0));
            auto inv_transform = make_shared(inverse(*transform));
            append(meshs, create_plane(transform, inv_transform, 5, 2));
        }

        {
            auto transform = make_shared(translate(-2.5f, 0, 0) * rotate(90, 0, 1, 0));
            auto inv_transform = make_shared(inverse(*transform));
            append(meshs, create_plane(transform, inv_transform, 5, 2));
        }
        auto primitives = create_mesh_primitives(meshs);



        auto blas = shared<MeshBLAS>(new MeshBLAS(primitives));
        {
            auto instance = shared<BLASInstance>(new BLASInstance(&t, &t, blas));
            instance_list.push_back(instance);
        }
    }

    // //create light
    std::vector<Light *> lights;
    {
        {
            // auto transform = make_shared(translate(Vector3f(0.0f, 0.0f, 1.0f)) * rotate(90, 1, 0, 0));
            // auto inv_transform = make_shared(inverse(*transform));
            // auto point_light = new PointLight(transform, inv_transform, Color(10, 10, 10));
            // lights.push_back(point_light);
        }
        {
            auto transform = make_shared(translate(Vector3f(0.0f, 1.0f, 1.0f)) * rotate(90, 1, 0, 0));
            auto inv_transform = make_shared(inverse(*transform));
            auto rect_light = new RectLight(transform, inv_transform, Color(10, 10, 10),false,1,1);
            lights.push_back(rect_light);
        }
       
    }

    // {
    //     auto transform = translate(Vector3f(0.0f, 1.0f, 1.0f))*rotate(90,1,0,0);
    //     auto inv_transform = inverse(transform);
    //     auto disk_light = std::make_shared<DiskLight>(&transform,&inv_transform,Color(10,10,10),false,0.2);
    //     auto disklight_triangles = load_mesh(*disk_light);
    //     lights.push_back(disk_light);
    //     primitives = _union(primitives,create_primitives(disklight_triangles,disk_light.get()));
    // }

    auto acce = shared<TLAS>(new TLAS(instance_list));

    Scene scene(acce, lights);
    Integrator integrator(&camera, &sampler);
    integrator.render(scene);
    parallel_for_clean();
    report_thread_statistics();
    print_statistics(std::cout);

    film->write_to_file("demo_mesh.png");
}
