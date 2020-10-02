#include "bgil.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cmath>
#include <pol/serial>
namespace fs = std::filesystem;

std::ifstream fin;
pol::memory_buffer fsize;
const int hsum = 2;

void x_gradient(const bg::rgba8_view_t& dst)
{
    int head = 0;
    for (int y=0; y<dst.height(); ++y)
        for (int x=0; x<dst.width(); ++x)
        {
            auto& ps = dst(x, y);
            if (head < hsum)
            {
                bg::get_color(ps, bg::red_t())   = fsize.data()[head << 2];
                bg::get_color(ps, bg::green_t()) = fsize.data()[head << 2 | 1];
                bg::get_color(ps, bg::blue_t())  = fsize.data()[head << 2 | 2];
                bg::get_color(ps, bg::alpha_t()) = fsize.data()[head << 2 | 3];
                ++head;
            }
            else
            {
                char r, g, b, a;
                if (!fin.get(r)) return;
                bg::get_color(ps, bg::red_t()) = r;
                if (!fin.get(g)) return;
                bg::get_color(ps, bg::green_t()) = g;
                if (!fin.get(b)) return;
                bg::get_color(ps, bg::blue_t()) = b;
                if (!fin.get(a)) return;
                bg::get_color(ps, bg::alpha_t()) = a;
            }
        }
}

int main(int argc, char* argv[]) try
{
    if (argc < 2) return 0;
    if (argc != 2) throw std::runtime_error("bad command");
    uint64_t size = fs::file_size(argv[1]);
    fsize = std::move(pol::serialize(size));
    uint64_t pix = std::ceil(double(size)/4);
    pix += hsum;
    uint64_t len = std::ceil(std::sqrt(double(pix)));
    bg::rgba8_image_t img(len, len);
    fin.open(argv[1]);
    x_gradient(bg::view(img));
    std::string fout(argv[1]);
    fout += ".png";
    bg::write_view(fout, bg::view(img), bg::png_tag());
    return 0;
}
catch (...) { std::terminate(); }
