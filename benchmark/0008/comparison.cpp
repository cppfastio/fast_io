#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_driver/timer.h>
 
#include <charconv>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <cfloat>
 
#include "dragonbox/dragonbox_to_chars.h"
 
// both fast_io::pr_rsv_size<char, double> and jkj::dragonbox::max_output_string_length<jkj::dragonbox::ieee754_binary64> are 24 so 25 with trailing null terminator
constexpr int buf_size = 25;
 
inline void charconv_ofstream(const std::vector<double>& vec)
{
    fast_io::timer t(u8"charconv");
    std::ofstream ofs{"charconv.txt"};
    for (const auto e : vec)
    {
        char buf[buf_size]{0};
        std::to_chars(buf, buf + buf_size, e, std::chars_format::scientific);
        ofs << buf << '\n';
    }
}
 
inline void charconv_fast_io(const std::vector<double>& vec)
{
    fast_io::timer t(u8"charconv2");
    fast_io::obuf_file file{u8"charconv2.txt"};
    for (const auto e : vec)
    {
        char buf[buf_size]{0};
        std::to_chars(buf, buf + buf_size, e, std::chars_format::scientific);
        println(file, fast_io::mnp::os_c_str(buf));
    }
}
 
inline void fast_io_ofstream(const std::vector<double>& vec)
{
    fast_io::timer t(u8"fast_io");
    std::ofstream ofs{"fast_io.txt"};
    for(const auto e : vec)
    {
        char buf[buf_size]{0};
        fast_io::pr_rsv_to_c_array(buf, fast_io::mnp::scientific(e));
        ofs << buf << '\n';
    }
}
 
inline void fast_io_fast_io(const std::vector<double>& vec)
{
    fast_io::timer t(u8"fast_io2");
    fast_io::obuf_file file{u8"fast_io2.txt"};
    for (const auto e : vec)
    {
        char buf[buf_size]{0};
        fast_io::pr_rsv_to_c_array(buf, fast_io::mnp::scientific(e));
        println(file, fast_io::mnp::os_c_str(buf));
    }
}
 
inline void dragonbox_ofstream(const std::vector<double>& vec)
{
    fast_io::timer t(u8"dragonbox");
    std::ofstream ofs{"dragonbox.txt"};
    for(const auto e : vec)
    {
        char buf[buf_size]{0};
        jkj::dragonbox::to_chars(e, buf);
        ofs << buf << '\n';
    }
}
 
inline void dragonbox_fast_io(const std::vector<double>& vec)
{
    fast_io::timer t(u8"dragonbox2");
    fast_io::obuf_file file{u8"dragonbox2.txt"};
    for(const auto e : vec)
    {
        char buf[buf_size]{0};
        jkj::dragonbox::to_chars(e, buf);
        println(file, fast_io::mnp::os_c_str(buf));
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
 
    charconv_ofstream(vec);
    charconv_fast_io(vec);
    
    fast_io_ofstream(vec);
    fast_io_fast_io(vec);
    
    dragonbox_ofstream(vec);
    dragonbox_fast_io(vec);
 
    return 0;
}