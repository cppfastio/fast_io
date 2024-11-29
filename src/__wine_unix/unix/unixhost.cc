#include <__wine_unix/__wine_unix.h>
#include <__wine_unix/__wine_unix_errno.h>
#include <errno.h>
#include <limits>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>

namespace __wine_unix
{

struct c_path_guard
{
	__wine_errno_t host_errno{};
	char const *filename_c_str{};
	constexpr c_path_malloc_guard() noexcept = default;
	explicit constexpr c_path_malloc_guard(__wine_errno_t hhost_errno, char const *pptr) noexcept
		: host_errno{hhost_errno}, ptr(pptr)
	{}
	c_path_malloc_guard(c_path_malloc_guard const &) = delete;
	c_path_malloc_guard &operator=(c_path_malloc_guard const &) = delete;
	c_path_malloc_guard() ~c_path_malloc_guard()
	{
		if (filename_c_str)
		{
			free(filename_c_str);
		}
	}
};

inline auto c_path_common(void const *filename, size_t filenamelen) noexcept
{
	__wine_errno_t host_errno{__WINE_UNIX_ERRNO_EINVAL};
	char const *cstr{static_cast<char const *>(str)};
	char *newmem{};
	if (filenamelen == SIZE_MAX)
	{
		goto failed;
	}
	if (__builtin_strnlen(cstr, len) != len)
	{
		goto failed;
	}
	newmem = reinterpret_cast<char *>(malloc(filenamelen + 1));
	if (newmem == nullptr)
	{
		host_errno = __WINE_UNIX_ERRNO_ENOMEM;
		goto failed;
	}
	if (filenamelen)
	{
		memcpy(newmem, filename, filenamelen);
	}
	newmem[filenamelen] = 0;
	host_errno = __WINE_UNIX_ERRNO_SUCCESS;
failed:
	return __wine_unix::c_path_malloc_guard(host_errno, newmem);
}

inline constexpr ::__wine_errno_t host_errno_to_wine_errno(int val) noexcept
{
	/*
	Todo. just implement a place holder first so i can test whether this works out
	*/
	// #ifdef __linux__
	return static_cast<::__wine_errno_t>(static_cast<unsigned>(val));
	// #endif
}

struct unix_fd_result
{
	__wine_errno_t errcode{};
	int unix_fd{-1};
};

inline unix_fd_result wine_unix_ret_host_fd_to_unix_fd_impl(__wine_host_fd_t host_fd) noexcept
{
	__wine_errno_t errcode = __WINE_UNIX_ERRNO_EBADF;
	int unixfd = -1;
	if (host_fd != 0)
	{
		--host_fd;
		constexpr unsigned intmx{static_cast<unsigned>(::std::numeric_limits<int>::max())};
		if (host_fd <= intmx)
		{
			unixfd = static_cast<int>(host_fd);
			errcode = __WINE_UNIX_ERRNO_SUCCESS;
		}
	}
	return {errcode, unixfd};
}

inline __wine_unix_result_ret_openat wine_unix_ret_unix_fd_to_host_fd_impl(int unixfd) __WINE_UNIX_NOEXCEPT
{
	if (unixfd < 0)
	{
		return {__WINE_UNIX_ERRNO_SUCCESS, 0};
	}
	__wine_host_fd_t host_fd{static_cast<__wine_host_fd_t>(unixfd)};
	++host_fd;
	return {__WINE_UNIX_ERRNO_SUCCESS, host_fd};
}

inline constexpr __wine_unix_result_ret_readwritev wine_unix_readwritev_result_common_split(ptrdiff_t ret, __wine_unix_iovec_t const *iovs, size_t iovsize) noexcept
{
	size_t const uret{static_cast<size_t>(ret)};
	auto *const iovsed{iovs + iovsize};
	auto *i{iovs};
	size_t lastn{uret};
	for (; i != iovsed; ++i)
	{
		size_t const ilen{i->len};
		if (lastn < ilen)
		{
			break;
		}
		lastn -= ilen;
	}
	return {0, uret, static_cast<size_t>(i - iovs), lastn};
}

template <typename Fn>
inline constexpr __wine_unix_result_ret_readwritev wine_unix_readwritev_result_common(__wine_host_fd_t host_fd, __wine_unix_iovec_t *iovs, size_t iovsize, Fn fn) noexcept
{
	auto [errcode, fd] = ::__wine_unix::wine_unix_ret_host_fd_to_unix_fd_impl(host_fd);
	if (errcode)
	{
		return {errcode, 0, 0, 0};
	}
	constexpr int mxval{
#ifdef IOV_MAX
		IOV_MAX
#else
		INT_MAX
#endif
	};

	if (mxval < iovsize)
	{
		iovsize = mxval;
	}
	auto ret = fn(fd, reinterpret_cast<struct iovec const *>(iovs), static_cast<int>(iovsize));
	if (ret == -1)
	{
		return {__wine_unix::host_errno_to_wine_errno(errno), 0, 0, 0};
	}
	return __wine_unix::wine_unix_readwritev_result_common_split(ret, iovs, iovsize);
}

} // namespace __wine_unix

extern "C"
{
	__WINE_UNIX_DLLEXPORT __wine_errno_t __WINE_UNIX_DEFAULTCALL __wine_unix_ret_host_fd_to_unix_fd(__wine_host_fd_t host_fd, void *punixfd, size_t unixfdsizeof) __WINE_UNIX_NOEXCEPT
	{
		if (punixfd == nullptr)
		{
			return __WINE_UNIX_ERRNO_EFAULT;
		}
		if (unixfdsizeof != sizeof(int))
		{
			return __WINE_UNIX_ERRNO_EINVAL;
		}
		auto [errcode, host_fd] = ::__wine_unix::wine_unix_ret_host_fd_to_unix_fd_impl(host_fd);
		*reinterpret_cast<int *>(punixfd) = host_fd;
		return errcode;
	}

	__WINE_UNIX_DLLEXPORT __wine_unix_result_ret_host_fd __WINE_UNIX_DEFAULTCALL __wine_unix_ret_unix_fd_to_host_fd(void const *punixfd, size_t unixfdsizeof) __WINE_UNIX_NOEXCEPT
	{
		if (punixfd == nullptr)
		{
			return {__WINE_UNIX_ERRNO_EFAULT, 0};
		}
		if (unixfdsizeof != sizeof(int))
		{
			return {__WINE_UNIX_ERRNO_EINVAL, 0};
		}
		return ::__wine_unix::wine_unix_ret_unix_fd_to_host_fd_impl(*reinterpret_cast<int *>(punixfd));
	}

	__WINE_UNIX_DLLEXPORT __wine_unix_result_ret_nt_handle __WINE_UNIX_DEFAULTCALL __wine_unix_ret_host_fd_to_nt_handle(__wine_host_fd_t) noexcept
	{
		return {__WINE_UNIX_ERRNO_EOPNOTSUPP, 0};
	}

	__WINE_UNIX_DLLEXPORT __wine_unix_result_ret_nt_handle __WINE_UNIX_DEFAULTCALL __wine_unix_ret_nt_handle_to_host_fd(ptrdiff_t) noexcept
	{
		return {__WINE_UNIX_ERRNO_EOPNOTSUPP, 0};
	}

	__WINE_UNIX_DLLEXPORT __wine_unix_result_ret_host_fd __WINE_UNIX_DEFAULTCALL __wine_unix_ret_openat(__wine_host_fd_t host_dirfd, void const *filename, size_t filenamelen, __wine_host_flags_t flags, __wine_host_mode_t mode) __WINE_UNIX_NOEXCEPT
	{
		auto [errcode, dirfd] = ::__wine_unix::wine_unix_ret_host_fd_to_unix_fd_impl(host_dirfd);
		if (errcode)
		{
			return {errcode, 0};
		}
		auto pathret = __wine_unix::c_path_common(filename, filenamelen);
		if (pathret.host_errno)
		{
			return {pathret.host_errno, 0};
		}
		return ::__wine_unix::wine_unix_ret_host_fd_to_unix_fd_impl(::openat(dirfd, pathret.filename_c_str, flags, mode));
	}

	__WINE_UNIX_DLLEXPORT __wine_errno_t __WINE_UNIX_DEFAULTCALL __wine_unix_ret_close(__wine_host_fd_t host_fd) __WINE_UNIX_NOEXCEPT
	{
		auto [errcode, fd] = ::__wine_unix::wine_unix_ret_host_fd_to_unix_fd_impl(host_dirfd);
		if (errcode)
		{
			return errcode;
		}
		if (::close(fd) == -1)
		{
			return __wine_unix::host_errno_to_wine_errno(errno);
		}
		return __WINE_UNIX_ERRNO_SUCCESS;
	}

	__WINE_UNIX_DLLEXPORT __wine_unix_result_ret_readwritev __WINE_UNIX_DEFAULTCALL __wine_unix_ret_writev(__wine_host_fd_t host_fd, __wine_unix_iovec_t const *iovs, size_t iovsize) __WINE_UNIX_NOEXCEPT
	{
		return __wine_unix::wine_unix_readwritev_result_common(ret, iovs, iovsize, [](int fd, struct iovec const *iov, int iovcnt) {
			return ::writev(fd, iov, iovcnt);
		});
	}

	__WINE_UNIX_DLLEXPORT __wine_unix_result_ret_readwritev __WINE_UNIX_DEFAULTCALL __wine_unix_ret_readv(__wine_host_fd_t host_fd, __wine_unix_iovec_t const *iovs, size_t iovsize) __WINE_UNIX_NOEXCEPT
	{
		return __wine_unix::wine_unix_readwritev_result_common(ret, iovs, iovsize, [](int fd, struct iovec const *iov, int iovcnt) {
			return ::readv(fd, iov, iovcnt);
		});
	}


	__WINE_UNIX_DLLEXPORT __wine_unix_result_ret_readwritev __WINE_UNIX_DEFAULTCALL __wine_unix_ret_pwritev(__wine_host_fd_t host_fd, __wine_unix_iovec_t const *iovs, size_t iovsize, __wine_off_t offset) __WINE_UNIX_NOEXCEPT
	{
		if constexpr (sizeof(off_t) < sizeof(__wine_off_t))
		{
			constexpr off_t off_min{::std::numeric_limits<off_t>::min()};
			constexpr off_t off_max{::std::numeric_limits<off_t>::max()};
			if (offset < off_min || off_max < offset)
			{
				return __WINE_UNIX_ERRNO_EOVERFLOW;
			}
		}
		return __wine_unix::wine_unix_readwritev_result_common(host_fd, iovs, iovsize, [offset](int fd, struct iovec const *iov, int iovcnt) {
			return ::pwritev(fd, iov, iovcnt, offset);
		});
	}

	__WINE_UNIX_DLLEXPORT __wine_unix_result_ret_readwritev __WINE_UNIX_DEFAULTCALL __wine_unix_ret_preadv(__wine_host_fd_t host_fd, __wine_unix_iovec_t const *iovs, size_t iovsize, __wine_off_t offset) __WINE_UNIX_NOEXCEPT
	{
		if constexpr (sizeof(off_t) < sizeof(__wine_off_t))
		{
			constexpr off_t off_min{::std::numeric_limits<off_t>::min()};
			constexpr off_t off_max{::std::numeric_limits<off_t>::max()};
			if (offset < off_min || off_max < offset)
			{
				return __WINE_UNIX_ERRNO_EOVERFLOW;
			}
		}
		return __wine_unix::wine_unix_readwritev_result_common(host_fd, iovs, iovsize, [offset](int fd, struct iovec const *iov, int iovcnt) {
			return ::preadv(fd, iov, iovcnt, offset);
		});
	}
}