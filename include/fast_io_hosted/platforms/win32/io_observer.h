#pragma once

namespace fast_io
{

struct win32_io_redirection_std : win32_io_redirection
{
	inline constexpr win32_io_redirection_std() noexcept = default;
	template <typename T>
		requires requires(T &&t) {
			{ redirect(::std::forward<T>(t)) } -> ::std::same_as<win32_io_redirection>;
		}
	inline constexpr win32_io_redirection_std(T &&t) noexcept
		: win32_io_redirection(redirect(::std::forward<T>(t)))
	{
	}
};

struct win32_process_io // Shared by win32 and nt
{
	win32_io_redirection_std in;
	win32_io_redirection_std out;
	win32_io_redirection_std err;
};

template <win32_family family, ::std::integral ch_type>
class basic_win32_family_io_observer
{
public:
	using native_handle_type = void *;
	using char_type = ch_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
	native_handle_type handle{};
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return handle;
	}
	inline explicit operator bool() const noexcept
	{
		return handle != nullptr && handle != reinterpret_cast<void *>(static_cast<::std::ptrdiff_t>(-1));
	}
	template <nt_family family2>
	inline explicit constexpr operator basic_nt_family_io_observer<family2, char_type>() const noexcept
	{
		return basic_nt_family_io_observer<family2, char_type>{handle};
	}
	template <win32_family family2>
	inline explicit constexpr operator basic_win32_family_io_observer<family2, char_type>() const noexcept
	{
		return basic_win32_family_io_observer<family2, char_type>{handle};
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

template <win32_family family, ::std::integral ch_type>
inline constexpr bool operator==(basic_win32_family_io_observer<family, ch_type> a,
								 basic_win32_family_io_observer<family, ch_type> b) noexcept
{
	return a.handle == b.handle;
}

#if __cpp_impl_three_way_comparison >= 201907L
template <win32_family family, ::std::integral ch_type>
inline constexpr auto operator<=>(basic_win32_family_io_observer<family, ch_type> a,
								  basic_win32_family_io_observer<family, ch_type> b) noexcept
{
	return a.handle <=> b.handle;
}
#endif

template <win32_family family, ::std::integral ch_type>
inline constexpr nt_at_entry at(basic_win32_family_io_observer<family, ch_type> wiob) noexcept
{
	return nt_at_entry{wiob.handle};
}

template <win32_family family, ::std::integral ch_type>
inline constexpr win32_io_redirection redirect(basic_win32_family_io_observer<family, ch_type> other) noexcept
{
	return {.win32_handle = other.handle};
}

template <nt_family family, ::std::integral ch_type>
inline constexpr win32_io_redirection redirect(basic_nt_family_io_observer<family, ch_type> other) noexcept
{
	return {.win32_handle = other.handle};
}

namespace win32::details
{

inline void *win32_dup_impl(void *handle)
	FAST_IO_HERBCEPTIONS_THROWS
{
	void *current_process{reinterpret_cast<void *>(static_cast<::std::ptrdiff_t>(-1))};
	void *new_handle{};
	if (!::fast_io::win32::DuplicateHandle(current_process, handle, current_process, __builtin_addressof(new_handle), 0,
										   true, 2 /*DUPLICATE_SAME_ACCESS*/))
	{
		throw_win32_error();
	}
	return new_handle;
}

inline void *win32_dup2_impl(void *handle, void *newhandle)
	FAST_IO_HERBCEPTIONS_THROWS
{
	auto temp{win32_dup_impl(handle)};
	if (newhandle) [[likely]]
	{
		::fast_io::win32::CloseHandle(newhandle);
	}
	return temp;
}

inline void win32_flush_impl(void *__restrict handle)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if (!::fast_io::win32::FlushFileBuffers(handle))
	{
		throw_win32_error();
	}
}

inline void win32_data_sync_impl(void *__restrict handle, data_sync_flags flags [[maybe_unused]])
	FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(_WIN32_WINDOWS)
	win32_flush_impl(handle);
#else
	::fast_io::win32::nt::details::nt_data_sync_impl<false>(handle, flags);
#endif
}

} // namespace win32::details

template <win32_family family, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#endif
inline void io_stream_buffer_flush_define(basic_win32_family_io_observer<family, char_type> wiob)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::details::win32_flush_impl(wiob.handle);
}

template <win32_family family, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#endif
inline void data_sync(basic_win32_family_io_observer<family, char_type> wiob, data_sync_flags flags)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::details::win32_data_sync_impl(wiob.handle, flags);
}

template <win32_family family, ::std::integral ch_type>
inline constexpr basic_win32_family_io_observer<family, ch_type>
io_stream_ref_define(basic_win32_family_io_observer<family, ch_type> other) noexcept
{
	return other;
}

template <win32_family family, ::std::integral ch_type>
inline constexpr basic_win32_family_io_observer<family, char>
io_bytes_stream_ref_define(basic_win32_family_io_observer<family, ch_type> other) noexcept
{
	return {other.handle};
}

template <win32_family family, ::std::integral char_type>
inline constexpr ::std::conditional_t<family == win32_family::ansi_9x, nop_file_lock, nt_file_lock>
file_lock(basic_win32_family_io_observer<family, char_type> wiob) noexcept
{
	if constexpr (family == win32_family::ansi_9x)
	{
		return {};
	}
	else
	{
		return {wiob.handle};
	}
}

template <win32_family family, ::std::integral ch_type, typename... Args>
	requires requires(basic_win32_family_io_observer<family, ch_type> h, Args &&...args) {
		::fast_io::win32::DeviceIoControl(h.handle, ::std::forward<Args>(args)...);
	}
inline void io_control(basic_win32_family_io_observer<family, ch_type> h, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if (!fast_io::win32::DeviceIoControl(h.handle, ::std::forward<Args>(args)...))
	{
		throw_win32_error();
	}
}

} // namespace fast_io
