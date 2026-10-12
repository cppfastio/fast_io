#pragma once

#include <fast_io_dsal/vector.h>
#include <fast_io_dsal/string.h>
#include <random>

template <typename T = ::fast_io::string>
inline ::fast_io::vector<T> gentest()
{
	::fast_io::vector<T> vec;
	::std::mt19937_64 eng;
	::std::uniform_int_distribution<std::size_t> ud(0, 61);
	::std::uniform_int_distribution<std::size_t> rlen(25, 30);
	for (::std::size_t i{}; i != 1000000; ++i)
	{
		T tempstr;
		::std::size_t n{rlen(eng)};
		tempstr.reserve(n);
		for (::std::size_t j{}; j != n; ++j)
		{
			char8_t ch(static_cast<char8_t>(ud(eng)));
			if (ch < 10u)
			{
				ch += u8'0';
			}
			else if (ch < 36u)
			{
				ch = ch - 10u + u8'a';
			}
			else
			{
				ch = ch - 36u + u8'A';
			}
			tempstr.push_back(ch);
		}
		vec.emplace_back(::std::move(tempstr));
	}
	return vec;
}
