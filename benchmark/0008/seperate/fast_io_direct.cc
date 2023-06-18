#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_driver/timer.h>
#include <random>
#include <vector>
#include <cfloat>
using namespace fast_io::io;

[[maybe_unused]] constexpr int buf_size = 25;

inline void fast_io_fast_io(const std::vector<double>& vec)
{
    fast_io::timer t(u8"fast_io_direct");
    fast_io::obuf_file file{u8"fast_io_direct.txt"};
    for (const auto e : vec)
    {
        println(file,fast_io::mnp::scientific(e));
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
    fast_io_fast_io(vec);
}
