#include "bgil.h"
#include <iostream>
#include <fstream>

std::ofstream fout;

void x_gradient(const bg::rgba8_view_t& src)
{
    for (int y=0; y<src.height(); ++y)
        for (int x=0; x<src.width(); ++x)
        {
            auto& ps = src(x,y);
            fout << '(' << x << ',' << y << ')' << ' '
                << (unsigned)bg::get_color(ps, bg::red_t()) << ' '
                << (unsigned)bg::get_color(ps, bg::green_t()) << ' '
                << (unsigned)bg::get_color(ps, bg::blue_t()) << ' '
                << (unsigned)bg::get_color(ps, bg::alpha_t()) << '\n';
        }
}

int main(int argc, char* argv[]) try
{
    if (argc < 2) return 0;
    if (argc != 2) throw std::runtime_error("missing argument");
    fout.open(std::string(argv[1]) + ".txt");
    if (!fout) throw std::runtime_error("cannot open output file");
    bg::rgba8_image_t img;
    bg::read_image(argv[1], img, bg::png_tag());
    x_gradient(bg::view(img));
    fout.close();
    return 0;
} catch (...) { std::terminate(); }
