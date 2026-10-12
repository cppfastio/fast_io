#pragma once

namespace fast_io
{

namespace win32::details
{

/*
Thanks Fseuio for providing source code.
*/
inline bool win32_is_character_device(void *handle) noexcept
{
	return (::fast_io::win32::GetFileType(handle) & 0xFFFF7FFF) == 2;
}

struct win32_console_mode_guard
{
	void *out_hdl{};
	::std::uint_least32_t mode{};
	inline win32_console_mode_guard(void *hd)
		FAST_IO_HERBCEPTIONS_THROWS
		: out_hdl{hd}
	{
		if (!::fast_io::win32::GetConsoleMode(out_hdl, __builtin_addressof(mode)))
		{
			throw_win32_error();
		}
		if (!::fast_io::win32::SetConsoleMode(out_hdl, mode | 0x0004))
		{
			throw_win32_error();
		}
	}
	inline win32_console_mode_guard(win32_console_mode_guard const &) = delete;
	inline win32_console_mode_guard &operator=(win32_console_mode_guard const &) = delete;
	inline ~win32_console_mode_guard()
	{
		::fast_io::win32::SetConsoleMode(out_hdl, mode);
	}
};

inline void win32_clear_screen_main(void *out_hdl)
	FAST_IO_HERBCEPTIONS_THROWS
{
	/*
	Since many people are using console like msys2, we need to first write something to this console
	*/
	constexpr char8_t const str[] = u8"\x1B[H\x1B[2J\x1B[3J";
	constexpr ::std::uint_least32_t written_chars{
		static_cast<::std::uint_least32_t>(::fast_io::details::string_literal_size(str))};
	// not bytes, but chars
	win32_console_mode_guard guard{out_hdl};
	if (!::fast_io::win32::WriteFile(out_hdl, str, written_chars, nullptr, nullptr))
	{
		throw_win32_error();
	}
}

inline void win32_clear_screen_impl(void *handle)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if (!win32_is_character_device(handle)) [[unlikely]]
	{
		return;
	}
	win32_clear_screen_main(handle);
}

} // namespace win32::details

template <win32_family family, ::std::integral ch_type>
inline void clear_screen(basic_win32_family_io_observer<family, ch_type> wiob)
	FAST_IO_HERBCEPTIONS_THROWS
{
	win32::details::win32_clear_screen_impl(wiob.handle);
}

template <win32_family family, ::std::integral ch_type>
inline bool is_character_device(basic_win32_family_io_observer<family, ch_type> wiob) noexcept
{
	return win32::details::win32_is_character_device(wiob.handle);
}

template <nt_family family, ::std::integral ch_type>
inline void clear_screen(basic_nt_family_io_observer<family, ch_type> niob)
	FAST_IO_HERBCEPTIONS_THROWS
{
	win32::details::win32_clear_screen_impl(niob.handle);
}

template <nt_family family, ::std::integral ch_type>
inline bool is_character_device(basic_nt_family_io_observer<family, ch_type> niob) noexcept
{
	return win32::details::win32_is_character_device(niob.handle);
}

} // namespace fast_io
