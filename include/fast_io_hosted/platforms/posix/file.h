#pragma once

namespace fast_io
{

struct posix_file_factory FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
{
	using native_handle_type = int;
	int fd{-1};
	inline explicit constexpr posix_file_factory(int v) noexcept
		: fd(v) {};
	inline posix_file_factory(posix_file_factory const &) = delete;
	inline posix_file_factory &operator=(posix_file_factory const &) = delete;
	inline ~posix_file_factory()
	{
		if (fd != -1) [[likely]]
		{
			::fast_io::details::sys_close(fd);
		}
	}
};

template <::fast_io::posix_family family, ::std::integral ch_type>
class basic_posix_family_file : public basic_posix_family_io_observer<family, ch_type>
{
public:
	using typename basic_posix_family_io_observer<family, ch_type>::char_type;
	using typename basic_posix_family_io_observer<family, ch_type>::input_char_type;
	using typename basic_posix_family_io_observer<family, ch_type>::output_char_type;
	using typename basic_posix_family_io_observer<family, ch_type>::native_handle_type;
	using file_factory_type = posix_file_factory;
	using basic_posix_family_io_observer<family, ch_type>::native_handle;
	inline constexpr basic_posix_family_file() noexcept = default;
	template <typename native_hd>
		requires ::std::same_as<native_handle_type, ::std::remove_cvref_t<native_hd>>
	inline explicit constexpr basic_posix_family_file(native_hd fd1) noexcept
		: basic_posix_family_io_observer<family, ch_type>{fd1}
	{
	}

	inline basic_posix_family_file(io_dup_t, basic_posix_family_io_observer<family, ch_type> piob)
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_posix_family_io_observer<family, ch_type>{::fast_io::details::sys_dup(piob.fd)}
	{
	}
	inline explicit constexpr basic_posix_family_file(posix_file_factory &&factory) noexcept
		: basic_posix_family_io_observer<family, ch_type>{factory.fd}
	{
		factory.fd = -1;
	}
	inline constexpr basic_posix_family_file(decltype(nullptr)) noexcept = delete;
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
	// windows specific. open posix file from win32/nt/zw file
	template <win32_family fam>
	inline basic_posix_family_file(basic_win32_family_file<fam, char_type> &&hd, open_mode m)
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_posix_family_io_observer<family, char_type>{details::open_fd_from_handle<ch_type>(hd.handle, m)}
	{
		hd.release();
	}
	template <nt_family fam>
	inline basic_posix_family_file(basic_nt_family_file<fam, char_type> &&hd, open_mode m) FAST_IO_HERBCEPTIONS_THROWS
		: basic_posix_family_io_observer<family, char_type>{details::open_fd_from_handle<ch_type>(hd.handle, m)}
	{
		hd.release();
	}
	inline basic_posix_family_file(nt_fs_dirent fsdirent, open_mode om, perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_posix_family_file(basic_win32_file<char_type>(fsdirent, om, pm), om)
	{
	}
	inline basic_posix_family_file(win32_9xa_fs_dirent fsdirent, open_mode om, perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_posix_family_file(basic_win32_file<char_type>(fsdirent, om, pm), om)
	{
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline basic_posix_family_file(T const &file, open_mode om, perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_posix_family_file(basic_win32_file<char_type>(file, om, pm), om)
	{
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline basic_posix_family_file(nt_at_entry nate, T const &file, open_mode om, perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_posix_family_file(basic_win32_file<char_type>(nate, file, om, pm), om)
	{
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline basic_posix_family_file(win32_9xa_at_entry nate, T const &file, open_mode om, perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_posix_family_file(basic_win32_file<char_type>(nate, file, om, pm), om)
	{
	}
	template <nt_family fam>
	inline explicit constexpr basic_posix_family_file(io_construct_t, basic_nt_family_io_observer<fam, char_type> hd,
													  open_mode m) noexcept
		: basic_posix_family_io_observer<family, char_type>{details::open_fd_from_handle<ch_type>(hd.handle, m)}
	{
	}
	template <win32_family fam>
	inline explicit constexpr basic_posix_family_file(io_construct_t, basic_win32_family_io_observer<fam, char_type> hd,
													  open_mode m) noexcept
		: basic_posix_family_io_observer<family, char_type>{details::open_fd_from_handle<ch_type>(hd.handle, m)}
	{
	}
#else

#if defined(__CYGWIN__)
	template <win32_family fam>
	inline basic_posix_family_file(basic_win32_family_file<fam, char_type> &&hd, open_mode m)
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_posix_family_io_observer<family, char_type>{details::cygwin_create_fd_with_win32_handle(hd.handle, m)}
	{
		hd.release();
	}
	template <nt_family fam>
	inline basic_posix_family_file(basic_nt_family_file<fam, char_type> &&hd, open_mode m)
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_posix_family_io_observer<family, char_type>{details::cygwin_create_fd_with_win32_handle(hd.handle, m)}
	{
		hd.release();
	}
#endif
	inline basic_posix_family_file(posix_fs_dirent fsdirent, open_mode om, perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_posix_family_file(details::my_posix_openat_file_internal_impl(fsdirent.fd, fsdirent.filename, om, pm))
	{
	}
#if !defined(__wasi__) && __has_include(<sys/socket.h>) && __has_include(<netinet/in.h>)
	inline basic_posix_family_file(sock_family d, sock_type t, open_mode m, sock_protocol p)
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_posix_family_io_observer<family, char_type>{::fast_io::details::open_socket_impl(d, t, m, p)}
	{
	}
#endif

	template <::fast_io::constructible_to_os_c_str T>
	inline explicit basic_posix_family_file(T const &filename, open_mode om, perms pm = static_cast<perms>(436)) FAST_IO_HERBCEPTIONS_THROWS
		: basic_posix_family_io_observer<family, char_type>{::fast_io::details::posix_open_file_impl(filename, om, pm)}
	{
	}

	template <::fast_io::constructible_to_os_c_str T>
	inline explicit basic_posix_family_file(posix_at_entry pate, T const &filename, open_mode om, perms pm = static_cast<perms>(436)) FAST_IO_HERBCEPTIONS_THROWS
		: basic_posix_family_io_observer<family, char_type>{::fast_io::details::posix_openat_file_impl(pate.fd, filename, om, pm)}
	{
	}

#endif
#if defined(FAST_IO_HAS_WINE_UNIX)
	template <::fast_io::wine_family fam>
	inline basic_posix_family_file(basic_wine_family_file<fam, char_type> &&wnf, open_mode m) FAST_IO_HERBCEPTIONS_THROWS
		: basic_posix_family_file(basic_nt_file<char_type>(::std::move(wnf), m), m)
	{
	}
#endif
	inline basic_posix_family_file(io_temp_t)
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_posix_family_io_observer<family, char_type>{::fast_io::details::my_open_posix_fd_temp_file()}
	{
	}

	inline constexpr basic_posix_family_file(basic_posix_family_io_observer<family, ch_type>) noexcept = delete;
	inline constexpr basic_posix_family_file &operator=(basic_posix_family_io_observer<family, ch_type>) noexcept = delete;

	inline basic_posix_family_file(basic_posix_family_file const &dp)
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_posix_family_io_observer<family, ch_type>{::fast_io::details::sys_dup(dp.fd)}
	{}
	inline basic_posix_family_file &operator=(basic_posix_family_file const &dp)
		FAST_IO_HERBCEPTIONS_THROWS
	{
		if (__builtin_addressof(dp) == this) [[unlikely]]
		{
			return *this;
		}
		if (this->fd == -1) [[unlikely]]
		{
			this->fd = ::fast_io::details::sys_dup(dp.fd);
		}
		else
		{
			this->fd = ::fast_io::details::sys_dup2(dp.fd, this->fd);
		}
		return *this;
	}
	inline constexpr basic_posix_family_file(basic_posix_family_file &&__restrict b) noexcept
		: basic_posix_family_io_observer<family, ch_type>{b.fd}
	{
		b.fd = -1;
	}
	inline basic_posix_family_file &operator=(basic_posix_family_file &&__restrict b) noexcept
	{
		if (__builtin_addressof(b) == this) [[unlikely]]
		{
			return *this;
		}
		if (this->fd != -1) [[likely]]
		{
			::fast_io::details::sys_close(this->fd);
		}
		this->fd = b.fd;
		b.fd = -1;
		return *this;
	}
	inline constexpr void reset(native_handle_type newfd = -1) noexcept
	{
		if (this->fd != -1) [[likely]]
		{
			::fast_io::details::sys_close(this->fd);
		}
		this->fd = newfd;
	}
	inline void close() FAST_IO_HERBCEPTIONS_THROWS
	{
		if (this->fd != -1) [[likely]]
		{
			::fast_io::details::sys_close_throw_error(this->fd);
		}
	}
	inline ~basic_posix_family_file() noexcept
	{
		if (this->fd != -1) [[likely]]
		{
			::fast_io::details::sys_close(this->fd);
		}
	}
};
#if (!defined(__NEWLIB__) || defined(__CYGWIN__)) && !defined(_PICOLIBC__)

namespace details
{

#if defined(__CYGWIN__)
#if (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if __has_cpp_attribute(__gnu__::__cdecl__)
[[__gnu__::__cdecl__]]
#endif
extern int ftruncate(int, off_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ftruncate")
#else
	__asm__("_ftruncate")
#endif
#else
	__asm__("ftruncate")
#endif
#endif
		;
#endif

inline void posix_truncate_impl(int fd, ::fast_io::uintfpos_t size)
	FAST_IO_HERBCEPTIONS_THROWS
{
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
#if (!defined(__MINGW32__) || __has_include(<_mingw_stat64.h>))
	if (size > static_cast<::std::uint_least64_t>(::std::numeric_limits<::std::int_least64_t>::max()))
	{
		throw_posix_error(EINVAL);
	}
	auto err(noexcept_call(_chsize_s, fd, static_cast<::std::int_least64_t>(size)));
	if (err)
	{
		throw_posix_error();
	}
#else
	if (size > ::std::numeric_limits<long>::max())
	{
		throw_posix_error(EINVAL);
	}
	auto err(noexcept_call(_chsize, fd, static_cast<long>(size)));
	if (err)
	{
		throw_posix_error();
	}
#endif

#elif defined(__linux__)
	if constexpr (sizeof(::std::size_t) >= sizeof(::std::uint_least64_t))
	{
#if defined(__NR_ftruncate)
		if constexpr (::std::numeric_limits<::fast_io::uintfpos_t>::max() > ::std::numeric_limits<::std::uint_least64_t>::max())
		{
			if (size > ::std::numeric_limits<::std::uint_least64_t>::max())
			{
				throw_posix_error(EINVAL);
			}
		}

		system_call_throw_error(system_call<__NR_ftruncate, int>(fd, static_cast<::std::uint_least64_t>(size)));
#else
		if constexpr (::std::numeric_limits<::fast_io::uintfpos_t>::max() > ::std::numeric_limits<off_t>::max())
		{
			if (size > ::std::numeric_limits<off_t>::max())
			{
				throw_posix_error(EINVAL);
			}
		}

		if (noexcept_call(::ftruncate, fd, static_cast<off_t>(size)) < 0)
		{
			throw_posix_error();
		}
#endif
	}
	else if constexpr (sizeof(::std::size_t) >= sizeof(::std::uint_least32_t))
	{
#if defined(__NR_ftruncate64)
		if constexpr (::std::numeric_limits<::fast_io::uintfpos_t>::max() > ::std::numeric_limits<::std::uint_least64_t>::max())
		{
			if (size > ::std::numeric_limits<::std::uint_least64_t>::max())
			{
				throw_posix_error(EINVAL);
			}
		}

		::std::uint_least64_t size_u64{static_cast<::std::uint_least64_t>(size)};
		::std::uint_least32_t size_u32_low{static_cast<::std::uint_least32_t>(size_u64)};
		::std::uint_least32_t size_u32_high{static_cast<::std::uint_least32_t>(size_u64 >> 32u)};

		int result_syscall; // no initlize

		if constexpr (::std::endian::native == ::std::endian::big)
		{
			/* 3 args: fd, size (high, low) */
			result_syscall = ::fast_io::system_call<__NR_ftruncate64, int>(fd, size_u32_high, size_u32_low);
		}
		else
		{
			/* 3 args: fd, size (low, high) */
			result_syscall = ::fast_io::system_call<__NR_ftruncate64, int>(fd, size_u32_low, size_u32_high);
		}

		system_call_throw_error(result_syscall);

#elif defined(__NR_ftruncate)

		if constexpr (::std::numeric_limits<::fast_io::uintfpos_t>::max() > ::std::numeric_limits<::std::uint_least32_t>::max())
		{
			if (size > ::std::numeric_limits<::std::uint_least32_t>::max())
			{
				throw_posix_error(EINVAL);
			}
		}

		system_call_throw_error(system_call<__NR_ftruncate, int>(fd, static_cast<::std::uint_least32_t>(size)));
#else
		if constexpr (::std::numeric_limits<::fast_io::uintfpos_t>::max() > ::std::numeric_limits<off_t>::max())
		{
			if (size > ::std::numeric_limits<off_t>::max())
			{
				throw_posix_error(EINVAL);
			}
		}

		if (noexcept_call(::ftruncate, fd, static_cast<off_t>(size)) < 0)
		{
			throw_posix_error();
		}
#endif
	}
	else
	{
		if constexpr (::std::numeric_limits<::fast_io::uintfpos_t>::max() > ::std::numeric_limits<off_t>::max())
		{
			if (size > ::std::numeric_limits<off_t>::max())
			{
				throw_posix_error(EINVAL);
			}
		}

		if (noexcept_call(::ftruncate, fd, static_cast<off_t>(size)) < 0)
		{
			throw_posix_error();
		}
	}
#else
	if constexpr (::std::numeric_limits<::fast_io::uintfpos_t>::max() > ::std::numeric_limits<off_t>::max())
	{
		if (size > ::std::numeric_limits<off_t>::max())
		{
			throw_posix_error(EINVAL);
		}
	}

	if (
#if defined(__CYGWIN__)
		::fast_io::details::ftruncate(fd, static_cast<off_t>(size)) < 0
#else
		noexcept_call(::ftruncate, fd, static_cast<off_t>(size)) < 0
#endif
	)
	{
		throw_posix_error();
	}
#endif
}

} // namespace details

template <::fast_io::posix_family family, ::std::integral ch_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#endif
inline void truncate(basic_posix_family_io_observer<family, ch_type> h, ::fast_io::uintfpos_t size)
	FAST_IO_HERBCEPTIONS_THROWS
{
	details::posix_truncate_impl(h.fd, size);
}
#endif

namespace freestanding
{
template <>
struct is_trivially_copyable_or_relocatable<posix_file_factory>
{
	inline static constexpr bool value = true;
};

} // namespace freestanding
} // namespace fast_io
