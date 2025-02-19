/*
Referenced from:
trcrsired/win-ldd
https://github.com/trcrsired/win-ldd/tree/master
greenjava/win-ldd
https://github.com/greenjava/win-ldd
*/
#if defined(_WIN32) || defined(__CYGWIN__)
#include <fast_io_dsal/vector.h>
#include <fast_io_dsal/string.h>
#include <fast_io_dsal/span.h>
#include <fast_io_dsal/array.h>
#include <fast_io.h>
#include <memory>
#include <algorithm>
#include <cstring>

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <dbghelp.h>
#undef min
#undef max

struct dll_ref_entry
{
	::fast_io::string dll_name;
	::fast_io::u16string dll_path;
};

inline constexpr bool operator==(dll_ref_entry const &a, dll_ref_entry const &b) noexcept
{
	return a.dll_name == b.dll_name && a.dll_path == b.dll_path;
}

inline constexpr auto operator<=>(dll_ref_entry const &a, dll_ref_entry const &b) noexcept
{
	auto ret{a.dll_name <=> b.dll_name};
	if (ret == 0)
	{
		return a.dll_path <=> b.dll_path;
	}
	return ret;
}

namespace fast_io::freestanding
{
template <>
struct is_trivially_relocatable<::dll_ref_entry>
{
	static inline constexpr bool value = true;
};
} // namespace fast_io::freestanding

inline auto get_dependencies(void *hMod)
{
	::fast_io::vector<dll_ref_entry> deps;

	IMAGE_DOS_HEADER *pDosHeader = (IMAGE_DOS_HEADER *)hMod;
	IMAGE_OPTIONAL_HEADER *pOptHeader = (IMAGE_OPTIONAL_HEADER *)((BYTE *)hMod + pDosHeader->e_lfanew + 24);
	IMAGE_IMPORT_DESCRIPTOR *pImportDesc = (IMAGE_IMPORT_DESCRIPTOR *)((BYTE *)hMod + pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	for (; pImportDesc->FirstThunk; ++pImportDesc)
	{
		::fast_io::string dllName(::fast_io::mnp::os_c_str((char const *)((BYTE *)hMod + pImportDesc->Name)));
		void *hmoddep = ::LoadLibraryExA(dllName.c_str(), NULL, DONT_RESOLVE_DLL_REFERENCES);
		::fast_io::u16string dllpath;
		if (hmoddep) [[likely]]
		{
			::fast_io::win32_dll_file df(hmoddep);
			dllpath.resize_and_overwrite(_MAX_PATH, [hmoddep](char16_t *buffer, size_t len) {
				if (!::GetModuleFileNameW((HMODULE)hmoddep, reinterpret_cast<wchar_t *>(buffer), static_cast<::std::uint_least32_t>(len)))
				{
					::fast_io::throw_win32_error();
				}
				return ::fast_io::cstr_nlen(buffer, len);
			});
		}
		deps.push_back(::dll_ref_entry{::std::move(dllName), ::std::move(dllpath)});
	}
	::std::ranges::sort(deps);
	deps.erase(::std::unique(deps.begin(), deps.end()), deps.end());
	return deps;
}

int main(int argc, char const **argv)
#if (defined(__cpp_exceptions) && (!defined(_MSC_VER) || defined(__clang__))) || __HAS_EXCEPTIONS == 1
try
#endif
{
	if (argc < 2)
	{
		if (argc == 0)
		{
			return 1;
		}
		::fast_io::io::perr("Usage:\n", ::fast_io::mnp::os_c_str(*argv), " <PE binary path1> [<PE binary path2>...]\n");
		return 1;
	}
	::fast_io::out_buf_type obf(::fast_io::out());
	::fast_io::span<char const *> args(argv + 1, argc - 1);
	for (auto libpath : args)
	{
		::fast_io::win32_dll_file wdllf(::fast_io::mnp::os_c_str(libpath), ::fast_io::dll_mode::win32_dont_resolve_dll_references);
		::fast_io::io::println(obf, ::fast_io::mnp::os_c_str(libpath));
		for (auto &ele : get_dependencies(wdllf.native_handle()))
		{
			::fast_io::io::println(obf, "\t", ele.dll_name, " => ", ::fast_io::mnp::code_cvt(ele.dll_path));
		}
	}
}
#if (defined(__cpp_exceptions) && (!defined(_MSC_VER) || defined(__clang__))) || __HAS_EXCEPTIONS == 1
catch (::fast_io::error e)
{
	::fast_io::io::perrln(e);
	return 1;
}
catch (...)
{
	::fast_io::io::perr("Unknown EH\n");
	return 1;
}
#endif


#else

int main()
{}

#endif