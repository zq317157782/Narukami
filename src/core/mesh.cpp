#include "core/mesh.h"
#include "core/memory.h"
#include <vector>
NARUKAMI_BEGIN

  MeshData::MeshData(const Transform& object2wrold,int triangle_num,const uint32_t *indices,int vertex_num,const Point3f *vertices,const Normal3f *normals,const Point2f *uvs){
            assert(vertices!=nullptr);
            assert(indices!=nullptr);
           
            this->indices =  std::unique_ptr<uint32_t[]>(new uint32_t[triangle_num*3]);
            memcpy(this->indices.get(),indices,sizeof(uint32_t)*triangle_num*3);
            this->vertices=std::unique_ptr<Point3f[]>(new Point3f[vertex_num]);
            for(int i=0;i<vertex_num;++i){
                 this->vertices[i]=object2wrold(vertices[i]);
            }

            if(normals){
               this->normals=std::unique_ptr<Normal3f[]>(new Normal3f[vertex_num]);
               for(int i=0;i<vertex_num;++i){
                    this->normals[i]=object2wrold(normals[i]);
               }
            }

            if(uvs){
               this->uvs=std::unique_ptr<Point2f[]>(new Point2f[vertex_num]);
               for(int i=0;i<vertex_num;++i){
                    this->uvs[i]=uvs[i];
               }
            }
            

  }

std::vector<std::shared_ptr<MeshTriangle>> CreateMeshTriangles(const Transform* object2wrold,const Transform* world2object,int triangle_num,const uint32_t *indices,int vertex_num,const Point3f *vertices,const Normal3f *normals,const Point2f *uvs){
     std::shared_ptr<MeshData> mesh_data=std::make_shared<MeshData>(*object2wrold,triangle_num,indices,vertex_num,vertices,normals,uvs);
     std::vector<std::shared_ptr<MeshTriangle>> triangles;
     for(int i=0;i<triangle_num;++i){
          triangles.push_back(std::make_shared<MeshTriangle>(object2wrold,world2object,mesh_data,&indices[3*i]));
     }
     return triangles;
}

NARUKAMI_END