/*
MIT License

Copyright (c) 2019 ZhuQian

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "core/imageio.h"
#include "lodepng.h"

NARUKAMI_BEGIN

static const char* suffix(const char* file_name){ const char* suf; const char *ptr, c = '.'; ptr = strrchr(file_name, c); int pos = ptr - file_name; suf = &file_name[pos + 1]; return suf; }

void write_image_to_file(const char* file_name,const float* data,const int width,const int height){
    assert(file_name);
    auto suf=suffix(file_name);
    if((std::strcmp(suf,"png")==0)||(std::strcmp(suf,"PNG")==0)){
        std::vector<uint8_t> image;
		for (int i = 0; i<width*height; ++i) {
			float rgb[3];
			rgb[0] = data[i * 3];
			rgb[1] = data[i * 3 + 1];
			rgb[2] = data[i * 3 + 2];

			image.push_back(rgb[0] * 255);//R
			image.push_back(rgb[1] * 255);//G
			image.push_back(rgb[2] * 255);//B
			image.push_back(255);		//A
		}

		unsigned error = lodepng::encode(file_name, image, width, height);
		if (error) {
            std::cerr<< error << ": " << lodepng_error_text(error)<<std::endl;
            exit(-1);
        }
    }
    else{
        std::cerr<<"the file "<<file_name<<" is unsupported!"<<std::endl;
        exit(-1);
    }
}

NARUKAMI_END