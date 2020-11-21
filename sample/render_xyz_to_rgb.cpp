#include "core/narukami.h"
#include "core/imageio.h"
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
        film.write_to_file("CIE_X_to_RGB.png");
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
        film.write_to_file("CIE_Y_to_RGB.png");
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
        film.write_to_file("CIE_Z_to_RGB.png");
    }
    //narukami::write_image_to_file("texture_space.png",data,128,128);
}