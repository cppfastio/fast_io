#pragma once

namespace fast_io
{

template <::std::integral char_type>
using basic_nt_io_observer = basic_nt_family_io_observer<nt_family::nt, char_type>;

template <::std::integral char_type>
using basic_nt_file = basic_nt_family_file<nt_family::nt, char_type>;

template <::std::integral char_type>
using basic_nt_pipe = basic_nt_family_pipe<nt_family::nt, char_type>;

using nt_io_observer = basic_nt_io_observer<char>;
using nt_file = basic_nt_file<char>;
using nt_pipe = basic_nt_pipe<char>;

using wnt_io_observer = basic_nt_io_observer<wchar_t>;
using wnt_file = basic_nt_file<wchar_t>;
using wnt_pipe = basic_nt_pipe<wchar_t>;

using u8nt_io_observer = basic_nt_io_observer<char8_t>;
using u8nt_file = basic_nt_file<char8_t>;
using u8nt_pipe = basic_nt_pipe<char8_t>;

using u16nt_io_observer = basic_nt_io_observer<char16_t>;
using u16nt_file = basic_nt_file<char16_t>;
using u16nt_pipe = basic_nt_pipe<char16_t>;

using u32nt_io_observer = basic_nt_io_observer<char32_t>;
using u32nt_file = basic_nt_file<char32_t>;
using u32nt_pipe = basic_nt_pipe<char32_t>;

template <::std::integral char_type>
using basic_zw_io_observer = basic_nt_family_io_observer<nt_family::zw, char_type>;

template <::std::integral char_type>
using basic_zw_file = basic_nt_family_file<nt_family::zw, char_type>;

template <::std::integral char_type>
using basic_zw_pipe = basic_nt_family_pipe<nt_family::zw, char_type>;

using zw_io_observer = basic_zw_io_observer<char>;
using zw_file = basic_zw_file<char>;
using zw_pipe = basic_zw_pipe<char>;

using wzw_io_observer = basic_zw_io_observer<wchar_t>;
using wzw_file = basic_zw_file<wchar_t>;
using wzw_pipe = basic_zw_pipe<wchar_t>;

using u8zw_io_observer = basic_zw_io_observer<char8_t>;
using u8zw_file = basic_zw_file<char8_t>;
using u8zw_pipe = basic_zw_pipe<char8_t>;

using u16zw_io_observer = basic_zw_io_observer<char16_t>;
using u16zw_file = basic_zw_file<char16_t>;
using u16zw_pipe = basic_zw_pipe<char16_t>;

using u32zw_io_observer = basic_zw_io_observer<char32_t>;
using u32zw_file = basic_zw_file<char32_t>;
using u32zw_pipe = basic_zw_pipe<char32_t>;

namespace freestanding
{
template <nt_family fm>
struct is_trivially_copyable_or_relocatable<nt_family_file_factory<fm>>
{
	inline static constexpr bool value = true;
};

template <nt_family fm, ::std::integral char_type>
struct is_zero_default_constructible<basic_nt_family_io_observer<fm, char_type>>
{
	inline static constexpr bool value = true;
};

template <nt_family fm, ::std::integral char_type>
struct is_trivially_copyable_or_relocatable<basic_nt_family_file<fm, char_type>>
{
	inline static constexpr bool value = true;
};

template <nt_family fm, ::std::integral char_type>
struct is_zero_default_constructible<basic_nt_family_file<fm, char_type>>
{
	inline static constexpr bool value = true;
};
} // namespace freestanding

} // namespace fast_io
