#pragma once

namespace fast_io
{

namespace details
{

inline constexpr ::fast_io::manipulators::scalar_flags fast_pipe_hex_scl_flg{.base=16,.floating=::fast_io::manipulators::floating_format::fixed};

template<win32_family family>
inline void* win32_family_refterm_fast_pipe_impl(open_mode mode,perms pm)
{
	std::uint_least32_t process_id{::fast_io::win32::GetCurrentProcessId()};
	using type = ::fast_io::mnp::scalar_manip_t<fast_pipe_hex_scl_flg,std::uint_least32_t>;
	if constexpr(family==win32_family::wide_nt)
	{
		char16_t buffer[256]=u"\\\\.\\pipe\\fastpipe";
		constexpr std::size_t sz{sizeof(u"\\\\.\\pipe\\fastpipe")/sizeof(char16_t)-1};
		char16_t* res{print_reserve_define(io_reserve_type<char16_t,type>,buffer+sz,{process_id})};
		*res=0;
		return win32_file_ntw(::fast_io::mnp::os_c_str(buffer),mode,pm).release();
	}
	else
	{
		char8_t buffer[256]=u8"\\\\.\\pipe\\fastpipe";
		constexpr std::size_t sz{sizeof(u8"\\\\.\\pipe\\fastpipe")/sizeof(char8_t)-1};
		char8_t* res{print_reserve_define(io_reserve_type<char8_t,type>,buffer+sz,{process_id})};
		*res=0;
		return win32_file_9xa(::fast_io::mnp::os_c_str(buffer),mode,pm).release();
	}

}

}

inline win32_file_factory win32_refterm_fast_pipe_9xa(open_mode mode=open_mode::in|open_mode::out|open_mode::excl,perms pm=static_cast<perms>(436))
{
	return win32_file_factory{details::win32_family_refterm_fast_pipe_impl<win32_family::ansi_9x>(mode,pm)};
}

inline win32_file_factory win32_refterm_fast_pipe_ntw(open_mode mode=open_mode::in|open_mode::out|open_mode::excl,perms pm=static_cast<perms>(436))
{
	return win32_file_factory{details::win32_family_refterm_fast_pipe_impl<win32_family::wide_nt>(mode,pm)};
}

inline win32_file_factory win32_refterm_fast_pipe(open_mode mode=open_mode::in|open_mode::out|open_mode::excl,perms pm=static_cast<perms>(436))
{
	return win32_file_factory{details::win32_family_refterm_fast_pipe_impl<win32_family::native>(mode,pm)};
}

}
