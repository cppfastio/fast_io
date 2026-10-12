#pragma once

namespace fast_io
{

namespace details
{

template <int fd>
inline void *nt_get_stdhandle() noexcept
{
	using prtl_user_process_parameters
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= ::fast_io::win32::nt::rtl_user_process_parameters *;
	auto pparam{
		reinterpret_cast<prtl_user_process_parameters>(::fast_io::win32::nt::nt_get_current_peb()->ProcessParameters)};
	if constexpr (fd == 0)
	{
		return pparam->StandardInput;
	}
	else if constexpr (fd == 1)
	{
		return pparam->StandardOutput;
	}
	else
	{
		return pparam->StandardError;
	}
}

} // namespace details

template <::std::integral char_type = char>
inline basic_nt_io_observer<char_type> nt_stdin() noexcept
{
	return {::fast_io::details::nt_get_stdhandle<0>()};
}

template <::std::integral char_type = char>
inline basic_nt_io_observer<char_type> nt_stdout() noexcept
{
	return {::fast_io::details::nt_get_stdhandle<1>()};
}

template <::std::integral char_type = char>
inline basic_nt_io_observer<char_type> nt_stderr() noexcept
{
	return {::fast_io::details::nt_get_stdhandle<2>()};
}

template <::std::integral char_type = char>
inline basic_zw_io_observer<char_type> zw_stdin() noexcept
{
	return {::fast_io::details::nt_get_stdhandle<0>()};
}

template <::std::integral char_type = char>
inline basic_zw_io_observer<char_type> zw_stdout() noexcept
{
	return {::fast_io::details::nt_get_stdhandle<1>()};
}

template <::std::integral char_type = char>
inline basic_zw_io_observer<char_type> zw_stderr() noexcept
{
	return {::fast_io::details::nt_get_stdhandle<2>()};
}

#if !defined(__WINE__) && !defined(__CYGWIN__) && !defined(__BIONIC__) && !defined(_WIN32_WINDOWS)
template <::std::integral char_type = char>
inline basic_nt_io_observer<char_type> native_stdin() noexcept
{
	return {::fast_io::details::nt_get_stdhandle<0>()};
}
template <::std::integral char_type = char>
inline basic_nt_io_observer<char_type> native_stdout() noexcept
{
	return {::fast_io::details::nt_get_stdhandle<1>()};
}
template <::std::integral char_type = char>
inline basic_nt_io_observer<char_type> native_stderr() noexcept
{
	return {::fast_io::details::nt_get_stdhandle<2>()};
}
#endif

} // namespace fast_io
