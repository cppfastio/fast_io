#pragma once

namespace fast_io::details
{

#ifdef __MSDOS__
extern int dup(int) noexcept __asm__("_dup");
extern int dup2(int, int) noexcept __asm__("_dup2");
extern int _close(int) noexcept __asm__("_close");
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

inline int sys_dup(int old_fd)
{
#if defined(__linux__) && defined(__NR_dup)
	int fd{system_call<__NR_dup, int>(old_fd)};
	system_call_throw_error(fd);
	return fd;
#else
	auto fd{noexcept_call(
#if defined(_WIN32) && !defined(__BIONIC__)
		_dup
#else
		dup
#endif
		,
		old_fd)};
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
	int fd{system_call<__NR_dup2, int>(old_fd, new_fd)};
	system_call_throw_error<always_terminate>(fd);
	return fd;
#else
	auto fd{noexcept_call(
#if defined(_WIN32) && !defined(__BIONIC__)
		_dup2
#else
		dup2
#endif
		,
		old_fd, new_fd)};
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
	int fd{system_call<__NR_dup2, int>(old_fd, new_fd)};
	if (linux_system_call_fails(fd))
	{
		return {-fd, true};
	}
#else
	auto fd{noexcept_call(
#if defined(_WIN32) && !defined(__BIONIC__)
		_dup2
#else
		dup2
#endif
		,
		old_fd, new_fd)};
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
		system_call<__NR_close, int>(fd);
#elif (defined(_WIN32) && !defined(__BIONIC__)) || defined(__MSDOS__)
		noexcept_call(_close, fd);
#else
		noexcept_call(close, fd);
#endif
}

inline void sys_close_throw_error(int &fd)
{
	auto ret{sys_close(fd)};
	fd = -1; // POSIX standard says we should never call close(2) again even close syscall fails
	system_call_throw_error(ret);
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
	auto result{system_call<__NR_fcntl, int>(fd, op, args...)};
	system_call_throw_error(result);
	return result;
#else
	auto result{posix::fcntl(fd, op, args...)};
	if (result == -1)
	{
		throw_posix_error();
	}
	return result;
#endif
}

#endif

} // namespace fast_io::details
