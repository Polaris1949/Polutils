#include <filesystem>
#include <iostream>
#include <fstream>
#include <cstdlib>
namespace fs = std::filesystem;

int main()
{
    fs::path pin = "in";
    fs::path pout = "out";
    fs::create_directory(pout);
    std::ofstream flog("failure.log");
    for (auto& p : fs::recursive_directory_iterator(pin))
    {
        std::string sa = p.path().string();
        std::size_t pos = sa.find("_alpha");
        if (pos == std::string::npos)
            continue;
        std::string sr = sa;
        sr.replace(pos, 6, "_rgb");
        if (!fs::exists(sr))
            continue;
        std::string sb = sa;
        sb.erase(pos, 6);
        sb = fs::path(sb).filename().string();
        std::cout << sb << '\n';
        fs::path fout = pout / sb;
        std::string cmd = "./combine ";
        cmd += sa; cmd += ' ';
        cmd += sr; cmd += ' ';
        cmd += fout.string();
        if (std::system(cmd.data()))
            flog << sb << '\n';
    }
    return 0;
}