#include "core/mesh.h"
#include "core/memory.h"
#include <vector>

NARUKAMI_BEGIN
shared<Mesh> create_plane(const shared<Transform> &object2world, const shared<Transform> &world2object, const float width, const float height)
{
    float hw = width * 0.5f;
    float hh = height * 0.5f;
    std::vector<Point3f> vertices = {Point3f(hw, hh, 0), Point3f(hw, -hh, 0), Point3f(-hw, -hh, 0), Point3f(-hw, hh, 0)};
    std::vector<Normal3f> normals = {Normal3f(0, 0, 1), Normal3f(0, 0, 1), Normal3f(0, 0, 1), Normal3f(0, 0, 1)};
    std::vector<Point2f> uvs = {Point2f(0, 1), Point2f(0, 0), Point2f(1, 0), Point2f(1, 1)};
    std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

    std::vector<MeshFace> faces;
    for(int i=0;i<2;++i)
    {
        uint32_t vi[3] = {indices[i*3],indices[i*3 + 1],indices[i*3 + 2]};
        uint32_t ni[3] = {indices[i*3],indices[i*3 + 1],indices[i*3 + 2]};
        uint32_t ti[3] = {indices[i*3],indices[i*3 + 1],indices[i*3 + 2]};
        faces.push_back(MeshFace(vi,ni,ti));
    }
    
    std::vector<MeshSegment> segments;
    MeshSegment segment(faces);
    segments.push_back(segment);

    return std::make_shared<Mesh>(object2world,world2object,vertices,normals,uvs,segments);

}

// std::vector<shared<TriangleMesh>> create_disk(const shared<Transform>&object2wrold, const shared<Transform>&world2object, float radius, const uint32_t vertex_density)
// {
//     assert(radius > 0);
//     std::vector<Point3f> vertices = {Point3f(0, 0, 0)};
//     std::vector<Normal3f> normals = {Normal3f(0, 0, 1)};
//     std::vector<Point2f> uvs = {Point2f(0, 0)};
//     std::vector<uint32_t> indices;
//     float theta_step = 2.0f * PI / vertex_density;
//     float theta = 0;
//     for (uint32_t i = 1; i <= vertex_density; i++)
//     {
//         Point3f v(cos(theta) * radius, sin(theta) * radius, 0);
//         vertices.push_back(v);
//         uvs.push_back(Point2f(0, 0));
//         if (i > 1)
//         {
//             indices.push_back(i - 1);
//             indices.push_back(0);
//             indices.push_back(i);
//         }
//         theta += theta_step;
//     }

//     //last piece
//     indices.push_back(vertex_density);
//     indices.push_back(0);
//     indices.push_back(1);

//     return create_mesh_triangles(object2wrold, world2object, indices, vertices, normals, uvs);
// }

NARUKAMI_END