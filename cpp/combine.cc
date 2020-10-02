#include "bgil.h"
#include <iostream>

#if POL_DEBUG
#include <fstream>
std::ofstream fout;
#endif

void x_gradient(const bg::rgba8_view_t& src_alpha, const bg::rgba8_view_t& src_rgb, const bg::rgba8_view_t& dst)
{
    for (int y=0; y<src_rgb.height(); ++y)
        for (int x=0; x<src_rgb.width(); ++x)
        {
            auto& pd = dst(x,y);
            auto& pr = src_rgb(x,y);
            auto& pa = src_alpha(x,y);
            auto& grey = bg::get_color(pa, bg::red_t());
            if ((unsigned)grey)
            {
                pd = pr;
                bg::get_color(pd, bg::alpha_t()) = grey;
            }
            /*else
            {
                bg::get_color(pd, bg::red_t()) = 0;
                bg::get_color(pd, bg::green_t()) = 0;
                bg::get_color(pd, bg::blue_t()) = 0;
                bg::get_color(pd, bg::alpha_t()) = 255;
            }*/
#if POL_DEBUG
            fout << '(' << x << ',' << y << ')' << ' '
                << (unsigned)bg::get_color(pd, bg::red_t()) << ' '
                << (unsigned)bg::get_color(pd, bg::green_t()) << ' '
                << (unsigned)bg::get_color(pd, bg::blue_t()) << ' '
                << (unsigned)bg::get_color(pd, bg::alpha_t()) << '\n';
#endif
        }
}

int main(int argc, char* argv[]) try
{
    if (argc < 2) return 0;
    if (argc != 4) throw std::runtime_error("missing argument");
#if POL_DEBUG
    fout.open(std::string(argv[3]) + ".txt");
#endif
    bg::rgba8_image_t img_alpha;
    bg::read_image(argv[1], img_alpha, bg::png_tag());
    bg::rgba8_image_t img_rgb;
    bg::read_image(argv[2], img_rgb, bg::png_tag());
    bg::rgba8_image_t img_beta(img_alpha.height(), img_alpha.width());
    x_gradient(bg::view(img_alpha), bg::view(img_rgb), bg::view(img_beta));
    bg::write_view(argv[3], bg::view(img_beta), bg::png_tag());
#if POL_DEBUG
    fout.close();
#endif
    return 0;
} catch (...) { std::terminate(); }
