#pragma once
#include <ios>
#include "../fast_io_core_impl/seek.h"

namespace fast_io
{

template <typename src_type>
concept boost_iostreams_input_device = requires(src_type &src, char *s, ::std::streamsize n) {
	{ src.read(s, n) } -> ::std::same_as<::std::streamsize>;
};

template <typename src_type>
concept boost_iostreams_output_device = requires(src_type &src, char const *s, ::std::streamsize n) {
	{ src.write(s, n) } -> ::std::same_as<::std::streamsize>;
};

template <typename src_type>
concept boost_iostreams_any_device = boost_iostreams_input_device<src_type> || boost_iostreams_output_device<src_type>;

template <typename src_type>
concept boost_iostreams_io_device = boost_iostreams_input_device<src_type> && boost_iostreams_output_device<src_type>;

template <typename src_type>
concept boost_iostreams_flushable_device = boost_iostreams_any_device<src_type> && requires(src_type &src) {
	{ src.flush() };
};

template <typename src_type>
concept boost_iostream_seekable_device =
	boost_iostreams_any_device<src_type> && requires(src_type &src, ::std::intmax_t offset, ::std::ios::seekdir dir) {
		{ src.seek(offset, dir) } -> ::std::convertible_to<::std::intmax_t>;
	};

template <::std::integral ch_type, boost_iostreams_any_device src_type>
class basic_boost_iostreams
{
public:
	using char_type = ch_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
	using source_type = src_type;
	source_type source;
};

template <::std::integral ch_type, boost_iostreams_input_device src_type>
inline ::std::byte *read_some_bytes_underflow_define(basic_boost_iostreams<ch_type, src_type> &in_device,
													 ::std::byte *first, ::std::size_t count)
{
	auto ret{in_device.source.read(reinterpret_cast<char *>(first), static_cast<::std::streamsize>(count))};
	if (ret == -1)
	{
		return first;
	}
	return first + ret;
}

template <::std::integral ch_type, boost_iostreams_output_device src_type>
inline ::std::byte const *write_some_bytes_overflow_define(basic_boost_iostreams<ch_type, src_type> &out_device,
														   ::std::byte const *first, ::std::size_t count)
{
	auto ret{out_device.source.write(reinterpret_cast<char const *>(first), static_cast<::std::streamsize>(count))};
	if (ret == -1)
	{
		return first;
	}
	return first + ret;
}
template <::std::integral ch_type, boost_iostreams_flushable_device src_type>
inline void io_stream_buffer_flush_define(basic_boost_iostreams<ch_type, src_type> &out_device)
{
	out_device.source.flush();
}

template <::std::integral ch_type, boost_iostreams_seekable_device src_type>
inline ::fast_io::intfpos_t io_stream_seek_bytes_define(basic_boost_iostreams<ch_type, src_type> &dev,
														::fast_io::intfpos_t off, ::fast_io::seekdir sdir)
{
	return static_cast<::fast_io::intfpos_t>(
		static_cast<::std::streamoff>(dev.source.seek(static_cast<boost::iostreams::stream_offset>(off),
													  static_cast<::std::ios::seekdir>(static_cast<int>(sdir)))));
}

template <boost_iostreams_any_device src_type>
using boost_iostreams = basic_boost_iostreams<char, src_type>;
template <boost_iostreams_any_device src_type>
using u8boost_iostreams = basic_boost_iostreams<char8_t, src_type>;
template <boost_iostreams_any_device src_type>
using wboost_iostreams = basic_boost_iostreams<wchar_t, src_type>;
} // namespace fast_io
