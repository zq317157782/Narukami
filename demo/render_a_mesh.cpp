#include "core/narukami.h"
#include <vector>
#include "lodepng.h"
#include "core/geometry.h"
#include "core/transform.h"
#include "core/mesh.h"
using namespace narukami;
int main(){
    

   
     Point3f vertices[4]={Point3f(0,1,1),Point3f(0,0,1),Point3f(1,0,1),Point3f(1,1,1)};
     Point2f uvs[4] = {Point2f(0,1),Point2f(0,0),Point2f(1,0),Point2f(1,1)};
     uint32_t indices[6]={0,1,3,1,2,3};
     auto transform = translate(Vector3f(0,0,0));
     auto transform2 = translate(Vector3f(0,0,0));
     auto triangles=create_mesh_triangles(&transform,&transform2,2,indices,4,vertices,nullptr,uvs);
    
     auto soa_triangles=cast2SoA(triangles,0,2);
    std::cout<<soa_triangles[0];

    std::vector<uint8_t> image;
	for (int i = 0; i<128*128; ++i) {
		narukami::Point2f uv;
		float t;
        narukami::SoARay ray(narukami::Point3f((i/128.0f)/128.0f,(i%128)/128.0f,0),narukami::Vector3f(0,0,1));
        int index;
        bool b =intersect(ray,soa_triangles[0],&t,&uv,&index);
       // std::cout<<index;
        // for(int j=0;j<soa_triangles.size();++j){
        //     b=(b||intersect(ray,soa_triangles[j],&t,&uv,&index));
        //     if(b){
        //         break;
        //     }
        // }
        uv=triangles[index]->sample_uv(uv);

		float rgb[3];
		if (b){
            rgb[0] = uv.x;
		    rgb[1] = uv.y;
		    rgb[2] = 0;
        }
        else{
            rgb[0] = 0;
		    rgb[1] = 0;
		    rgb[2] = 0;
        }
        
		image.push_back(rgb[0] * 255);//R
		image.push_back(rgb[1] * 255);//G
		image.push_back(rgb[2] * 255);//B
		image.push_back(255);		//A
	}
	unsigned error = lodepng::encode("mesh.png", image, 128, 128);
}