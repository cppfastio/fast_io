#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_driver/timer.h>
#include <vector>
#include <string>
#include <fast_io_dsal/string.h>
#include <charconv>
#include <cstdlib>
#include <fast_float/fast_float.h>

using namespace fast_io::io;

static std::string make_hex_numbers_buffer(std::size_t n)
{
	std::string s;
	s.reserve(n * 8);
	for (std::size_t i{}; i != n; ++i)
	{
		auto old = s.size();
		s.resize(old + 32);
		auto *first = s.data() + old;
		auto *last = s.data() + s.size();
		auto res = std::to_chars(first, last - 1, i, 16);
		*res.ptr = '\n';
		s.resize(static_cast<std::size_t>(res.ptr - s.data() + 1));
	}
	return s;
}

int main()
{
	constexpr std::size_t N = 10'000'000;
	auto buf = make_hex_numbers_buffer(N);
	char const *begin = buf.data();
	char const *end = buf.data() + buf.size();

	{
		std::size_t lines{};
		for (char const *p = begin; p < end; ++p)
		{
			lines += (*p == '\n');
		}
		fast_io::println("lines=", lines);
	}

	// strtoul (hex)
	{
		fast_io::timer t(u8"strtoul_hex");
		std::uint64_t sum{};
		char const *p = begin;
		while (p < end)
		{
			char *endptr{};
			auto v = std::strtoul(p, &endptr, 16);
			sum += static_cast<std::uint64_t>(v);
			p = endptr;
			if (p < end && *p == '\n')
			{
				++p;
			}
		}
		std::uint64_t volatile sink = sum;
		(void)sink;
	}

	// std::from_chars (hex)
	{
		fast_io::timer t(u8"std_from_chars_hex");
		std::uint64_t sum{};
		char const *p = begin;
		while (p < end)
		{
			std::uint64_t v{};
			auto res = std::from_chars(p, end, v, 16);
			sum += v;
			p = res.ptr;
			if (p < end && *p == '\n')
			{
				++p;
			}
		}
		std::uint64_t volatile sink = sum;
		(void)sink;
	}

	// fast_io char_digit_to_literal (hex)
	{
		fast_io::timer t(u8"fastio_char_digit_to_literal_hex");
		std::uint64_t sum{};
		char const *p = begin;
		while (p < end)
		{
			using UCh = std::make_unsigned_t<char>;
			std::uint64_t v{};
			char const *q = p;
			while (q < end && *q != '\n')
			{
				UCh ch = static_cast<UCh>(*q);
				if (fast_io::details::char_digit_to_literal<16, char>(ch))
				{
					break;
				}
				v = (v << 4) + static_cast<std::uint64_t>(ch);
				++q;
			}
			sum += v;
			p = (q < end ? q + 1 : q);
		}
		std::uint64_t volatile sink = sum;
		(void)sink;
	}

	// fast_float (hex)
	{
		fast_io::timer t(u8"fast_float_from_chars_hex");
		std::uint64_t sum{};
		char const *p = begin;
		while (p < end)
		{
			std::uint64_t v{};
			auto res = fast_float::from_chars(p, end, v, 16);
			sum += v;
			p = res.ptr;
			if (p < end && *p == '\n')
			{
				++p;
			}
		}
		std::uint64_t volatile sink = sum;
		(void)sink;
	}
}

