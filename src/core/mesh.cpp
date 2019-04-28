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

  std::vector<SoATriangle> cast2SoA(const std::vector<std::shared_ptr<MeshTriangle>>& triangles,uint32_t start,uint32_t count){
        assert(count>0);
        assert((start+count)<=triangles.size());

        size_t soa_count = (uint32_t)count/SSE_FLOAT_COUNT + 1;

        std::vector<Point3f> v0_array;
        std::vector<Vector3f> e1_array;
        std::vector<Vector3f> e2_array;

        for(size_t i=0;i<soa_count*SSE_FLOAT_COUNT;++i){
            if(i<count){
                auto v0 = (*triangles[start+i])[0];
                auto e1 = (*triangles[start+i])[1]-v0;
                auto e2 = (*triangles[start+i])[2]-v0;
               
                v0_array.push_back(v0);
                e1_array.push_back(e1);
                e2_array.push_back(e2);
            }else{
                v0_array.push_back(Point3f());
                e1_array.push_back(Vector3f());
                e2_array.push_back(Vector3f());
            }
           
        }
        std::vector<SoATriangle> soa_triangles;
          
        for(size_t i=0;i<soa_count;++i){
            SoATriangle triangle;
            triangle.v0=load(&v0_array[i*SSE_FLOAT_COUNT]);
            triangle.e1=load(&e1_array[i*SSE_FLOAT_COUNT]);
            triangle.e2=load(&e2_array[i*SSE_FLOAT_COUNT]);
            soa_triangles.push_back(triangle);
        }
        
        return soa_triangles;
      
    }

std::vector<std::shared_ptr<MeshTriangle>> create_mesh_triangles(const Transform* object2wrold,const Transform* world2object,int triangle_num,const uint32_t *indices,int vertex_num,const Point3f *vertices,const Normal3f *normals,const Point2f *uvs){
     std::shared_ptr<MeshData> mesh_data=std::make_shared<MeshData>(*object2wrold,triangle_num,indices,vertex_num,vertices,normals,uvs);
     std::vector<std::shared_ptr<MeshTriangle>> triangles;
     for(int i=0;i<triangle_num;++i){
          triangles.push_back(std::make_shared<MeshTriangle>(object2wrold,world2object,mesh_data,&indices[3*i]));
     }
     return triangles;
}

NARUKAMI_END