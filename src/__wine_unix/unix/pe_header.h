#include <cstdint>
#include <cstddef>
#include <type_traits>

namespace __wine_unix
{
/*
Referenced from wine:
https://github.com/wine-mirror/wine/blob/master/include/winnt.h
*/
struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	image_file_header
{
	::std::uint_least16_t Machine;
	::std::uint_least16_t NumberOfSections;
	::std::uint_least32_t TimeDateStamp;
	::std::uint_least32_t PointerToSymbolTable;
	::std::uint_least32_t NumberOfSymbols;
	::std::uint_least16_t SizeOfOptionalHeader;
	::std::uint_least16_t Characteristics;
};

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	image_data_directory
{
	::std::uint_least32_t VirtualAddress;
	::std::uint_least32_t Size;
};

inline constexpr ::std::size_t image_numberof_directory_entries{16u};

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	image_optional_header64
{
	::std::uint_least16_t Magic; /* 0x20b */
	::std::uint_least8_t MajorLinkerVersion;
	::std::uint_least8_t MinorLinkerVersion;
	::std::uint_least32_t SizeOfCode;
	::std::uint_least32_t SizeOfInitializedData;
	::std::uint_least32_t SizeOfUninitializedData;
	::std::uint_least32_t AddressOfEntryPoint;
	::std::uint_least32_t BaseOfCode;
	::std::uint_least64_t ImageBase;
	::std::uint_least32_t SectionAlignment;
	::std::uint_least32_t FileAlignment;
	::std::uint_least16_t MajorOperatingSystemVersion;
	::std::uint_least16_t MinorOperatingSystemVersion;
	::std::uint_least16_t MajorImageVersion;
	::std::uint_least16_t MinorImageVersion;
	::std::uint_least16_t MajorSubsystemVersion;
	::std::uint_least16_t MinorSubsystemVersion;
	::std::uint_least32_t Win32VersionValue;
	::std::uint_least32_t SizeOfImage;
	::std::uint_least32_t SizeOfHeaders;
	::std::uint_least32_t CheckSum;
	::std::uint_least16_t Subsystem;
	::std::uint_least16_t DllCharacteristics;
	::std::uint_least64_t SizeOfStackReserve;
	::std::uint_least64_t SizeOfStackCommit;
	::std::uint_least64_t SizeOfHeapReserve;
	::std::uint_least64_t SizeOfHeapCommit;
	::std::uint_least32_t LoaderFlags;
	::std::uint_least32_t NumberOfRvaAndSizes;
	::__wine_unix::image_data_directory DataDirectory[::__wine_unix::image_numberof_directory_entries];
};

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	image_optional_header32
{
	/* Standard fields */

	::std::uint_least16_t Magic; /* 0x10b or 0x107 */ /* 0x00 */
	::std::uint_least8_t MajorLinkerVersion;
	::std::uint_least8_t MinorLinkerVersion;
	::std::uint_least32_t SizeOfCode;
	::std::uint_least32_t SizeOfInitializedData;
	::std::uint_least32_t SizeOfUninitializedData;
	::std::uint_least32_t AddressOfEntryPoint; /* 0x10 */
	::std::uint_least32_t BaseOfCode;
	::std::uint_least32_t BaseOfData;

	/* NT additional fields */

	::std::uint_least32_t ImageBase;
	::std::uint_least32_t SectionAlignment; /* 0x20 */
	::std::uint_least32_t FileAlignment;
	::std::uint_least16_t MajorOperatingSystemVersion;
	::std::uint_least16_t MinorOperatingSystemVersion;
	::std::uint_least16_t MajorImageVersion;
	::std::uint_least16_t MinorImageVersion;
	::std::uint_least16_t MajorSubsystemVersion; /* 0x30 */
	::std::uint_least16_t MinorSubsystemVersion;
	::std::uint_least32_t Win32VersionValue;
	::std::uint_least32_t SizeOfImage;
	::std::uint_least32_t SizeOfHeaders;
	::std::uint_least32_t CheckSum; /* 0x40 */
	::std::uint_least16_t Subsystem;
	::std::uint_least16_t DllCharacteristics;
	::std::uint_least32_t SizeOfStackReserve;
	::std::uint_least32_t SizeOfStackCommit;
	::std::uint_least32_t SizeOfHeapReserve; /* 0x50 */
	::std::uint_least32_t SizeOfHeapCommit;
	::std::uint_least32_t LoaderFlags;
	::std::uint_least32_t NumberOfRvaAndSizes;
	::__wine_unix::image_data_directory DataDirectory[::__wine_unix::image_numberof_directory_entries]; /* 0x60 */
	/* 0xE0 */
};

template <typename T>
struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	basic_image_nt_headers
{
	::std::uint_least32_t Signature; /* "PE"\0\0 */ /* 0x00 */
	::__wine_unix::image_file_header FileHeader;    /* 0x04 */
	T OptionalHeader;                               /* 0x18 */
};

using image_nt_headers32 = ::__wine_unix::basic_image_nt_headers<::__wine_unix::image_optional_header32>;
using image_nt_headers64 = ::__wine_unix::basic_image_nt_headers<::__wine_unix::image_optional_header64>;

using image_nt_headers = ::std::conditional_t<(sizeof(::std::size_t) <= sizeof(::std::uint_least32_t)),
											  ::__wine_unix::image_nt_headers32,
											  ::__wine_unix::image_nt_headers64>;

} // namespace __wine_unix
