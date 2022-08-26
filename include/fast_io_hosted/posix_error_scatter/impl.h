#pragma once

namespace fast_io
{
/*
Unfortunately, strerror is not thread-safe and strerror_r is not guaranteed to be available on all platforms.
BTW. I wouldn't say I like the idea this can be affected by locale. Locale is trash.
The libc implementation will also make it fail to work under the EBCDIC execution charset.
Even libstdc++ is doing the wrong thing here. I believe this is a severe CVE vulnerability.
https://github.com/gcc-mirror/gcc/blob/16e2427f50c208dfe07d07f18009969502c25dc8/libstdc%2B%2B-v3/src/c%2B%2B11/system_error.cc#L51
We use the same strerror strings in GNU newlib since newlib is the standard in OS kernel, and the linker will remove the same string literals.

*/

namespace details::tsc_namespace
{
template<typename char_type,std::size_t N>
inline constexpr basic_io_scatter_t<char_type> tsc(char_type const (&a)[N]) noexcept
{
	return {a,N-1};
}
}

template<details::character char_type>
inline constexpr basic_io_scatter_t<char_type> get_posix_errno_scatter([[maybe_unused]] int errn) noexcept
{
/*
Referenced from GNU newlib
https://github.com/bminor/newlib/blob/ea275093c179fea636470173509615eb6bddad0f/newlib/libc/string/strerror.c
*/
	using namespace details::tsc_namespace;
#if __has_include(<errno.h>)
#if defined(__AVR__)
#include"avr.h"
#else
#include"generic.h"
#endif
#else
#include"unknown.h"
#endif
}

}