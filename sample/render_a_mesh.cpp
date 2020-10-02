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
    auto camera_transform = translate(0, 0, -4)  ;//* rotate(-1.5f,0,0,1);
    auto camera_transform2 = translate(0, 0, -4) ;//* rotate( 1.5f,0,0,1);
    auto sampler = Sampler(1);
    auto film = std::make_shared<Film>(Point2i(1920, 1080), Bounds2f(Point2f(0, 0), Point2f(1, 1)));
    float aspect = 16.0f / 9.0f;

    

    auto camera = PerspectiveCamera(make_shared(AnimatedTransform(make_shared(camera_transform),0,make_shared(camera_transform2),1)),0,1,Bounds2f{{-1 * aspect, -1}, {1 * aspect, 1}}, 45, film);

    std::vector<shared<BLASInstance>> instance_list;

    auto blas_to_wrold = make_shared(translate(1.0f, 0, 0));
    auto world_to_blas = make_shared(inverse(*blas_to_wrold));

    auto blas_to_wrold2 = make_shared(translate(-1.0f, 0, 0) * rotate(0,0,0,1));
    auto world_to_blas2 = make_shared(inverse(*blas_to_wrold2));

    auto blas_to_wrold3 = make_shared(translate(-1.0f, 0, 0) * rotate(35,0,0,1));
    auto world_to_blas3 = make_shared(inverse(*blas_to_wrold3));
    
    {
        shared<Mesh> mesh;
        {
            auto transform = make_shared(translate(Vector3f(0, 0, 0.0f)) * scale(0.01f, 0.01f, 0.01f) /** rotate(90, Vector3f(0, 1, 0))*/);
            auto inv_transform = make_shared(inverse(*transform)); // translate(Vector3f(-0.5f, -0.5f, -1))*scale(0.2f,0.2f,0.2f)*rotate(-90,Vector3f(0,1,0));
            //append(meshs, load_mesh_triangles_from_obj(transform, inv_transform, "bunny.obj", "."));
            mesh = load_mesh<MeshFileFormat::PLY>(transform,inv_transform,"Armadillo.ply");
        }
        auto primitives = create_mesh_primitives(mesh);
        auto blas = shared<CompactBLAS<MeshPrimitive,CompactMeshPrimitive>>(new CompactBLAS<MeshPrimitive,CompactMeshPrimitive>(primitives));

        {
            
            auto instance = shared<BLASInstance>(new BLASInstance(make_shared<AnimatedTransform>(blas_to_wrold), blas));
            instance_list.push_back(instance);
        }

        {
          
            auto instance = shared<BLASInstance>(new BLASInstance(make_shared<AnimatedTransform>(blas_to_wrold2,0,blas_to_wrold3,1),  blas));
            instance_list.push_back(instance);
        }
    }
    
     auto t = identity();
     {
         
         {
            auto transform = make_shared(translate(0, -1, 0) * rotate(90, 1, 0, 0));
            auto inv_transform = make_shared(inverse(*transform));
            shared<Mesh> mesh = create_plane(transform, inv_transform, 5, 5);
            std::vector<shared<MeshPrimitive>> primitives = create_mesh_primitives(mesh);
            auto blas = shared<CompactBLAS<MeshPrimitive,CompactMeshPrimitive>>(new CompactBLAS<MeshPrimitive,CompactMeshPrimitive>(primitives));
            auto instance = shared<BLASInstance>(new BLASInstance(make_shared<AnimatedTransform>(make_shared(t)), blas));
            instance_list.push_back(instance);
         }
        {
            auto transform = make_shared(translate(0, 1, 0) * rotate(90, 1, 0, 0));
            auto inv_transform = make_shared(inverse(*transform));
             shared<Mesh> mesh = create_plane(transform, inv_transform, 5, 5);
            std::vector<shared<MeshPrimitive>> primitives = create_mesh_primitives(mesh);
            auto blas = shared<CompactBLAS<MeshPrimitive,CompactMeshPrimitive>>(new CompactBLAS<MeshPrimitive,CompactMeshPrimitive>(primitives));
            auto instance = shared<BLASInstance>(new BLASInstance(make_shared<AnimatedTransform>(make_shared(t)), blas));
            instance_list.push_back(instance);
        }

        {
            auto transform = make_shared(translate(0, 0, 2.5f));
            auto inv_transform = make_shared(inverse(*transform));
             shared<Mesh> mesh = create_plane(transform, inv_transform, 5, 5);
            std::vector<shared<MeshPrimitive>> primitives = create_mesh_primitives(mesh);
            auto blas = shared<CompactBLAS<MeshPrimitive,CompactMeshPrimitive>>(new CompactBLAS<MeshPrimitive,CompactMeshPrimitive>(primitives));
            auto instance = shared<BLASInstance>(new BLASInstance(make_shared<AnimatedTransform>(make_shared(t)), blas));
            instance_list.push_back(instance);
        }

        {
            auto transform = make_shared(translate(2.5f, 0, 0) * rotate(90, 0, 1, 0));
            auto inv_transform = make_shared(inverse(*transform));
            shared<Mesh> mesh = create_plane(transform, inv_transform, 5, 5);
            std::vector<shared<MeshPrimitive>> primitives = create_mesh_primitives(mesh);
            auto blas = shared<CompactBLAS<MeshPrimitive,CompactMeshPrimitive>>(new CompactBLAS<MeshPrimitive,CompactMeshPrimitive>(primitives));
            auto instance = shared<BLASInstance>(new BLASInstance(make_shared<AnimatedTransform>(make_shared(t)), blas));
            instance_list.push_back(instance);
        }

        {
            auto transform = make_shared(translate(-2.5f, 0, 0) * rotate(90, 0, 1, 0));
            auto inv_transform = make_shared(inverse(*transform));
            shared<Mesh> mesh = create_plane(transform, inv_transform, 5, 5);
            std::vector<shared<MeshPrimitive>> primitives = create_mesh_primitives(mesh);
            auto blas = shared<CompactBLAS<MeshPrimitive,CompactMeshPrimitive>>(new CompactBLAS<MeshPrimitive,CompactMeshPrimitive>(primitives));
            auto instance = shared<BLASInstance>(new BLASInstance(make_shared<AnimatedTransform>(make_shared(t)), blas));
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
            auto transform = make_shared(translate(Vector3f(0.0f, 1.0f, 1.0f)) * rotate(-90, 1, 0, 0));
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
