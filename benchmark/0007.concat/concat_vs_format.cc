﻿#include <string>
#include <fast_io.h>
#include <fast_io_dsal/string.h>
#include <version>
#if __has_include(<format>) && !defined(DISABLE_STD_FORMAT_BENCH)
#include <format>
#endif
#if __has_include(<fmt/core.h>) && defined(ENABLE_FMT_BENCH)
#include <fmt/core.h>
#if __has_include(<fmt/compile.h>)
#include <fmt/compile.h>
#endif
#endif
#include <sstream>

struct benchmark_return
{
	std::size_t total_size{};
	fast_io::unix_timestamp timestamp{};
};

template <typename Func>
inline benchmark_return benchmark(Func meth)
{
	std::size_t total_size{};
	auto start{fast_io::posix_clock_gettime(fast_io::posix_clock_id::monotonic_raw)};
	for (std::uint_least16_t r{}; r != 256; ++r)
	{
		for (std::uint_least16_t g{}; g != 256; ++g)
		{
			for (std::uint_least16_t b{}; b != 256; ++b)
			{
				total_size += meth(static_cast<::std::uint_least8_t>(r), static_cast<::std::uint_least8_t>(g),
								   static_cast<::std::uint_least8_t>(b))
								  .size();
			}
		}
	}
	return {total_size, fast_io::posix_clock_gettime(fast_io::posix_clock_id::monotonic_raw) - start};
}

#if __cpp_lib_format >= 201907L && !defined(DISABLE_STD_FORMAT_BENCH)
inline std::string color_format(std::uint_least8_t r, std::uint_least8_t g, std::uint_least8_t b)
{
	return std::format("Red: {}, Green: {}, Blue: {}", r, g, b);
}
#endif

inline ::fast_io::string color_concat_fast_io(std::uint_least8_t r, std::uint_least8_t g, std::uint_least8_t b)
{
	return ::fast_io::concat_fast_io("Red: ", r, ", Green: ", g, ", Blue: ", b);
}

inline ::std::string color_concat_std(std::uint_least8_t r, std::uint_least8_t g, std::uint_least8_t b)
{
	return ::fast_io::concat_std("Red: ", r, ", Green: ", g, ", Blue: ", b);
}

inline ::fast_io::u32string color_u32concat_fast_io(std::uint_least8_t r, std::uint_least8_t g, std::uint_least8_t b)
{
	return ::fast_io::u32concat_fast_io(U"Red: ", r, U", Green: ", g, U", Blue: ", b);
}

inline ::std::u32string color_u32concat_std(std::uint_least8_t r, std::uint_least8_t g, std::uint_least8_t b)
{
	return ::fast_io::u32concat_std(U"Red: ", r, U", Green: ", g, U", Blue: ", b);
}

inline std::string color_ostringstream(std::uint_least8_t r, std::uint_least8_t g, std::uint_least8_t b)
{
	std::ostringstream oss;
	oss << "Red: " << r << ", Green: " << g << ", Blue: " << b;
	return std::move(oss.str());
}

#if __has_include(<fmt/core.h>) && defined(ENABLE_FMT_BENCH)

inline std::string color_fmt_format(std::uint_least8_t r, std::uint_least8_t g, std::uint_least8_t b)
{
	return fmt::format("Red: {}, Green: {}, Blue: {}", r, g, b);
}

#if __has_include(<fmt/compile.h>)
inline std::string color_fmt_format_compile(std::uint_least8_t r, std::uint_least8_t g, std::uint_least8_t b)
{
	return fmt::format(FMT_COMPILE("Red: {}, Green: {}, Blue: {}"), r, g, b);
}

#endif

#endif

int main()
{
#if __cpp_lib_format >= 201907L && !defined(DISABLE_STD_FORMAT_BENCH)
	auto format_time = benchmark(color_format);
#endif
	auto concat_fast_io_time = benchmark(color_concat_fast_io);
	auto concat_std_time = benchmark(color_concat_std);
	auto u32concat_fast_io_time = benchmark(color_u32concat_fast_io);
	auto u32concat_std_time = benchmark(color_u32concat_std);
#if !defined(DISABLE_OSTRINGSTREAM_BENCH)
	auto ostringstream_time = benchmark(color_ostringstream);
#endif
#if __has_include(<fmt/core.h>) && defined(ENABLE_FMT_BENCH)
	auto fmt_format_time = benchmark(color_fmt_format);
#if __has_include(<fmt/compile.h>)
	auto fmt_format_compile_time = benchmark(color_fmt_format_compile);
#endif
#endif
	using namespace fast_io::io;
	print(
#if __cpp_lib_format >= 201907L && !defined(DISABLE_STD_FORMAT_BENCH)
		"std::format (total size:", format_time.total_size, ") took ", format_time.timestamp,
		"s.\n"
#endif
		"fast_io::concat_fast_io (total size: ",
		concat_fast_io_time.total_size, ") took ", concat_fast_io_time.timestamp,
		"s.\n"
		"fast_io::concat_std (total size: ",
		concat_std_time.total_size, ") took ", concat_std_time.timestamp,
		"s.\n"
		"fast_io::u32concat_fast_io (total size: ",
		u32concat_fast_io_time.total_size, ") took ", u32concat_fast_io_time.timestamp,
		"s.\n"
		"fast_io::u32concat_std (total size: ",
		u32concat_std_time.total_size, ") took ", u32concat_std_time.timestamp,
		"s.\n"
#if !defined(DISABLE_OSTRINGSTREAM_BENCH)
		"std::ostringstream (total size: ",
		ostringstream_time.total_size, ") took ", ostringstream_time.timestamp,
		"s.\n"
#endif
#if __has_include(<fmt/core.h>) && defined(ENABLE_FMT_BENCH)
		"fmt::format (total size:",
		fmt_format_time.total_size, ") took ", fmt_format_time.timestamp,
		"s.\n"
#if __has_include(<fmt/compile.h>)
		"fmt::format compile (total size:",
		fmt_format_compile_time.total_size, ") took ", fmt_format_compile_time.timestamp, "s.\n"
#endif
#endif
	);
}
