#include "core/narukami.h"
#include "core/geometry.h"
#include "core/mesh.h"
#include "core/imageio.h"
using namespace narukami;
int main(){
    
    std::vector<uint8_t> image;
    float data[128*128*3];
    SoABounds3f bound(SoAPoint3f(Point3f(0.1f,0.1f,1.0f),Point3f(0.1f,0.6f,1.0f),Point3f(0.6f,0.1f,1.0f),Point3f(0.6f,0.6f,1.0f)),SoAPoint3f(Point3f(0.4f,0.4f,1.0f),Point3f(0.4f,0.9f,1.0f),Point3f(0.9f,0.4f,1.0f),Point3f(0.9f,0.9f,1.0f)));
    int isPositive[3]={1,1,1};
	for (int i = 0; i<128*128; ++i) {
        if(collide(SoAPoint3f((i/128.0f)/128.0f,(i%128)/128.0f,0),SoAVector3f(INFINITE,INFINITE,1),float4(0),float4(1),isPositive,bound)){
            data[i*3] = 1;
            data[i*3+1] = 0;
            data[i*3+2] = 0; 
        }
        else{
            data[i*3] = 0;
            data[i*3+1] = 0;
            data[i*3+2] = 0; 
        }
	}

    narukami::write_image_to_file("bounds.png",data,128,128);
	//unsigned error = lodepng::encode(, image, 128, 128);
}