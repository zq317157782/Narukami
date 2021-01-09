#include "core/narukami.h"
#include "core/film.h"
#include "core/spectrum.h"
using namespace narukami;
int main()
{
    Spectrum::init();
    {
        Film film(Point2i(128, 128), Bounds2f(Point2f(0, 0), Point2f(1, 1)));
        for (int h = 0; h < 128; ++h)
        {
            for (int w = 0; w < 128; ++w)
            {
                film.add_sample(Point2f(w, h), Spectrum::X, 1);
            }
        }
        auto image = film.get_image();
        image->write_to_png("CIE_X_to_RGB.png");
    }

    {
        Film film(Point2i(128, 128), Bounds2f(Point2f(0, 0), Point2f(1, 1)));
        for (int h = 0; h < 128; ++h)
        {
            for (int w = 0; w < 128; ++w)
            {
                film.add_sample(Point2f(w, h), Spectrum::Y, 1);
            }
        }
        auto image = film.get_image();
        image->write_to_png("CIE_Y_to_RGB.png");
    }

    {
        Film film(Point2i(128, 128), Bounds2f(Point2f(0, 0), Point2f(1, 1)));
        for (int h = 0; h < 128; ++h)
        {
            for (int w = 0; w < 128; ++w)
            {
                film.add_sample(Point2f(w, h), Spectrum::Z, 1);
            }
        }
        auto image = film.get_image();
        image->write_to_png("CIE_Z_to_RGB.png");

        Image a("mesh.png", PixelFormat::sRGB8);
        a.write_to_png("CIE_Z_to_RGB2.png",PixelFormat::R8);
    }
    //narukami::write_image_to_file("texture_space.png",data,128,128);
}