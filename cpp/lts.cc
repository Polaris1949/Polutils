#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 2) return 0;
    char* s = argv[1];
    std::cout << std::hex;
    while (*s)
    {
        switch (*s)
        {
        case '\'':
            std::cout << "\\\'";
            break;
        case '\"':
            std::cout << "\\\"";
            break;
        case '\?':
            std::cout << "\\?";
            break;
        case '\\':
            std::cout << "\\\\";
            break;
        case '\a':
            std::cout << "\\a";
            break;
        case '\b':
            std::cout << "\\b";
            break;
        case '\f':
            std::cout << "\\f";
            break;
        case '\n':
            std::cout << "\\n";
            break;
        case '\r':
            std::cout << "\\r";
            break;
        case '\t':
            std::cout << "\\t";
            break;
        case '\v':
            std::cout << "\\v";
            break;
        default:
            if (std::isprint(*s))
                std::cout << *s;
            else
                std::cout << "\\x" << static_cast<unsigned int>(static_cast<unsigned char>(*s));
        }
        ++s;
    }
    std::cout << '\n';
    return 0;
}