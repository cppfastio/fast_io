#pragma once

namespace fast_io
{

// black_hole is a helper class which helps you remove device requirement for iobuf or iotransform.
// sha256 for example. You do not need a real device. You want to send all your bits to black hole

template <::std::integral ch_type>
struct basic_black_hole
{
	using char_type = ch_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
};

using black_hole = basic_black_hole<char>;
using wblack_hole = basic_black_hole<wchar_t>;
using u8black_hole = basic_black_hole<char8_t>;
using u16black_hole = basic_black_hole<char16_t>;
using u32black_hole = basic_black_hole<char32_t>;

template <::std::integral ch_type>
inline constexpr ch_type *read_some_underflow_define(basic_black_hole<ch_type>, ch_type *first, ch_type *) noexcept
{
	return first;
}

template <::std::integral ch_type>
inline constexpr void write_all_overflow_define(basic_black_hole<ch_type>, ch_type const *, ch_type const *) noexcept
{
}

template <::std::integral ch_type>
inline constexpr ::std::byte *read_some_bytes_underflow_define(basic_black_hole<ch_type>, ::std::byte *first,
															   ::std::byte *) noexcept
{
	return first;
}

template <::std::integral ch_type>
inline constexpr void write_all_bytes_overflow_define(basic_black_hole<ch_type>, ::std::byte const *,
													  ::std::byte const *) noexcept
{
}

template <::std::integral ch_type>
inline constexpr ch_type *pread_some_underflow_define(basic_black_hole<ch_type>, ch_type *first, ch_type *,
													  ::fast_io::intfpos_t) noexcept
{
	return first;
}

template <::std::integral ch_type>
inline constexpr void pwrite_all_overflow_define(basic_black_hole<ch_type>, ch_type const *, ch_type const *,
												 ::fast_io::intfpos_t) noexcept
{
}

template <::std::integral ch_type>
inline constexpr ::std::byte *pread_some_bytes_underflow_define(basic_black_hole<ch_type>, ::std::byte *first,
																::std::byte *, ::fast_io::intfpos_t) noexcept
{
	return first;
}

template <::std::integral ch_type>
inline constexpr void pwrite_all_bytes_overflow_define(basic_black_hole<ch_type>, ::std::byte const *,
													   ::std::byte const *, ::fast_io::intfpos_t) noexcept
{
}

template <::std::integral ch_type>
inline constexpr basic_black_hole<ch_type> io_stream_ref_define(basic_black_hole<ch_type> other) noexcept
{
	return other;
}

template <::std::integral ch_type>
inline constexpr basic_black_hole<ch_type> io_bytes_stream_ref_define(basic_black_hole<ch_type> other) noexcept
{
	return other;
}

template <::std::integral ch_type>
inline constexpr void io_stream_buffer_flush_define(basic_black_hole<ch_type>) noexcept
{
}

#if 0
template<::std::integral ch_type>
inline constexpr ::fast_io::intfpos_t io_stream_seek_define(basic_black_hole<ch_type> fb, ::fast_io::intfpos_t off, ::fast_io::seekdir sdir) noexcept {
	return {};
}

template<::std::integral ch_type>
inline constexpr ::fast_io::intfpos_t io_stream_seek_bytes_define(basic_black_hole<ch_type> fb, ::fast_io::intfpos_t off, ::fast_io::seekdir sdir) noexcept {
	return {};
}
#endif

} // namespace fast_io
