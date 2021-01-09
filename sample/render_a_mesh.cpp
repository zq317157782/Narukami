#include "core/narukami.h"
#include "core/geometry.h"
#include "core/mesh.h"
#include "core/hairstrands.h"
#include "core/film.h"
#include "core/spectrum.h"
#include "core/sampler.h"
#include "core/transform.h"
#include "cameras/orthographic.h"
#include "cameras/perspective.h"
#include "core/meshloader.h"
#include "core/hairstrandsloader.h"
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

    Spectrum::init();
    auto camera_transform = translate(0, 0, -4);  //* rotate(-1.5f,0,0,1);
    auto camera_transform2 = translate(0, 0, -4); //* rotate( 1.5f,0,0,1);
    auto sampler = Sampler(32);
    auto film = std::make_shared<Film>(Point2i(1920, 1080), Bounds2f(Point2f(0, 0), Point2f(1, 1)));
    float aspect = 16.0f / 9.0f;

    auto camera = PerspectiveCamera(std::make_shared<AnimatedTransform>(std::make_shared<Transform>(camera_transform), 0, std::make_shared<Transform>(camera_transform2), 1), 0, 1, Bounds2f{{-1 * aspect, -1}, {1 * aspect, 1}}, 45, film);

    std::vector<shared<BLASInstance>> instance_list;

    auto blas_to_wrold = std::make_shared<Transform>(translate(1.0f, 0, 0));
    auto world_to_blas = std::make_shared<Transform>(inverse(*blas_to_wrold));

    auto blas_to_wrold2 = std::make_shared<Transform>(translate(-1.0f, 0, 0) * rotate(0, 0, 0, 1));
    auto world_to_blas2 = std::make_shared<Transform>(inverse(*blas_to_wrold2));

    auto blas_to_wrold3 = std::make_shared<Transform>(translate(-1.0f, 0, 0) * rotate(35, 0, 0, 1));
    auto world_to_blas3 = std::make_shared<Transform>(inverse(*blas_to_wrold3));

    {
        shared<Mesh> mesh;
        {
            auto transform = std::make_shared<Transform>(translate(Vector3f(0, 0, 1.0f)) * scale(0.01f, 0.01f, 0.01f) /** rotate(90, Vector3f(0, 1, 0))*/);
            auto inv_transform = std::make_shared<Transform>(inverse(*transform)); // translate(Vector3f(-0.5f, -0.5f, -1))*scale(0.2f,0.2f,0.2f)*rotate(-90,Vector3f(0,1,0));
            //append(meshs, load_mesh_triangles_from_obj(transform, inv_transform, "bunny.obj", "."));
            mesh = load_mesh<MeshFileFormat::PLY>(transform, inv_transform, "xyzrgb_dragon.ply");
        }
        auto primitives = create_mesh_triangle_primitives(mesh);
        auto blas = shared<CompactBLAS<MeshTrianglePrimitive, CompactMeshTrianglePrimitive>>(new CompactBLAS<MeshTrianglePrimitive, CompactMeshTrianglePrimitive>(primitives));

        {

            auto instance = shared<BLASInstance>(new BLASInstance(std::make_shared<AnimatedTransform>(blas_to_wrold), blas));
            instance_list.push_back(instance);
        }

        // {

        //     auto instance = shared<BLASInstance>(new BLASInstance(std::make_shared<AnimatedTransform>(blas_to_wrold2, 0, blas_to_wrold3, 1), blas));
        //     instance_list.push_back(instance);
        // }
    }

    auto blas_to_wrold4 = std::make_shared<Transform>(translate(-1.0f, 0, 0));
    auto world_to_blas4 = std::make_shared<Transform>(inverse(*blas_to_wrold4));

    {
        shared<HairStrands> hairstrands;
        {
            auto transform = std::make_shared<Transform>(translate(Vector3f(0, 0, 1.0f)) * scale(0.01f, 0.01f, 0.01f) * rotate(90, Vector3f(0, 0, 1)) * rotate(180, Vector3f(0, 1, 0)));
            auto inv_transform = std::make_shared<Transform>(inverse(*transform)); // translate(Vector3f(-0.5f, -0.5f, -1))*scale(0.2f,0.2f,0.2f)*rotate(-90,Vector3f(0,1,0));
            hairstrands = load_hairstrands<HairStrandsFileFormat::HAIR>(transform, inv_transform, "wCurly.hair", 0.1f);
        }
        auto primitives = create_hair_segment_primitives(hairstrands);
        auto blas = shared<CompactBLAS<HairSegmentPrimitive, CompactHairSegmentPrimitive>>(new CompactBLAS<HairSegmentPrimitive, CompactHairSegmentPrimitive>(primitives));

        {

            auto instance = shared<BLASInstance>(new BLASInstance(std::make_shared<AnimatedTransform>(blas_to_wrold4), blas));
            instance_list.push_back(instance);
        }
    }

    auto t = identity();
    {

        {
            auto transform = std::make_shared<Transform>(translate(0, -1, 0) * rotate(90, 1, 0, 0));
            auto inv_transform = std::make_shared<Transform>(inverse(*transform));
            shared<Mesh> mesh = create_plane(transform, inv_transform, 5, 5);
            std::vector<shared<MeshTrianglePrimitive>> primitives = create_mesh_triangle_primitives(mesh);
            auto blas = shared<CompactBLAS<MeshTrianglePrimitive, CompactMeshTrianglePrimitive>>(new CompactBLAS<MeshTrianglePrimitive, CompactMeshTrianglePrimitive>(primitives));
            auto instance = std::make_shared<BLASInstance>(std::make_shared<AnimatedTransform>(std::make_shared<Transform>(t)), blas);
            instance_list.push_back(instance);
        }
        {
            auto transform = std::make_shared<Transform>(translate(0, 1, 0) * rotate(90, 1, 0, 0));
            auto inv_transform = std::make_shared<Transform>(inverse(*transform));
            shared<Mesh> mesh = create_plane(transform, inv_transform, 5, 5);
            std::vector<shared<MeshTrianglePrimitive>> primitives = create_mesh_triangle_primitives(mesh);
            auto blas = shared<CompactBLAS<MeshTrianglePrimitive, CompactMeshTrianglePrimitive>>(new CompactBLAS<MeshTrianglePrimitive, CompactMeshTrianglePrimitive>(primitives));
            auto instance = std::make_shared<BLASInstance>(std::make_shared<AnimatedTransform>(std::make_shared<Transform>(t)), blas);
            instance_list.push_back(instance);
        }

        {
            auto transform = std::make_shared<Transform>(translate(0, 0, 2.5f));
            auto inv_transform = std::make_shared<Transform>(inverse(*transform));
            shared<Mesh> mesh = create_plane(transform, inv_transform, 5, 5);
            std::vector<shared<MeshTrianglePrimitive>> primitives = create_mesh_triangle_primitives(mesh);
            auto blas = shared<CompactBLAS<MeshTrianglePrimitive, CompactMeshTrianglePrimitive>>(new CompactBLAS<MeshTrianglePrimitive, CompactMeshTrianglePrimitive>(primitives));
            auto instance = std::make_shared<BLASInstance>(std::make_shared<AnimatedTransform>(std::make_shared<Transform>(t)), blas);
            instance_list.push_back(instance);
        }

        {
            auto transform = std::make_shared<Transform>(translate(2.5f, 0, 0) * rotate(90, 0, 1, 0));
            auto inv_transform = std::make_shared<Transform>(inverse(*transform));
            shared<Mesh> mesh = create_plane(transform, inv_transform, 5, 5);
            std::vector<shared<MeshTrianglePrimitive>> primitives = create_mesh_triangle_primitives(mesh);
            auto blas = shared<CompactBLAS<MeshTrianglePrimitive, CompactMeshTrianglePrimitive>>(new CompactBLAS<MeshTrianglePrimitive, CompactMeshTrianglePrimitive>(primitives));
            auto instance = std::make_shared<BLASInstance>(std::make_shared<AnimatedTransform>(std::make_shared<Transform>(t)), blas);
            instance_list.push_back(instance);
        }

        {
            auto transform = std::make_shared<Transform>(translate(-2.5f, 0, 0) * rotate(90, 0, 1, 0));
            auto inv_transform = std::make_shared<Transform>(inverse(*transform));
            shared<Mesh> mesh = create_plane(transform, inv_transform, 5, 5);
            std::vector<shared<MeshTrianglePrimitive>> primitives = create_mesh_triangle_primitives(mesh);
            auto blas = shared<CompactBLAS<MeshTrianglePrimitive, CompactMeshTrianglePrimitive>>(new CompactBLAS<MeshTrianglePrimitive, CompactMeshTrianglePrimitive>(primitives));
            auto instance = std::make_shared<BLASInstance>(std::make_shared<AnimatedTransform>(std::make_shared<Transform>(t)), blas);
            instance_list.push_back(instance);
        }
    }

    // //create light
    std::vector<Light *> lights;
    {
        {
            // auto transform = std::make_shared(translate(Vector3f(0.0f, 0.0f, 1.0f)) * rotate(90, 1, 0, 0));
            // auto inv_transform = std::make_shared(inverse(*transform));
            // auto point_light = new PointLight(transform, inv_transform, Color(10, 10, 10));
            // lights.push_back(point_light);
        } {
            auto transform = std::make_shared<Transform>(translate(Vector3f(0.0f, 1.0f, 0.5f)) * rotate(-90, 1, 0, 0));
            auto inv_transform = std::make_shared<Transform>(inverse(*transform));
            auto rect_light = new RectLight(transform, inv_transform, tungsten_lamp_3000k(50), false, 1, 1);
            lights.push_back(rect_light);
        }
    }

    // {
    //     auto transform = translate(Vector3f(0.0f, 1.0f, 1.0f))*rotate(90,1,0,0);
    //     auto inv_transform = inverse(transform);
    //     auto disk_light = std::std::make_shared<DiskLight>(&transform,&inv_transform,Color(10,10,10),false,0.2);
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

    auto image = film->get_image();
    image->write_to_png("mesh.png",PixelFormat::sRGBA8);
}
