#pragma once

namespace fast_io
{

struct win32_code
{
	std::uint32_t ec{};
};

inline constexpr std::uintptr_t domain_define(error_type_t<win32_code>) noexcept
{
	if constexpr(sizeof(std::uintptr_t)<=sizeof(std::uint_least16_t))
		return 8987u;
	else if constexpr(sizeof(std::uintptr_t)<=sizeof(std::uint_least32_t))
		return 588553401u;
	else
		return 12911969238956088748ULL;
}

inline constexpr std::uintptr_t win32_domain_value{domain_define(error_type<win32_code>)};

namespace details
{

inline std::size_t print_reserve_define_win32_error_9xa_impl(char* ptr, std::uint32_t ec) noexcept
{
	constexpr std::size_t char8_buffer_size{65536};
	return win32::FormatMessageA(
	0x00000200 | 0x00001000,
	nullptr,
	ec,
	(1 << 10),
	ptr,
	char8_buffer_size,
	nullptr);
}

inline std::size_t print_reserve_define_win32_error_ntw_impl(wchar_t* ptr, std::uint32_t ec) noexcept
{
	constexpr std::size_t char16_buffer_size{32768};
	return win32::FormatMessageW(
	0x00000200 | 0x00001000,
	nullptr,
	ec,
	(1 << 10),
	ptr,
	char16_buffer_size,
	nullptr);
}

template<win32_family family,std::integral char_type>
inline constexpr char_type* print_reserve_define_win32_error_ptr_impl(char_type* ptr, std::uint32_t ec) noexcept
{
	if constexpr(family==win32_family::wide_nt)
	{
		if constexpr(std::same_as<char_type,wchar_t>)
			return ptr+print_reserve_define_win32_error_ntw_impl(ptr,ec);
		else if constexpr(sizeof(char_type)==2)
		{
			using char_type_may_alias_ptr
#if __has_cpp_attribute(gnu::may_alias)
			[[gnu::may_alias]]
#endif
			= wchar_t*;
			return ptr+print_reserve_define_win32_error_ntw_impl(reinterpret_cast<char_type_may_alias_ptr>(ptr),ec);
		}
		else
		{
			::fast_io::freestanding::array<char16_t,32768> buffer;
			using char_type_may_alias_ptr
#if __has_cpp_attribute(gnu::may_alias)
			[[gnu::may_alias]]
#endif
			= wchar_t*;
			std::size_t sz{print_reserve_define_win32_error_ntw_impl(reinterpret_cast<char_type_may_alias_ptr>(buffer.data()),ec)};
			return ::fast_io::details::codecvt::general_code_cvt_full(buffer.data(),buffer.data()+sz,ptr);
		}
	}
	else
	{
		if constexpr(std::same_as<char_type,char>)
			return ptr+print_reserve_define_win32_error_9xa_impl(ptr,ec);
		else if constexpr(sizeof(char_type)==1)
		{
			return ptr+print_reserve_define_win32_error_9xa_impl(reinterpret_cast<char*>(ptr),ec);
		}
		else
		{
			::fast_io::freestanding::array<char8_t,65536> buffer;
			std::size_t sz{print_reserve_define_win32_error_9xa_impl(reinterpret_cast<char*>(buffer.data()),ec)};
			return ::fast_io::details::codecvt::general_code_cvt_full(buffer.data(),buffer.data()+sz,ptr);
		}
	}
}

template<win32_family family,::fast_io::freestanding::contiguous_iterator Iter>
inline constexpr Iter print_reserve_define_win32_error_impl(Iter iter,std::uint32_t ec) noexcept
{
	if constexpr(std::is_pointer_v<::fast_io::freestanding::iter_value_t<Iter>>)
		return print_reserve_define_win32_error_ptr_impl<family>(iter,ec);
	else
		return print_reserve_define_win32_error_ptr_impl<family>(::fast_io::freestanding::to_address(iter),ec)-::fast_io::freestanding::to_address(iter)+iter;
}

}

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,win32_code>) noexcept
{
	if constexpr(win32_family::native==win32_family::wide_nt)
	{
		if constexpr(sizeof(char_type)==2)
			return 32768;
		else
		{
			constexpr std::size_t sz{::fast_io::details::cal_decorated_reserve_size<2,sizeof(char_type)>(32768)};
			return sz;
		}
	}
	else
	{
		if constexpr(sizeof(char_type)==1)
			return 65536;
		else
		{
			constexpr std::size_t sz{::fast_io::details::cal_decorated_reserve_size<1,sizeof(char_type)>(65536)};
			return sz;
		}
	}
}

template<::fast_io::freestanding::contiguous_iterator Iter>
inline constexpr Iter print_reserve_define(io_reserve_type_t<::fast_io::freestanding::iter_value_t<Iter>,win32_code>,Iter iter,win32_code e) noexcept
{
	return details::print_reserve_define_win32_error_impl<win32_family::native>(iter,e.ec);
}

[[noreturn]] inline void throw_win32_error()
{
#ifdef __cpp_exceptions
#if defined(_MSC_VER) && (!defined(_HAS_EXCEPTIONS) || _HAS_EXCEPTIONS == 0)
	fast_terminate();
#else
	throw ::fast_io::error{win32_domain_value,static_cast<std::uintptr_t>(win32::GetLastError())};
#endif
#else
	fast_terminate();
#endif
}
[[noreturn]] inline void throw_win32_error([[maybe_unused]] std::uint_least32_t err)
{
#ifdef __cpp_exceptions
#if defined(_MSC_VER) && (!defined(_HAS_EXCEPTIONS) || _HAS_EXCEPTIONS == 0)
	fast_terminate();
#else
	throw ::fast_io::error{win32_domain_value,static_cast<std::uintptr_t>(err)};
#endif
#else
	fast_terminate();
#endif
}

}