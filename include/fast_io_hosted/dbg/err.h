#pragma once

namespace fast_io
{
inline auto dbg() noexcept
{
	return native_stderr<char>();
}
inline auto wdbg() noexcept
{
	return native_stderr<wchar_t>();
}
inline auto u8dbg() noexcept
{
	return native_stderr<char8_t>();
}
inline auto u16dbg() noexcept
{
	return native_stderr<char16_t>();
}
inline auto u32dbg() noexcept
{
	return native_stderr<char32_t>();
}
}
