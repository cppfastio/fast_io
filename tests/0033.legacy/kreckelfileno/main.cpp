/*
https://www.ginac.de/~kreckel/fileno/main.cpp
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include "fileno.hpp"

void check_default_streams()
{
    int fd;
    fd = fileno(std::cin);
    if (fd != 0)
        std::cerr << "fileno(std::cin)==" << fd << std::endl;
    fd = fileno(std::cout);
    if (fd != 1)
        std::cerr << "fileno(std::cout)==" << fd << std::endl;
    fd = fileno(std::cerr);
    if (fd != 2)
        std::cerr << "fileno(std::cerr)==" << fd << std::endl;
    fd = fileno(std::clog);
    if (fd != 2)
        std::cerr << "fileno(std::clog)==" << fd << std::endl;
}

void check_default_wstreams()
{
    int fd;
    fd = fileno(std::wcin);
    if (fd != 0)
        std::cerr << "fileno(std::wcin)==" << fd << std::endl;
    fd = fileno(std::wcout);
    if (fd != 1)
        std::cerr << "fileno(std::wcout)==" << fd << std::endl;
    fd = fileno(std::wcerr);
    if (fd != 2)
        std::cerr << "fileno(std::wcerr)==" << fd << std::endl;
    fd = fileno(std::wclog);
    if (fd != 2)
        std::cerr << "fileno(std::wclog)==" << fd << std::endl;
}

int main()
{
    std::ios_base::sync_with_stdio(true);
    check_default_streams();
    check_default_wstreams();
    std::ios_base::sync_with_stdio(false);
    check_default_streams();
    check_default_wstreams();

    int fd;
    std::ifstream ist("/dev/zero");
    fd = fileno(ist);
    if (fd != 3)
        std::cerr << "fileno(\"/dev/zero\")==" << fd << std::endl;
    std::ofstream ost("/dev/null");
    fd = fileno(ost);
    if (fd != 4)
        std::cerr << "fileno(\"/dev/null\")==" << fd << std::endl;
    std::stringstream sst;
    fd = fileno(sst);
    if (fd != -1)
        std::cerr << "fileno(stringstream)==" << fd << std::endl;

    std::wifstream wist("/dev/zero");
    fd = fileno(wist);
    if (fd != 5)
        std::cerr << "fileno(L\"/dev/zero\")==" << fd << std::endl;
    std::wifstream wost("/dev/null");
    fd = fileno(wost);
    if (fd != 6)
        std::cerr << "fileno(L\"/dev/null\")==" << fd << std::endl;
    std::wstringstream wsst;
    fd = fileno(wsst);
    if (fd != -1)
        std::cerr << "fileno(wstringstream)==" << fd << std::endl;
    return 0;
}
