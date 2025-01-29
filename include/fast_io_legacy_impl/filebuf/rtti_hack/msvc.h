#pragma once

#include <vcruntime_typeinfo.h>

/*
Referenced from:
https://github.com/scottslacksmith/__RTDynamicCast/blob/master/main.cpp
*/

namespace fast_io
{
namespace msvc
{
extern
#if defined(_DLL) && !defined(__WINE__)
	__declspec(dllimport)
#endif
	void *__cdecl msvc__RTtypeid(void *) noexcept
#if (defined(__GNUC__) || defined(__clang__))
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
	__asm__("__RTtypeid")
#else
	__asm__("___RTtypeid")
#endif
#else
	__asm__("__RTtypeid")
#endif
#endif
		;

} // namespace msvc

namespace rtti_hack
{
inline char const *abi_type_info_name_or_nullptr(void *mythis) noexcept
{
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wlanguage-extension-token"
#endif
	__try
	{
		return ::__std_type_info_name(reinterpret_cast<::__std_type_info_data *>(reinterpret_cast<char *>(::fast_io::msvc::msvc__RTtypeid(mythis)) + sizeof(void *)),
									  __builtin_addressof(::__type_info_root_node));
	}
	__except (1)
	{
		return nullptr;
	}
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
}
} // namespace rtti_hack
} // namespace fast_io
