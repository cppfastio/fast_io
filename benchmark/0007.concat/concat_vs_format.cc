#include<string>
#include<fast_io.h>
#include<format>
#include<vector>
#if __has_include(<fmt/core.h>) && defined(ENABLE_FMT_BENCH)
#include <fmt/core.h>
#if __has_include(<fmt/compile.h>)
#include <fmt/compile.h>
#endif
#endif

struct benchmark_return
{
	std::size_t total_size{};
	fast_io::unix_timestamp timestamp{};
};

template<typename Func>
inline benchmark_return benchmark(Func meth)
{
	std::size_t total_size{};
	auto start{fast_io::posix_clock_gettime(fast_io::posix_clock_id::monotonic_raw)};
	for(std::uint_least16_t r{};r!=256;++r)
	{
		for(std::uint_least16_t g{};g!=256;++g)
		{
			for(std::uint_least16_t b{};b!=256;++b)
			{
				total_size+=meth(r,g,b).size();
			}
		}
	}
	return {total_size,fast_io::posix_clock_gettime(fast_io::posix_clock_id::monotonic_raw)-start};
}

inline std::string color_format(std::uint_least8_t r,std::uint_least8_t g,std::uint_least8_t b)
{
	return std::format("Red: {}, Green: {}, Blue: {}",r,g,b);
}

inline std::string color_concat(std::uint_least8_t r,std::uint_least8_t g,std::uint_least8_t b)
{
	return fast_io::concat("Red: ",r,", Green: ",g,", Blue: ",b);
}

#if __has_include(<fmt/core.h>) && defined(ENABLE_FMT_BENCH)

inline std::string color_fmt_format(std::uint_least8_t r,std::uint_least8_t g,std::uint_least8_t b)
{
	return fmt::format("Red: {}, Green: {}, Blue: {}",r,g,b);
}

#if __has_include(<fmt/compile.h>)
inline std::string color_fmt_format_compile(std::uint_least8_t r,std::uint_least8_t g,std::uint_least8_t b)
{
	return fmt::format(FMT_COMPILE("Red: {}, Green: {}, Blue: {}"),r,g,b);
}

#endif

#endif

int main()
{
	auto format_time = benchmark(color_format);
	auto concat_time = benchmark(color_concat);
#if __has_include(<fmt/core.h>) && defined(ENABLE_FMT_BENCH)
	auto fmt_format_time = benchmark(color_fmt_format);
#if __has_include(<fmt/compile.h>)
	auto fmt_format_compile_time = benchmark(color_fmt_format_compile);
#endif
#endif    
    
	print("std::format (total size:",format_time.total_size,") took ",format_time.timestamp,"s.\n"
		"fast_io::concat (total size: ",concat_time.total_size,") took ",concat_time.timestamp,"s.\n"
#if __has_include(<fmt/core.h>) && defined(ENABLE_FMT_BENCH)
		"fmt::format (total size:",fmt_format_time.total_size,") took ",fmt_format_time.timestamp,"s.\n"
#if __has_include(<fmt/compile.h>)
		"fmt::format compile (total size:",fmt_format_compile_time.total_size,") took ",fmt_format_compile_time.timestamp,"s.\n"
#endif
#endif
    	);
}

