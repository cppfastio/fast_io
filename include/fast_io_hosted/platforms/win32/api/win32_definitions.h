#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include "../../../win32_family.h"
#include "../../../../fast_io_core_impl/socket/posix_sockaddr.h"

namespace fast_io::win32
{

/**
 * @brief fast_io will never define these types which defined in WDK
 */
// using NTSTATUS = ::std::int_least32_t;
// using LUID = ::std::int_least64_t;

struct overlapped
{
	::std::conditional_t<(sizeof(::std::size_t) > 4), ::std::uint_least64_t, ::std::uint_least32_t> Internal,
		InternalHigh;
	union dummy_union_name_t
	{
		struct dummy_struct_name_t
		{
			::std::uint_least32_t Offset;
			::std::uint_least32_t OffsetHigh;
		} dummy_struct_name;
		void *Pointer;
	} dummy_union_name;
	void *hEvent;
};

struct security_attributes
{
	::std::uint_least32_t nLength;
	void *lpSecurityDescriptor;
	int bInheritHandle;
};

struct startupinfoa
{
	::std::uint_least32_t cb;
	char *lpReserved;
	char *lpDesktop;
	char *lpTitle;
	::std::uint_least32_t dwX;
	::std::uint_least32_t dwY;
	::std::uint_least32_t dwXSize;
	::std::uint_least32_t dwYSize;
	::std::uint_least32_t dwXCountChars;
	::std::uint_least32_t dwYCountChars;
	::std::uint_least32_t dwFillAttribute;
	::std::uint_least32_t dwFlags;
	::std::uint_least16_t wShowWindow;
	::std::uint_least16_t cbReserved2;
	::std::uint_least8_t *lpReserved2;
	void *hStdInput;
	void *hStdOutput;
	void *hStdError;
};

struct startupinfow
{
	::std::uint_least32_t cb;
	char16_t *lpReserved;
	char16_t *lpDesktop;
	char16_t *lpTitle;
	::std::uint_least32_t dwX;
	::std::uint_least32_t dwY;
	::std::uint_least32_t dwXSize;
	::std::uint_least32_t dwYSize;
	::std::uint_least32_t dwXCountChars;
	::std::uint_least32_t dwYCountChars;
	::std::uint_least32_t dwFillAttribute;
	::std::uint_least32_t dwFlags;
	::std::uint_least16_t wShowWindow;
	::std::uint_least16_t cbReserved2;
	::std::uint_least8_t *lpReserved2;
	void *hStdInput;
	void *hStdOutput;
	void *hStdError;
};

struct process_information
{
	void *hProcess;
	void *hThread;
	::std::uint_least32_t dwProcessId;
	::std::uint_least32_t dwThreadId;
};

/*
https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getfileinformationbyhandleex
*/

struct file_standard_info
{
	::std::int_least64_t AllocationSize;
	::std::int_least64_t EndOfFile;
	::std::uint_least32_t NumberOfLinks;
	int DeletePending;
	int Directory;
};

struct file_attribute_tag_info
{
	::std::uint_least32_t FileAttributes;
	::std::uint_least32_t ReparseTag;
};

enum class file_info_by_handle_class
{
	FileBasicInfo,
	FileStandardInfo,
	FileNameInfo,
	FileRenameInfo,
	FileDispositionInfo,
	FileAllocationInfo,
	FileEndOfFileInfo,
	FileStreamInfo,
	FileCompressionInfo,
	FileAttributeTagInfo,
	FileIdBothDirectoryInfo,
	FileIdBothDirectoryRestartInfo,
	FileIoPriorityHintInfo,
	FileRemoteProtocolInfo,
	FileFullDirectoryInfo,
	FileFullDirectoryRestartInfo,
	FileStorageInfo,
	FileAlignmentInfo,
	FileIdInfo,
	FileIdExtdDirectoryInfo,
	FileIdExtdDirectoryRestartInfo,
	FileDispositionInfoEx,
	FileRenameInfoEx,
	FileCaseSensitiveInfo,
	FileNormalizedNameInfo,
	MaximumFileInfoByHandleClass
};

struct filetime
{
	::std::uint_least32_t dwLowDateTime, dwHighDateTime;
};

struct by_handle_file_information
{
	::std::uint_least32_t dwFileAttributes;
	filetime ftCreationTime;
	filetime ftLastAccessTime;
	filetime ftLastWriteTime;
	::std::uint_least32_t dwVolumeSerialNumber;
	::std::uint_least32_t nFileSizeHigh;
	::std::uint_least32_t nFileSizeLow;
	::std::uint_least32_t nNumberOfLinks;
	::std::uint_least32_t nFileIndexHigh;
	::std::uint_least32_t nFileIndexLow;
};

struct coord
{
	::std::int_least16_t X, Y;
};

struct small_rect
{
	::std::int_least16_t Left, Top, Right, Bottom;
};

struct char_info
{
	char16_t character;
	::std::uint_least16_t Attrib;
};

struct console_screen_buffer_info
{
	coord Size, CursorPosition;
	::std::uint_least16_t Attrib;
	small_rect Window;
	coord MaxWindowSize;
};

struct guid
{
	unsigned long Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char Data4[8];
};

inline constexpr ::std::size_t wsaprotocol_len{255};

struct wsaprotocolchain
{
	int ChainLen;
	::std::uint_least32_t ChainEntries[7];
};

struct wsaprotocol_infow
{
	::std::uint_least32_t dwServiceFlags1;
	::std::uint_least32_t dwServiceFlags2;
	::std::uint_least32_t dwServiceFlags3;
	::std::uint_least32_t dwServiceFlags4;
	::std::uint_least32_t dwProviderFlags;
	guid ProviderId;
	::std::uint_least32_t dwCatalogEntryId;
	wsaprotocolchain ProtocolChain;
	int iVersion;
	int iAddressFamily;
	int iMaxSockAddr;
	int iMinSockAddr;
	int iSocketType;
	int iProtocol;
	int iProtocolMaxOffset;
	int iNetworkByteOrder;
	int iSecurityScheme;
	::std::uint_least32_t dwMessageSize;
	::std::uint_least32_t dwProviderReserved;
	char16_t szProtocol[wsaprotocol_len + 1];
};

struct wsaprotocol_infoa
{
	::std::uint_least32_t dwServiceFlags1;
	::std::uint_least32_t dwServiceFlags2;
	::std::uint_least32_t dwServiceFlags3;
	::std::uint_least32_t dwServiceFlags4;
	::std::uint_least32_t dwProviderFlags;
	guid ProviderId;
	::std::uint_least32_t dwCatalogEntryId;
	wsaprotocolchain ProtocolChain;
	int iVersion;
	int iAddressFamily;
	int iMaxSockAddr;
	int iMinSockAddr;
	int iSocketType;
	int iProtocol;
	int iProtocolMaxOffset;
	int iNetworkByteOrder;
	int iSecurityScheme;
	::std::uint_least32_t dwMessageSize;
	::std::uint_least32_t dwProviderReserved;
	char szProtocol[wsaprotocol_len + 1];
};

inline constexpr ::std::size_t wsadescription_len{256};
inline constexpr ::std::size_t wsasys_status_len{128};

struct wsadata
{
	::std::uint_least16_t wVersion;
	::std::uint_least16_t wHighVersion;
#ifdef _WIN64
	unsigned short iMaxSockets;
	unsigned short iMaxUdpDg;
	char *lpVendorInfo;
	char szDescription[wsadescription_len + 1];
	char szSystemStatus[wsasys_status_len + 1];
#else
	char szDescription[wsadescription_len + 1];
	char szSystemStatus[wsasys_status_len + 1];
	unsigned short iMaxSockets;
	unsigned short iMaxUdpDg;
	char *lpVendorInfo;
#endif
};

struct wsabuf
{
	::std::uint_least32_t len;
	char *buf;
};

struct wsamsg
{
	void *name;
	int namelen;
	wsabuf *lpBuffers;
	::std::uint_least32_t dwBufferCount;
	wsabuf Control;
	::std::uint_least32_t dwflags;
};

inline constexpr ::std::size_t fd_max_events{10u};

struct wsanetworkevents
{
	::std::int_least32_t lNetworkEvents;
	int iErrorCode[fd_max_events];
};

using ptimerapcroutine = void(
#if defined(_MSC_VER) && (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
	__stdcall
#elif (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
	__attribute__((__stdcall__))
#endif
		*)(void *, ::std::uint_least32_t, ::std::uint_least32_t) noexcept;

using lpwsaoverlapped_completion_routine = void(
#if defined(_MSC_VER) && (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
	__stdcall
#elif (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
	__attribute__((__stdcall__))
#endif
		*)(::std::uint_least32_t dwError, ::std::uint_least32_t cbTransferred, overlapped *lpOverlapped,
		   ::std::uint_least32_t dwFlags) noexcept;

// PTP_TIMER_CALLBACK: the threadpool timer fires it as
// (instance, context, timer)
using ptp_timer_callback = void(
#if defined(_MSC_VER) && (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
	__stdcall
#elif (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
	__attribute__((__stdcall__))
#endif
		*)(void *, void *, void *) noexcept;

// LPTHREAD_START_ROUTINE: QueueUserWorkItem fires it as (context)
using lthread_start_routine = ::std::uint_least32_t(
#if defined(_MSC_VER) && (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
	__stdcall
#elif (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
	__attribute__((__stdcall__))
#endif
		*)(void *) noexcept;

struct flowspec
{
	::std::uint_least32_t TokenRate;
	::std::uint_least32_t TokenBucketSize;
	::std::uint_least32_t PeakBandwidth;
	::std::uint_least32_t Latency;
	::std::uint_least32_t DelayVariation;
	::std::uint_least32_t ServiceType;
	::std::uint_least32_t MaxSduSize;
	::std::uint_least32_t MinimumPolicedSize;
};

struct qualityofservice
{
	flowspec SendingFlowspec;
	flowspec ReceivingFlowspec;
	wsabuf ProviderSpecific;
};

using lpconditionproc = void(
#if defined(_MSC_VER) && (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
	__stdcall
#elif (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
	__attribute__((__stdcall__))
#endif
		*)(wsabuf *, wsabuf *, qualityofservice *, qualityofservice *, wsabuf *, wsabuf *, ::std::uint_least32_t *,
		   ::std::size_t) noexcept;

/* LPFN_ACCEPTEX: the AcceptEx extension function, resolved at runtime
 * through WSAIoctl(SIO_GET_EXTENSION_FUNCTION_POINTER) since it lives
 * in mswsock.dll rather than ws2_32 */
using acceptex_func = int(
#if defined(_MSC_VER) && (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
	__stdcall
#elif (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
	__attribute__((__stdcall__))
#endif
		*)(::std::size_t, ::std::size_t, void *, ::std::uint_least32_t, ::std::uint_least32_t,
		   ::std::uint_least32_t, ::std::uint_least32_t *, overlapped *) noexcept;

/* LPFN_CONNECTEX: the ConnectEx extension function, resolved at runtime
 * through WSAIoctl(SIO_GET_EXTENSION_FUNCTION_POINTER) like AcceptEx —
 * the socket must be bound before the call and carry WSA_FLAG_OVERLAPPED */
using connectex_func = int(
#if defined(_MSC_VER) && (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
	__stdcall
#elif (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
	__attribute__((__stdcall__))
#endif
		*)(::std::size_t, void const *, int, void *, ::std::uint_least32_t,
		   ::std::uint_least32_t *, overlapped *) noexcept;

template <win32_family fam>
	requires(fam == win32_family::ansi_9x || fam == win32_family::wide_nt)
struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	win32_family_addrinfo
{
	int ai_flags{};
	int ai_family{};
	int ai_socktype{};
	int ai_protocol{};
	::std::size_t ai_addrlen{};
	::std::conditional_t<fam == win32_family::ansi_9x, char, char16_t> *ai_canonname{};
	posix_sockaddr *ai_addr{};
	win32_family_addrinfo<fam> *ai_next{};
};

using win32_addrinfo_9xa = win32_family_addrinfo<win32_family::ansi_9x>;
using win32_addrinfo_ntw = win32_family_addrinfo<win32_family::wide_nt>;

/* GetAdaptersAddresses types — field-faithful prefixes of the SDK's
 * IP_ADAPTER_* structs: every member up to the last one accessed is
 * declared, trailing members are irrelevant since traversal follows
 * Next pointers rather than sizeof. */
struct win32_socket_address
{
	posix_sockaddr *lpSockaddr;
	int iSockaddrLength;
};

struct win32_ip_adapter_prefix;
struct win32_ip_adapter_anycast_address;
struct win32_ip_adapter_multicast_address;
struct win32_ip_adapter_dns_server_address;

struct win32_ip_adapter_unicast_address
{
	struct length_and_flags
	{
		::std::uint_least32_t length;
		::std::uint_least32_t flags;
	};
	union
	{
		::std::uint_least64_t alignment;
		length_and_flags fields;
	};
	win32_ip_adapter_unicast_address *next;
	win32_socket_address address;
	::std::uint_least32_t prefix_origin;
	::std::uint_least32_t suffix_origin;
	::std::uint_least32_t dad_state;
	::std::uint_least32_t valid_lifetime;
	::std::uint_least32_t preferred_lifetime;
	::std::uint_least32_t lease_lifetime;
	::std::uint_least8_t on_link_prefix_length;
};

struct win32_ip_adapter_addresses
{
	struct length_and_if_index
	{
		::std::uint_least32_t length;
		::std::uint_least32_t if_index;
	};
	union
	{
		::std::uint_least64_t alignment;
		length_and_if_index fields;
	};
	win32_ip_adapter_addresses *next;
	char *adapter_name;
	win32_ip_adapter_unicast_address *first_unicast_address;
	win32_ip_adapter_anycast_address *first_anycast_address;
	win32_ip_adapter_multicast_address *first_multicast_address;
	win32_ip_adapter_dns_server_address *first_dns_server_address;
	char16_t *dns_suffix;
	char16_t *description;
	char16_t *friendly_name;
	::std::uint_least8_t physical_address[8];
	::std::uint_least32_t physical_address_length;
	::std::uint_least32_t flags;
};

struct systemtime
{
	::std::uint_least16_t wYear;
	::std::uint_least16_t wMonth;
	::std::uint_least16_t wDayOfWeek;
	::std::uint_least16_t wDay;
	::std::uint_least16_t wHour;
	::std::uint_least16_t wMinute;
	::std::uint_least16_t wSecond;
	::std::uint_least16_t wMilliseconds;
};

struct time_zone_information
{
	::std::int_least32_t Bias;
	char16_t StandardName[32];
	systemtime StandardDate;
	::std::int_least32_t StandardBias;
	char16_t DaylightName[32];
	systemtime DaylightDate;
	::std::int_least32_t DaylightBias;
};

struct system_info
{
	union DUMMYUNIONNAMEU
	{
		::std::uint_least32_t dwOemId; // Obsolete field...do not use

		struct DUMMYSTRUCTNAMET
		{
			::std::uint_least16_t wProcessorArchitecture;
			::std::uint_least16_t wReserved;
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;

	::std::uint_least32_t dwPageSize;
	void *lpMinimumApplicationAddress;
	void *lpMaximumApplicationAddress;
	::std::size_t dwActiveProcessorMask;
	::std::uint_least32_t dwNumberOfProcessors;
	::std::uint_least32_t dwProcessorType;
	::std::uint_least32_t dwAllocationGranularity;
	::std::uint_least16_t wProcessorLevel;
	::std::uint_least16_t wProcessorRevision;
};

struct file_basic_info
{
	::std::int_least64_t CreationTime;
	::std::int_least64_t LastAccessTime;
	::std::int_least64_t LastWriteTime;
	::std::int_least64_t ChangeTime;
	::std::uint_least32_t FileAttributes;
};

struct win32_find_dataa
{
	::std::uint_least32_t dwFileAttributes;
	filetime ftCreationTime;
	filetime ftLastAccessTime;
	filetime ftLastWriteTime;
	::std::uint_least32_t nFileSizeHigh;
	::std::uint_least32_t nFileSizeLow;
	::std::uint_least32_t dwReserved0;
	::std::uint_least32_t dwReserved1;
	char cFileName[260];
	char cAlternateFileName[14];
#ifdef _MAC
	::std::uint_least32_t dwFileType;
	::std::uint_least32_t dwCreatorType;
	::std::uint_least16_t wFinderFlags;
#endif
};

struct win32_find_dataw
{
	::std::uint_least32_t dwFileAttributes;
	filetime ftCreationTime;
	filetime ftLastAccessTime;
	filetime ftLastWriteTime;
	::std::uint_least32_t nFileSizeHigh;
	::std::uint_least32_t nFileSizeLow;
	::std::uint_least32_t dwReserved0;
	::std::uint_least32_t dwReserved1;
	char16_t cFileName[260];
	char16_t cAlternateFileName[14];
#if defined(_68K_) || defined(_MPPC_)
	::std::uint_least32_t dwFileType;
	::std::uint_least32_t dwCreatorType;
	::std::uint_least16_t wFinderFlags;
#endif
};

using farproc = ::std::ptrdiff_t(FAST_IO_STDCALL *)() noexcept;

struct win32_memory_range_entry
{
	void *VirtualAddress;
	::std::size_t NumberOfBytes;
};

inline constexpr ::std::size_t exception_maximum_parameters{15u};

struct exception_record
{
	::std::uint_least32_t ExceptionCode;
	::std::uint_least32_t ExceptionFlags;
	exception_record *ExceptionRecord;
	void *ExceptionAddress;
	::std::uint_least32_t NumberParameters;
	::std::size_t ExceptionInformation[exception_maximum_parameters];
};

struct exception_record32
{
	::std::uint_least32_t ExceptionCode;
	::std::uint_least32_t ExceptionFlags;
	::std::uint_least32_t ExceptionRecord;
	::std::uint_least32_t ExceptionAddress;
	::std::uint_least32_t NumberParameters;
	::std::uint_least32_t ExceptionInformation[exception_maximum_parameters];
};

struct exception_record64
{
	::std::uint_least32_t ExceptionCode;
	::std::uint_least32_t ExceptionFlags;
	::std::uint_least64_t ExceptionRecord;
	::std::uint_least64_t ExceptionAddress;
	::std::uint_least32_t NumberParameters;
	::std::uint_least32_t UnusedAlignment;
	::std::uint_least64_t ExceptionInformation[exception_maximum_parameters];
};

struct exception_pointers
{
	exception_record *ExceptionRecord;
	void *ContextRecord;
};

using pvectored_exception_handler = ::std::int_least32_t(FAST_IO_WINSTDCALL *)(exception_pointers *) noexcept;

using address_family = ::std::uint_least16_t;

inline constexpr ::std::size_t ss_maxsize{128u};
inline constexpr ::std::size_t ss_alignsize{sizeof(::std::int_least64_t)};
inline constexpr ::std::size_t ss_pad1size{ss_alignsize - sizeof(::std::uint_least16_t)};
inline constexpr ::std::size_t ss_pad2size{ss_maxsize - (sizeof(::std::uint_least16_t) + ss_pad1size + ss_alignsize)};

//
// Definitions used for sockaddr_storage structure paddings design.
//

struct sockaddr_storage
{
	address_family ss_family;        // address family
	char ss_pad1[ss_pad1size];       // 6 byte pad, this is to make
									 //   implementation specific pad up to
									 //   alignment field that follows explicit
									 //   in the data structure
	::std::int_least64_t __ss_align; // Field to force desired structure
	char ss_pad2[ss_pad2size];       // 112 byte pad to achieve desired size;
									 //   _SS_MAXSIZE value minus size of
									 //   ss_family, __ss_pad1, and
									 //   __ss_align fields is 112
};

struct in_addr
{
	union S_un_u
	{
		struct S_un_b_t
		{
			::std::uint_least8_t s_b1, s_b2, s_b3, s_b4;
		} S_un_b;
		struct S_un_w_t
		{
			::std::uint_least16_t s_w1, s_w2;
		} S_un_w;
		::std::uint_least32_t S_addr;
	} S_un;
};

struct sockaddr_in
{
	address_family sin_family;
	::std::uint_least16_t sin_port;
	in_addr sin_addr;
	char sin_zero[8];
};

struct in6_addr
{
	union u_u
	{
		::std::uint_least8_t Byte[16];
		::std::uint_least16_t Word[8];
	} u;
};

struct scope_id
{
	union DUMMYUNIONNAME_U
	{
		struct DUMMYSTRUCTNAME_T
		{
			::std::uint_least32_t Zone : 28;
			::std::uint_least32_t Level : 4;
		} DUMMYSTRUCTNAME;
		::std::uint_least32_t Value;
	} DUMMYUNIONNAME;
};

struct sockaddr_in6
{
	address_family sin6_family;          // AF_INET6.
	::std::uint_least16_t sin6_port;     // Transport level port number.
	::std::uint_least32_t sin6_flowinfo; // IPv6 flow information.
	in6_addr sin6_addr;                  // IPv6 address.
	union sin6_scope_u
	{
		::std::uint_least32_t sin6_scope_id; // Set of interfaces for a scope.
		scope_id sin6_scope_struct;
	} sin6_scope;
};

} // namespace fast_io::win32
