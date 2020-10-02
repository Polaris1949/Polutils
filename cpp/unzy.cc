#include "bgil.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cmath>
#include <pol/serial>
namespace fs = std::filesystem;

std::ofstream fout;
pol::mbyte fsize[8];
const int hsum = 2;

void x_gradient(const bg::rgba8_view_t& dst)
{
    int head = 0;
    uint64_t size = 0;
    uint64_t ocnt = 0;
    for (int y=0; y<dst.height(); ++y)
        for (int x=0; x<dst.width(); ++x)
        {
            auto& ps = dst(x, y);
            if (head < hsum)
            {
                fsize[head << 2]     = bg::get_color(ps, bg::red_t());
                fsize[head << 2 | 1] = bg::get_color(ps, bg::green_t());
                fsize[head << 2 | 2] = bg::get_color(ps, bg::blue_t());
                fsize[head << 2 | 3] = bg::get_color(ps, bg::alpha_t());
                ++head;
                if (head == hsum)
                    size = pol::deserialize<uint64_t>(pol::memory_view(fsize, 8));
            }
            else
            {
                if (ocnt++ >= size) return;
                fout.put(char(bg::get_color(ps, bg::red_t())));
                if (ocnt++ >= size) return;
                fout.put(char(bg::get_color(ps, bg::green_t())));
                if (ocnt++ >= size) return;
                fout.put(char(bg::get_color(ps, bg::blue_t())));
                if (ocnt++ >= size) return;
                fout.put(char(bg::get_color(ps, bg::alpha_t())));
            }
        }
}

int main(int argc, char* argv[]) try
{
    if (argc < 2) return 0;
    if (argc != 2) throw std::runtime_error("bad command");
    bg::rgba8_image_t img;
    bg::read_image(argv[1], img, bg::png_tag());
    fout.open(fs::path(argv[1]).replace_extension().string());
    x_gradient(bg::view(img));
    return 0;
}
catch (...) { std::terminate(); }
