#pragma once

namespace fast_io::win32
{

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetLastError() noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetLastError@0")
#else
__asm__("_GetLastError@0")
#endif
#else
__asm__("GetLastError")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void*
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
LoadLibraryA(char const*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("LoadLibraryA@4")
#else
__asm__("_LoadLibraryA@4")
#endif
#else
__asm__("LoadLibraryA")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void *
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
LoadLibraryW(char16_t const*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("LoadLibraryW@4")
#else
__asm__("_LoadLibraryW@4")
#endif
#else
__asm__("LoadLibraryW")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void*
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
LoadLibraryExA(char const*,void*,std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("LoadLibraryExA@12")
#else
__asm__("_LoadLibraryExA@12")
#endif
#else
__asm__("LoadLibraryExA")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void *
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
LoadLibraryExW(char16_t const*,void*,std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("LoadLibraryExW@12")
#else
__asm__("_LoadLibraryExW@12")
#endif
#else
__asm__("LoadLibraryExW")
#endif
#endif
;

// Used to retrieve a locale-specific message string for some error code
#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
FormatMessageA(std::uint_least32_t, char const*, std::uint_least32_t,std::uint_least32_t, char*, std::uint_least32_t, void /*va_list*/ *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("FormatMessageA@28")
#else
__asm__("_FormatMessageA@28")
#endif
#else
__asm__("FormatMessageA")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
FormatMessageW(std::uint_least32_t, char16_t const*, std::uint_least32_t,std::uint_least32_t, char16_t*, std::uint_least32_t, void /*va_list*/ *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("FormatMessageW@28")
#else
__asm__("_FormatMessageW@28")
#endif
#else
__asm__("FormatMessageW")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void*
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
CreateFileMappingA(void*,security_attributes*,std::uint_least32_t,std::uint_least32_t,std::uint_least32_t,char const*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("CreateFileMappingA@24")
#else
__asm__("_CreateFileMappingA@24")
#endif
#else
__asm__("CreateFileMappingA")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void*
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
CreateFileMappingW(void*,security_attributes*,std::uint_least32_t,std::uint_least32_t,std::uint_least32_t,char16_t const*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("CreateFileMappingW@24")
#else
__asm__("_CreateFileMappingW@24")
#endif
#else
__asm__("CreateFileMappingW")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void*
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
MapViewOfFile(void*,std::uint_least32_t,std::uint_least32_t,std::uint_least32_t,std::size_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("MapViewOfFile@20")
#else
__asm__("_MapViewOfFile@20")
#endif
#else
__asm__("MapViewOfFile")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
SetEndOfFile(void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("SetEndOfFile@4")
#else
__asm__("_SetEndOfFile@4")
#endif
#else
__asm__("SetEndOfFile")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
UnmapViewOfFile(void const*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("UnmapViewOfFile@4")
#else
__asm__("_UnmapViewOfFile@4")
#endif
#else
__asm__("UnmapViewOfFile")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
WriteFile(void*,void const*,std::uint_least32_t,std::uint_least32_t*,overlapped*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("WriteFile@20")
#else
__asm__("_WriteFile@20")
#endif
#else
__asm__("WriteFile")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
ReadFile(void*,void const*,std::uint_least32_t,std::uint_least32_t*,overlapped*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ReadFile@20")
#else
__asm__("_ReadFile@20")
#endif
#else
__asm__("ReadFile")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
SetFilePointer(void*,std::int_least32_t,std::int_least32_t*,std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("SetFilePointer@16")
#else
__asm__("_SetFilePointer@16")
#endif
#else
__asm__("SetFilePointer")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
SetFilePointerEx(void*,std::int_least64_t,std::int_least64_t*,std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("SetFilePointerEx@20")
#else
__asm__("_SetFilePointerEx@20")
#endif
#else
__asm__("SetFilePointerEx")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
DuplicateHandle(void*,void*,void*,void**,std::uint_least32_t,int,std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("DuplicateHandle@28")
#else
__asm__("_DuplicateHandle@28")
#endif
#else
__asm__("DuplicateHandle")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void*
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetStdHandle(std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetStdHandle@4")
#else
__asm__("_GetStdHandle@4")
#endif
#else
__asm__("GetStdHandle")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
CreatePipe(void**,void**,security_attributes*,std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("CreatePipe@16")
#else
__asm__("_CreatePipe@16")
#endif
#else
__asm__("CreatePipe")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
FreeLibrary(void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("FreeLibrary@4")
#else
__asm__("_FreeLibrary@4")
#endif
#else
__asm__("FreeLibrary")
#endif
#endif
;

using farproc = intptr_t(
#if defined(_MSC_VER) && (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
__stdcall
#elif (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
__attribute__((__stdcall__))
#endif
*)() noexcept;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern farproc
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetProcAddress(void*,char const*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetProcAddress@8")
#else
__asm__("_GetProcAddress@8")
#endif
#else
__asm__("GetProcAddress")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void*
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetModuleHandleA(char const*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetModuleHandleA@4")
#else
__asm__("_GetModuleHandleA@4")
#endif
#else
__asm__("GetModuleHandleA")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void*
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetModuleHandleW(char16_t const*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetModuleHandleW@4")
#else
__asm__("_GetModuleHandleW@4")
#endif
#else
__asm__("GetModuleHandleW")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
WaitForSingleObject(void*,std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("WaitForSingleObject@8")
#else
__asm__("_WaitForSingleObject@8")
#endif
#else
__asm__("WaitForSingleObject")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
CancelIo(void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("CancelIo@4")
#else
__asm__("_CancelIo@4")
#endif
#else
__asm__("CancelIo")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetFileInformationByHandle(void* __restrict,by_handle_file_information* __restrict) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetFileInformationByHandle@8")
#else
__asm__("_GetFileInformationByHandle@8")
#endif
#else
__asm__("GetFileInformationByHandle")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetUserDefaultLocaleName(char16_t*,int) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetUserDefaultLocaleName@8")
#else
__asm__("_GetUserDefaultLocaleName@8")
#endif
#else
__asm__("GetUserDefaultLocaleName")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetUserDefaultLCID(void) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetUserDefaultLCID@0")
#else
__asm__("_GetUserDefaultLCID@0")
#endif
#else
__asm__("GetUserDefaultLCID")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetSystemTimePreciseAsFileTime(filetime*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetSystemTimePreciseAsFileTime@4")
#else
__asm__("_GetSystemTimePreciseAsFileTime@4")
#endif
#else
__asm__("GetSystemTimePreciseAsFileTime")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetSystemTimeAsFileTime(filetime*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetSystemTimeAsFileTime@4")
#else
__asm__("_GetSystemTimeAsFileTime@4")
#endif
#else
__asm__("GetSystemTimeAsFileTime")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
QueryUnbiasedInterruptTime(std::uint_least64_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("QueryUnbiasedInterruptTime@4")
#else
__asm__("_QueryUnbiasedInterruptTime@4")
#endif
#else
__asm__("QueryUnbiasedInterruptTime")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
QueryPerformanceCounter(std::int_least64_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("QueryPerformanceCounter@4")
#else
__asm__("_QueryPerformanceCounter@4")
#endif
#else
__asm__("QueryPerformanceCounter")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
QueryPerformanceFrequency(std::int_least64_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("QueryPerformanceFrequency@4")
#else
__asm__("_QueryPerformanceFrequency@4")
#endif
#else
__asm__("QueryPerformanceFrequency")
#endif
#endif
;


#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetProcessTimes(void*,filetime*,filetime*,filetime*,filetime*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetProcessTimes@20")
#else
__asm__("_GetProcessTimes@20")
#endif
#else
__asm__("GetProcessTimes")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetThreadTimes(void*,filetime*,filetime*,filetime*,filetime*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetThreadTimes@20")
#else
__asm__("_GetThreadTimes@20")
#endif
#else
__asm__("GetThreadTimes")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetHandleInformation(void*,std::uint_least32_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetHandleInformation@8")
#else
__asm__("_GetHandleInformation@8")
#endif
#else
__asm__("GetHandleInformation")
#endif
#endif
;
#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
SetHandleInformation(void*,std::uint_least32_t,std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("SetHandleInformation@12")
#else
__asm__("_SetHandleInformation@12")
#endif
#else
__asm__("SetHandleInformation")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetTempPathA(std::uint_least32_t,char* buffer) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetTempPathA@8")
#else
__asm__("_GetTempPathA@8")
#endif
#else
__asm__("GetTempPathA")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetTempPathW(std::uint_least32_t,char16_t* buffer) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetTempPathW@8")
#else
__asm__("_GetTempPathW@8")
#endif
#else
__asm__("GetTempPathW")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void*
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
CreateFileA(char const*,std::uint_least32_t,std::uint_least32_t,security_attributes*,std::uint_least32_t,std::uint_least32_t,void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("CreateFileA@28")
#else
__asm__("_CreateFileA@28")
#endif
#else
__asm__("CreateFileA")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void*
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
CreateFileW(char16_t const*,std::uint_least32_t,std::uint_least32_t,security_attributes*,std::uint_least32_t,std::uint_least32_t,void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("CreateFileW@28")
#else
__asm__("_CreateFileW@28")
#endif
#else
__asm__("CreateFileW")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void*
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
CreateIoCompletionPort(void*,void*,std::uintptr_t,std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("CreateIoCompletionPort@16")
#else
__asm__("_CreateIoCompletionPort@16")
#endif
#else
__asm__("CreateIoCompletionPort")
#endif
#endif
;
#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
SystemFunction036(void*,std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("SystemFunction036@8")
#else
__asm__("_SystemFunction036@8")
#endif
#else
__asm__("SystemFunction036")
#endif
#endif
;
#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
CloseHandle(void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("CloseHandle@4")
#else
__asm__("_CloseHandle@4")
#endif
#else
__asm__("CloseHandle")
#endif
#endif
;
#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
LockFileEx(void*,std::uint_least32_t,std::uint_least32_t,std::uint_least32_t,std::uint_least32_t,overlapped*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("LockFileEx@24")
#else
__asm__("_LockFileEx@24")
#endif
#else
__asm__("LockFileEx")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
UnlockFileEx(void*,std::uint_least32_t,std::uint_least32_t,std::uint_least32_t,overlapped*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("UnlockFileEx@20")
#else
__asm__("_UnlockFileEx@20")
#endif
#else
__asm__("UnlockFileEx")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
DeviceIoControl(void*,std::uint_least32_t,void*,std::uint_least32_t,void*,std::uint_least32_t,void*,overlapped*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("DeviceIoControl@32")
#else
__asm__("_DeviceIoControl@32")
#endif
#else
__asm__("DeviceIoControl")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetFileType(void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetFileType@4")
#else
__asm__("_GetFileType@4")
#endif
#else
__asm__("GetFileType")
#endif
#endif
;


#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetACP() noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetACP@0")
#else
__asm__("_GetACP@0")
#endif
#else
__asm__("GetACP")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetEnvironmentVariableA(char const*,char*,::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetEnvironmentVariableA@12")
#else
__asm__("_GetEnvironmentVariableA@12")
#endif
#else
__asm__("GetEnvironmentVariableA")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetEnvironmentVariableW(char16_t const*,char16_t*,::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetEnvironmentVariableW@12")
#else
__asm__("_GetEnvironmentVariableW@12")
#endif
#else
__asm__("GetEnvironmentVariableW")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
MessageBoxA(void*,char const*,char const*,std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("MessageBoxA@16")
#else
__asm__("_MessageBoxA@16")
#endif
#else
__asm__("MessageBoxA")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
MessageBoxW(void*,char16_t const*,char16_t const*,std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("MessageBoxW@16")
#else
__asm__("_MessageBoxW@16")
#endif
#else
__asm__("MessageBoxW")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetConsoleMode(void *, std::uint_least32_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetConsoleMode@8")
#else
__asm__("_GetConsoleMode@8")
#endif
#else
__asm__("GetConsoleMode")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
SetConsoleMode(void *, std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("SetConsoleMode@8")
#else
__asm__("_SetConsoleMode@8")
#endif
#else
__asm__("SetConsoleMode")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
ReadConsoleA(void*,void*,std::uint_least32_t,std::uint_least32_t*,void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ReadConsoleA@20")
#else
__asm__("_ReadConsoleA@20")
#endif
#else
__asm__("ReadConsoleA")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
ReadConsoleW(void*,void*,std::uint_least32_t,std::uint_least32_t*,void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ReadConsoleW@20")
#else
__asm__("_ReadConsoleW@20")
#endif
#else
__asm__("ReadConsoleW")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
WriteConsoleA(void*,void const*,std::uint_least32_t,std::uint_least32_t*,void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("WriteConsoleA@20")
#else
__asm__("_WriteConsoleA@20")
#endif
#else
__asm__("WriteConsoleA")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
WriteConsoleW(void*,void const*,std::uint_least32_t,std::uint_least32_t*,void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("WriteConsoleW@20")
#else
__asm__("_WriteConsoleW@20")
#endif
#else
__asm__("WriteConsoleW")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetConsoleScreenBufferInfo(void *, console_screen_buffer_info *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetConsoleScreenBufferInfo@8")
#else
__asm__("_GetConsoleScreenBufferInfo@8")
#endif
#else
__asm__("GetConsoleScreenBufferInfo")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
ScrollConsoleScreenBufferA(void *, small_rect const *, small_rect const *, coord, char_info const *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ScrollConsoleScreenBufferA@20")
#else
__asm__("_ScrollConsoleScreenBufferA@20")
#endif
#else
__asm__("ScrollConsoleScreenBufferA")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
ScrollConsoleScreenBufferW(void *, small_rect const *, small_rect const *, coord, char_info const *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ScrollConsoleScreenBufferW@20")
#else
__asm__("_ScrollConsoleScreenBufferW@20")
#endif
#else
__asm__("ScrollConsoleScreenBufferW")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
SetConsoleCursorPosition(void *, coord) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("SetConsoleCursorPosition@8")
#else
__asm__("_SetConsoleCursorPosition@8")
#endif
#else
__asm__("SetConsoleCursorPosition")
#endif
#endif
;


#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
InitializeCriticalSection(void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("InitializeCriticalSection@4")
#else
__asm__("_InitializeCriticalSection@4")
#endif
#else
__asm__("InitializeCriticalSection")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
EnterCriticalSection(void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("EnterCriticalSection@4")
#else
__asm__("_EnterCriticalSection@4")
#endif
#else
__asm__("EnterCriticalSection")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
TryEnterCriticalSection(void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("TryEnterCriticalSection@4")
#else
__asm__("_TryEnterCriticalSection@4")
#endif
#else
__asm__("TryEnterCriticalSection")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
LeaveCriticalSection(void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("LeaveCriticalSection@4")
#else
__asm__("_LeaveCriticalSection@4")
#endif
#else
__asm__("LeaveCriticalSection")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
DeleteCriticalSection(void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("DeleteCriticalSection@4")
#else
__asm__("_DeleteCriticalSection@4")
#endif
#else
__asm__("DeleteCriticalSection")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
WSADuplicateSocketA(void*,std::uint_least32_t,wsaprotocol_infoa*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("WSADuplicateSocketA@12")
#else
__asm__("_WSADuplicateSocketA@12")
#endif
#else
__asm__("WSADuplicateSocketA")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
WSADuplicateSocketW(void*,std::uint_least32_t,wsaprotocol_infow*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("WSADuplicateSocketW@12")
#else
__asm__("_WSADuplicateSocketW@12")
#endif
#else
__asm__("WSADuplicateSocketW")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
WSACleanup() noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("WSACleanup@0")
#else
__asm__("_WSACleanup@0")
#endif
#else
__asm__("WSACleanup")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
WSAStartup(std::uint_least32_t,wsadata*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("WSAStartup@8")
#else
__asm__("_WSAStartup@8")
#endif
#else
__asm__("WSAStartup")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
WSAGetLastError() noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("WSAGetLastError@0")
#else
__asm__("_WSAGetLastError@0")
#endif
#else
__asm__("WSAGetLastError")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
closesocket(std::uintptr_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("closesocket@4")
#else
__asm__("_closesocket@4")
#endif
#else
__asm__("closesocket")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern std::uintptr_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
WSASocketW(int,int,int,wsaprotocol_infow*,std::uint_least32_t,std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("WSASocketW@24")
#else
__asm__("_WSASocketW@24")
#endif
#else
__asm__("WSASocketW")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern std::uintptr_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
WSASocketA(int,int,int,wsaprotocol_infoa*,std::uint_least32_t,std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("WSASocketA@24")
#else
__asm__("_WSASocketA@24")
#endif
#else
__asm__("WSASocketA")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
bind(std::uintptr_t,void const*,int) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("bind@12")
#else
__asm__("_bind@12")
#endif
#else
__asm__("bind")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
listen(std::uintptr_t,int) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("listen@8")
#else
__asm__("_listen@8")
#endif
#else
__asm__("listen")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern std::uintptr_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
WSAAccept(std::uintptr_t,void const*,int*,lpconditionproc,std::uintptr_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("WSAAccept@20")
#else
__asm__("_WSAAccept@20")
#endif
#else
__asm__("WSAAccept")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
ioctlsocket(std::uintptr_t,long,std::uint_least32_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ioctlsocket@12")
#else
__asm__("_ioctlsocket@12")
#endif
#else
__asm__("ioctlsocket")
#endif
#endif
;


#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
WSASend(std::uintptr_t,wsabuf*,std::uint_least32_t,std::uint_least32_t*,std::uint_least32_t,overlapped*,lpwsaoverlapped_completion_routine) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("WSASend@28")
#else
__asm__("_WSASend@28")
#endif
#else
__asm__("WSASend")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
WSASendMsg(std::uintptr_t,wsamsg*,std::uint_least32_t,std::uint_least32_t*,overlapped*,lpwsaoverlapped_completion_routine) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("WSASendMsg@24")
#else
__asm__("_WSASendMsg@24")
#endif
#else
__asm__("WSASendMsg")
#endif
#endif
;


#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
WSASendTo(std::uintptr_t,wsabuf*,std::uint_least32_t,std::uint_least32_t*,std::uint_least32_t,void const*,int,overlapped*,lpwsaoverlapped_completion_routine) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("WSASendTo@36")
#else
__asm__("_WSASendTo@36")
#endif
#else
__asm__("WSASendTo")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
recv(std::uintptr_t,char* buf,int len,int flags) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("recv@16")
#else
__asm__("_recv@16")
#endif
#else
__asm__("recv")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
WSARecv(std::uintptr_t,wsabuf*,std::uint_least32_t,std::uint_least32_t*,std::uint_least32_t*,overlapped*,lpwsaoverlapped_completion_routine) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("WSARecv@28")
#else
__asm__("_WSARecv@28")
#endif
#else
__asm__("WSARecv")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
WSARecvFrom(std::uintptr_t,wsabuf*,std::uint_least32_t,std::uint_least32_t*,std::uint_least32_t*,void const*,int*,overlapped*,lpwsaoverlapped_completion_routine) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("WSARecvFrom@36")
#else
__asm__("_WSARecvFrom@36")
#endif
#else
__asm__("WSARecvFrom")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
connect(std::uintptr_t,void const*,int) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("connect@12")
#else
__asm__("_connect@12")
#endif
#else
__asm__("connect")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
WSAConnect(std::uintptr_t,void const*,int,wsabuf*,wsabuf*,qualityofservice*,qualityofservice*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("WSAConnect@28")
#else
__asm__("_WSAConnect@28")
#endif
#else
__asm__("WSAConnect")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
shutdown(std::uintptr_t,void const*,int) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("shutdown@12")
#else
__asm__("_shutdown@12")
#endif
#else
__asm__("shutdown")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetCurrentProcessId() noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetCurrentProcessId@0")
#else
__asm__("_GetCurrentProcessId@0")
#endif
#else
__asm__("GetCurrentProcessId")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
FlushFileBuffers(void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("FlushFileBuffers@4")
#else
__asm__("_FlushFileBuffers@4")
#endif
#else
__asm__("FlushFileBuffers")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetQueuedCompletionStatus(void*,std::uint_least32_t*,std::uintptr_t*,overlapped*,std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetQueuedCompletionStatus@20")
#else
__asm__("_GetQueuedCompletionStatus@20")
#endif
#else
__asm__("GetQueuedCompletionStatus")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
freeaddrinfo(win32_addrinfo_9xa*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("freeaddrinfo@4")
#else
__asm__("_freeaddrinfo@4")
#endif
#else
__asm__("freeaddrinfo")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
FreeAddrInfoW(win32_addrinfo_ntw*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("FreeAddrInfoW@4")
#else
__asm__("_FreeAddrInfoW@4")
#endif
#else
__asm__("FreeAddrInfoW")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
getaddrinfo(char const*,char const*,win32_addrinfo_9xa const*,win32_addrinfo_9xa**) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("getaddrinfo@16")
#else
__asm__("_getaddrinfo@16")
#endif
#else
__asm__("getaddrinfo")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetAddrInfoW(char16_t const*,char16_t const*,win32_addrinfo_ntw const*,win32_addrinfo_ntw**) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetAddrInfoW@16")
#else
__asm__("_GetAddrInfoW@16")
#endif
#else
__asm__("GetAddrInfoW")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
CryptAcquireContextA(::std::uintptr_t*,char8_t const*,char8_t const*,::std::uint_least32_t,::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("CryptAcquireContextA@20")
#else
__asm__("_CryptAcquireContextA@20")
#endif
#else
__asm__("CryptAcquireContextA")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
CryptAcquireContextW(::std::uintptr_t*,char16_t const*,char16_t const*,::std::uint_least32_t,::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("CryptAcquireContextW@20")
#else
__asm__("_CryptAcquireContextW@20")
#endif
#else
__asm__("CryptAcquireContextW")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
CryptReleaseContext(::std::uintptr_t,::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("CryptReleaseContext@8")
#else
__asm__("_CryptReleaseContext@8")
#endif
#else
__asm__("CryptReleaseContext")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
CryptGenRandom(::std::uintptr_t,::std::uint_least32_t,char unsigned*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("CryptGenRandom@12")
#else
__asm__("_CryptGenRandom@12")
#endif
#else
__asm__("CryptGenRandom")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
RegOpenKeyA(::std::uintptr_t,char8_t const*,::std::uintptr_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RegOpenKeyA@12")
#else
__asm__("_RegOpenKeyA@12")
#endif
#else
__asm__("RegOpenKeyA")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
RegOpenKeyW(::std::uintptr_t,char16_t const*,::std::uintptr_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RegOpenKeyW@12")
#else
__asm__("_RegOpenKeyW@12")
#endif
#else
__asm__("RegOpenKeyW")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
RegQueryValueExA(::std::uintptr_t,char8_t const*,::std::uint_least32_t*,::std::uint_least32_t*,void*,::std::uint_least32_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RegQueryValueExA@24")
#else
__asm__("_RegQueryValueExA@24")
#endif
#else
__asm__("RegQueryValueExA")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
RegQueryValueExW(::std::uintptr_t,char16_t const*,::std::uint_least32_t*,::std::uint_least32_t*,void*,::std::uint_least32_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RegQueryValueExW@24")
#else
__asm__("_RegQueryValueExW@24")
#endif
#else
__asm__("RegQueryValueExW")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
RegCloseKey(::std::uintptr_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RegCloseKey@4")
#else
__asm__("_RegCloseKey@4")
#endif
#else
__asm__("RegCloseKey")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetTimeZoneInformation(time_zone_information*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetTimeZoneInformation@4")
#else
__asm__("_GetTimeZoneInformation@4")
#endif
#else
__asm__("GetTimeZoneInformation")
#endif
#endif
;

}
