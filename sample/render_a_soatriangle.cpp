#include "core/narukami.h"
#include <vector>
#include "lodepng.h"
#include "core/geometry.h"
int main(){
    narukami::TrianglePack triangle;
    triangle.v0 = narukami::Point3fPack(0,0,0);
    triangle.e1 = narukami::Vector3fPack(1,0,0);
    triangle.e2 = narukami::Vector3fPack(0,1,0);

    

    std::vector<uint8_t> image;
	for (int i = 0; i<128*128; ++i) {
		narukami::Point2f uv;
		float t;
        narukami::RayPack ray(narukami::Point3f((i/128.0f)/128.0f,(i%128)/128.0f,0),narukami::Vector3f(0,0,1));
        bool b=intersect(ray,triangle,&t,&uv,nullptr);
		
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
	unsigned error = lodepng::encode("soatriangle.png", image, 128, 128);
}