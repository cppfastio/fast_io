#pragma once

/*
Referenced from:
https://github.com/scottslacksmith/__RTDynamicCast/blob/master/main.cpp
*/

namespace fast_io
{
namespace msvc
{
FAST_IO_DLL_DLLIMPORT extern void *FAST_IO_WINCDECL msvc__RTtypeid(void *) noexcept FAST_IO_WINCDECL_RENAME(__RTtypeid, 4);
} // namespace msvc

namespace rtti_hack
{
/*
MSVC std::type_info layout: vftable pointer, then a spare pointer slot where the CRT
caches the undecorated name, then the decorated (.?AV...) name inline. The decorated
name is what the exception-handling RTTI tables match on and is identical for MSVC STL
and libc++ on the MSVC ABI. The undecorated name returned by __std_type_info_name /
type_info::name() is demangled text and is not stable enough for comparison.
*/
struct msvc_raw_type_info
{
	void *vftable;
	char const *undecorated_name;
	char decorated_name[1];
};

inline char const *abi_type_info_name_or_nullptr(void *mythis) noexcept
{
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wlanguage-extension-token"
#endif
	__try
	{
		auto type_info{reinterpret_cast<msvc_raw_type_info const *>(::fast_io::msvc::msvc__RTtypeid(mythis))};
		if (type_info == nullptr)
		{
			return nullptr;
		}
		return type_info->decorated_name;
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
