#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_driver/timer.h>
#include "dragonbox/dragonbox_to_chars.h"
#include <random>
#include <vector>
#include <cfloat>

constexpr int buf_size = 25;

inline void dragonbox_fast_io(const std::vector<double>& vec)
{
    fast_io::timer t(u8"dragonbox2");
    fast_io::obuf_file file{u8"dragonbox2.txt"};
    for(const auto e : vec)
    {
        char buf[buf_size]{0};
        auto i{jkj::dragonbox::to_chars(e, buf)};
        *i='\n';
        ++i;
        write(file,buf,i);
    }
}

int main()
{
    constexpr std::size_t N = 10'000'000;
    
    std::mt19937_64 eng{};
    std::uniform_real_distribution<double> dis(DBL_MIN, DBL_MAX);
    std::vector<double> vec;
    vec.reserve(N);
    for(std::size_t i{}; i != N; ++i)
    {
        vec.emplace_back(dis(eng));
    }
    dragonbox_fast_io(vec);
}
