#pragma once

#if defined(_MSC_VER) && !defined(_KERNEL_MODE)
#pragma comment(lib,"user32.lib")
#endif

namespace fast_io
{

template<win32_family family,std::integral ch_type>
struct basic_win32_family_box_t
{
	using char_type = ch_type;
	explicit constexpr basic_win32_family_box_t() noexcept = default;
};

template<std::integral char_type>
using basic_win32_box_9xa_t = basic_win32_family_box_t<win32_family::ansi_9x,char_type>;
using win32_box_9xa_t = basic_win32_box_9xa_t<char>;
using wwin32_box_9xa_t = basic_win32_box_9xa_t<wchar_t>;
using u8win32_box_9xa_t = basic_win32_box_9xa_t<char8_t>;
using u16win32_box_9xa_t = basic_win32_box_9xa_t<char16_t>;
using u32win32_box_9xa_t = basic_win32_box_9xa_t<char32_t>;

template<std::integral char_type>
using basic_win32_box_ntw_t = basic_win32_family_box_t<win32_family::wide_nt,char_type>;
using win32_box_ntw_t = basic_win32_box_ntw_t<char>;
using wwin32_box_ntw_t = basic_win32_box_ntw_t<wchar_t>;
using u8win32_box_ntw_t = basic_win32_box_ntw_t<char8_t>;
using u16win32_box_ntw_t = basic_win32_box_ntw_t<char16_t>;
using u32win32_box_ntw_t = basic_win32_box_ntw_t<char32_t>;

template<std::integral char_type>
using basic_win32_box_t = basic_win32_family_box_t<win32_family::native,char_type>;
using win32_box_t = basic_win32_box_t<char>;
using wwin32_box_t = basic_win32_box_t<wchar_t>;
using u8win32_box_t = basic_win32_box_t<char8_t>;
using u16win32_box_t = basic_win32_box_t<char16_t>;
using u32win32_box_t = basic_win32_box_t<char32_t>;

namespace details
{

template<win32_family family>
requires (family==win32_family::wide_nt)
inline void win32_box_write_impl(char16_t* first,char16_t* last)
{
	*::fast_io::freestanding::remove(first,last,0)=0;
	using char16_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= char16_t const*;
	if(!::fast_io::win32::MessageBoxW(nullptr,
		reinterpret_cast<char16_may_alias_ptr>(first),
		reinterpret_cast<char16_may_alias_ptr>(u"fast_io"),
		0x00000040L /*MB_ICONINFORMATION*/))
		throw_win32_error();
}

template<win32_family family>
requires (family==win32_family::ansi_9x)
inline void win32_box_write_impl(char8_t* first,char8_t* last)
{
	*::fast_io::freestanding::remove(first,last,0)=0;
	if(!::fast_io::win32::MessageBoxA(nullptr,
		reinterpret_cast<char const*>(first),
		reinterpret_cast<char const*>(u8"fast_io"),
		0x00000040L /*MB_ICONINFORMATION*/))
		throw_win32_error();
}

template<win32_family family,std::integral char_type>
inline void win32_box_converter_path_impl(char_type const* first,char_type const* last)
{
	win32_family_api_encoding_converter<family> converter(first,static_cast<std::size_t>(last-first));
	win32_box_write_impl<family>(converter.buffer_data,converter.buffer_data_end);
}

}

template<win32_family family,std::integral char_type,::std::contiguous_iterator Iter>
requires std::same_as<char_type,::std::iter_value_t<Iter>>
inline void write(basic_win32_family_box_t<family,char_type>,Iter first,Iter last)
{
	details::win32_box_converter_path_impl<family>(::std::to_address(first),::std::to_address(last));
}

}
