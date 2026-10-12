#pragma once

namespace fast_io
{

namespace details
{
#if !defined(__WINE__)
inline void *my_get_osfile_handle(int fd) noexcept;
#endif
} // namespace details

struct nt_at_entry
{
	using native_handle_type = void *;
	void *handle{};
	inline explicit constexpr nt_at_entry() noexcept = default;
	inline explicit constexpr nt_at_entry(void *mhandle) noexcept
		: handle(mhandle)
	{}
#if !defined(__WINE__)
	inline nt_at_entry(posix_at_entry pate) noexcept
		: handle(details::my_get_osfile_handle(pate.fd))
	{}
#endif
};

#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline nt_at_entry nt_at_fdcwd() noexcept
{
	constexpr ::std::ptrdiff_t value{-3}; // use -3 as at_fdwcd handle
	return nt_at_entry{bit_cast<void *>(value)};
}

#if !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__) && !defined(_WIN32_WINDOWS)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline nt_at_entry at_fdcwd() noexcept
{
	return nt_at_fdcwd();
}
#endif

template <nt_family family>
struct nt_family_at_entry : nt_at_entry
{
	using nt_at_entry::native_handle_type;
	inline explicit constexpr nt_family_at_entry() noexcept = default;
	inline explicit constexpr nt_family_at_entry(void *mhandle) noexcept
		: nt_at_entry{mhandle}
	{}
};

struct nt_fs_dirent
{
	void *handle{};
	::fast_io::manipulators::basic_os_c_str_with_known_size<char16_t> filename{};
};

using zw_at_entry = nt_family_at_entry<nt_family::zw>;

template <win32_family family, ::std::integral ch_type>
class basic_win32_family_io_observer;

template <nt_family family, ::std::integral ch_type>
	requires(family == nt_family::nt || family == nt_family::zw)
class basic_nt_family_io_observer
{
public:
	using native_handle_type = void *;
	using char_type = ch_type;
	using input_char_type = ch_type;
	using output_char_type = ch_type;
	native_handle_type handle{};
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return handle;
	}
	inline explicit constexpr operator bool() const noexcept
	{
		return handle != nullptr;
	}
	template <win32_family family2>
	inline explicit operator basic_win32_family_io_observer<family2, char_type>() const noexcept
	{
		return basic_win32_family_io_observer<family2, char_type>{reinterpret_cast<void *>(handle)};
	}
	template <nt_family family2>
	inline constexpr operator basic_nt_family_io_observer<family2, char_type>() const noexcept
	{
		return basic_nt_family_io_observer<family2, char_type>{handle};
	}
#if defined(FAST_IO_HAS_WINE_UNIX)
	template <wine_family family2>
	inline explicit operator basic_wine_family_io_observer<family2, char_type>() const
		FAST_IO_HERBCEPTIONS_THROWS
	{
		return basic_wine_family_io_observer<family2, char_type>{
			::fast_io::wine::wine_unix_nt_handle_to_host_fd_ref(
				static_cast<::std::ptrdiff_t>(reinterpret_cast<::std::uintptr_t>(handle)))};
	}
#endif
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{handle};
		handle = nullptr;
		return temp;
	}
};

template <nt_family family, ::std::integral ch_type>
inline constexpr bool operator==(basic_nt_family_io_observer<family, ch_type> a,
								 basic_nt_family_io_observer<family, ch_type> b) noexcept
{
	return a.handle == b.handle;
}

#if __cpp_impl_three_way_comparison >= 201907L
template <nt_family family, ::std::integral ch_type>
inline constexpr auto operator<=>(basic_nt_family_io_observer<family, ch_type> a,
								  basic_nt_family_io_observer<family, ch_type> b) noexcept
{
	return a.handle <=> b.handle;
}
#endif

template <nt_family family, ::std::integral ch_type>
inline constexpr auto at(basic_nt_family_io_observer<family, ch_type> niob) noexcept
{
	if constexpr (family == nt_family::nt)
	{
		return nt_at_entry{niob.handle};
	}
	else
	{
		return nt_family_at_entry<family>{niob.handle};
	}
}

template <nt_family family, ::std::integral ch_type>
inline constexpr basic_nt_family_io_observer<family, ch_type>
io_stream_ref_define(basic_nt_family_io_observer<family, ch_type> other) noexcept
{
	return other;
}

template <nt_family family, ::std::integral ch_type>
inline constexpr basic_nt_family_io_observer<family, char>
io_bytes_stream_ref_define(basic_nt_family_io_observer<family, ch_type> other) noexcept
{
	return {other.handle};
}

namespace win32::nt::details
{

template <bool zw>
inline void nt_flush_impl(void *handle)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::nt::io_status_block block;
	::std::uint_least32_t status{::fast_io::win32::nt::nt_flush_buffers_file<zw>(handle, __builtin_addressof(block))};
	if (status)
	{
		::fast_io::herbceptions::throws_nt_errc_with_value(status);
	}
}

template <bool zw>
inline void nt_data_sync_impl(void *handle, data_sync_flags flags [[maybe_unused]])
	FAST_IO_HERBCEPTIONS_THROWS
{
#if (!defined(_WIN32_WINNT) || _WIN32_WINNT >= 0x0602) || WINVER >= 0x0602
	/*
	NtFlushBuffersFileEx and ZwFlushBuffersFileEx are only provided since windows 8
	*/
	::fast_io::win32::nt::io_status_block block;
	::std::uint_least32_t status{::fast_io::win32::nt::nt_flush_buffers_file_ex<zw>(
		handle, static_cast<::std::uint_least32_t>(flags), nullptr, 0u, __builtin_addressof(block))};
	if (status)
	{
		::fast_io::herbceptions::throws_nt_errc_with_value(status);
	}
#else
	nt_flush_impl<zw>(handle);
#endif
}

} // namespace win32::nt::details

template <nt_family family, ::std::integral ch_type>
inline void io_stream_buffer_flush_define(basic_nt_family_io_observer<family, ch_type> ntiob)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::nt::details::nt_flush_impl<family == nt_family::zw>(ntiob.handle);
}

template <nt_family family, ::std::integral ch_type>
inline void data_sync(basic_nt_family_io_observer<family, ch_type> ntiob, data_sync_flags flags)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::nt::details::nt_data_sync_impl<family == nt_family::zw>(ntiob.handle, flags);
}

} // namespace fast_io
