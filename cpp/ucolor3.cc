#include <iostream>
#include <pol/format>
#define POL_EXT 1

int main()
{
    while (true)
    {
        double dr, dg, db;
#if POL_EXT
        char sep; // ','
        std::cin >> dr >> sep >> dg >> sep >> db;
#else
        std::cin >> dr >> dg >> db;
#endif
        if (!std::cin) break;
        int r = dr * 255.0;
        int g = dg * 255.0;
        int b = db * 255.0;
        fmt::print(fmt::fg(fmt::rgb(r, g, b)), "#{:02x}{:02x}{:02x}", r, g, b);
        fmt::print("\n");
    }
    return 0;
}
