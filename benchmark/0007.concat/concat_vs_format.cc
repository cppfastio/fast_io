#include<string>
#include<fast_io.h>
#include<version>
#if __has_include(<format>) && !defined(DISABLE_STD_FORMAT_BENCH)
#include<format>
#endif
#if __has_include(<fmt/core.h>) && defined(ENABLE_FMT_BENCH)
#include <fmt/core.h>
#if __has_include(<fmt/compile.h>)
#include <fmt/compile.h>
#endif
#endif
#include<sstream>

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
				total_size+=meth(static_cast<::std::uint_least8_t>(r),static_cast<::std::uint_least8_t>(g),static_cast<::std::uint_least8_t>(b)).size();
			}
		}
	}
	return {total_size,fast_io::posix_clock_gettime(fast_io::posix_clock_id::monotonic_raw)-start};
}

#if __cpp_lib_format >= 201907L && !defined(DISABLE_STD_FORMAT_BENCH)
inline std::string color_format(std::uint_least8_t r,std::uint_least8_t g,std::uint_least8_t b)
{
	return std::format("Red: {}, Green: {}, Blue: {}",r,g,b);
}
#endif

inline std::string color_concat(std::uint_least8_t r,std::uint_least8_t g,std::uint_least8_t b)
{
	return fast_io::concat("Red: ",r,", Green: ",g,", Blue: ",b);
}

inline std::string color_ostringstream(std::uint_least8_t r,std::uint_least8_t g,std::uint_least8_t b)
{
	std::ostringstream oss;
	oss<<"Red: "<<r<<", Green: "<<g<<", Blue: "<<b;
	return std::move(oss.str());
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
#if __cpp_lib_format >= 201907L && !defined(DISABLE_STD_FORMAT_BENCH)
	auto format_time = benchmark(color_format);
#endif
	auto concat_time = benchmark(color_concat);
	auto ostringstream_time = benchmark(color_ostringstream);
#if __has_include(<fmt/core.h>) && defined(ENABLE_FMT_BENCH)
	auto fmt_format_time = benchmark(color_fmt_format);
#if __has_include(<fmt/compile.h>)
	auto fmt_format_compile_time = benchmark(color_fmt_format_compile);
#endif
#endif
	using namespace fast_io::io;
	print(
#if __cpp_lib_format >= 201907L && !defined(DISABLE_STD_FORMAT_BENCH)
		"std::format (total size:",format_time.total_size,") took ",format_time.timestamp,"s.\n"
#endif
		"fast_io::concat (total size: ",concat_time.total_size,") took ",concat_time.timestamp,"s.\n"
		"std::ostringstream (total size: ",ostringstream_time.total_size,") took ",ostringstream_time.timestamp,"s.\n"
#if __has_include(<fmt/core.h>) && defined(ENABLE_FMT_BENCH)
		"fmt::format (total size:",fmt_format_time.total_size,") took ",fmt_format_time.timestamp,"s.\n"
#if __has_include(<fmt/compile.h>)
		"fmt::format compile (total size:",fmt_format_compile_time.total_size,") took ",fmt_format_compile_time.timestamp,"s.\n"
#endif
#endif
		);
}

