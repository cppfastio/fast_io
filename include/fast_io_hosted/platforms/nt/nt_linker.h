#pragma once

namespace fast_io::win32::nt
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
rtl_nt_status_to_dos_error(std::uint_least32_t status) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RtlNtStatusToDosError@4")
#else
__asm__("_RtlNtStatusToDosError@4")
#endif
#else
__asm__("RtlNtStatusToDosError")
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
NtClose(void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("NtClose@4")
#else
__asm__("_NtClose@4")
#endif
#else
__asm__("NtClose")
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
ZwClose(void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ZwClose@4")
#else
__asm__("_ZwClose@4")
#endif
#else
__asm__("ZwClose")
#endif
#endif
;

template<bool zw>
inline std::uint_least32_t nt_close(void* handle) noexcept
{
	if constexpr(zw)
		return ZwClose(handle);
	else
		return NtClose(handle);
}

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
NtCreateFile(void**,std::uint_least32_t,object_attributes*,io_status_block*,std::int_least64_t*,
			std::uint_least32_t,std::uint_least32_t,std::uint_least32_t,std::uint_least32_t,void*,std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("NtCreateFile@44")
#else
__asm__("_NtCreateFile@44")
#endif
#else
__asm__("NtCreateFile")
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
ZwCreateFile(void**,std::uint_least32_t,object_attributes*,io_status_block*,std::int_least64_t*,
			std::uint_least32_t,std::uint_least32_t,std::uint_least32_t,std::uint_least32_t,void*,std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ZwCreateFile@44")
#else
__asm__("_ZwCreateFile@44")
#endif
#else
__asm__("ZwCreateFile")
#endif
#endif
;

template<bool zw,typename... Args>
requires (sizeof...(Args)==11)
inline std::uint_least32_t nt_create_file(Args... args) noexcept
{
	if constexpr(zw)
		return ZwCreateFile(args...);
	else
		return NtCreateFile(args...);
}

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
NtCreateSection(void** __restrict,std::uint_least32_t,object_attributes* __restrict,
		std::uint_least64_t*,std::uint_least32_t,std::uint_least32_t,void* __restrict) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("NtCreateSection@28")
#else
__asm__("_NtCreateSection@28")
#endif
#else
__asm__("NtCreateSection")
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
ZwCreateSection(void** __restrict,std::uint_least32_t,object_attributes* __restrict,
		std::uint_least64_t*,std::uint_least32_t,std::uint_least32_t,void* __restrict) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ZwCreateSection@28")
#else
__asm__("_ZwCreateSection@28")
#endif
#else
__asm__("ZwCreateSection")
#endif
#endif
;

template<bool zw,typename... Args>
requires (sizeof...(Args)==7)
inline std::uint_least32_t nt_create_section(Args... args) noexcept
{
	if constexpr(zw)
		return ZwCreateSection(args...);
	else
		return NtCreateSection(args...);
}

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
NtQueryInformationProcess(void* __restrict,
		process_information_class,process_basic_information*,
		std::uint_least32_t,std::uint_least32_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("NtQueryInformationProcess@20")
#else
__asm__("_NtQueryInformationProcess@20")
#endif
#else
__asm__("NtQueryInformationProcess")
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
ZwQueryInformationProcess(void* __restrict,
		process_information_class,process_basic_information*,
		std::uint_least32_t,std::uint_least32_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ZwQueryInformationProcess@20")
#else
__asm__("_ZwQueryInformationProcess@20")
#endif
#else
__asm__("ZwQueryInformationProcess")
#endif
#endif
;

template<bool zw,typename... Args>
requires (sizeof...(Args)==5)
inline std::uint_least32_t nt_query_information_process(Args... args) noexcept
{
	if constexpr(zw)
		return ZwQueryInformationProcess(args...);
	else
		return NtQueryInformationProcess(args...);
}

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
NtWriteFile(void*,void*,pio_apc_routine,void*,io_status_block*,
				void const*,std::uint_least32_t,std::int_least64_t*,std::uint_least32_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("NtWriteFile@36")
#else
__asm__("_NtWriteFile@36")
#endif
#else
__asm__("NtWriteFile")
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
ZwWriteFile(void*,void*,pio_apc_routine,void*,io_status_block*,
				void const*,std::uint_least32_t,std::int_least64_t*,std::uint_least32_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ZwWriteFile@36")
#else
__asm__("_ZwWriteFile@36")
#endif
#else
__asm__("ZwWriteFile")
#endif
#endif
;

template<bool zw,typename... Args>
requires (sizeof...(Args)==9)
inline std::uint_least32_t nt_write_file(Args... args) noexcept
{
	if constexpr(zw)
		return ZwWriteFile(args...);
	else
		return NtWriteFile(args...);
}

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
NtReadFile(void*,void*,pio_apc_routine,void*,io_status_block*,
				void const*,std::uint_least32_t,std::int_least64_t*,std::uint_least32_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("NtReadFile@36")
#else
__asm__("_NtReadFile@36")
#endif
#else
__asm__("NtReadFile")
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
ZwReadFile(void*,void*,pio_apc_routine,void*,io_status_block*,
				void const*,std::uint_least32_t,std::int_least64_t*,std::uint_least32_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ZwReadFile@36")
#else
__asm__("_ZwReadFile@36")
#endif
#else
__asm__("ZwReadFile")
#endif
#endif
;

template<bool zw,typename... Args>
requires (sizeof...(Args)==9)
inline std::uint_least32_t nt_read_file(Args... args) noexcept
{
	if constexpr(zw)
		return ZwReadFile(args...);
	else
		return NtReadFile(args...);
}

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
NtQueryObject(void*,object_information_class,void*,::std::uint_least32_t,::std::uint_least32_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("NtQueryObject@20")
#else
__asm__("_NtQueryObject@20")
#endif
#else
__asm__("NtQueryObject")
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
NtQueryDirectoryFile(void*,void*,pio_apc_routine,void*,io_status_block*,
				void*,std::uint_least32_t,file_information_class,int,unicode_string*,int) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("NtQueryDirectoryFile@44")
#else
__asm__("_NtQueryDirectoryFile@44")
#endif
#else
__asm__("NtQueryDirectoryFile")
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
ZwQueryDirectoryFile(void*,void*,pio_apc_routine,void*,io_status_block*,
				void*,std::uint_least32_t,file_information_class,int,unicode_string*,int) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ZwQueryDirectoryFile@44")
#else
__asm__("_ZwQueryDirectoryFile@44")
#endif
#else
__asm__("ZwQueryDirectoryFile")
#endif
#endif
;

template<bool zw,typename... Args>
requires (sizeof...(Args)==11)
inline std::uint_least32_t nt_query_directory_file(Args... args) noexcept
{
	if constexpr(zw)
		return ZwQueryDirectoryFile(args...);
	else
		return NtQueryDirectoryFile(args...);
}

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
NtQuerySection(void*,section_information_class,void*,std::size_t,std::size_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("NtQuerySection@20")
#else
__asm__("_NtQuerySection@20")
#endif
#else
__asm__("NtQuerySection")
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
ZwQuerySection(void*,section_information_class,void*,std::size_t,std::size_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ZwQuerySection@20")
#else
__asm__("_ZwQuerySection@20")
#endif
#else
__asm__("ZwQuerySection")
#endif
#endif
;

template<bool zw,typename... Args>
requires (sizeof...(Args)==5)
inline std::uint_least32_t nt_query_section(Args... args) noexcept
{
	if constexpr(zw)
		return ZwQuerySection(args...);
	else
		return NtQuerySection(args...);
}


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
NtQueryInformationFile(void* __restrict,io_status_block* __restrict,void* __restrict,std::uint_least32_t,file_information_class) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("NtQueryInformationFile@20")
#else
__asm__("_NtQueryInformationFile@20")
#endif
#else
__asm__("NtQueryInformationFile")
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
ZwQueryInformationFile(void* __restrict,io_status_block* __restrict,void* __restrict,std::uint_least32_t,file_information_class) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ZwQueryInformationFile@20")
#else
__asm__("_ZwQueryInformationFile@20")
#endif
#else
__asm__("ZwQueryInformationFile")
#endif
#endif
;

template<bool zw,typename... Args>
requires (sizeof...(Args)==5)
inline std::uint_least32_t nt_query_information_file(Args... args) noexcept
{
	if constexpr(zw)
		return ZwQueryInformationFile(args...);
	else
		return NtQueryInformationFile(args...);
}


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
NtSetInformationFile(void* __restrict,io_status_block* __restrict,void* __restrict,std::uint_least32_t,file_information_class) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("NtSetInformationFile@28")
#else
__asm__("_NtSetInformationFile@28")
#endif
#else
__asm__("NtSetInformationFile")
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
ZwSetInformationFile(void* __restrict,io_status_block* __restrict,void* __restrict,std::uint_least32_t,file_information_class) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ZwSetInformationFile@28")
#else
__asm__("_ZwSetInformationFile@28")
#endif
#else
__asm__("ZwSetInformationFile")
#endif
#endif
;

template<bool zw,typename... Args>
requires (sizeof...(Args)==5)
inline std::uint_least32_t nt_set_information_file(Args... args) noexcept
{
	if constexpr(zw)
		return ZwSetInformationFile(args...);
	else
		return NtSetInformationFile(args...);
}

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
NtDuplicateObject(void*,void*,void*,void**,std::uint_least32_t,std::uint_least32_t,std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("NtDuplicateObject@28")
#else
__asm__("_NtDuplicateObject@28")
#endif
#else
__asm__("NtDuplicateObject")
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
ZwDuplicateObject(void*,void*,void*,void**,std::uint_least32_t,std::uint_least32_t,std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ZwDuplicateObject@28")
#else
__asm__("_ZwDuplicateObject@28")
#endif
#else
__asm__("ZwDuplicateObject")
#endif
#endif
;

template<bool zw,typename... Args>
requires (sizeof...(Args)==7)
inline std::uint_least32_t nt_duplicate_object(Args... args) noexcept
{
	if constexpr(zw)
		return ZwDuplicateObject(args...);
	else
		return NtDuplicateObject(args...);
}

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
NtWaitForSingleObject(void*,int,std::uint_least64_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("NtWaitForSingleObject@12")
#else
__asm__("_NtWaitForSingleObject@12")
#endif
#else
__asm__("NtWaitForSingleObject")
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
ZwWaitForSingleObject(void*,int,std::uint_least64_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ZwWaitForSingleObject@12")
#else
__asm__("_ZwWaitForSingleObject@12")
#endif
#else
__asm__("ZwWaitForSingleObject")
#endif
#endif
;

template<bool zw,typename... Args>
requires (sizeof...(Args)==3)
inline std::uint_least32_t nt_wait_for_single_object(Args... args) noexcept
{
	if constexpr(zw)
		return ZwWaitForSingleObject(args...);
	else
		return NtWaitForSingleObject(args...);
}

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
NtSetSystemTime(std::uint_least64_t*,std::uint_least64_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("NtSetSystemTime@8")
#else
__asm__("_NtSetSystemTime@8")
#endif
#else
__asm__("NtSetSystemTime")
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
ZwSetSystemTime(std::uint_least64_t*,std::uint_least64_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ZwSetSystemTime@8")
#else
__asm__("_ZwSetSystemTime@8")
#endif
#else
__asm__("ZwSetSystemTime")
#endif
#endif
;

template<bool zw,typename... Args>
requires (sizeof...(Args)==2)
inline std::uint_least32_t nt_set_system_time(Args... args) noexcept
{
	if constexpr(zw)
		return ZwSetSystemTime(args...);
	else
		return NtSetSystemTime(args...);
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
NtCreateProcess(void**,std::uint_least32_t,object_attributes*,void*,std::uint_least32_t,void*,void*,void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("NtCreateProcess@32")
#else
__asm__("_NtCreateProcess@32")
#endif
#else
__asm__("NtCreateProcess")
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
std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
ZwCreateProcess(void**,std::uint_least32_t,object_attributes*,void*,std::uint_least32_t,void*,void*,void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ZwCreateProcess@32")
#else
__asm__("_ZwCreateProcess@32")
#endif
#else
__asm__("ZwCreateProcess")
#endif
#endif
;

template<bool zw,typename... Args>
requires (sizeof...(Args)==8)
inline std::uint_least32_t nt_create_process(Args... args) noexcept
{
	if constexpr(zw)
		return NtCreateProcess(args...);
	else
		return ZwCreateProcess(args...);
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern char unsigned
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
rtl_dos_path_name_to_nt_path_name_u(char16_t const*,unicode_string*,char16_t const**,rtl_relative_name_u*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RtlDosPathNameToNtPathName_U@16")
#else
__asm__("_RtlDosPathNameToNtPathName_U@16")
#endif
#else
__asm__("RtlDosPathNameToNtPathName_U")
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
rtl_dos_path_name_to_nt_path_name_u_with_status(char16_t const*,unicode_string*,char16_t const**,rtl_relative_name_u*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RtlDosPathNameToNtPathName_U_WithStatus@16")
#else
__asm__("_RtlDosPathNameToNtPathName_U_WithStatus@16")
#endif
#else
__asm__("RtlDosPathNameToNtPathName_U_WithStatus")
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
rtl_free_unicode_string(unicode_string*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RtlFreeUnicodeString@4")
#else
__asm__("_RtlFreeUnicodeString@4")
#endif
#else
__asm__("RtlFreeUnicodeString")
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
RtlInitializeCriticalSection(void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RtlInitializeCriticalSection@4")
#else
__asm__("_RtlInitializeCriticalSection@4")
#endif
#else
__asm__("RtlInitializeCriticalSection")
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
RtlEnterCriticalSection(void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RtlEnterCriticalSection@4")
#else
__asm__("_RtlEnterCriticalSection@4")
#endif
#else
__asm__("RtlEnterCriticalSection")
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
RtlTryEnterCriticalSection(void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RtlTryEnterCriticalSection@4")
#else
__asm__("_RtlTryEnterCriticalSection@4")
#endif
#else
__asm__("RtlTryEnterCriticalSection")
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
RtlLeaveCriticalSection(void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RtlLeaveCriticalSection@4")
#else
__asm__("_RtlLeaveCriticalSection@4")
#endif
#else
__asm__("RtlLeaveCriticalSection")
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
RtlDeleteCriticalSection(void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RtlDeleteCriticalSection@4")
#else
__asm__("_RtlDeleteCriticalSection@4")
#endif
#else
__asm__("RtlDeleteCriticalSection")
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
RtlCreateUserThread(void*,void*,int,std::uint_least32_t,std::size_t,std::size_t,void*,void*,void**,client_id*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RtlCreateUserThread@40")
#else
__asm__("_RtlCreateUserThread@40")
#endif
#else
__asm__("RtlCreateUserThread")
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
NtResumeThread(void*,std::uint_least32_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("NtResumeThread@8")
#else
__asm__("_NtResumeThread@8")
#endif
#else
__asm__("NtResumeThread")
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
ZwResumeThread(void*,std::uint_least32_t*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ZwResumeThread@8")
#else
__asm__("_ZwResumeThread@8")
#endif
#else
__asm__("ZwResumeThread")
#endif
#endif
;

template<bool zw,typename... Args>
requires (sizeof...(Args)==2)
inline std::uint_least32_t nt_resume_thread(Args... args) noexcept
{
	if constexpr(zw)
		return ZwResumeThread(args...);
	else
		return NtResumeThread(args...);
}

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
NtLockFile(void*,void*,pio_apc_routine,void*,io_status_block*,std::int_least64_t*,std::int_least64_t*,std::uint_least32_t,char unsigned,char unsigned) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("NtLockFile@40")
#else
__asm__("_NtLockFile@40")
#endif
#else
__asm__("NtLockFile")
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
ZwLockFile(void*,void*,pio_apc_routine,void*,io_status_block*,std::int_least64_t*,std::int_least64_t*,std::uint_least32_t,char unsigned,char unsigned) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ZwLockFile@40")
#else
__asm__("_ZwLockFile@40")
#endif
#else
__asm__("ZwLockFile")
#endif
#endif
;

template<bool zw,typename... Args>
requires (sizeof...(Args)==10)
inline std::uint_least32_t nt_lock_file(Args... args) noexcept
{
	if constexpr(zw)
		return ZwLockFile(args...);
	else
		return NtLockFile(args...);
}

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
NtUnlockFile(void*,io_status_block*,std::int_least64_t*,std::int_least64_t*,std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("NtUnlockFile@20")
#else
__asm__("_NtUnlockFile@20")
#endif
#else
__asm__("NtUnlockFile")
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
ZwUnlockFile(void*,io_status_block*,std::int_least64_t*,std::int_least64_t*,std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ZwUnlockFile@20")
#else
__asm__("_ZwUnlockFile@20")
#endif
#else
__asm__("ZwUnlockFile")
#endif
#endif
;

template<bool zw,typename... Args>
requires (sizeof...(Args)==5)
inline std::uint_least32_t nt_unlock_file(Args... args) noexcept
{
	if constexpr(zw)
		return ZwUnlockFile(args...);
	else
		return NtUnlockFile(args...);
}

/*
Available starting with Windows Vista???
https://hfiref0x.github.io/syscalls.html
Actually this shows that it is available since the start of NT
*/

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
NtFlushBuffersFile(void*,io_status_block*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("NtFlushBuffersFile@8")
#else
__asm__("_NtFlushBuffersFile@8")
#endif
#else
__asm__("NtFlushBuffersFile")
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
ZwFlushBuffersFile(void*,io_status_block*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ZwFlushBuffersFile@8")
#else
__asm__("_ZwFlushBuffersFile@8")
#endif
#else
__asm__("ZwFlushBuffersFile")
#endif
#endif
;

template<bool zw,typename... Args>
requires (sizeof...(Args)==2)
inline std::uint_least32_t nt_flush_buffers_file(Args... args) noexcept
{
	if constexpr(zw)
		return ZwFlushBuffersFile(args...);
	else
		return NtFlushBuffersFile(args...);
}

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
NtFlushBuffersFileEx(void*,std::uint_least32_t,void*,std::uint_least32_t,io_status_block*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("NtFlushBuffersFileEx@20")
#else
__asm__("_NtFlushBuffersFileEx@20")
#endif
#else
__asm__("NtFlushBuffersFileEx")
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
ZwFlushBuffersFileEx(void*,std::uint_least32_t,void*,std::uint_least32_t,io_status_block*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ZwFlushBuffersFileEx@20")
#else
__asm__("_ZwFlushBuffersFileEx@20")
#endif
#else
__asm__("ZwFlushBuffersFileEx")
#endif
#endif
;

template<bool zw,typename... Args>
requires (sizeof...(Args)==5)
inline std::uint_least32_t nt_flush_buffers_file_ex(Args... args) noexcept
{
	if constexpr(zw)
		return ZwFlushBuffersFileEx(args...);
	else
		return NtFlushBuffersFileEx(args...);
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if __has_cpp_attribute(__gnu__::__cdecl__)
[[__gnu__::__cdecl__]]
#endif
extern std::uint_least32_t
#if !__has_cpp_attribute(__gnu__::__cdecl__) && defined(_MSC_VER)
__cdecl
#endif
DbgPrint(char const*,...) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("DbgPrint")
#else
__asm__("_DbgPrint")
#endif
#else
__asm__("DbgPrint")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if __has_cpp_attribute(__gnu__::__cdecl__)
[[__gnu__::__cdecl__]]
#endif
extern std::uint_least32_t
#if !__has_cpp_attribute(__gnu__::__cdecl__) && defined(_MSC_VER)
__cdecl
#endif
DbgPrintEx(std::uint_least32_t,std::uint_least32_t,char const*,...) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("DbgPrintEx")
#else
__asm__("_DbgPrintEx")
#endif
#else
__asm__("DbgPrintEx")
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
RtlCreateProcessParameters(rtl_user_process_parameters**,unicode_string*,unicode_string*,unicode_string*,unicode_string*,void*,unicode_string*,unicode_string*,unicode_string*,unicode_string*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RtlCreateProcessParameters@40")
#else
__asm__("_RtlCreateProcessParameters@40")
#endif
#else
__asm__("RtlCreateProcessParameters")
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
RtlCreateProcessParametersEx(rtl_user_process_parameters**,unicode_string*,unicode_string*,unicode_string*,unicode_string*,void*,unicode_string*,unicode_string*,unicode_string*,unicode_string*,::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RtlCreateProcessParametersEx@44")
#else
__asm__("_RtlCreateProcessParametersEx@44")
#endif
#else
__asm__("RtlCreateProcessParametersEx")
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
RtlDestroyProcessParameters(rtl_user_process_parameters*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RtlDestroyProcessParameters@4")
#else
__asm__("_RtlDestroyProcessParameters@4")
#endif
#else
__asm__("RtlDestroyProcessParameters")
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
NtCreateUserProcess(void*,void*,::std::uint_least32_t,::std::uint_least32_t,object_attributes*,object_attributes*,::std::uint_least32_t,::std::uint_least32_t,rtl_user_process_parameters*,ps_create_info*,ps_attribute_list*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("NtCreateUserProcess@44")
#else
__asm__("_NtCreateUserProcess@44")
#endif
#else
__asm__("NtCreateUserProcess")
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
ZwCreateUserProcess(void*,void*,::std::uint_least32_t,::std::uint_least32_t,object_attributes*,object_attributes*,::std::uint_least32_t,::std::uint_least32_t,rtl_user_process_parameters*,ps_create_info*,ps_attribute_list*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ZwCreateUserProcess@44")
#else
__asm__("_ZwCreateUserProcess@44")
#endif
#else
__asm__("ZwCreateUserProcess")
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
RtlCreateUserProcess(unicode_string*,::std::uint_least32_t,rtl_user_process_parameters*,security_descriptor*,security_descriptor*,void*,char unsigned,void*,void*,rtl_user_process_information*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RtlCreateUserProcess@40")
#else
__asm__("_RtlCreateUserProcess@40")
#endif
#else
__asm__("RtlCreateUserProcess")
#endif
#endif
;


}
