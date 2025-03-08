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
{
	if (status) [[unlikely]]
	{
		throw_nt_error(status);
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

	mode.DesiredAccess |= 0x00100000L /*SYNCHRONIZE*/ | 0x0080 /*FILE_READ_ATTRIBUTES*/;

	if ((value & open_mode::no_shared_read) == open_mode::none)
	{
		mode.ShareAccess |= 1; // FILE_SHARE_READ
	}
	if ((value & open_mode::no_shared_write) == open_mode::none)
	{
		mode.ShareAccess |= 2; // FILE_SHARE_DELETE
	}
	if ((value & open_mode::shared_delete) != open_mode::none)
	{
		mode.ShareAccess |= 4; // FILE_SHARE_WRITE
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
	if ((value & open_mode::follow) != open_mode::none)
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
	else
	{
		mode.CreateOptions |= 0x00000010; // FILE_SYNCHRONOUS_IO_ALERT 0x00000010
	}

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
inline void *nt_create_file_common(void *directory, ::fast_io::win32::nt::unicode_string *relative_path, nt_open_mode const &mode)
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
		throw_nt_error(status);
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
	inline void *operator()(void *directory_handle, ::fast_io::win32::nt::unicode_string *relative_path) const
	{
		return nt_create_file_common<zw>(directory_handle, relative_path, mode); // get rid of this pointer
	}
};

template <bool zw>
inline void *nt_family_create_file_impl(char16_t const *filename_cstr, open_mode_perms ompm)
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
	{
		return nt_family_create_file_impl<zw>(filename_cstr, ompm);
	}
};

template <bool zw, typename T>
	requires(::fast_io::constructible_to_os_c_str<T>)
inline void *nt_create_file_impl(T const &t, open_mode_perms ompm)
{
	return nt_api_common(t, nt_family_open_file_parameter<zw>{ompm});
}

template <bool zw, bool kernel>
inline void *nt_family_create_file_at_impl(void *directory_handle, char16_t const *filename_c_str,
										   ::std::size_t filename_c_str_len, open_mode_perms md)
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
												  ::std::size_t filename_c_str_len, open_mode_perms md)
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
											   open_mode_perms ompm)
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
	{
		return nt_family_create_file_kernel_impl<zw>(filename_cstr, filename_c_str_len, ompm);
	}
};

template <bool zw, typename T>
	requires(::fast_io::constructible_to_os_c_str<T>)
inline void *nt_create_file_kernel_impl(T const &t, open_mode_perms op)
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
	{
		return nt_family_create_file_at_impl<zw, kernel>(directory_handle, filename_cstr, filename_c_str_len, ompm);
	}
};

template <bool zw, bool kernel = false, typename T>
	requires(::fast_io::constructible_to_os_c_str<T>)
inline void *nt_create_file_at_impl(void *directory_handle, T const &t, open_mode_perms op)
{
	return nt_api_common(t, nt_family_open_file_at_parameter<zw, kernel>{directory_handle, op});
}

template <::fast_io::nt_family family>
inline ::std::uint_least64_t nt_calculate_current_file_offset(void *__restrict handle)
{
	::fast_io::win32::nt::io_status_block block;
	::std::uint_least64_t fps{};
	auto status{::fast_io::win32::nt::nt_query_information_file<family == ::fast_io::nt_family::zw>(handle, __builtin_addressof(block), __builtin_addressof(fps),
																									static_cast<::std::uint_least32_t>(sizeof(::std::uint_least64_t)),
																									::fast_io::win32::nt::file_information_class::FilePositionInformation)};
	if (status)
	{
		throw_nt_error(status);
	}
	return fps;
}

inline ::std::int_least64_t nt_calculate_offset_impl(::fast_io::intfpos_t off)
{
	if constexpr (sizeof(::fast_io::intfpos_t) > sizeof(::std::int_least64_t))
	{
		[[maybe_unused]] constexpr ::std::int_least64_t mn{::std::numeric_limits<::std::int_least64_t>::min()};
		constexpr ::std::int_least64_t mx{::std::numeric_limits<::std::int_least64_t>::max()};
		if (off < 0 || off > mx)
		{
			throw_nt_error(0xC0000106);
		}
	}
	return static_cast<::std::int_least64_t>(off);
}

template <nt_family family>
inline ::std::byte *nt_read_pread_some_bytes_common_impl(void *__restrict handle, ::std::byte *first, ::std::byte *last,
														 ::std::int_least64_t *pbyteoffset)
{
	// some poeple in zwclose7 forum said we do not need to initialize io_status_block
	::fast_io::win32::nt::io_status_block block;
	auto const status{::fast_io::win32::nt::nt_read_file<family == ::fast_io::nt_family::zw>(handle, nullptr, nullptr, nullptr, __builtin_addressof(block), first,
																							 ::fast_io::details::read_write_bytes_compute<::std::uint_least32_t>(first, last), pbyteoffset, nullptr)};
	if (status) [[unlikely]]
	{
		if (status == 0xC0000011 /*file*/ || status == 0xC000014B /*pipe*/) [[likely]]
		{
			return first;
		}
		throw_nt_error(status);
	}
	return first + block.Information;
}

template <nt_family family>
inline ::std::byte *nt_read_some_bytes_impl(void *__restrict handle, ::std::byte *first, ::std::byte *last)
{
	return ::fast_io::win32::nt::details::nt_read_pread_some_bytes_common_impl<family>(handle, first, last, nullptr);
}

template <nt_family family>
inline ::std::byte *nt_pread_some_bytes_impl(void *__restrict handle, ::std::byte *first, ::std::byte *last,
											 ::fast_io::intfpos_t off)
{
	::std::int_least64_t offs{nt_calculate_offset_impl(off)};
	return ::fast_io::win32::nt::details::nt_read_pread_some_bytes_common_impl<family>(handle, first, last,
																					   __builtin_addressof(offs));
}

template <nt_family family>
inline ::std::byte const *nt_write_pwrite_some_bytes_common_impl(void *__restrict handle, ::std::byte const *first,
																 ::std::byte const *last,
																 ::std::int_least64_t *pbyteoffset)
{
	::fast_io::win32::nt::io_status_block block;
	auto const status{::fast_io::win32::nt::nt_write_file<family == ::fast_io::nt_family::zw>(handle, nullptr, nullptr, nullptr, __builtin_addressof(block), first,
																							  ::fast_io::details::read_write_bytes_compute<::std::uint_least32_t>(first, last), pbyteoffset, nullptr)};
	if (status)
	{
		throw_nt_error(status);
	}
	return first + block.Information;
}

template <nt_family family>
inline ::std::byte const *nt_write_some_bytes_impl(void *__restrict handle, ::std::byte const *first,
												   ::std::byte const *last)
{
	return ::fast_io::win32::nt::details::nt_write_pwrite_some_bytes_common_impl<family>(handle, first, last, nullptr);
}

template <nt_family family>
inline ::std::byte const *nt_pwrite_some_bytes_impl(void *__restrict handle, ::std::byte const *first,
													::std::byte const *last, ::fast_io::intfpos_t off)
{
	::std::int_least64_t offs{nt_calculate_offset_impl(off)};
	return ::fast_io::win32::nt::details::nt_write_pwrite_some_bytes_common_impl<family>(handle, first, last,
																						 __builtin_addressof(offs));
}

} // namespace win32::nt::details

namespace details
{
#if !defined(__WINE__)
inline void *my_get_osfile_handle(int fd) noexcept;
#endif
} // namespace details

struct nt_at_entry
{
	using native_handle_type = void *;
	void *handle{};
	inline explicit constexpr nt_at_entry() noexcept = default;
	inline explicit constexpr nt_at_entry(void *mhandle) noexcept
		: handle(mhandle)
	{}
#if !defined(__WINE__)
	inline nt_at_entry(posix_at_entry pate) noexcept
		: handle(details::my_get_osfile_handle(pate.fd))
	{}
#endif
};

#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline nt_at_entry nt_at_fdcwd() noexcept
{
	constexpr ::std::ptrdiff_t value{-3}; // use -3 as at_fdwcd handle
	return nt_at_entry{bit_cast<void *>(value)};
}

#if !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__) && !defined(_WIN32_WINDOWS)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline nt_at_entry at_fdcwd() noexcept
{
	return nt_at_fdcwd();
}
#endif

template <nt_family family>
struct nt_family_at_entry : nt_at_entry
{
	using nt_at_entry::native_handle_type;
	inline explicit constexpr nt_family_at_entry() noexcept = default;
	inline explicit constexpr nt_family_at_entry(void *mhandle) noexcept
		: nt_at_entry{mhandle}
	{}
};

struct nt_fs_dirent
{
	void *handle{};
	::fast_io::manipulators::basic_os_c_str_with_known_size<char16_t> filename{};
};

using zw_at_entry = nt_family_at_entry<nt_family::zw>;

template <win32_family family, ::std::integral ch_type>
class basic_win32_family_io_observer;

template <nt_family family, ::std::integral ch_type>
	requires(family == nt_family::nt || family == nt_family::zw)
class basic_nt_family_io_observer
{
public:
	using native_handle_type = void *;
	using char_type = ch_type;
	using input_char_type = ch_type;
	using output_char_type = ch_type;
	native_handle_type handle{};
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return handle;
	}
	inline explicit constexpr operator bool() const noexcept
	{
		return handle != nullptr;
	}
	template <win32_family family2>
	inline explicit operator basic_win32_family_io_observer<family2, char_type>() const noexcept
	{
		return basic_win32_family_io_observer<family2, char_type>{reinterpret_cast<void *>(handle)};
	}
	template <nt_family family2>
	inline constexpr operator basic_nt_family_io_observer<family2, char_type>() const noexcept
	{
		return basic_nt_family_io_observer<family2, char_type>{handle};
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{handle};
		handle = nullptr;
		return temp;
	}
};

template <nt_family family, ::std::integral ch_type>
inline ::std::byte *read_some_bytes_underflow_define(basic_nt_family_io_observer<family, ch_type> niob,
													 ::std::byte *first, ::std::byte *last)
{
	return ::fast_io::win32::nt::details::nt_read_some_bytes_impl<family>(niob.handle, first, last);
}

template <nt_family family, ::std::integral ch_type>
inline ::std::byte const *write_some_bytes_overflow_define(basic_nt_family_io_observer<family, ch_type> niob,
														   ::std::byte const *first, ::std::byte const *last)
{
	return ::fast_io::win32::nt::details::nt_write_some_bytes_impl<family>(niob.handle, first, last);
}

template <nt_family family, ::std::integral ch_type>
inline ::std::byte *pread_some_bytes_underflow_define(basic_nt_family_io_observer<family, ch_type> niob,
													  ::std::byte *first, ::std::byte *last, ::fast_io::intfpos_t off)
{
	return ::fast_io::win32::nt::details::nt_pread_some_bytes_impl<family>(niob.handle, first, last, off);
}

template <nt_family family, ::std::integral ch_type>
inline ::std::byte const *pwrite_some_bytes_overflow_define(basic_nt_family_io_observer<family, ch_type> niob,
															::std::byte const *first, ::std::byte const *last,
															::fast_io::intfpos_t off)
{
	return ::fast_io::win32::nt::details::nt_pwrite_some_bytes_impl<family>(niob.handle, first, last, off);
}

template <nt_family family, ::std::integral ch_type>
inline constexpr bool operator==(basic_nt_family_io_observer<family, ch_type> a,
								 basic_nt_family_io_observer<family, ch_type> b) noexcept
{
	return a.handle == b.handle;
}

#if __cpp_impl_three_way_comparison >= 201907L
template <nt_family family, ::std::integral ch_type>
inline constexpr auto operator<=>(basic_nt_family_io_observer<family, ch_type> a,
								  basic_nt_family_io_observer<family, ch_type> b) noexcept
{
	return a.handle <=> b.handle;
}
#endif

template <nt_family family, ::std::integral ch_type>
inline constexpr auto at(basic_nt_family_io_observer<family, ch_type> niob) noexcept
{
	if constexpr (family == nt_family::nt)
	{
		return nt_at_entry{niob.handle};
	}
	else
	{
		return nt_family_at_entry<family>{niob.handle};
	}
}

template <nt_family family, ::std::integral ch_type>
inline constexpr basic_nt_family_io_observer<family, ch_type>
io_stream_ref_define(basic_nt_family_io_observer<family, ch_type> other) noexcept
{
	return other;
}

template <nt_family family, ::std::integral ch_type>
inline constexpr basic_nt_family_io_observer<family, char>
io_bytes_stream_ref_define(basic_nt_family_io_observer<family, ch_type> other) noexcept
{
	return {other.handle};
}

namespace win32::nt::details
{

template <bool zw>
inline void nt_flush_impl(void *handle)
{
	::fast_io::win32::nt::io_status_block block;
	::std::uint_least32_t status{::fast_io::win32::nt::nt_flush_buffers_file<zw>(handle, __builtin_addressof(block))};
	if (status)
	{
		throw_nt_error(status);
	}
}

template <bool zw>
inline void nt_data_sync_impl(void *handle, data_sync_flags flags [[maybe_unused]])
{
#if (!defined(_WIN32_WINNT) || _WIN32_WINNT >= 0x0602) || WINVER >= 0x0602
	/*
	NtFlushBuffersFileEx and ZwFlushBuffersFileEx are only provided since windows 8
	*/
	::fast_io::win32::nt::io_status_block block;
	::std::uint_least32_t status{::fast_io::win32::nt::nt_flush_buffers_file_ex<zw>(
		handle, static_cast<::std::uint_least32_t>(flags), nullptr, 0u, __builtin_addressof(block))};
	if (status)
	{
		throw_nt_error(status);
	}
#else
	nt_flush_impl<zw>(handle);
#endif
}

} // namespace win32::nt::details

template <nt_family family, ::std::integral ch_type>
inline void flush(basic_nt_family_io_observer<family, ch_type> ntiob)
{
	::fast_io::win32::nt::details::nt_flush_impl<family == nt_family::zw>(ntiob.handle);
}

template <nt_family family, ::std::integral ch_type>
inline void data_sync(basic_nt_family_io_observer<family, ch_type> ntiob, data_sync_flags flags)
{
	::fast_io::win32::nt::details::nt_data_sync_impl<family == nt_family::zw>(ntiob.handle, flags);
}

namespace win32::nt::details
{
struct nt_file_position_status
{
	::std::uint_least32_t status;
	::std::int_least64_t file_position;
};

template <bool zw>
inline nt_file_position_status nt_get_file_position_impl(void *__restrict handle, ::std::int_least64_t offset,
														 seekdir s)
{
	::std::uint_least64_t file_position{static_cast<::std::uint_least64_t>(offset)};
	::fast_io::win32::nt::io_status_block block;
	switch (s)
	{
	case seekdir::cur:
	{
		::std::uint_least64_t fps{};
		auto status{::fast_io::win32::nt::nt_query_information_file<zw>(
			handle, __builtin_addressof(block), __builtin_addressof(fps),
			static_cast<::std::uint_least32_t>(sizeof(::std::uint_least64_t)),
			::fast_io::win32::nt::file_information_class::FilePositionInformation)};
		if (status)
		{
			return {status};
		}
		file_position += fps;
	}
	break;
	case seekdir::end:
	{
		::fast_io::win32::nt::file_standard_information fsi;
		auto status{::fast_io::win32::nt::nt_query_information_file<zw>(
			handle, __builtin_addressof(block), __builtin_addressof(fsi),
			static_cast<::std::uint_least32_t>(sizeof(::fast_io::win32::nt::file_standard_information)),
			::fast_io::win32::nt::file_information_class::FileStandardInformation)};
		if (status)
		{
			return {status};
		}
		file_position += fsi.end_of_file;
	}
	break;
	default:;
	}
	if (static_cast<::std::uint_least64_t>(::std::numeric_limits<::std::int_least64_t>::max()) < file_position)
	{
		file_position = 0;
	}
	return {0, static_cast<::std::int_least64_t>(file_position)};
}

template <bool zw>
inline ::std::int_least64_t nt_seek64_impl(void *__restrict handle, ::std::int_least64_t offset, seekdir s)
{
	auto [status, file_position] = nt_get_file_position_impl<zw>(handle, offset, s);
	if (status)
	{
		throw_nt_error(status);
	}
	::fast_io::win32::nt::io_status_block block;
	status = ::fast_io::win32::nt::nt_set_information_file<zw>(handle, __builtin_addressof(block),
															   __builtin_addressof(file_position), sizeof(::std::uint_least64_t),
															   ::fast_io::win32::nt::file_information_class::FilePositionInformation);
	if (status)
	{
		throw_nt_error(status);
	}
	return file_position;
}

template <bool zw>
inline ::fast_io::intfpos_t nt_seek_impl(void *__restrict handle, ::fast_io::intfpos_t offset, seekdir s)
{
	return static_cast<::fast_io::intfpos_t>(nt_seek64_impl<zw>(handle, static_cast<::std::int_least64_t>(offset), s));
}

template <bool zw>
inline void *nt_dup_impl(void *handle)
{
	void *current_process{reinterpret_cast<void *>(static_cast<::std::ptrdiff_t>(-1))};
	void *new_handle{};
	auto status{::fast_io::win32::nt::nt_duplicate_object<zw>(current_process, handle, current_process,
															  __builtin_addressof(new_handle), 0, 0x00000002L, 2)};
	if (status)
	{
		throw_nt_error(status);
	}
	return new_handle;
}

template <bool zw>
inline void *nt_dup2_impl(void *handle, void *newhandle)
{
	auto temp{nt_dup_impl<zw>(handle)};
	if (newhandle) [[likely]]
	{
		::fast_io::win32::nt::nt_close<zw>(newhandle);
	}
	return temp;
}

template <bool zw>
inline void nt_truncate_impl(void *handle, ::std::uintmax_t newfilesizem)
{
	::std::uint_least64_t newfilesize{static_cast<::std::uint_least64_t>(newfilesizem)};
	::fast_io::win32::nt::io_status_block block;
	auto status{::fast_io::win32::nt::nt_set_information_file<zw>(handle, __builtin_addressof(block),
																  __builtin_addressof(newfilesize), sizeof(::std::uint_least64_t),
																  ::fast_io::win32::nt::file_information_class::FileEndOfFileInformation)};
	if (status)
	{
		throw_nt_error(status);
	}
}

template <bool zw>
inline ::std::uint_least32_t nt_family_file_lock_common_impl(void *__restrict handle, flock_request_l64 &req,
															 bool failedimmediately)
{
	auto [status, file_position] = nt_get_file_position_impl<zw>(handle, req.start, req.whence);
	if (status)
	{
		return status;
	}
	::fast_io::win32::nt::io_status_block block;
	::std::int_least64_t len{req.len};
	if (len == 0)
	{
		len = INT_LEAST64_MAX;
	}
	req.whence = seekdir::beg;
	req.start = file_position;
	req.len = len;
	if (req.type == file_lock_mode::unlock)
	{
		status = ::fast_io::win32::nt::nt_unlock_file<zw>(handle, __builtin_addressof(block), __builtin_addressof(file_position),
														  __builtin_addressof(len), 0u);
	}
	else
	{
		status = ::fast_io::win32::nt::nt_lock_file<zw>(handle, nullptr, nullptr, nullptr, __builtin_addressof(block),
														__builtin_addressof(file_position), __builtin_addressof(len), 0u,
														failedimmediately, req.type != file_lock_mode::shared_lock);
	}
	return status;
}

template <bool zw>
inline void nt_family_file_unlock_common_impl(void *__restrict handle, flock_request_l64 &req) noexcept
{
	auto [status, file_position] = nt_get_file_position_impl<zw>(handle, req.start, req.whence);
	::std::int_least64_t len{req.len};
	if (status)
	{
		file_position = 0;
		len = 0;
	}
	if (len == 0)
	{
		len = INT_LEAST64_MAX;
	}
	req.whence = seekdir::beg;
	req.start = file_position;
	req.len = len;
	::fast_io::win32::nt::io_status_block block;
	::fast_io::win32::nt::nt_unlock_file<zw>(handle, __builtin_addressof(block), __builtin_addressof(file_position),
											 __builtin_addressof(len), 0u);
}

template <bool zw>
inline void nt_family_file_lock_common_impl(void *__restrict handle, flock_request_l64 &req)
{
	auto status{nt_family_file_lock_common_impl<zw>(handle, req, false)};
	if (status)
	{
		throw_nt_error(status);
	}
}

template <bool zw, ::std::integral int_type>
inline void nt_family_file_lock_impl(void *__restrict handle, basic_flock_request<int_type> &__restrict t)
{
	if constexpr (sizeof(int_type) >= sizeof(::std::int_least64_t))
	{
		constexpr int_type mn{::std::numeric_limits<::std::int_least64_t>::min()};
		constexpr int_type mx{::std::numeric_limits<::std::int_least64_t>::max()};
		if (t.start < mn || mx < t.start || t.len < mn || mx < t.len)
		{
			throw_nt_error(0xC0000095); // STATUS_INTEGER_OVERFLOW
		}
	}
	nt_family_file_lock_common_impl<zw>(handle, t);
}

template <bool zw, ::std::integral int_type>
inline void nt_family_file_unlock_impl(void *__restrict handle, basic_flock_request<int_type> &__restrict t)
{
	if constexpr (sizeof(int_type) >= sizeof(::std::int_least64_t))
	{
		constexpr int_type mn{::std::numeric_limits<::std::int_least64_t>::min()};
		constexpr int_type mx{::std::numeric_limits<::std::int_least64_t>::max()};
		if (t.start < mn || mx < t.start || t.len < mn || mx < t.len)
		{
			fast_terminate();
		}
	}
	nt_family_file_unlock_common_impl<zw>(handle, t);
}

template <bool zw, ::std::integral int_type>
inline bool nt_family_file_try_lock_impl(void *__restrict handle, basic_flock_request<int_type> &__restrict t)
{
	if constexpr (sizeof(int_type) >= sizeof(::std::int_least64_t))
	{
		constexpr int_type mn{::std::numeric_limits<::std::int_least64_t>::min()};
		constexpr int_type mx{::std::numeric_limits<::std::int_least64_t>::max()};
		if (t.start < mn || mx < t.start || t.len < mn || mx < t.len)
		{
			return false;
		}
	}
	if constexpr (::std::same_as<int_type, ::std::int_least64_t>)
	{
		return nt_family_file_lock_common_impl<zw>(handle, t, true) == 0;
	}
	else
	{
		static_assert(::std::same_as<int_type, ::std::int_least64_t>);
		return false;
#if 0
		return nt_family_file_lock_common_impl<zw>(handle,flock_request_l64{t.type,t.whence,static_cast<::std::int_least64_t>(t.start),static_cast<::std::int_least64_t>(t.len)},true)==0;
#endif
	}
}

} // namespace win32::nt::details

template <nt_family family, ::std::integral ch_type>
inline ::fast_io::intfpos_t io_stream_seek_bytes_define(basic_nt_family_io_observer<family, ch_type> handle,
														::fast_io::intfpos_t offset, seekdir s)
{
	return ::fast_io::win32::nt::details::nt_seek_impl<family == nt_family::zw>(handle.handle, offset, s);
}

template <nt_family family, ::std::integral ch_type>
inline void truncate(basic_nt_family_io_observer<family, ch_type> handle, ::std::uintmax_t newfilesize)
{
	::fast_io::win32::nt::details::nt_truncate_impl<family == nt_family::zw>(handle.handle, newfilesize);
}

namespace win32::nt::details
{
inline constexpr unix_timestamp to_unix_timestamp(::std::uint_least64_t date_time) noexcept
{
	/*
	116444736000000000
	18446744073709551616
	 999999999
	1000000000
	*/

	constexpr ::std::uint_least64_t gap{11644473600000ULL * 10000ULL};
	constexpr ::std::uint_least64_t mul_factor{uint_least64_subseconds_per_second / 10000000u};
	::std::uint_least64_t unix_time{date_time - gap};
	if (date_time < gap) [[unlikely]]
	{
		unix_time = 0;
	}
	return {static_cast<::std::int_least64_t>(unix_time / 10000000ULL),
			static_cast<::std::uint_least64_t>(unix_time % 10000000ULL) * mul_factor};
}

inline constexpr file_type file_type_impl(::std::uint_least32_t DeviceType) noexcept
{
	if (DeviceType > 0x2B)
	{
		if (DeviceType != 80) [[unlikely]]
		{
			return file_type::unknown;
		}
		return file_type::character;
	}
	if (DeviceType == 43 || DeviceType == 21)
	{
		return file_type::character;
	}
	if (DeviceType > 0x15)
	{
		if (DeviceType > 0x1D)
		{
			if (DeviceType != 31 && DeviceType != 32 && DeviceType != 33 && DeviceType != 34 && DeviceType == 36) [[likely]]
			{
				return file_type::regular;
			}
			return file_type::unknown;
		}
		switch (DeviceType)
		{
		case 0x1D:
			[[fallthrough]];
		case 0x16:
			return file_type::character;
		case 0x17:
			[[unlikely]] return file_type::unknown;
		case 0x18:
			[[fallthrough]];
		case 0x1B:
			[[fallthrough]];
		case 0x28:
			return file_type::character;
		default:
			[[unlikely]] return file_type::unknown;
		}
	}
	if (DeviceType > 8)
	{
		if (DeviceType != 11 && DeviceType != 15)
		{
			if (DeviceType == 17) [[likely]]
			{
				return file_type::fifo;
			}
			return file_type::unknown;
		}
		return file_type::character;
	}
	return file_type::regular;
}

template <nt_family family>
inline posix_file_status nt_status_impl(void *__restrict handle)
{
	::fast_io::win32::nt::io_status_block isb;

	::fast_io::win32::nt::file_fs_device_type ffdt;
	auto status{::fast_io::win32::nt::nt_query_volume_information_file<family == ::fast_io::nt_family::zw>(handle, __builtin_addressof(isb), __builtin_addressof(ffdt),
																										   static_cast<::std::uint_least32_t>(sizeof(ffdt)),
																										   ::fast_io::win32::nt::fs_information_class::FileFsDeviceInformation)};
	if (status) [[unlikely]]
	{
		throw_nt_error(status);
	}

	auto ft{file_type_impl(ffdt.DeviceType)};

	if (ft == file_type::fifo || ft == file_type::character)
	{
		return posix_file_status{0,
								 0,
								 static_cast<perms>(436),
								 ft,
								 1,
								 0,
								 0,
								 static_cast<::std::uintmax_t>(reinterpret_cast<::std::size_t>(handle)),
								 0,
								 131072,
								 0,
								 {},
								 {},
								 {},
								 {},
								 0,
								 0};
	}

	::fast_io::win32::nt::file_basic_information fbi;
	status = ::fast_io::win32::nt::nt_query_information_file<family == ::fast_io::nt_family::zw>(handle, __builtin_addressof(isb), __builtin_addressof(fbi),
																								 static_cast<::std::uint_least32_t>(sizeof(fbi)),
																								 ::fast_io::win32::nt::file_information_class::FileBasicInformation);

	if (status) [[unlikely]]
	{
		throw_nt_error(status);
	}

	::fast_io::win32::nt::file_internal_information fii;
	status = ::fast_io::win32::nt::nt_query_information_file<family == ::fast_io::nt_family::zw>(handle, __builtin_addressof(isb), __builtin_addressof(fii),
																								 static_cast<::std::uint_least32_t>(sizeof(fii)),
																								 ::fast_io::win32::nt::file_information_class::FileInternalInformation);

	if (status) [[unlikely]]
	{
		throw_nt_error(status);
	}

	struct
	{
		::fast_io::win32::nt::file_fs_volume_information FileFsVolume;
		char16_t Name[255];
	} ffvi;

	status = ::fast_io::win32::nt::nt_query_volume_information_file<family == ::fast_io::nt_family::zw>(handle, __builtin_addressof(isb),
																										__builtin_addressof(ffvi), static_cast<::std::uint_least32_t>(sizeof(ffvi)),
																										::fast_io::win32::nt::fs_information_class::FileFsVolumeInformation);
	if (status) [[unlikely]]
	{
		throw_nt_error(status);
	}

	::fast_io::win32::nt::file_standard_information fsi;
	status = ::fast_io::win32::nt::nt_query_information_file<family == ::fast_io::nt_family::zw>(handle, __builtin_addressof(isb), __builtin_addressof(fsi),
																								 static_cast<::std::uint_least32_t>(sizeof(fsi)),
																								 ::fast_io::win32::nt::file_information_class::FileStandardInformation);

	if (status) [[unlikely]]
	{
		throw_nt_error(status);
	}

	::std::uintmax_t file_size{static_cast<::std::uintmax_t>(fsi.end_of_file)};
	::std::underlying_type_t<perms> pm{0444};
	if ((fbi.FileAttributes & 0x1) == 0x0)
	{
		pm |= 0222;
	}
	if ((fbi.FileAttributes & 0x400) == 0x400)
	{
		ft = file_type::symlink;
	}
	else if ((fbi.FileAttributes & 0x10) == 0x10)
	{
		ft = file_type::directory;
	}
	return posix_file_status{static_cast<::std::uintmax_t>(ffvi.FileFsVolume.VolumeSerialNumber),
							 static_cast<::std::uintmax_t>(fii.IndexNumber),
							 static_cast<perms>(pm),
							 ft,
							 static_cast<::std::uintmax_t>(fsi.number_of_links),
							 0,
							 0,
							 0,
							 file_size,
							 131072,
							 file_size >> 9,
							 to_unix_timestamp(fbi.LastAccessTime),
							 to_unix_timestamp(fbi.LastWriteTime),
							 to_unix_timestamp(fbi.ChangeTime),
							 to_unix_timestamp(fbi.CreationTime),
							 0,
							 0};
}

} // namespace win32::nt::details

template <nt_family family, ::std::integral ch_type>
inline posix_file_status status(basic_nt_family_io_observer<family, ch_type> wiob)
{
	return win32::nt::details::nt_status_impl<family>(wiob.handle);
}

template <nt_family family>
struct nt_family_file_lock
{
	void *handle{};
	template <::std::signed_integral int_type>
		requires(sizeof(int_type) >= sizeof(::std::int_least64_t))
	inline void lock(basic_flock_request<int_type> &__restrict t)
	{
		::fast_io::win32::nt::details::nt_family_file_lock_impl<family == nt_family::zw>(this->handle, t);
	}
	template <::std::signed_integral int_type>
		requires(sizeof(int_type) >= sizeof(::std::int_least64_t))
	inline void unlock(basic_flock_request<int_type> &__restrict t) noexcept
	{
		::fast_io::win32::nt::details::nt_family_file_unlock_impl<family == nt_family::zw>(this->handle, t);
	}
	template <::std::signed_integral int_type>
		requires(sizeof(int_type) >= sizeof(::std::int_least64_t))
	inline bool try_lock(basic_flock_request<int_type> &__restrict t) noexcept
	{
		return ::fast_io::win32::nt::details::nt_family_file_try_lock_impl<family == nt_family::zw>(this->handle, t);
	}
};

using nt_file_lock = nt_family_file_lock<nt_family::nt>;
using zw_file_lock = nt_family_file_lock<nt_family::zw>;

template <nt_family family, ::std::integral char_type>
inline constexpr nt_family_file_lock<family> file_lock(basic_nt_family_io_observer<family, char_type> niob) noexcept
{
	return {niob};
}

template <nt_family family>
struct nt_family_file_factory FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
{
	using native_handle_type = void *;
	void *handle{};
	inline explicit constexpr nt_family_file_factory(void *hd) noexcept
		: handle(hd) {};
	inline nt_family_file_factory(nt_family_file_factory const &) = delete;
	inline nt_family_file_factory &operator=(nt_family_file_factory const &) = delete;
	inline ~nt_family_file_factory()
	{
		if (handle) [[likely]]
		{
			::fast_io::win32::nt::nt_close<family == nt_family::zw>(this->handle);
		}
	}
};

using nt_file_factory = nt_family_file_factory<nt_family::nt>;
using zw_file_factory = nt_family_file_factory<nt_family::zw>;

template <nt_family family, ::std::integral ch_type>
class basic_nt_family_file FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE : public basic_nt_family_io_observer<family, ch_type>
{
public:
	using typename basic_nt_family_io_observer<family, ch_type>::char_type;
	using typename basic_nt_family_io_observer<family, ch_type>::input_char_type;
	using typename basic_nt_family_io_observer<family, ch_type>::output_char_type;
	using typename basic_nt_family_io_observer<family, ch_type>::native_handle_type;
	using file_factory_type = nt_family_file_factory<family>;
	inline constexpr basic_nt_family_file() noexcept = default;
	inline constexpr basic_nt_family_file(basic_nt_family_io_observer<family, ch_type>) noexcept = delete;
	inline constexpr basic_nt_family_file &operator=(basic_nt_family_io_observer<family, ch_type>) noexcept = delete;

	template <typename native_hd>
		requires ::std::same_as<native_handle_type, ::std::remove_cvref_t<native_hd>>
	inline explicit constexpr basic_nt_family_file(native_hd hd) noexcept
		: basic_nt_family_io_observer<family, ch_type>{hd}
	{
	}
	inline constexpr basic_nt_family_file(decltype(nullptr)) noexcept = delete;
	inline explicit constexpr basic_nt_family_file(nt_family_file_factory<family> &&hd) noexcept
		: basic_nt_family_io_observer<family, ch_type>{hd}
	{
		hd.handle = nullptr;
	}
	inline explicit basic_nt_family_file(io_dup_t, basic_nt_family_io_observer<family, ch_type> wiob)
		: basic_nt_family_io_observer<family, ch_type>{
			  ::fast_io::win32::nt::details::nt_dup_impl<family == nt_family::zw>(wiob.handle)}
	{
	}
	inline explicit basic_nt_family_file(nt_fs_dirent fsdirent, open_mode om, perms pm = static_cast<perms>(436))
		: basic_nt_family_io_observer<family, char_type>{
			  ::fast_io::win32::nt::details::nt_family_create_file_fs_dirent_impl<family == nt_family::zw>(fsdirent.handle, fsdirent.filename.c_str(), fsdirent.filename.size(), {om, pm})}
	{
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline explicit basic_nt_family_file(T const &t, open_mode om, perms pm = static_cast<perms>(436))
		: basic_nt_family_io_observer<family, ch_type>{
			  ::fast_io::win32::nt::details::nt_create_file_impl<family == nt_family::zw>(t, {om, pm})}
	{
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline explicit basic_nt_family_file(nt_at_entry ent, T const &t, open_mode om, perms pm = static_cast<perms>(436))
		: basic_nt_family_io_observer<family, ch_type>{
			  ::fast_io::win32::nt::details::nt_create_file_at_impl<family == nt_family::zw>(ent.handle, t, {om, pm})}
	{
	}

	template <::fast_io::constructible_to_os_c_str T>
	inline explicit basic_nt_family_file(io_kernel_t, T const &t, open_mode om, perms pm = static_cast<perms>(436))
		: basic_nt_family_io_observer<family, ch_type>{
			  ::fast_io::win32::nt::details::nt_create_file_kernel_impl<family == nt_family::zw>(t, {om, pm})}
	{
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline explicit basic_nt_family_file(io_kernel_t, nt_at_entry ent, T const &t, open_mode om,
										 perms pm = static_cast<perms>(436))
		: basic_nt_family_io_observer<family, ch_type>{
			  ::fast_io::win32::nt::details::nt_create_file_at_impl<family == nt_family::zw, true>(ent.handle, t,
																								   {om, pm})}
	{
	}

	inline void close()
	{
		if (this->handle) [[likely]]
		{
			auto status{::fast_io::win32::nt::nt_close<family == nt_family::zw>(this->handle)};
			this->handle = nullptr; // POSIX standard says we should never call close(2) again even close syscall fails
			if (status) [[unlikely]]
			{
				throw_nt_error(status);
			}
		}
	}
	inline void reset(native_handle_type newhandle = nullptr) noexcept
	{
		if (this->handle) [[likely]]
		{
			::fast_io::win32::nt::nt_close<family == nt_family::zw>(this->handle);
		}
		this->handle = newhandle;
	}
	inline basic_nt_family_file(basic_nt_family_file const &other)
		: basic_nt_family_io_observer<family, ch_type>(
			  ::fast_io::win32::nt::details::nt_dup_impl<family == nt_family::zw>(other.handle))
	{
	}
	inline basic_nt_family_file &operator=(basic_nt_family_file const &other)
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		this->handle = ::fast_io::win32::nt::details::nt_dup2_impl<family == nt_family::zw>(other.handle, this->handle);
		return *this;
	}
	inline constexpr basic_nt_family_file(basic_nt_family_file &&__restrict other) noexcept
		: basic_nt_family_io_observer<family, ch_type>{other.handle}
	{
		other.handle = nullptr;
	}
	inline basic_nt_family_file &operator=(basic_nt_family_file &&__restrict other) noexcept
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		if (this->handle) [[likely]]
		{
			::fast_io::win32::nt::nt_close<family == nt_family::zw>(this->handle);
		}
		this->handle = other.handle;
		other.handle = nullptr;
		return *this;
	}
	inline ~basic_nt_family_file()
	{
		if (this->handle) [[likely]]
		{
			::fast_io::win32::nt::nt_close<family == nt_family::zw>(this->handle);
		}
	}
};

namespace win32::nt::details
{

template <bool zw>
inline void nt_create_pipe(void **hReadPipe, void **hWritePipe)
{
	::fast_io::win32::nt::io_status_block isb;
	constexpr decltype(auto) namedpipe_part{u"\\Device\\NamedPipe\\"};
	::fast_io::win32::nt::unicode_string us{
		.Length = static_cast<::std::uint_least16_t>(sizeof(namedpipe_part) - sizeof(char16_t)),
		.MaximumLength = ::fast_io::win32::nt::details::nt_filename_bytes_check(sizeof(namedpipe_part)),
		.Buffer = const_cast<char16_t *>(namedpipe_part)};

	::fast_io::win32::nt::object_attributes obj{.Length = sizeof(::fast_io::win32::nt::object_attributes),
												.RootDirectory = nullptr,
												.ObjectName = __builtin_addressof(us),
												.Attributes = 0,
												.SecurityDescriptor = nullptr,
												.SecurityQualityOfService = nullptr};

	void *namedpipedir;
	auto status = ::fast_io::win32::nt::nt_create_file<zw>(
		__builtin_addressof(namedpipedir), 0x80100000, __builtin_addressof(obj),
		__builtin_addressof(isb), nullptr, 0, 3, 0x00000001, 0x00000020, nullptr, 0u);

	if (status)
	{
		throw_nt_error(status);
	}

	::fast_io::basic_nt_family_file<zw ? nt_family::zw : nt_family::nt, char> file{namedpipedir};

	::std::int_least64_t DefaultTimeout{-1200000000};

	void *ReadPipeHandle;
	::fast_io::win32::nt::unicode_string us2{};
	::fast_io::win32::nt::object_attributes obj2{.Length = sizeof(::fast_io::win32::nt::object_attributes),
												 .RootDirectory = file.native_handle(),
												 .ObjectName = __builtin_addressof(us2),
												 .Attributes = 0x42 /* InheritHandle */,
												 .SecurityDescriptor = nullptr,
												 .SecurityQualityOfService = nullptr};

	status = ::fast_io::win32::nt::nt_create_named_pipe_file<zw>(__builtin_addressof(ReadPipeHandle),
																 0x80000000L | 0x0100 | 0x00100000L /*GENERIC_READ | FILE_WRITE_ATTRIBUTES | SYNCHRONIZE*/,
																 __builtin_addressof(obj2),
																 __builtin_addressof(isb),
																 0x01 | 0x02 /*FILE_SHARE_READ | FILE_SHARE_DELETE*/,
																 0x00000002 /*FILE_CREATE*/,
																 0x00000020 /*FILE_SYNCHRONOUS_IO_NONALERT*/,
																 0x00000000 /*FILE_PIPE_BYTE_STREAM_TYPE*/,
																 0x00000000 /*FILE_PIPE_BYTE_STREAM_MODE*/,
																 0x00000000 /*FILE_PIPE_QUEUE_OPERATION*/,
																 1,
																 0x4000 /*buffer size*/,
																 0x4000 /*buffer size*/,
																 __builtin_addressof(DefaultTimeout));

	if (status)
	{
		::fast_io::throw_nt_error(status);
	}

	obj2.RootDirectory = ReadPipeHandle;

	void *WritePipeHandle;
	status = ::fast_io::win32::nt::nt_create_file<zw>(
		__builtin_addressof(WritePipeHandle), 0x40100080, __builtin_addressof(obj2), __builtin_addressof(isb), nullptr,
		0, 3, 0x00000001, 0x00000020 | 0x00000040, nullptr, 0u);

	if (status)
	{
		::fast_io::throw_nt_error(status);
	}

	*hReadPipe = ReadPipeHandle;
	*hWritePipe = WritePipeHandle;
}
} // namespace win32::nt::details

template <nt_family family, ::std::integral ch_type>
class basic_nt_family_pipe
{
public:
	using char_type = ch_type;
	basic_nt_family_file<family, ch_type> pipes[2];
	inline basic_nt_family_pipe()
	{
		win32::nt::details::nt_create_pipe<family == nt_family::zw>(__builtin_addressof(pipes[0].handle), __builtin_addressof(pipes[1].handle));
	}
	inline constexpr auto &in() noexcept
	{
		return *pipes;
	}
	inline constexpr auto &out() noexcept
	{
		return pipes[1];
	}
};

template <nt_family family, ::std::integral ch_type>
inline constexpr win32_io_redirection redirect(basic_nt_family_pipe<family, ch_type> &hd)
{
	return {.win32_pipe_in_handle = hd.in().handle, .win32_pipe_out_handle = hd.out().handle};
}

template <nt_family family, ::std::integral ch_type>
inline constexpr basic_nt_family_io_observer<family, ch_type>
input_stream_ref_define(basic_nt_family_pipe<family, ch_type> &pp) noexcept
{
	return {pp.in().handle};
}

template <nt_family family, ::std::integral ch_type>
inline constexpr basic_nt_family_io_observer<family, char>
input_bytes_stream_ref_define(basic_nt_family_pipe<family, ch_type> &pp) noexcept
{
	return {pp.in().handle};
}

template <nt_family family, ::std::integral ch_type>
inline constexpr basic_nt_family_io_observer<family, ch_type>
output_stream_ref_define(basic_nt_family_pipe<family, ch_type> &pp) noexcept
{
	return {pp.out().handle};
}

template <nt_family family, ::std::integral ch_type>
inline constexpr basic_nt_family_io_observer<family, char>
output_bytes_stream_ref_define(basic_nt_family_pipe<family, ch_type> &pp) noexcept
{
	return {pp.out().handle};
}

template <::std::integral char_type>
using basic_nt_io_observer = basic_nt_family_io_observer<nt_family::nt, char_type>;

template <::std::integral char_type>
using basic_nt_file = basic_nt_family_file<nt_family::nt, char_type>;

template <::std::integral char_type>
using basic_nt_pipe = basic_nt_family_pipe<nt_family::nt, char_type>;

using nt_io_observer = basic_nt_io_observer<char>;
using nt_file = basic_nt_file<char>;
using nt_pipe = basic_nt_pipe<char>;

using wnt_io_observer = basic_nt_io_observer<wchar_t>;
using wnt_file = basic_nt_file<wchar_t>;
using wnt_pipe = basic_nt_pipe<wchar_t>;

using u8nt_io_observer = basic_nt_io_observer<char8_t>;
using u8nt_file = basic_nt_file<char8_t>;
using u8nt_pipe = basic_nt_pipe<char8_t>;

using u16nt_io_observer = basic_nt_io_observer<char16_t>;
using u16nt_file = basic_nt_file<char16_t>;
using u16nt_pipe = basic_nt_pipe<char16_t>;

using u32nt_io_observer = basic_nt_io_observer<char32_t>;
using u32nt_file = basic_nt_file<char32_t>;
using u32nt_pipe = basic_nt_pipe<char32_t>;

namespace details
{

template <int fd>
inline void *nt_get_stdhandle() noexcept
{
	using prtl_user_process_parameters
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= ::fast_io::win32::nt::rtl_user_process_parameters *;
	auto pparam{
		reinterpret_cast<prtl_user_process_parameters>(::fast_io::win32::nt::nt_get_current_peb()->ProcessParameters)};
	if constexpr (fd == 0)
	{
		return pparam->StandardInput;
	}
	else if constexpr (fd == 1)
	{
		return pparam->StandardOutput;
	}
	else
	{
		return pparam->StandardError;
	}
}

} // namespace details

template <::std::integral char_type = char>
inline basic_nt_io_observer<char_type> nt_stdin() noexcept
{
	return {::fast_io::details::nt_get_stdhandle<0>()};
}

template <::std::integral char_type = char>
inline basic_nt_io_observer<char_type> nt_stdout() noexcept
{
	return {::fast_io::details::nt_get_stdhandle<1>()};
}

template <::std::integral char_type = char>
inline basic_nt_io_observer<char_type> nt_stderr() noexcept
{
	return {::fast_io::details::nt_get_stdhandle<2>()};
}

template <::std::integral char_type>
using basic_zw_io_observer = basic_nt_family_io_observer<nt_family::zw, char_type>;

template <::std::integral char_type>
using basic_zw_file = basic_nt_family_file<nt_family::zw, char_type>;

template <::std::integral char_type>
using basic_zw_pipe = basic_nt_family_pipe<nt_family::zw, char_type>;

using zw_io_observer = basic_zw_io_observer<char>;
using zw_file = basic_zw_file<char>;
using zw_pipe = basic_zw_pipe<char>;

using wzw_io_observer = basic_zw_io_observer<wchar_t>;
using wzw_file = basic_zw_file<wchar_t>;
using wzw_pipe = basic_zw_pipe<wchar_t>;

using u8zw_io_observer = basic_zw_io_observer<char8_t>;
using u8zw_file = basic_zw_file<char8_t>;
using u8zw_pipe = basic_zw_pipe<char8_t>;

using u16zw_io_observer = basic_zw_io_observer<char16_t>;
using u16zw_file = basic_zw_file<char16_t>;
using u16zw_pipe = basic_zw_pipe<char16_t>;

using u32zw_io_observer = basic_zw_io_observer<char32_t>;
using u32zw_file = basic_zw_file<char32_t>;
using u32zw_pipe = basic_zw_pipe<char32_t>;

template <::std::integral char_type = char>
inline basic_zw_io_observer<char_type> zw_stdin() noexcept
{
	return {::fast_io::details::nt_get_stdhandle<0>()};
}

template <::std::integral char_type = char>
inline basic_zw_io_observer<char_type> zw_stdout() noexcept
{
	return {::fast_io::details::nt_get_stdhandle<1>()};
}

template <::std::integral char_type = char>
inline basic_zw_io_observer<char_type> zw_stderr() noexcept
{
	return {::fast_io::details::nt_get_stdhandle<2>()};
}

#if !defined(__WINE__) && !defined(__CYGWIN__) && !defined(__BIONIC__) && !defined(_WIN32_WINDOWS)
template <::std::integral char_type = char>
inline basic_nt_io_observer<char_type> native_stdin() noexcept
{
	return {::fast_io::details::nt_get_stdhandle<0>()};
}
template <::std::integral char_type = char>
inline basic_nt_io_observer<char_type> native_stdout() noexcept
{
	return {::fast_io::details::nt_get_stdhandle<1>()};
}
template <::std::integral char_type = char>
inline basic_nt_io_observer<char_type> native_stderr() noexcept
{
	return {::fast_io::details::nt_get_stdhandle<2>()};
}
#endif

namespace freestanding
{
template <nt_family fm>
struct is_trivially_copyable_or_relocatable<nt_family_file_factory<fm>>
{
	inline static constexpr bool value = true;
};

template <nt_family fm, ::std::integral char_type>
struct is_zero_default_constructible<basic_nt_family_io_observer<fm, char_type>>
{
	inline static constexpr bool value = true;
};

template <nt_family fm, ::std::integral char_type>
struct is_trivially_copyable_or_relocatable<basic_nt_family_file<fm, char_type>>
{
	inline static constexpr bool value = true;
};

template <nt_family fm, ::std::integral char_type>
struct is_zero_default_constructible<basic_nt_family_file<fm, char_type>>
{
	inline static constexpr bool value = true;
};
} // namespace freestanding

} // namespace fast_io
