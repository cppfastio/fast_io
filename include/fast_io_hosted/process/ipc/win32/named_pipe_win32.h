#pragma once

namespace fast_io
{
template <win32_family family, ::std::integral ch_type>
class basic_win32_family_named_pipe_ipc_observer
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
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{handle};
		handle = nullptr;
		return temp;
	}
};

template <win32_family family, ::std::integral ch_type>
inline constexpr bool operator==(basic_win32_family_named_pipe_ipc_observer<family, ch_type> a,
								 basic_win32_family_named_pipe_ipc_observer<family, ch_type> b) noexcept
{
	return a.handle == b.handle;
}

#if __cpp_impl_three_way_comparison >= 201907L
template <win32_family family, ::std::integral ch_type>
inline constexpr auto operator<=>(basic_win32_family_named_pipe_ipc_observer<family, ch_type> a,
								  basic_win32_family_named_pipe_ipc_observer<family, ch_type> b) noexcept
{
	return a.handle <=> b.handle;
}
#endif

template <win32_family family, ::std::integral ch_type>
class basic_win32_family_named_pipe_ipc : public basic_win32_family_named_pipe_ipc_observer<family, ch_type>
{
public:
	using typename basic_win32_family_named_pipe_ipc_observer<family, ch_type>::char_type;
	using typename basic_win32_family_named_pipe_ipc_observer<family, ch_type>::input_char_type;
	using typename basic_win32_family_named_pipe_ipc_observer<family, ch_type>::output_char_type;
	using typename basic_win32_family_named_pipe_ipc_observer<family, ch_type>::native_handle_type;
	using basic_win32_family_named_pipe_ipc_observer<family, ch_type>::native_handle;
	
	inline explicit constexpr basic_win32_family_named_pipe_ipc() noexcept = default;

	inline constexpr basic_win32_family_named_pipe_ipc(basic_win32_family_named_pipe_ipc_observer<family, ch_type>) noexcept = delete;
	inline constexpr basic_win32_family_named_pipe_ipc &operator=(basic_win32_family_named_pipe_ipc_observer<family, ch_type>) noexcept = delete;

};


template <::std::integral ch_type>
using basic_win32_named_pipe_ipc_observer = basic_win32_family_named_pipe_ipc_observer<win32_family::native, ch_type>;
template <::std::integral ch_type>
using basic_win32_named_pipe_ipc_observer_ntw = basic_win32_family_named_pipe_ipc_observer<win32_family::wide_nt, ch_type>;
template <::std::integral ch_type>
using basic_win32_named_pipe_ipc_observer_9xa = basic_win32_family_named_pipe_ipc_observer<win32_family::ansi_9x, ch_type>;

using win32_named_pipe_ipc_observer = basic_win32_named_pipe_ipc_observer<char>;
using win32_named_pipe_ipc_observer_ntw = basic_win32_named_pipe_ipc_observer_ntw<char>;
using win32_named_pipe_ipc_observer_9xa = basic_win32_named_pipe_ipc_observer_9xa<char>;

using wwin32_named_pipe_ipc_observer = basic_win32_named_pipe_ipc_observer<wchar_t>;
using wwin32_named_pipe_ipc_observer_ntw = basic_win32_named_pipe_ipc_observer_ntw<wchar_t>;
using wwin32_named_pipe_ipc_observer_9xa = basic_win32_named_pipe_ipc_observer_9xa<wchar_t>;

using u8win32_named_pipe_ipc_observer = basic_win32_named_pipe_ipc_observer<char8_t>;
using u8win32_named_pipe_ipc_observer_ntw = basic_win32_named_pipe_ipc_observer_ntw<char8_t>;
using u8win32_named_pipe_ipc_observer_9xa = basic_win32_named_pipe_ipc_observer_9xa<char8_t>;

using u16win32_named_pipe_ipc_observer = basic_win32_named_pipe_ipc_observer<char16_t>;
using u16win32_named_pipe_ipc_observer_ntw = basic_win32_named_pipe_ipc_observer_ntw<char16_t>;
using u16win32_named_pipe_ipc_observer_9xa = basic_win32_named_pipe_ipc_observer_9xa<char16_t>;

using u32win32_named_pipe_ipc_observer = basic_win32_named_pipe_ipc_observer<char32_t>;
using u32win32_named_pipe_ipc_observer_ntw = basic_win32_named_pipe_ipc_observer_ntw<char32_t>;
using u32win32_named_pipe_ipc_observer_9xa = basic_win32_named_pipe_ipc_observer_9xa<char32_t>;
} // namespace fast_io

