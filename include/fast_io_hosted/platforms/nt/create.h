#pragma once

namespace fast_io
{

/*
Referenced from Windows Internal Version 7
*/
enum class nt_family
{
	alpc,   // Alpc: Advanced Local Procedure Calls
	cc,     // Cc: Common Cache
	cm,     // Cm: Configuration manager
	dbg,    // Dbg: kernel debug support
	dbgk,   // Dbgk: Debugging Framework for user mode
	em,     // Em: Errata manager
	etw,    // Event: Event Tracing for Windows
	ex,     // Executive support routines
	fs_rtl, // FsRtl: File System Runtime Library
	hv,     // Hv: Hive library
	hvl,    // Hvl: Hypervisor Library
	io,     // Io: I/O manager
	kd,     // Kd: Kernel debugger
	ke,     // Ke: Kernel
	kse,    // Kse: Kernel Shim Engine
	ldr,    // Ldr: NT Loader
	lsa,    // Lsa: Local Security Authority
	mm,     // Mm: Memory manager
	nt,     // Nt: NT system services (accessible from user mode through system calls)
	ob,     // Ob: Object manager
	pf,     // Pf: Prefetcher
	po,     // Po: Power manager
	po_fx,  // PoFx: Power framework
	pp,     // Pp: PnP manager
	ppm,    // Ppm: Processor power manager
	ps,     // Ps: Process support
	rtl,    // Rtl: Run-time library
	se,     // Se: Security Reference Monitor
	sm,     // Sm: Store Manager
	tm,     // Tm: Transaction
	ttm,    // Ttm: Terminal timeout manager
	vf,     // Vf: Driver verifier
	vsl,    // Vsl: Virtual secure mode library
	wdi,    // Wdi: Windows Diagnostic Infrastructure
	wfp,    // Wfp: Windows FingerPrint
	whea,   // Whea: Windows Hardware Error Architecture
	wmi,    // Wmi: Windows Management Instrumentation
	zw      // Zw: Mirror entry point for system services (beginning with Nt) that sets previous access mode to kernel, which
			// eliminates parameter validation, because Nt system services validate parameters only if previous access mode
			// is user
};

namespace win32::nt::details
{
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline void check_nt_status(::std::uint_least32_t status)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if (status) [[unlikely]]
	{
		::fast_io::herbceptions::throws_nt_errc_with_value(status);
	}
}

struct nt_open_mode
{
	::std::uint_least32_t DesiredAccess{};
	::std::uint_least32_t FileAttributes{};
	::std::uint_least32_t ShareAccess{};
	::std::uint_least32_t CreateDisposition{};
	::std::uint_least32_t CreateOptions{};
	::std::uint_least32_t ObjAttributes{};
};

/*
https://docs.microsoft.com/en-us/windows/win32/secauthz/access-mask-format
*/

inline constexpr nt_open_mode calculate_nt_open_mode(open_mode_perms ompm) noexcept
{
	open_mode value{ompm.om};
	perms pm{ompm.pm};
	nt_open_mode mode;

	constexpr auto default_write_attribute{0x00020000L /*READ_CONTROL*/ | 0x0002 /*FILE_WRITE_DATA*/ | 0x0004 /*FILE_APPEND_DATA*/};
	constexpr auto default_read_attribute{0x00020000L /*READ_CONTROL*/ | 0x0001 /*FILE_READ_DATA*/};

	mode.DesiredAccess |= 0x00100000L /*SYNCHRONIZE*/ | 0x0080 /*FILE_READ_ATTRIBUTES*/ | 0x0100 /*FILE_WRITE_ATTRIBUTES*/;

	if ((value & open_mode::no_shared_read) == open_mode::none)
	{
		mode.ShareAccess |= 1; // FILE_SHARE_READ
	}
	if ((value & open_mode::no_shared_write) == open_mode::none)
	{
		mode.ShareAccess |= 2; // FILE_SHARE_WRITE
	}
	if ((value & open_mode::shared_delete) != open_mode::none)
	{
		mode.ShareAccess |= 4; // FILE_SHARE_DELETE
	}
	bool generic_write{};
	if ((value & open_mode::app) != open_mode::none)
	{
		mode.DesiredAccess |= 4; // FILE_APPEND_DATA
	}
	else if ((value & open_mode::out) != open_mode::none)
	{
		mode.DesiredAccess |= default_write_attribute;
		generic_write = true;
	}
	if (((value & open_mode::in) != open_mode::none) || ((value & open_mode::app) != open_mode::none))
	{
		mode.DesiredAccess |= default_read_attribute;
		if ((value & open_mode::out) != open_mode::none &&
			((value & open_mode::app) != open_mode::none && (value & open_mode::trunc) != open_mode::none))
		{
			mode.DesiredAccess |= default_write_attribute;
			generic_write = true;
		}
	}
	/*

	https://doxygen.reactos.org/d6/d0e/ndk_2iotypes_8h.html
	#define 	FILE_SUPERSEDE   0x00000000
	#define 	FILE_OPEN   0x00000001
	#define 	FILE_CREATE   0x00000002
	#define 	FILE_OPEN_IF   0x00000003
	#define 	FILE_OVERWRITE   0x00000004
	#define 	FILE_OVERWRITE_IF   0x00000005

	https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntcreatefile
	CreateDisposition value	Action if file exists	Action if file does not exist
	FILE_SUPERSEDE	Replace the file.	Create the file. 0x00000000
	FILE_OPEN	Open the file.	Return an error. 0x00000001
	FILE_CREATE	Return an error.	Create the file. 0x00000002
	FILE_OPEN_IF	Open the file.	Create the file. 0x00000003
	FILE_OVERWRITE	Open the file, and overwrite it.	Return an error. 0x00000004
	FILE_OVERWRITE_IF	Open the file, and overwrite it.	Create the file. 0x00000005


	https://doxygen.reactos.org/dd/d83/dll_2win32_2kernel32_2client_2file_2create_8c_source.html
	Line 88:

	win32		=>	nt
	CREATE_NEW	=>	FILE_CREATE		(0x00000002)
	CREATE_ALWAYS	=>	FILE_OVERWRITE_IF	(0x00000005)
	OPEN_EXISITNG	=>	FILE_OPEN		(0x00000001)
	OPEN_ALWAYS	=>	FILE_OPEN_IF		(0x00000003)
	TRUNCATING_EXISITING=>	FILE_OVERWRITE		(0x00000004)

	File access
	mode string	Meaning	Explanation	Action if file
	already exists	Action if file
	does not exist
	"r"	read	Open a file for reading	read from start	failure to open
	"w"	write	Create a file for writing	destroy contents	create new
	"a"	append	Append to a file	write to end	create new
	"r+"	read extended	Open a file for read/write	read from start	error
	"w+"	write extended	Create a file for read/write	destroy contents	create new
	"a+"	append extended	Open a file for read/write	write to end	create new
	*/
	if ((value & open_mode::trunc) != open_mode::none)
	{
		if ((value & open_mode::excl) != open_mode::none)
		{
			mode.CreateDisposition = 0x00000002; // CREATE_NEW	=>	FILE_CREATE		(0x00000002)
		}
		else
		{
			mode.CreateDisposition = 0x00000005; // CREATE_ALWAYS	=>	FILE_OVERWRITE_IF	(0x00000005)
		}
	}
	else if ((value & open_mode::in) == open_mode::none)
	{
		if ((value & open_mode::app) != open_mode::none)
		{
			mode.CreateDisposition = 0x00000003; // OPEN_ALWAYS	=>	FILE_OPEN_IF		(0x00000003)
		}
		else if ((value & open_mode::out) != open_mode::none)
		{
			if ((value & open_mode::excl) != open_mode::none)
			{
				mode.CreateDisposition = 0x00000002; // CREATE_NEW	=>	FILE_CREATE		(0x00000002)
			}
			else
			{
				mode.CreateDisposition = 0x00000005; // CREATE_ALWAYS	=>	FILE_OVERWRITE_IF	(0x00000005)
			}
		}
	}
	else if ((value & open_mode::app) != open_mode::none)
	{
		mode.CreateDisposition = 0x00000003; // OPEN_ALWAYS		=>	FILE_OPEN_IF		(0x00000003)
	}
	else
	{
		mode.CreateDisposition = 0x00000001; // OPEN_EXISTING	=>	FILE_OPEN		(0x00000001)
	}

	if ((value & open_mode::creat) != open_mode::none)
	{
		if ((value & open_mode::excl) != open_mode::none)
		{
			mode.CreateDisposition = 0x00000002; // CREATE_NEW	=>	FILE_CREATE		(0x00000002)
		}
		else
		{
			mode.CreateDisposition = 0x00000003; // OPEN_ALWAYS	=>	FILE_OPEN_IF		(0x00000003)
		}
	}
	if ((value & open_mode::direct) != open_mode::none)
	{
		mode.CreateOptions |= 0x00000008; // FILE_NO_INTERMEDIATE_BUFFERING
	}
	if ((value & open_mode::sync) != open_mode::none)
	{
		mode.CreateOptions |= 0x00000002; // FILE_WRITE_THROUGH
	}
	if ((value & open_mode::follow) == open_mode::none)
	{
		mode.CreateOptions |= 0x00200000; // FILE_FLAG_OPEN_REPARSE_POINT => FILE_OPEN_REPARSE_POINT (0x00200000)
	}
	bool set_normal{true};
	if ((value & open_mode::archive) != open_mode::none)
	{
		mode.FileAttributes |= 0x20; // FILE_ATTRIBUTE_ARCHIVE
		set_normal = {};
	}
	if ((value & open_mode::encrypted) != open_mode::none)
	{
		mode.FileAttributes |= 0x4000; // FILE_ATTRIBUTE_ENCRYPTED
		set_normal = {};
	}
	if ((value & open_mode::hidden) != open_mode::none)
	{
		mode.FileAttributes |= 0x2; // FILE_ATTRIBUTE_HIDDEN
		set_normal = {};
	}
	if ((value & open_mode::compressed) != open_mode::none)
	{
		mode.FileAttributes |= 0x800; // FILE_ATTRIBUTE_COMPRESSED
		set_normal = {};
	}
	if ((value & open_mode::system) != open_mode::none)
	{
		mode.FileAttributes |= 0x4; // FILE_ATTRIBUTE_SYSTEM
		set_normal = {};
	}
	if ((value & open_mode::offline) != open_mode::none)
	{
		mode.FileAttributes |= 0x1000; // FILE_ATTRIBUTE_OFFLINE
		set_normal = {};
	}
	if (set_normal) [[likely]]
	{
		mode.FileAttributes |= 0x80; // FILE_ATTRIBUTE_NORMAL
	}

	if ((value & open_mode::directory) == open_mode::none)
	{
		mode.CreateOptions |= 0x00000040; // FILE_NON_DIRECTORY_FILE 0x00000040
	}
	else
	{
		if (mode.CreateDisposition == 0)
		{
			mode.DesiredAccess |= default_read_attribute; // FILE_GENERIC_READ
			mode.CreateDisposition = 0x00000001;          // OPEN_EXISTING
		}
		mode.CreateOptions |= 0x00004000; // FILE_OPEN_FOR_BACKUP_INTENT
		mode.CreateOptions |= 0x00000001; // FILE_DIRECTORY_FILE
		if (generic_write)
		{
			mode.CreateOptions |= 0x00000400; // FILE_OPEN_REMOTE_INSTANCE
		}
		if ((value & open_mode::creat) != open_mode::none)
		{
			mode.DesiredAccess |= default_write_attribute | default_read_attribute; // GENERIC_READ | GENERIC_WRITE
		}
	}

	if ((value & open_mode::no_block) == open_mode::none)
	{
		mode.CreateOptions |= 0x00000020; // FILE_SYNCHRONOUS_IO_NONALERT 0x00000020
	}
#if 0
	else
	{
		mode.CreateOptions |= 0x00000010; // FILE_SYNCHRONOUS_IO_ALERT 0x00000010
	}
#endif

	if ((value & open_mode::random_access) == open_mode::none)
	{
		if ((value & open_mode::directory) == open_mode::none)
		{
			mode.CreateOptions |= 0x00000004; // FILE_SEQUENTIAL_ONLY 0x00000004
		}
	}
	else
	{
		mode.CreateOptions |= 0x00000800;
	}
	if ((value & open_mode::no_recall) != open_mode::none)
	{
		mode.CreateOptions |= 0x00400000; // FILE_OPEN_NO_RECALL 0x00400000
	}
	if ((value & open_mode::case_insensitive) != open_mode::none)
	{
		mode.ObjAttributes |= 0x00000040; // OBJ_CASE_INSENSITIVE
	}
	if ((value & open_mode::inherit) != open_mode::none)
	{
		mode.ObjAttributes |= 0x00000002; // OBJ_INHERIT
	}

	if ((value & open_mode::session_aware) != open_mode::none)
	{
		mode.CreateOptions |= 0x00040000; // FILE_SESSION_AWARE
	}
	if ((value & open_mode::temporary) != open_mode::none)
	{
		mode.CreateOptions |= 0x00001000; // FILE_DELETE_ON_CLOSE
		mode.FileAttributes |= 0x100;     // FILE_ATTRIBUTE_TEMPORARY??
		mode.DesiredAccess |= 0x00010000; // DELETE
	}
	if ((pm & perms::owner_write) == perms::none)
	{
		mode.FileAttributes |= 0x00000001; // FILE_ATTRIBUTE_READONLY
	}
	return mode;
}

template <bool zw>
inline void *nt_create_file_common(void *directory, ::fast_io::win32::nt::unicode_string *relative_path, nt_open_mode const &mode) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::security_attributes sec_attr{sizeof(::fast_io::win32::security_attributes), nullptr, true};
	::fast_io::win32::nt::object_attributes obj{.Length = sizeof(::fast_io::win32::nt::object_attributes),
												.RootDirectory = directory,
												.ObjectName = relative_path,
												.Attributes = mode.ObjAttributes,
												.SecurityDescriptor =
													mode.ObjAttributes & 0x00000002 ? __builtin_addressof(sec_attr) : nullptr,
												.SecurityQualityOfService = nullptr};
	void *handle;
	::fast_io::win32::nt::io_status_block block;
	auto const status{::fast_io::win32::nt::nt_create_file<zw>(
		__builtin_addressof(handle), mode.DesiredAccess, __builtin_addressof(obj), __builtin_addressof(block), nullptr,
		mode.FileAttributes, mode.ShareAccess, mode.CreateDisposition, mode.CreateOptions, nullptr, 0u)};
	if (status)
	{
		::fast_io::herbceptions::throws_nt_errc_with_value(status);
	}
	return handle;
}

template <bool zw>
struct nt_create_callback
{
	nt_open_mode const &mode;
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline void *operator()(void *directory_handle, ::fast_io::win32::nt::unicode_string *relative_path) const FAST_IO_HERBCEPTIONS_THROWS
	{
		return nt_create_file_common<zw>(directory_handle, relative_path, mode); // get rid of this pointer
	}
};

template <bool zw>
inline void *nt_family_create_file_impl(char16_t const *filename_cstr, open_mode_perms ompm) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::nt::details::nt_call_invoke_without_directory_handle_impl(
		filename_cstr, nt_create_callback<zw>{::fast_io::win32::nt::details::calculate_nt_open_mode(ompm)});
}

template <bool zw>
struct nt_family_open_file_parameter
{
	open_mode_perms ompm{};
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline void *operator()(char16_t const *filename_cstr)
		FAST_IO_HERBCEPTIONS_THROWS
	{
		return nt_family_create_file_impl<zw>(filename_cstr, ompm);
	}
};

template <bool zw, typename T>
	requires(::fast_io::constructible_to_os_c_str<T>)
inline void *nt_create_file_impl(T const &t, open_mode_perms ompm) FAST_IO_HERBCEPTIONS_THROWS
{
	return nt_api_common(t, nt_family_open_file_parameter<zw>{ompm});
}

template <bool zw, bool kernel>
inline void *nt_family_create_file_at_impl(void *directory_handle, char16_t const *filename_c_str,
										   ::std::size_t filename_c_str_len, open_mode_perms md) FAST_IO_HERBCEPTIONS_THROWS
{
	if constexpr (kernel)
	{
		return ::fast_io::win32::nt::details::nt_call_kernel_callback(
			directory_handle, filename_c_str, filename_c_str_len,
			nt_create_callback<zw>{::fast_io::win32::nt::details::calculate_nt_open_mode(md)});
	}
	else
	{
		return ::fast_io::win32::nt::details::nt_call_callback(
			directory_handle, filename_c_str, filename_c_str_len,
			nt_create_callback<zw>{::fast_io::win32::nt::details::calculate_nt_open_mode(md)});
	}
}

template <bool zw>
inline void *nt_family_create_file_fs_dirent_impl(void *directory_handle, char16_t const *filename_c_str,
												  ::std::size_t filename_c_str_len, open_mode_perms md) FAST_IO_HERBCEPTIONS_THROWS
{
	using char16_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char16_t const *;
	return ::fast_io::win32::nt::details::nt_call_kernel_fs_dirent_callback(
		directory_handle, reinterpret_cast<char16_may_alias_const_ptr>(filename_c_str), filename_c_str_len,
		nt_create_callback<zw>{::fast_io::win32::nt::details::calculate_nt_open_mode(md)});
}

template <bool zw>
inline void *nt_family_create_file_kernel_impl(char16_t const *filename_cstr, ::std::size_t filename_c_str_len,
											   open_mode_perms ompm) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::nt::details::nt_call_kernel_nodir_callback(
		filename_cstr, filename_c_str_len,
		nt_create_callback<zw>{::fast_io::win32::nt::details::calculate_nt_open_mode(ompm)});
}

template <bool zw>
struct nt_family_open_file_kernel_parameter
{
	open_mode_perms ompm{};
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline void *operator()(char16_t const *filename_cstr, ::std::size_t filename_c_str_len)
		FAST_IO_HERBCEPTIONS_THROWS
	{
		return nt_family_create_file_kernel_impl<zw>(filename_cstr, filename_c_str_len, ompm);
	}
};

template <bool zw, typename T>
	requires(::fast_io::constructible_to_os_c_str<T>)
inline void *nt_create_file_kernel_impl(T const &t, open_mode_perms op) FAST_IO_HERBCEPTIONS_THROWS
{
	return nt_api_common(t, nt_family_open_file_kernel_parameter<zw>{op});
}

template <bool zw, bool kernel>
struct nt_family_open_file_at_parameter
{
	void *directory_handle{};
	open_mode_perms ompm{};
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline void *operator()(char16_t const *filename_cstr, ::std::size_t filename_c_str_len)
		FAST_IO_HERBCEPTIONS_THROWS
	{
		return nt_family_create_file_at_impl<zw, kernel>(directory_handle, filename_cstr, filename_c_str_len, ompm);
	}
};

template <bool zw, bool kernel = false, typename T>
	requires(::fast_io::constructible_to_os_c_str<T>)
inline void *nt_create_file_at_impl(void *directory_handle, T const &t, open_mode_perms op) FAST_IO_HERBCEPTIONS_THROWS
{
	return nt_api_common(t, nt_family_open_file_at_parameter<zw, kernel>{directory_handle, op});
}

} // namespace win32::nt::details

} // namespace fast_io
