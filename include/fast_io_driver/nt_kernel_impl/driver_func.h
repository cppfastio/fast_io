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
	IoDeleteSymbolicLink(::fast_io::win32::nt::unicode_string *SymbolicLinkName) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("IoDeleteSymbolicLink@4")
#else
	__asm__("_IoDeleteSymbolicLink@4")
#endif
#else
	__asm__("IoDeleteSymbolicLink")
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
	IoDeleteDevice(::fast_io::win32::nt::device_object *DeviceObject) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("IoDeleteDevice@4")
#else
	__asm__("_IoDeleteDevice@4")
#endif
#else
	__asm__("IoDeleteDevice")
#endif
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__fastcall__) && !defined(__WINE__))
[[__gnu__::__fastcall__]]
#endif
extern ::std::uint_least32_t
#if (!__has_cpp_attribute(__gnu__::__fastcall__) && !defined(__WINE__)) && defined(_MSC_VER)
	__fastcall
#endif
	IofCompleteRequest(::fast_io::win32::nt::irp *Irp, char PriorityBoost) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("IofCompleteRequest@8")
#else
	__asm__("_IofCompleteRequest@8")
#endif
#else
	__asm__("IofCompleteRequest")
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
	IoCreateDevice(::fast_io::win32::nt::driver_object *DriverObject,
				   ::std::uint_least32_t DeviceExtensionSize,
				   ::fast_io::win32::nt::unicode_string *DeviceName,
				   ::std::uint_least32_t DeviceType,
				   ::std::uint_least32_t DeviceCharacteristics,
				   ::std::uint_least8_t Exclusive,
				   ::fast_io::win32::nt::device_object **DeviceObject) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("IoCreateDevice@28")
#else
	__asm__("_IoCreateDevice@28")
#endif
#else
	__asm__("IoCreateDevice")
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
	IoCreateSymbolicLink(::fast_io::win32::nt::unicode_string *SymbolicLinkName, ::fast_io::win32::nt::unicode_string *DeviceName) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
	__asm__("IoCreateSymbolicLink@8")
#else
	__asm__("_IoCreateSymbolicLink@8")
#endif
#else
	__asm__("IoCreateSymbolicLink")
#endif
#endif
		;

} // namespace fast_io::win32::nt
