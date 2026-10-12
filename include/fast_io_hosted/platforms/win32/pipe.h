#pragma once

namespace fast_io
{

template <win32_family family, ::std::integral ch_type>
class basic_win32_family_pipe
{
public:
	using char_type = ch_type;
	basic_win32_family_file<family, ch_type> pipes[2];
	inline basic_win32_family_pipe()
		FAST_IO_HERBCEPTIONS_THROWS
	{
		win32::security_attributes sec_attr{sizeof(win32::security_attributes), nullptr, 1};
		if (!::fast_io::win32::CreatePipe(__builtin_addressof(pipes[0].handle),
										  __builtin_addressof(pipes[1].handle), __builtin_addressof(sec_attr), 0x4000)) [[unlikely]]
		{
			throw_win32_error();
		}
	}
	inline constexpr auto &in() noexcept
	{
		return pipes[0];
	}
	inline constexpr auto &out() noexcept
	{
		return pipes[1];
	}
};

template <win32_family family, ::std::integral ch_type>
inline constexpr win32_io_redirection redirect(basic_win32_family_pipe<family, ch_type> &hd) noexcept
{
	return {.win32_pipe_in_handle = hd.in().handle, .win32_pipe_out_handle = hd.out().handle};
}

template <win32_family family, ::std::integral ch_type>
inline constexpr basic_win32_family_io_observer<family, ch_type>
input_stream_ref_define(basic_win32_family_pipe<family, ch_type> &pp) noexcept
{
	return {pp.in().handle};
}

template <win32_family family, ::std::integral ch_type>
inline constexpr basic_win32_family_io_observer<family, char>
input_bytes_stream_ref_define(basic_win32_family_pipe<family, ch_type> &pp) noexcept
{
	return {pp.in().handle};
}

template <win32_family family, ::std::integral ch_type>
inline constexpr basic_win32_family_io_observer<family, ch_type>
output_stream_ref_define(basic_win32_family_pipe<family, ch_type> &pp) noexcept
{
	return {pp.out().handle};
}

template <win32_family family, ::std::integral ch_type>
inline constexpr basic_win32_family_io_observer<family, char>
output_bytes_stream_ref_define(basic_win32_family_pipe<family, ch_type> &pp) noexcept
{
	return {pp.out().handle};
}

} // namespace fast_io
