#pragma once

namespace fast_io::details
{
namespace posix
{
#if defined(__MSDOS__) || defined(__DJGPP__)
extern int dup(int) noexcept __asm__("_dup");
extern int dup2(int, int) noexcept __asm__("_dup2");
extern int close(int) noexcept __asm__("_close");
#elif defined(__wasi__)
inline int dup(int) noexcept
{
	errno = ENOSYS;
	return -1;
}

inline int dup2(int old_fd, int new_fd) noexcept
{
	return ::fast_io::noexcept_call(__wasi_fd_renumber, old_fd, new_fd);
}
#endif
} // namespace posix

inline int sys_dup(int old_fd)
{
#if defined(__linux__) && defined(__NR_dup)
	int fd{::fast_io::system_call<__NR_dup, int>(old_fd)};
	::fast_io::system_call_throw_error(fd);
	return fd;
#else

#if (defined(__MSDOS__) || defined(__DJGPP__)) || defined(__wasi__)
	auto fd{::fast_io::details::dup(old_fd)};
#else
	auto fd{::fast_io::noexcept_call(
#if defined(_WIN32) && !defined(__BIONIC__)
		::_dup
#else
		::dup
#endif
		,
		old_fd)};
#endif

	if (fd == -1)
	{
		throw_posix_error();
	}
	return fd;
#endif
}

template <bool always_terminate = false>
inline int sys_dup2(int old_fd, int new_fd)
{
#if defined(__linux__) && defined(__NR_dup2)
	int fd{::fast_io::system_call<__NR_dup2, int>(old_fd, new_fd)};
	::fast_io::system_call_throw_error<always_terminate>(fd);
	return fd;
#else

#if (defined(__MSDOS__) || defined(__DJGPP__)) || defined(__wasi__)
	auto fd{::fast_io::details::dup2(old_fd, new_fd)};
#else
	auto fd{::fast_io::noexcept_call(
#if defined(_WIN32) && !defined(__BIONIC__)
		::_dup2
#else
		::dup2
#endif
		,
		old_fd, new_fd)};
#endif

	if (fd == -1)
	{
		if constexpr (always_terminate)
		{
			fast_terminate();
		}
		else
		{
			throw_posix_error();
		}
	}
	return fd;
#endif
}

struct return_code
{
	int code{};
	bool error{};
};

inline return_code sys_dup2_nothrow(int old_fd, int new_fd) noexcept
{
#if defined(__linux__) && defined(__NR_dup2)
	int fd{::fast_io::system_call<__NR_dup2, int>(old_fd, new_fd)};
	if (::fast_io::linux_system_call_fails(fd))
	{
		return {-fd, true};
	}
#else

#if (defined(__MSDOS__) || defined(__DJGPP__)) || defined(__wasi__)
	auto fd{::fast_io::details::dup2(old_fd, new_fd)};
#else
	auto fd{::fast_io::noexcept_call(
#if defined(_WIN32) && !defined(__BIONIC__)
		::_dup2
#else
		dup2
#endif
		,
		old_fd, new_fd)};
#endif

	if (fd == -1)
	{
		return {errno, true};
	}
#endif
	return {fd};
}

inline int sys_close(int fd) noexcept
{
	return
#if defined(__linux__) && defined(__NR_close)
		::fast_io::system_call<__NR_close, int>(fd);
#elif (defined(_WIN32) && !defined(__BIONIC__)) || defined(__MSDOS__)
		::fast_io::noexcept_call(::_close, fd);
#elif defined(__MSDOS__) || defined(__DJGPP__)
		::fast_io::details::posix::close(fd);
#else
		::fast_io::noexcept_call(::close, fd);
#endif
}

inline void sys_close_throw_error(int &fd)
{
	auto ret{::fast_io::details::sys_close(fd)};
	fd = -1; // POSIX standard says we should never call close(2) again even close syscall fails
#if defined(__linux__) && defined(__NR_close)
	::fast_io::system_call_throw_error(ret);
#else
	if (ret == -1) [[unlikely]]
	{
		::fast_io::throw_posix_error();
	}
#endif
}
#if (!defined(__NEWLIB__) || defined(__CYGWIN__)) && !defined(_WIN32) && __has_include(<dirent.h>) && !defined(_PICOLIBC__)

namespace posix
{
extern int fcntl(int fd, int cmd, ... /* arg */) noexcept
#if !defined(__MSDOS__) && !defined(__DARWIN_C_LEVEL)
	__asm__("fcntl")
#else
	__asm__("_fcntl")
#endif
		;
} // namespace posix

template <typename... Args>
	requires(::std::is_scalar_v<Args> && ...)
inline int sys_fcntl(int fd, int op, Args... args)
{
#if defined(__linux__) && defined(__NR_fcntl)
	auto result{::fast_io::system_call<__NR_fcntl, int>(fd, op, args...)};
	::fast_io::system_call_throw_error(result);
	return result;
#else
	auto result{posix::fcntl(fd, op, args...)};
	if (result == -1)
	{
		::fast_io::throw_posix_error();
	}
	return result;
#endif
}

#endif

} // namespace fast_io::details
