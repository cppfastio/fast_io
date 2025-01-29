#pragma once

namespace fast_io::win32::nt
{

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	rtl_nt_status_to_dos_error(::std::uint_least32_t status) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtClose(void *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwClose(void *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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

template <bool zw>
inline ::std::uint_least32_t nt_close(void *handle) noexcept
{
	if constexpr (zw)
	{
		return ZwClose(handle);
	}
	else
	{
		return NtClose(handle);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtCreateFile(void **, ::std::uint_least32_t, object_attributes *, io_status_block *, ::std::int_least64_t *,
				 ::std::uint_least32_t, ::std::uint_least32_t, ::std::uint_least32_t, ::std::uint_least32_t, void *,
				 ::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwCreateFile(void **, ::std::uint_least32_t, object_attributes *, io_status_block *, ::std::int_least64_t *,
				 ::std::uint_least32_t, ::std::uint_least32_t, ::std::uint_least32_t, ::std::uint_least32_t, void *,
				 ::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 11)
inline ::std::uint_least32_t nt_create_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwCreateFile(args...);
	}
	else
	{
		return NtCreateFile(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtCreateSection(void **__restrict, ::std::uint_least32_t, object_attributes *__restrict, ::std::uint_least64_t *,
					::std::uint_least32_t, ::std::uint_least32_t, void *__restrict) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwCreateSection(void **__restrict, ::std::uint_least32_t, object_attributes *__restrict, ::std::uint_least64_t *,
					::std::uint_least32_t, ::std::uint_least32_t, void *__restrict) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 7)
inline ::std::uint_least32_t nt_create_section(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwCreateSection(args...);
	}
	else
	{
		return NtCreateSection(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtQueryInformationProcess(void *__restrict, process_information_class, void *, ::std::uint_least32_t,
							  ::std::uint_least32_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwQueryInformationProcess(void *__restrict, process_information_class, void *, ::std::uint_least32_t,
							  ::std::uint_least32_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 5)
inline ::std::uint_least32_t nt_query_information_process(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwQueryInformationProcess(args...);
	}
	else
	{
		return NtQueryInformationProcess(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtWriteFile(void *, void *, pio_apc_routine, void *, io_status_block *, void const *, ::std::uint_least32_t,
				::std::int_least64_t *, ::std::uint_least32_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwWriteFile(void *, void *, pio_apc_routine, void *, io_status_block *, void const *, ::std::uint_least32_t,
				::std::int_least64_t *, ::std::uint_least32_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 9)
inline ::std::uint_least32_t nt_write_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwWriteFile(args...);
	}
	else
	{
		return NtWriteFile(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtReadFile(void *, void *, pio_apc_routine, void *, io_status_block *, void const *, ::std::uint_least32_t,
			   ::std::int_least64_t *, ::std::uint_least32_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwReadFile(void *, void *, pio_apc_routine, void *, io_status_block *, void const *, ::std::uint_least32_t,
			   ::std::int_least64_t *, ::std::uint_least32_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 9)
inline ::std::uint_least32_t nt_read_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwReadFile(args...);
	}
	else
	{
		return NtReadFile(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtQueryObject(void *, object_information_class, void *, ::std::uint_least32_t, ::std::uint_least32_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwQueryObject(void *, object_information_class, void *, ::std::uint_least32_t, ::std::uint_least32_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwQueryObject@20")
#else
	__asm__("_ZwQueryObject@20")
#endif
#else
	__asm__("ZwQueryObject")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 5)
inline ::std::uint_least32_t nt_query_object(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwQueryObject(args...);
	}
	else
	{
		return NtQueryObject(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtQueryDirectoryFile(void *, void *, pio_apc_routine, void *, io_status_block *, void *, ::std::uint_least32_t,
						 file_information_class, int, unicode_string *, int) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwQueryDirectoryFile(void *, void *, pio_apc_routine, void *, io_status_block *, void *, ::std::uint_least32_t,
						 file_information_class, int, unicode_string *, int) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 11)
inline ::std::uint_least32_t nt_query_directory_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwQueryDirectoryFile(args...);
	}
	else
	{
		return NtQueryDirectoryFile(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtQuerySection(void *, section_information_class, void *, ::std::size_t, ::std::size_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwQuerySection(void *, section_information_class, void *, ::std::size_t, ::std::size_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 5)
inline ::std::uint_least32_t nt_query_section(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwQuerySection(args...);
	}
	else
	{
		return NtQuerySection(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtQueryInformationFile(void *__restrict, io_status_block *__restrict, void *__restrict, ::std::uint_least32_t,
						   file_information_class) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwQueryInformationFile(void *__restrict, io_status_block *__restrict, void *__restrict, ::std::uint_least32_t,
						   file_information_class) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 5)
inline ::std::uint_least32_t nt_query_information_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwQueryInformationFile(args...);
	}
	else
	{
		return NtQueryInformationFile(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtSetInformationFile(void *__restrict, io_status_block *__restrict, void *__restrict, ::std::uint_least32_t,
						 file_information_class) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwSetInformationFile(void *__restrict, io_status_block *__restrict, void *__restrict, ::std::uint_least32_t,
						 file_information_class) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 5)
inline ::std::uint_least32_t nt_set_information_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwSetInformationFile(args...);
	}
	else
	{
		return NtSetInformationFile(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtDuplicateObject(void *, void *, void *, void **, ::std::uint_least32_t, ::std::uint_least32_t,
					  ::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwDuplicateObject(void *, void *, void *, void **, ::std::uint_least32_t, ::std::uint_least32_t,
					  ::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 7)
inline ::std::uint_least32_t nt_duplicate_object(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwDuplicateObject(args...);
	}
	else
	{
		return NtDuplicateObject(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtWaitForSingleObject(void *, int, ::std::uint_least64_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwWaitForSingleObject(void *, int, ::std::uint_least64_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 3)
inline ::std::uint_least32_t nt_wait_for_single_object(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwWaitForSingleObject(args...);
	}
	else
	{
		return NtWaitForSingleObject(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtSetSystemTime(::std::uint_least64_t *, ::std::uint_least64_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwSetSystemTime(::std::uint_least64_t *, ::std::uint_least64_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 2)
inline ::std::uint_least32_t nt_set_system_time(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwSetSystemTime(args...);
	}
	else
	{
		return NtSetSystemTime(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtCreateProcess(void **, ::std::uint_least32_t, object_attributes *, void *, ::std::uint_least32_t, void *, void *,
					void *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwCreateProcess(void **, ::std::uint_least32_t, object_attributes *, void *, ::std::uint_least32_t, void *, void *,
					void *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 8)
inline ::std::uint_least32_t nt_create_process(Args... args) noexcept
{
	if constexpr (zw)
	{
		return NtCreateProcess(args...);
	}
	else
	{
		return ZwCreateProcess(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern char unsigned
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	rtl_dos_path_name_to_nt_path_name_u(char16_t const *, unicode_string *, char16_t const **,
										rtl_relative_name_u *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	rtl_dos_path_name_to_nt_path_name_u_with_status(char16_t const *, unicode_string *, char16_t const **,
													rtl_relative_name_u *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	RtlNtPathNameToDosPathName(::std::uint_least32_t, rtl_unicode_string_buffer *, ::std::uint_least32_t *, char16_t **) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("RtlNtPathNameToDosPathName@16")
#else
	__asm__("_RtlNtPathNameToDosPathName@16")
#endif
#else
	__asm__("RtlNtPathNameToDosPathName")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	rtl_free_unicode_string(unicode_string *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	RtlInitializeCriticalSection(void *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	RtlEnterCriticalSection(void *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	RtlTryEnterCriticalSection(void *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	RtlLeaveCriticalSection(void *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	RtlDeleteCriticalSection(void *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	RtlCreateUserThread(void *, void *, int, ::std::uint_least32_t, ::std::size_t, ::std::size_t, void *, void *,
						void **, client_id *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtResumeThread(void *, ::std::uint_least32_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwResumeThread(void *, ::std::uint_least32_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 2)
inline ::std::uint_least32_t nt_resume_thread(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwResumeThread(args...);
	}
	else
	{
		return NtResumeThread(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtLockFile(void *, void *, pio_apc_routine, void *, io_status_block *, ::std::int_least64_t *,
			   ::std::int_least64_t *, ::std::uint_least32_t, char unsigned, char unsigned) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwLockFile(void *, void *, pio_apc_routine, void *, io_status_block *, ::std::int_least64_t *,
			   ::std::int_least64_t *, ::std::uint_least32_t, char unsigned, char unsigned) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 10)
inline ::std::uint_least32_t nt_lock_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwLockFile(args...);
	}
	else
	{
		return NtLockFile(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtUnlockFile(void *, io_status_block *, ::std::int_least64_t *, ::std::int_least64_t *,
				 ::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwUnlockFile(void *, io_status_block *, ::std::int_least64_t *, ::std::int_least64_t *,
				 ::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 5)
inline ::std::uint_least32_t nt_unlock_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwUnlockFile(args...);
	}
	else
	{
		return NtUnlockFile(args...);
	}
}

/*
Available starting with Windows Vista???
https://hfiref0x.github.io/syscalls.html
Actually this shows that it is available since the start of NT
*/

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtFlushBuffersFile(void *, io_status_block *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwFlushBuffersFile(void *, io_status_block *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 2)
inline ::std::uint_least32_t nt_flush_buffers_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwFlushBuffersFile(args...);
	}
	else
	{
		return NtFlushBuffersFile(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtFlushBuffersFileEx(void *, ::std::uint_least32_t, void *, ::std::uint_least32_t, io_status_block *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwFlushBuffersFileEx(void *, ::std::uint_least32_t, void *, ::std::uint_least32_t, io_status_block *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 5)
inline ::std::uint_least32_t nt_flush_buffers_file_ex(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwFlushBuffersFileEx(args...);
	}
	else
	{
		return NtFlushBuffersFileEx(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if __has_cpp_attribute(__gnu__::__cdecl__)
[[__gnu__::__cdecl__]]
#endif
extern ::std::uint_least32_t
#if !__has_cpp_attribute(__gnu__::__cdecl__) && defined(_MSC_VER)
	__cdecl
#endif
	DbgPrint(char const *, ...) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if __has_cpp_attribute(__gnu__::__cdecl__)
[[__gnu__::__cdecl__]]
#endif
extern ::std::uint_least32_t
#if !__has_cpp_attribute(__gnu__::__cdecl__) && defined(_MSC_VER)
	__cdecl
#endif
	DbgPrintEx(::std::uint_least32_t, ::std::uint_least32_t, char const *, ...) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	RtlCreateProcessParameters(rtl_user_process_parameters **, unicode_string *, unicode_string *, unicode_string *,
							   unicode_string *, void *, unicode_string *, unicode_string *, unicode_string *,
							   unicode_string *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	RtlCreateProcessParametersEx(rtl_user_process_parameters **, unicode_string *, unicode_string *, unicode_string *,
								 unicode_string *, void *, unicode_string *, unicode_string *, unicode_string *,
								 unicode_string *, ::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	RtlDestroyProcessParameters(rtl_user_process_parameters *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtCreateUserProcess(void *, void *, ::std::uint_least32_t, ::std::uint_least32_t, object_attributes *,
						object_attributes *, ::std::uint_least32_t, ::std::uint_least32_t,
						rtl_user_process_parameters *, ps_create_info *, ps_attribute_list *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwCreateUserProcess(void *, void *, ::std::uint_least32_t, ::std::uint_least32_t, object_attributes *,
						object_attributes *, ::std::uint_least32_t, ::std::uint_least32_t,
						rtl_user_process_parameters *, ps_create_info *, ps_attribute_list *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 11)
inline ::std::uint_least32_t nt_create_user_process(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwCreateUserProcess(args...);
	}
	else
	{
		return NtCreateUserProcess(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	RtlCreateUserProcess(unicode_string *, ::std::uint_least32_t, rtl_user_process_parameters *, security_descriptor *,
						 security_descriptor *, void *, char unsigned, void *, void *,
						 rtl_user_process_information *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
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

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtMapViewOfSection(void *, void *, void **, ::std::size_t, ::std::size_t, large_integer const *, ::std::size_t *,
					   section_inherit, ::std::uint_least32_t, ::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtMapViewOfSection@40")
#else
	__asm__("_NtMapViewOfSection@40")
#endif
#else
	__asm__("NtMapViewOfSection")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwMapViewOfSection(void *, void *, void **, ::std::size_t, ::std::size_t, large_integer const *, ::std::size_t *,
					   section_inherit, ::std::uint_least32_t, ::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwMapViewOfSection@40")
#else
	__asm__("_ZwMapViewOfSection@40")
#endif
#else
	__asm__("ZwMapViewOfSection")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 10)
inline ::std::uint_least32_t nt_map_view_of_section(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwMapViewOfSection(args...);
	}
	else
	{
		return NtMapViewOfSection(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtUnmapViewOfSection(void *, void *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtUnmapViewOfSection@8")
#else
	__asm__("_NtUnmapViewOfSection@8")
#endif
#else
	__asm__("NtUnmapViewOfSection")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwUnmapViewOfSection(void *, void *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwUnmapViewOfSection@8")
#else
	__asm__("_ZwUnmapViewOfSection@8")
#endif
#else
	__asm__("ZwUnmapViewOfSection")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 2)
inline ::std::uint_least32_t nt_unmap_view_of_section(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwUnmapViewOfSection(args...);
	}
	else
	{
		return NtUnmapViewOfSection(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtReadVirtualMemory(void *, void *, void *, ::std::size_t, ::std::size_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtReadVirtualMemory@20")
#else
	__asm__("_NtReadVirtualMemory@20")
#endif
#else
	__asm__("NtReadVirtualMemory")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwReadVirtualMemory(void *, void *, void *, ::std::size_t, ::std::size_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwReadVirtualMemory@20")
#else
	__asm__("_ZwReadVirtualMemory@20")
#endif
#else
	__asm__("ZwReadVirtualMemory")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 5)
inline ::std::uint_least32_t nt_read_virtual_memory(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwReadVirtualMemory(args...);
	}
	else
	{
		return NtReadVirtualMemory(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtWriteVirtualMemory(void *, void *, void *, ::std::size_t, ::std::size_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtWriteVirtualMemory@20")
#else
	__asm__("_NtWriteVirtualMemory@20")
#endif
#else
	__asm__("NtWriteVirtualMemory")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwWriteVirtualMemory(void *, void *, void *, ::std::size_t, ::std::size_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwWriteVirtualMemory@20")
#else
	__asm__("_ZwWriteVirtualMemory@20")
#endif
#else
	__asm__("ZwWriteVirtualMemory")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 5)
inline ::std::uint_least32_t nt_write_virtual_memory(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwWriteVirtualMemory(args...);
	}
	else
	{
		return NtWriteVirtualMemory(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	RtlAcquirePebLock() noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("RtlAcquirePebLock@0")
#else
	__asm__("_RtlAcquirePebLock@0")
#endif
#else
	__asm__("RtlAcquirePebLock")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	RtlReleasePebLock() noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("RtlReleasePebLock@0")
#else
	__asm__("_RtlReleasePebLock@0")
#endif
#else
	__asm__("RtlReleasePebLock")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtAllocateVirtualMemory(void *, void **, ::std::size_t, ::std::size_t *, ::std::uint_least32_t,
							::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtAllocateVirtualMemory@24")
#else
	__asm__("_NtAllocateVirtualMemory@24")
#endif
#else
	__asm__("NtAllocateVirtualMemory")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwAllocateVirtualMemory(void *, void **, ::std::size_t, ::std::size_t *, ::std::uint_least32_t,
							::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwAllocateVirtualMemory@24")
#else
	__asm__("_ZwAllocateVirtualMemory@24")
#endif
#else
	__asm__("ZwAllocateVirtualMemory")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 6)
inline ::std::uint_least32_t nt_allocate_virtual_memory(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwAllocateVirtualMemory(args...);
	}
	else
	{
		return NtAllocateVirtualMemory(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtFreeVirtualMemory(void *, void **, ::std::size_t *, ::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtFreeVirtualMemory@16")
#else
	__asm__("_NtFreeVirtualMemory@16")
#endif
#else
	__asm__("NtFreeVirtualMemory")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwFreeVirtualMemory(void *, void **, ::std::size_t *, ::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwFreeVirtualMemory@16")
#else
	__asm__("_ZwFreeVirtualMemory@16")
#endif
#else
	__asm__("ZwFreeVirtualMemory")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 4)
inline ::std::uint_least32_t nt_free_virtual_memory(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwFreeVirtualMemory(args...);
	}
	else
	{
		return NtFreeVirtualMemory(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	RtlInitUnicodeString(unicode_string *, char16_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("RtlInitUnicodeString@8")
#else
	__asm__("_RtlInitUnicodeString@8")
#endif
#else
	__asm__("RtlInitUnicodeString")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	CsrClientCallServer(void *, void *, ::std::uint_least32_t, ::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("CsrClientCallServer@16")
#else
	__asm__("_CsrClientCallServer@16")
#endif
#else
	__asm__("CsrClientCallServer")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtQuerySystemInformation(system_information_class, void *, ::std::uint_least32_t, ::std::uint_least32_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtQuerySystemInformation@16")
#else
	__asm__("_NtQuerySystemInformation@16")
#endif
#else
	__asm__("NtQuerySystemInformation")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwQuerySystemInformation(system_information_class, void *, ::std::uint_least32_t, ::std::uint_least32_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwQuerySystemInformation@16")
#else
	__asm__("_ZwQuerySystemInformation@16")
#endif
#else
	__asm__("ZwQuerySystemInformation")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 4)
inline ::std::uint_least32_t nt_query_system_information(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwQuerySystemInformation(args...);
	}
	else
	{
		return NtQuerySystemInformation(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	RtlAcquireSRWLockExclusive(void *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("RtlAcquireSRWLockExclusive@4")
#else
	__asm__("_RtlAcquireSRWLockExclusive@4")
#endif
#else
	__asm__("RtlAcquireSRWLockExclusive")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	RtlTryAcquireSRWLockExclusive(void *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("RtlTryAcquireSRWLockExclusive@4")
#else
	__asm__("_RtlTryAcquireSRWLockExclusive@4")
#endif
#else
	__asm__("RtlTryAcquireSRWLockExclusive")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	RtlReleaseSRWLockExclusive(void *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("RtlReleaseSRWLockExclusive@4")
#else
	__asm__("_RtlReleaseSRWLockExclusive@4")
#endif
#else
	__asm__("RtlReleaseSRWLockExclusive")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtQueryVolumeInformationFile(void *__restrict, io_status_block *__restrict, void *__restrict, ::std::uint_least32_t,
								 fs_information_class) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtQueryVolumeInformationFile@20")
#else
	__asm__("_NtQueryVolumeInformationFile@20")
#endif
#else
	__asm__("NtQueryVolumeInformationFile")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwQueryVolumeInformationFile(void *__restrict, io_status_block *__restrict, void *__restrict, ::std::uint_least32_t,
								 fs_information_class) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwQueryVolumeInformationFile@20")
#else
	__asm__("_ZwQueryVolumeInformationFile@20")
#endif
#else
	__asm__("ZwQueryVolumeInformationFile")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 5)
inline ::std::uint_least32_t nt_query_volume_information_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwQueryVolumeInformationFile(args...);
	}
	else
	{
		return NtQueryVolumeInformationFile(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtFsControlFile(void *__restrict, void *__restrict, pio_apc_routine *__restrict, void *, io_status_block *,
					::std::uint_least32_t, void *, ::std::uint_least32_t, void *, ::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtFsControlFile@40")
#else
	__asm__("_NtFsControlFile@40")
#endif
#else
	__asm__("NtFsControlFile")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwFsControlFile(void *__restrict, void *__restrict, pio_apc_routine *__restrict, void *, io_status_block *,
					::std::uint_least32_t, void *, ::std::uint_least32_t, void *, ::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwFsControlFile@40")
#else
	__asm__("_ZwFsControlFile@40")
#endif
#else
	__asm__("ZwFsControlFile")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 10)
inline ::std::uint_least32_t nt_fs_control_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwFsControlFile(args...);
	}
	else
	{
		return NtFsControlFile(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtCreateNamedPipeFile(void **__restrict, ::std::uint_least32_t, object_attributes *__restrict, io_status_block *__restrict,
						  ::std::uint_least32_t, ::std::uint_least32_t, ::std::uint_least32_t, ::std::uint_least32_t, ::std::uint_least32_t,
						  ::std::uint_least32_t, ::std::uint_least32_t, ::std::uint_least32_t, ::std::uint_least32_t, ::std::int_least64_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtCreateNamedPipeFile@56")
#else
	__asm__("_NtCreateNamedPipeFile@56")
#endif
#else
	__asm__("NtCreateNamedPipeFile")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwCreateNamedPipeFile(void **__restrict, ::std::uint_least32_t, object_attributes *__restrict, io_status_block *__restrict,
						  ::std::uint_least32_t, ::std::uint_least32_t, ::std::uint_least32_t, ::std::uint_least32_t, ::std::uint_least32_t,
						  ::std::uint_least32_t, ::std::uint_least32_t, ::std::uint_least32_t, ::std::uint_least32_t, ::std::int_least64_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwCreateNamedPipeFile@56")
#else
	__asm__("_ZwCreateNamedPipeFile@56")
#endif
#else
	__asm__("ZwCreateNamedPipeFile")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 14)
inline ::std::uint_least32_t nt_create_named_pipe_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwCreateNamedPipeFile(args...);
	}
	else
	{
		return NtCreateNamedPipeFile(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtQueryPerformanceCounter(::std::int_least64_t *, ::std::int_least64_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtQueryPerformanceCounter@8")
#else
	__asm__("_NtQueryPerformanceCounter@8")
#endif
#else
	__asm__("NtQueryPerformanceCounter")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwQueryPerformanceCounter(::std::int_least64_t *, ::std::int_least64_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwQueryPerformanceCounter@8")
#else
	__asm__("_ZwQueryPerformanceCounter@8")
#endif
#else
	__asm__("ZwQueryPerformanceCounter")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 2)
inline ::std::uint_least32_t nt_query_performance_counter(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwQueryPerformanceCounter(args...);
	}
	else
	{
		return NtQueryPerformanceCounter(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least64_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	RtlGetSystemTimePrecise() noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("RtlGetSystemTimePrecise@0")
#else
	__asm__("_RtlGetSystemTimePrecise@0")
#endif
#else
	__asm__("RtlGetSystemTimePrecise")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtQueryInformationThread(void *__restrict, thread_information_class, void *, ::std::uint_least32_t,
							 ::std::uint_least32_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtQueryInformationThread@20")
#else
	__asm__("_NtQueryInformationThread@20")
#endif
#else
	__asm__("NtQueryInformationThread")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwQueryInformationThread(void *__restrict, thread_information_class, void *, ::std::uint_least32_t,
							 ::std::uint_least32_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwQueryInformationThread@20")
#else
	__asm__("_ZwQueryInformationThread@20")
#endif
#else
	__asm__("ZwQueryInformationThread")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 5)
inline ::std::uint_least32_t nt_query_information_thread(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwQueryInformationThread(args...);
	}
	else
	{
		return NtQueryInformationThread(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	RtlAdjustPrivilege(::std::uint_least32_t, ::std::uint_least8_t, ::std::uint_least8_t, ::std::uint_least8_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("RtlAdjustPrivilege@16")
#else
	__asm__("_RtlAdjustPrivilege@16")
#endif
#else
	__asm__("RtlAdjustPrivilege")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtSetInformationObject(void *__restrict, object_information_class, void *, ::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtSetInformationObject@16")
#else
	__asm__("_NtSetInformationObject@16")
#endif
#else
	__asm__("NtSetInformationObject")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwSetInformationObject(void *__restrict, object_information_class, void *, ::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwSetInformationObject@16")
#else
	__asm__("_ZwSetInformationObject@16")
#endif
#else
	__asm__("ZwSetInformationObject")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 4)
inline ::std::uint_least32_t nt_set_information_object(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwSetInformationObject(args...);
	}
	else
	{
		return NtSetInformationObject(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtQueryAuxiliaryCounterFrequency(::std::uint_least64_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtQueryAuxiliaryCounterFrequency@4")
#else
	__asm__("_NtQueryAuxiliaryCounterFrequency@4")
#endif
#else
	__asm__("NtQueryAuxiliaryCounterFrequency")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwQueryAuxiliaryCounterFrequency(::std::uint_least64_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwQueryAuxiliaryCounterFrequency@4")
#else
	__asm__("_ZwQueryAuxiliaryCounterFrequency@4")
#endif
#else
	__asm__("ZwQueryAuxiliaryCounterFrequency")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 1)
inline ::std::uint_least32_t nt_query_auxiliary_counter_frequency(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwQueryAuxiliaryCounterFrequency(args...);
	}
	else
	{
		return NtQueryAuxiliaryCounterFrequency(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern rtl_path_type
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	RtlDetermineDosPathNameType_U(char16_t const *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("RtlDetermineDosPathNameType_U@4")
#else
	__asm__("_RtlDetermineDosPathNameType_U@4")
#endif
#else
	__asm__("RtlDetermineDosPathNameType_U")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	RtlGetFullPathName_U(char16_t const *, ::std::uint_least32_t, char16_t *, char16_t **) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("RtlGetFullPathName_U@16")
#else
	__asm__("_RtlGetFullPathName_U@16")
#endif
#else
	__asm__("RtlGetFullPathName_U")
#endif
#endif
		;


#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void *
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	AlpcGetMessageAttribute(alpc_message_attributes *, ::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("AlpcGetMessageAttribute@8")
#else
	__asm__("_AlpcGetMessageAttribute@8")
#endif
#else
	__asm__("AlpcGetMessageAttribute")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	AlpcInitializeMessageAttribute(::std::uint_least32_t, alpc_message_attributes *, ::std::size_t, ::std::size_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("AlpcInitializeMessageAttribute@16")
#else
	__asm__("_AlpcInitializeMessageAttribute@16")
#endif
#else
	__asm__("AlpcInitializeMessageAttribute")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtAlpcCreatePort(void **, object_attributes *, alpc_port_attributes *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtAlpcCreatePort@12")
#else
	__asm__("_NtAlpcCreatePort@12")
#endif
#else
	__asm__("NtAlpcCreatePort")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwAlpcCreatePort(void **, object_attributes *, alpc_port_attributes *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwAlpcCreatePort@12")
#else
	__asm__("_ZwAlpcCreatePort@12")
#endif
#else
	__asm__("ZwAlpcCreatePort")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 3)
inline ::std::uint_least32_t nt_alpc_create_port(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwAlpcCreatePort(args...);
	}
	else
	{
		return NtAlpcCreatePort(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtAlpcConnectPort(void **, unicode_string *, object_attributes *, alpc_port_attributes *,
					  ::std::uint_least32_t, void *, port_message *, ::std::size_t *,
					  alpc_message_attributes *, alpc_message_attributes *, ::std::int_least64_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtAlpcConnectPort@44")
#else
	__asm__("_NtAlpcConnectPort@44")
#endif
#else
	__asm__("NtAlpcConnectPort")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwAlpcConnectPort(void **, unicode_string *, object_attributes *, alpc_port_attributes *,
					  ::std::uint_least32_t, void *, port_message *, ::std::size_t *,
					  alpc_message_attributes *, alpc_message_attributes *, ::std::int_least64_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwAlpcConnectPort@44")
#else
	__asm__("_ZwAlpcConnectPort@44")
#endif
#else
	__asm__("ZwAlpcConnectPort")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 11)
inline ::std::uint_least32_t nt_alpc_connect_port(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwAlpcConnectPort(args...);
	}
	else
	{
		return NtAlpcConnectPort(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtAlpcSendWaitReceivePort(void *, ::std::uint_least32_t, port_message *, alpc_message_attributes *,
							  port_message *, ::std::size_t *, alpc_message_attributes *, ::std::int_least64_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtAlpcSendWaitReceivePort@32")
#else
	__asm__("_NtAlpcSendWaitReceivePort@32")
#endif
#else
	__asm__("NtAlpcSendWaitReceivePort")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwAlpcSendWaitReceivePort(void *, ::std::uint_least32_t, port_message *, alpc_message_attributes *,
							  port_message *, ::std::size_t *, alpc_message_attributes *, ::std::int_least64_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwAlpcSendWaitReceivePort@32")
#else
	__asm__("_ZwAlpcSendWaitReceivePort@32")
#endif
#else
	__asm__("ZwAlpcSendWaitReceivePort")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 8)
inline ::std::uint_least32_t nt_alpc_send_wait_receive_port(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwAlpcSendWaitReceivePort(args...);
	}
	else
	{
		return NtAlpcSendWaitReceivePort(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtAlpcAcceptConnectPort(void **, void *, ::std::uint_least32_t,
							object_attributes *, alpc_port_attributes *, void *, port_message *,
							alpc_message_attributes *, ::std::uint_least8_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtAlpcAcceptConnectPort@36")
#else
	__asm__("_NtAlpcAcceptConnectPort@36")
#endif
#else
	__asm__("NtAlpcAcceptConnectPort")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwAlpcAcceptConnectPort(void **, void *, ::std::uint_least32_t,
							object_attributes *, alpc_port_attributes *, void *, port_message *,
							alpc_message_attributes *, ::std::uint_least8_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwAlpcAcceptConnectPort@36")
#else
	__asm__("_ZwAlpcAcceptConnectPort@36")
#endif
#else
	__asm__("ZwAlpcAcceptConnectPort")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 9)
inline ::std::uint_least32_t nt_alpc_accept_connect_port(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwAlpcAcceptConnectPort(args...);
	}
	else
	{
		return NtAlpcAcceptConnectPort(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::size_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	AlpcGetHeaderSize(::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("AlpcGetHeaderSize@4")
#else
	__asm__("_AlpcGetHeaderSize@4")
#endif
#else
	__asm__("AlpcGetHeaderSize")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtAlpcImpersonateClientOfPort(void *, port_message *, void *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtAlpcImpersonateClientOfPort@12")
#else
	__asm__("_NtAlpcImpersonateClientOfPort@12")
#endif
#else
	__asm__("NtAlpcImpersonateClientOfPort")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwAlpcImpersonateClientOfPort(void *, port_message *, void *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwAlpcImpersonateClientOfPort@12")
#else
	__asm__("_ZwAlpcImpersonateClientOfPort@12")
#endif
#else
	__asm__("ZwAlpcImpersonateClientOfPort")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 3)
inline ::std::uint_least32_t nt_alpc_impersonate_client_of_port(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwAlpcImpersonateClientOfPort(args...);
	}
	else
	{
		return NtAlpcImpersonateClientOfPort(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtAlpcImpersonateClientContainerOfPort(void *, port_message *, ::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtAlpcImpersonateClientContainerOfPort@12")
#else
	__asm__("_NtAlpcImpersonateClientContainerOfPort@12")
#endif
#else
	__asm__("NtAlpcImpersonateClientContainerOfPort")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwAlpcImpersonateClientContainerOfPort(void *, port_message *, ::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwAlpcImpersonateClientContainerOfPort@12")
#else
	__asm__("_ZwAlpcImpersonateClientContainerOfPort@12")
#endif
#else
	__asm__("ZwAlpcImpersonateClientContainerOfPort")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 3)
inline ::std::uint_least32_t nt_alpc_impersonate_client_container_of_port(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwAlpcImpersonateClientContainerOfPort(args...);
	}
	else
	{
		return NtAlpcImpersonateClientContainerOfPort(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtAlpcDisconnectPort(void *, ::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtAlpcDisconnectPort@8")
#else
	__asm__("_NtAlpcDisconnectPort@8")
#endif
#else
	__asm__("NtAlpcDisconnectPort")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwAlpcDisconnectPort(void *, ::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwAlpcDisconnectPort@8")
#else
	__asm__("_ZwAlpcDisconnectPort@8")
#endif
#else
	__asm__("ZwAlpcDisconnectPort")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 2)
inline ::std::uint_least32_t nt_alpc_disconnect_port(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwAlpcDisconnectPort(args...);
	}
	else
	{
		return NtAlpcDisconnectPort(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtAlpcCreatePortSection(void *, ::std::uint_least32_t, void *, ::std::size_t, void **, ::std::size_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtAlpcCreatePortSection@24")
#else
	__asm__("_NtAlpcCreatePortSection@24")
#endif
#else
	__asm__("NtAlpcCreatePortSection")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwAlpcCreatePortSection(void *, ::std::uint_least32_t, void *, ::std::size_t, void **, ::std::size_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwAlpcCreatePortSection@24")
#else
	__asm__("_ZwAlpcCreatePortSection@24")
#endif
#else
	__asm__("ZwAlpcCreatePortSection")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 6)
inline ::std::uint_least32_t nt_alpc_create_port_section(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwAlpcCreatePortSection(args...);
	}
	else
	{
		return NtAlpcCreatePortSection(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtAlpcCreateSectionView(void *, ::std::uint_least32_t, alpc_data_view_attr *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtAlpcCreateSectionView@12")
#else
	__asm__("_NtAlpcCreateSectionView@12")
#endif
#else
	__asm__("NtAlpcCreateSectionView")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwAlpcCreateSectionView(void *, ::std::uint_least32_t, alpc_data_view_attr *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwAlpcCreateSectionView@12")
#else
	__asm__("_ZwAlpcCreateSectionView@12")
#endif
#else
	__asm__("ZwAlpcCreateSectionView")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 3)
inline ::std::uint_least32_t nt_alpc_create_section_view(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwAlpcCreateSectionView(args...);
	}
	else
	{
		return NtAlpcCreateSectionView(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	AlpcMaxAllowedMessageLength() noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("AlpcMaxAllowedMessageLength@0")
#else
	__asm__("_AlpcMaxAllowedMessageLength@0")
#endif
#else
	__asm__("AlpcMaxAllowedMessageLength")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtAlpcQueryInformationMessage(void *, port_message *, alpc_message_information_class, void *, ::std::size_t, ::std::size_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtAlpcQueryInformationMessage@24")
#else
	__asm__("_NtAlpcQueryInformationMessage@24")
#endif
#else
	__asm__("NtAlpcQueryInformationMessage")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwAlpcQueryInformationMessage(void *, port_message *, alpc_message_information_class, void *, ::std::size_t, ::std::size_t *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwAlpcQueryInformationMessage@24")
#else
	__asm__("_ZwAlpcQueryInformationMessage@24")
#endif
#else
	__asm__("ZwAlpcQueryInformationMessage")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 6)
inline ::std::uint_least32_t nt_alpc_query_information_message(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwAlpcQueryInformationMessage(args...);
	}
	else
	{
		return NtAlpcQueryInformationMessage(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtAlpcCreateSecurityContext(void *, ::std::uint_least32_t, alpc_security_attr *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtAlpcCreateSecurityContext@12")
#else
	__asm__("_NtAlpcCreateSecurityContext@12")
#endif
#else
	__asm__("NtAlpcCreateSecurityContext")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwAlpcCreateSecurityContext(void *, ::std::uint_least32_t, alpc_security_attr *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwAlpcCreateSecurityContext@12")
#else
	__asm__("_ZwAlpcCreateSecurityContext@12")
#endif
#else
	__asm__("ZwAlpcCreateSecurityContext")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 3)
inline ::std::uint_least32_t nt_alpc_create_security_context(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwAlpcCreateSecurityContext(args...);
	}
	else
	{
		return NtAlpcCreateSecurityContext(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtAlpcOpenSenderThread(void **, void *, port_message *, ::std::uint_least32_t, ::std::uint_least32_t, object_attributes *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtAlpcOpenSenderThread@24")
#else
	__asm__("_NtAlpcOpenSenderThread@24")
#endif
#else
	__asm__("NtAlpcOpenSenderThread")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwAlpcOpenSenderThread(void **, void *, port_message *, ::std::uint_least32_t, ::std::uint_least32_t, object_attributes *) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwAlpcOpenSenderThread@24")
#else
	__asm__("_ZwAlpcOpenSenderThread@24")
#endif
#else
	__asm__("ZwAlpcOpenSenderThread")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 6)
inline ::std::uint_least32_t nt_alpc_open_sender_thread(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwAlpcOpenSenderThread(args...);
	}
	else
	{
		return NtAlpcOpenSenderThread(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtTerminateProcess(void *, ::std::int_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtTerminateProcess@8")
#else
	__asm__("_NtTerminateProcess@8")
#endif
#else
	__asm__("NtTerminateProcess")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwTerminateProcess(void *, ::std::int_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("ZwTerminateProcess@8")
#else
	__asm__("_ZwTerminateProcess@8")
#endif
#else
	__asm__("ZwTerminateProcess")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 2)
inline ::std::uint_least32_t nt_terminate_process(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwTerminateProcess(args...);
	}
	else
	{
		return NtTerminateProcess(args...);
	}
}

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	NtDeviceIoControlFile(void *, void *, pio_apc_routine *, void *, io_status_block *, ::std::uint_least32_t,
						  void *, ::std::uint_least32_t, void *, ::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtDeviceIoControlFile@40")
#else
	__asm__("_NtDeviceIoControlFile@40")
#endif
#else
	__asm__("NtDeviceIoControlFile")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern ::std::int_least32_t
#if (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__stdcall
#endif
	ZwDeviceIoControlFile(void *, void *, pio_apc_routine *, void *, io_status_block *, ::std::uint_least32_t,
						  void *, ::std::uint_least32_t, void *, ::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("NtDeviceIoControlFile@40")
#else
	__asm__("_NtDeviceIoControlFile@40")
#endif
#else
	__asm__("NtDeviceIoControlFile")
#endif
#endif
		;

template <bool zw, typename... Args>
	requires(sizeof...(Args) == 10)
inline ::std::uint_least32_t nt_device_io_control_file(Args... args) noexcept
{
	if constexpr (zw)
	{
		return ZwDeviceIoControlFile(args...);
	}
	else
	{
		return NtDeviceIoControlFile(args...);
	}
}
} // namespace fast_io::win32::nt
