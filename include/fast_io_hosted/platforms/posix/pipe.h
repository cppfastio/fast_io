#pragma once

namespace fast_io
{

template <::fast_io::posix_family family, ::std::integral ch_type>
class basic_posix_family_pipe
{
public:
	using char_type = ch_type;
	basic_posix_family_file<family, ch_type> pipes[2];
	inline basic_posix_family_pipe()
		FAST_IO_HERBCEPTIONS_THROWS
	{
#if defined(__wasi__)
		throw_posix_error(ENOTSUP);
#else

		int a2[2]{-1, -1};

#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
		if (noexcept_call(::_pipe, a2, 131072u, _O_BINARY) == -1)
		{
			throw_posix_error();
		}
#elif defined(__linux__)
		if (noexcept_call(::pipe2, a2, O_CLOEXEC) == -1)
		{
			throw_posix_error();
		}
#elif (defined(__MSDOS__) || defined(__DJGPP__)) || (defined(__NEWLIB__) && !defined(__CYGWIN__))
		if (noexcept_call(::pipe, a2) == -1)
		{
			throw_posix_error();
		}
#else
		{
			if (noexcept_call(::pipe, a2) == -1)
			{
				throw_posix_error();
			}
			::fast_io::posix_file_factory fd0(a2[0]);
			::fast_io::posix_file_factory fd1(a2[1]);
			::fast_io::details::sys_fcntl(fd0.fd, F_SETFD, FD_CLOEXEC);
			::fast_io::details::sys_fcntl(fd1.fd, F_SETFD, FD_CLOEXEC);
			fd0.fd = -1;
			fd1.fd = -1;
		}
#endif
		pipes->fd = *a2;
		pipes[1].fd = a2[1];
#endif
	}
	inline constexpr auto &in() noexcept
	{
		return *pipes;
	}
	inline constexpr auto &out() noexcept
	{
		return pipes[1];
	}
};

template <::fast_io::posix_family family, ::std::integral ch_type>
inline constexpr basic_posix_family_io_observer<family, ch_type> input_stream_ref_define(basic_posix_family_pipe<family, ch_type> &pp) noexcept
{
	return {pp.in().fd};
}

template <::fast_io::posix_family family, ::std::integral ch_type>
inline constexpr basic_posix_family_io_observer<family, char> input_bytes_stream_ref_define(basic_posix_family_pipe<family, ch_type> &pp) noexcept
{
	return {pp.in().fd};
}

template <::fast_io::posix_family family, ::std::integral ch_type>
inline constexpr basic_posix_family_io_observer<family, ch_type> output_stream_ref_define(basic_posix_family_pipe<family, ch_type> &pp) noexcept
{
	return {pp.out().fd};
}

template <::fast_io::posix_family family, ::std::integral ch_type>
inline constexpr basic_posix_family_io_observer<family, char> output_bytes_stream_ref_define(basic_posix_family_pipe<family, ch_type> &pp) noexcept
{
	return {pp.out().fd};
}

#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
template <::fast_io::posix_family family, ::std::integral ch_type>
inline constexpr win32_io_redirection redirect_handle(basic_posix_family_pipe<family, ch_type> &h) noexcept
{
	return {__builtin_addressof(h.in().fd),
			__builtin_addressof(h.out().fd)};
}
#else
template <::fast_io::posix_family family, ::std::integral ch_type>
inline constexpr posix_io_redirection redirect(basic_posix_family_pipe<family, ch_type> &h) noexcept
{
	return {.pipe_fds = __builtin_addressof(h.in().fd)};
}
#endif
} // namespace fast_io
