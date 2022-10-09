#pragma once

namespace fast_io::details::chacha
{
#if __has_cpp_attribute(__gnu__::__flatten__)
[[__gnu__::__flatten__]]
#endif
inline constexpr void chacha_runtime_routine(::std::byte* __restrict outdata,
	::std::uint_least32_t const* __restrict indata) noexcept
{
	chacha_main_routine(outdata,indata);
}
}