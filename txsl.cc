#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <filesystem>

#define TXSL_VERSION "1.0"
namespace fs = std::filesystem;

std::string help_msg =
"Usage: %s [OPTION]... [STRING]\n"
"Make convertion between text and string literal.\n\n"
"Options:\n"
"      -s         convert text to string literal (aka --mode=s)\n"
"      -t         convert string literal to text (aka --mode=t)\n"
"      -r         open response mode (only support -s)\n"
"      -e         enable escape sequence (aka --escape=std)\n"
"      -o         octal number for escape sequence (aka --number=oct)\n"
"      -x         hexdecimal number for escape sequence (aka --number=hex)\n"
"      -n         number only for escape sequence (aka --escape=number)\n"
"      --mode={s|t} (default=s)\n"
"                 select convertion mode\n"
"      --escape={never|std|strict|number} (default=std)\n"
"                 control escape sequence\n"
"      --number={oct|hex} (default=hex)\n"
"                 escape sequence number\n"
"      --help     display this help and exit\n"
"      --version  ouput version information and exit";
std::string version_msg =
"%s (Polaris polutils) " TXSL_VERSION "\n"
"Copyright (C) 2019 Free Software Foundation, Inc.\n"
"License GPLv3+: GNU GPL version 3 or later "
"<http://gnu.org/licenses/gpl.html>.\n"
"This is free software: you are free to change and redistribute it.\n"
"There is NO WARRANTY, to the extent permitted by law.\n\n"
"Written by Polaris Zhao.";
std::string exe_name;

int optmode = 0; //0:default,1:s,2:t
int optr = 0;
int escape = 0; //0:default,1:never,2:std,3:strict,4:number
int opte = 0;
int optn = 0;
int opthelp = 0;
int optversion = 0;
std::string error_msg;

bool parse_cmd(int, char**);
int check_opt(const char*);
bool check_opt_value();
bool parse_opt(const char*);
bool check_opt_arg(const char*, int);
bool parse_arg_mode(const char*);
bool parse_exe_name();
void error_opt_bad(const char*);
void error_opt_conflict(const char*, const char*);
void error_opt_unrecognized(const char*);
void error_arg_bad(const char*, const char*);
void error_arg_missing(const char*);
void sorry_qaq();
void show_error();
int work_mode_s();
int work_mode_s_without_r();
int work_mode_s_with_r();
int work_mode_t();

bool parse_exe_name(const char* name)
{
    exe_name = fs::path{name}.filename();
    help_msg.replace(help_msg.find("%s"), 2, exe_name);
    version_msg.replace(version_msg.find("%s"), 2, exe_name);
    return true;
}

int check_opt(const char* opt)
{
    if (opt[0] != '-')
    {
        error_opt_bad(opt);
        return 0;
    }
    else
    {
        if (opt[1] == '-')
        {
            if (std::islower(opt[2])) return 2;
            else {error_opt_bad(opt); return 0;}
        }
        else return 1;
    }
}

bool parse_opt(const char* opt)
{
    std::string topt{opt};

    if (topt == "help")
    {
        opthelp = 1;
    }
    else if (topt == "version")
    {
        optversion = 1;
    }
    else if (topt.substr(0, 4) == "mode")
    {
        if (!check_opt_arg(opt, 4)) return false;
        if (!parse_arg_mode(opt + 5)) return false;
    }
    else if (topt == "s")
    {
        if (optmode == 2)
        {
            error_opt_conflict("-s", "-t");
            return false;
        }
        optmode = 1;
    }
    else if (topt == "t")
    {
        if (optmode == 1)
        {
            error_opt_conflict("-t", "-s");
            return false;
        }
        optmode = 2;
    }
    else if (topt == "r")
    {
        if (optmode == 2)
        {
            error_opt_conflict("-r", "-t");
            return false;
        }
        optr = 1;
    }
    else if (topt == "e")
    {
        opte = 1;
    }
    else if (topt == "n")
    {
        optn = 1;
    }
    else
    {
        error_opt_unrecognized(opt);
        return false;
    }
    return true;
}

bool check_opt_arg(const char* opt, int len)
{
    if (*(opt + len) == '=' && *(opt + len + 1) != '\0')
        return true;
    else
    {
        error_arg_missing(opt);
        return false;
    }
}

bool parse_arg_mode(const char* arg)
{
    std::string targ{arg};
    if (targ == "s") optmode = 1;
    else if (targ == "t") optmode = 2;
    else
    {
        error_arg_bad(arg, "--mode=");
        return false;
    }
    return true;
}

bool parse_cmd(int argc, char** argv)
{
    if (!parse_exe_name(argv[0]))
        return false;

    for (int i = 1; i < argc; ++i)
    {
        std::string s{argv[i]};
        int t = check_opt(argv[i]);
        if (t == 0) return false;
        if (!parse_opt(argv[i] + t)) return false;
    }
    return true;
}

void error_opt_conflict(const char* optx, const char* opty)
{
    error_msg = "set conflict option \'%s\' after \'%s\'";
    error_msg.replace(error_msg.find("%s"), 2, optx);
    error_msg.replace(error_msg.find("%s"), 2, opty);
}

void error_opt_unrecognized(const char* opt)
{
    error_msg = "unrecognized command line option \'%s\'";
    error_msg.replace(error_msg.find("%s"), 2, opt);
}

void error_opt_bad(const char* opt)
{
    error_msg = "bad command line option \'%s\'";
    error_msg.replace(error_msg.find("%s"), 2, opt);
}

void error_arg_bad(const char* arg, const char* opt)
{
    error_msg = "bad argument \'%s\' for option \'%s\'";
    error_msg.replace(error_msg.find("%s"), 2, arg);
    error_msg.replace(error_msg.find("%s"), 2, opt);
}

void error_arg_missing(const char* opt)
{
    error_msg = "missing argument for option \'%s\'";
    error_msg.replace(error_msg.find("%s"), 2, opt);
}

void sorry_qaq()
{ std::cerr << exe_name << ": fatal error: not implemented; coo~ QAQ!\n"; }

void show_error()
{ std::cerr << exe_name << ": error: " << error_msg << '\n'; }

bool check_opt_value()
{
    if (optmode == 0) optmode = 1;
    return true;
}

int work_mode_s()
{
    if (optr == 1) return work_mode_s_with_r();
    else return work_mode_s_without_r();
}

int work_mode_s_without_r()
{
    std::string s;
    std::cout << std::hex << std::setfill('0');
    while (std::getline(std::cin, s))
    {
        std::cout << '\"';
        for (char c : s)
        {
            if (optn)
            {
                std::cout << "\\x" << std::setw(2) << (int(c)&0xff);
                continue;
            }

            if (c == '\'')
            {
                std::cout << "\\\'";
            }
            else if (c == '\"')
            {
                std::cout << "\\\"";
            }
            else if (c == '\\')
            {
                std::cout << "\\\\";
            }
            else if (c == '\0')
            {
                std::cout << "\\0";
            }
            else if (std::iscntrl(c))
            {
                std::cout << "\\x" << std::setw(2) << (int(c)&0xff);
            }
            else
            {
                std::cout << c;
            }
        }
        std::cout << "\"\n";
    }
    return 0;
}

int work_mode_s_with_r()
{
    sorry_qaq();
    return 1;
}

int work_mode_t()
{
    sorry_qaq();
    return 1;
}

int main(int argc, char** argv)
{
    if (!parse_cmd(argc, argv))
    {
        show_error();
        return 1;
    }

    check_opt_value();

    if (opthelp)
    {
        std::cerr << help_msg << '\n';
        return 0;
    }

    if (optversion)
    {
        std::cerr << version_msg << '\n';
        return 0;
    }

    if (optmode == 1) return work_mode_s();
    else if (optmode == 2) return work_mode_t();

    return 2;
}
