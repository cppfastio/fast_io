
#include <fast_io_dsal/string.h>
#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_driver/timer.h>
#include <cassert>
using namespace fast_io::io;

namespace fast_io
{

/// Trims whitespace characters from the end of a fast_io::basic_string
/// @tparam CharT Character type (must be std::integral)
/// @tparam Allocator Allocator type (defaults to native_global_allocator)
/// @param str The string to trim (modified in-place)
/// @return Reference to the modified string
template <::std::integral CharT, typename Allocator = native_global_allocator>
inline constexpr basic_string<CharT, Allocator> &
trim_right(basic_string<CharT, Allocator> &str) noexcept
{
	if (str.empty())
	{
		return str;
	}

	auto first = str.data();
	auto last = str.data() + str.size();

	// Find the position after the last non-whitespace character
	CharT const *trim_pos = last;
	CharT const *current = first;

	while (current < last)
	{
		// Find next whitespace character
		auto next_space = details::find_space_common_impl<false, false>(current, last);
		if (next_space == last)
		{
			// No more whitespace characters found
			trim_pos = last;
			break;
		}

		// Find next non-whitespace character after the whitespace
		auto next_non_space = details::find_space_common_impl<false, true>(next_space, last);
		if (next_non_space == last)
		{
			// From next_space to end are all whitespace characters
			trim_pos = next_space;
			break;
		}

		current = next_non_space;
	}

	// Convert const pointer back to iterator for erase
	auto trim_iterator = str.begin() + (trim_pos - first);
	str.erase(trim_iterator, str.end());
	return str;
}

} // namespace fast_io


int main()
{
	{
		constexpr std::size_t N(40000000);
		{
			fast_io::timer t(u8"trim_right string");
			fast_io::string str("    ok    someone like trim    ok    ");
			for (std::size_t i{}; i != N; ++i)
			{
				str.push_back(' ');
				str.push_back('\t');
				str.push_back('\r');
				str.push_back('\f');
				str.push_back('\v');
				str.push_back('\n');
			}
			assert(fast_io::trim_right(str) == fast_io::string("    ok    someone like trim    ok"));
		}
	}
	{
		constexpr std::size_t N(40000000);
		{
			fast_io::timer t(u8"trim_right u16string");
			fast_io::u16string str(u"    ok    someone like trim    ok    ");
			for (std::size_t i{}; i != N; ++i)
			{
				str.push_back(u' ');
				str.push_back(u'\t');
				str.push_back(u'\r');
				str.push_back(u'\f');
				str.push_back(u'\v');
				str.push_back(u'\n');
			}
			assert(fast_io::trim_right(str) == fast_io::u16string(u"    ok    someone like trim    ok"));
		}
	}
}
