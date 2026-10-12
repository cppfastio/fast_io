#pragma once

namespace fast_io
{

namespace details
{

namespace win32
{
/*
Reference from crypto++'s osrng.
Based on information from cryptopp, microsoft patched KB238187 because this api has issues before Windows Server 2012 R2
We use an implementation referenced form crypto++'s implementation.
To avoid name collision, we use itanium abi of name mangling for i686-w64-mingw32-g++ -D_WIN32_WINDOWS=0x0400
_ZN7fast_io7details5win3230crypt_acquire_context_fallbackILNS_12win32_familyE0EEEjv
*/
template <::fast_io::win32_family family>
inline ::std::size_t crypt_acquire_context_fallback()
	FAST_IO_HERBCEPTIONS_THROWS
{
	::std::size_t hprov{};
	if constexpr (family == ::fast_io::win32_family::ansi_9x)
	{
		if (!::fast_io::win32::CryptAcquireContextA(__builtin_addressof(hprov), nullptr, nullptr, 0x1, 0xf0000000))
		{
			::std::uint_least32_t firsterr{::fast_io::win32::GetLastError()};
			if (!::fast_io::win32::CryptAcquireContextA(
					__builtin_addressof(hprov),
					u8"_ZN7fast_io7details5win3230crypt_acquire_context_fallbackILNS_12win32_familyE0EEEjv", nullptr,
					0x1, 0x8))
			{
				if (!::fast_io::win32::CryptAcquireContextA(
						__builtin_addressof(hprov),
						u8"_ZN7fast_io7details5win3230crypt_acquire_context_fallbackILNS_12win32_familyE0EEEjv",
						nullptr, 0x1, 0x20 | 0x8))
				{
					throw_win32_error(firsterr);
				}
			}
		}
	}
	else
	{
		if (!::fast_io::win32::CryptAcquireContextW(__builtin_addressof(hprov), nullptr, nullptr, 0x1, 0xf0000000))
		{
			::std::uint_least32_t firsterr{::fast_io::win32::GetLastError()};
			if (!::fast_io::win32::CryptAcquireContextW(
					__builtin_addressof(hprov),
					u"_ZN7fast_io7details5win3230crypt_acquire_context_fallbackILNS_12win32_familyE0EEEjv", nullptr,
					0x1, 0x8))
			{
				if (!::fast_io::win32::CryptAcquireContextW(
						__builtin_addressof(hprov),
						u"_ZN7fast_io7details5win3230crypt_acquire_context_fallbackILNS_12win32_familyE0EEEjv", nullptr,
						0x1, 0x20 | 0x8))
				{
					throw_win32_error(firsterr);
				}
			}
		}
	}
	return hprov;
}

template <::fast_io::win32_family family>
struct win32_family_hcryptprov_guard
{
	::std::size_t hprov{};
	inline win32_family_hcryptprov_guard()
		FAST_IO_HERBCEPTIONS_THROWS
		: hprov{crypt_acquire_context_fallback<family>()}
	{}
	inline win32_family_hcryptprov_guard(win32_family_hcryptprov_guard const &) = delete;
	inline win32_family_hcryptprov_guard &operator=(win32_family_hcryptprov_guard const &) = delete;
	inline ~win32_family_hcryptprov_guard()
	{
		::fast_io::win32::CryptReleaseContext(hprov, 0);
	}
};

} // namespace win32

template <win32_family family>
inline void *create_win32_temp_file_impl()
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr bool is_nt{family == win32_family::wide_nt};
	using char_type = ::std::conditional_t<is_nt, char16_t, char>;
	using replace_char_type = ::std::conditional_t<is_nt, char16_t, char8_t>;
	using char_type_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char_type *;
	using char_type_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char_type const *;
	constexpr ::std::uint_least32_t maximum_temp_path_size{261};
	constexpr ::std::uint_least32_t uuid_size{36};
	constexpr ::std::uint_least32_t literal_tmp_size{5};
	constexpr ::std::uint_least32_t total_buffer_size{maximum_temp_path_size + uuid_size + literal_tmp_size};
	replace_char_type arr[total_buffer_size];
	::std::uint_least32_t temp_path_size;
	if constexpr (is_nt)
	{
		temp_path_size =
			::fast_io::win32::GetTempPathW(maximum_temp_path_size, reinterpret_cast<char_type_may_alias_ptr>(arr));
	}
	else
	{
		temp_path_size =
			::fast_io::win32::GetTempPathA(maximum_temp_path_size, reinterpret_cast<char_type_may_alias_ptr>(arr));
	}
	if (temp_path_size == 0)
	{
		throw_win32_error();
	}
	if (temp_path_size > maximum_temp_path_size) [[unlikely]]
	{
		throw_win32_error(122 /*ERROR_INSUFFICIENT_BUFFER*/);
	}
	auto arrp{arr + temp_path_size};
	constexpr bool winver_support_rtl_gen_random{
#if !defined(_WIN32_WINDOWS) && (!defined(_WIN32_WINNT) || _WIN32_WINNT >= 0x0501)
		// RtlGenRandom support starts since Windows XP
		is_nt
#endif
	};
	::std::conditional_t<winver_support_rtl_gen_random, ::fast_io::details::empty,
						 ::fast_io::details::win32::win32_family_hcryptprov_guard<family>>
		hcryptguard;
	for (::std::size_t retries{}; retries != 100; ++retries) // retry 100 times. Or reporting error
	{
		constexpr ::std::uint_least32_t uuid_buffer_sz{16};
		::std::byte uuid_buffer[uuid_buffer_sz];
		if constexpr (winver_support_rtl_gen_random)
		{
			if (!::fast_io::win32::SystemFunction036(uuid_buffer, uuid_buffer_sz))
			{
				// However, according to MSDN, this API does not set LastError.
				throw_win32_error(31 /*ERROR_GEN_FAILURE*/);
			}
		}
		else
		{
			if (!::fast_io::win32::CryptGenRandom(hcryptguard.hprov, uuid_buffer_sz,
												  reinterpret_cast<char unsigned *>(uuid_buffer)))
			{
				throw_win32_error();
			}
		}
		auto p{::fast_io::details::pr_rsv_uuid<false>(arrp, uuid_buffer)};
		if constexpr (is_nt)
		{
			copy_string_literal(u".tmp\0", p);
		}
		else
		{
			copy_string_literal(u8".tmp\0", p);
		}
		void *handle;
		if constexpr (is_nt)
		{
			handle = ::fast_io::win32::CreateFileW(
				reinterpret_cast<char_type_may_alias_const_ptr>(arr),
				0x40000000 | 0x80000000, // GENERIC_READ|GENERIC_WRITE
				0,                       // Prevents other processes from opening a file or device if they request delete, read, or write
										 // access.
				nullptr,
				1, // CREATE_NEW
				0x100 | 0x04000000 | 0x01000000 |
					0x08000000, // FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE|FILE_FLAG_POSIX_SEMANTICS|FILE_FLAG_SEQUENTIAL_SCAN
				nullptr);
		}
		else
		{
			handle = ::fast_io::win32::CreateFileA(
				reinterpret_cast<char_type_may_alias_const_ptr>(arr),
				0x40000000 | 0x80000000, // GENERIC_READ|GENERIC_WRITE
				0,                       // Prevents other processes from opening a file or device if they request delete, read, or write
										 // access.
				nullptr,
				1, // CREATE_NEW
				0x100 | 0x04000000 | 0x01000000 |
					0x08000000, // FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE|FILE_FLAG_POSIX_SEMANTICS|FILE_FLAG_SEQUENTIAL_SCAN
				nullptr);
		}
		if (handle == (void *)(::std::ptrdiff_t)-1) [[unlikely]]
		{
			::std::uint_least32_t code{::fast_io::win32::GetLastError()};
			if (code == 80u) // ERROR_FILE_EXISTS
			{
				continue;
			}
			throw_win32_error(code);
		}
		return handle;
	}
	throw_win32_error(80u); // ERROR_FILE_EXISTS
}

inline void *create_io_completion_port(void *filehandle, void *existing_completionport, ::std::size_t completionkey,
									   ::std::uint_least32_t numberofconcurrentthreads)
	FAST_IO_HERBCEPTIONS_THROWS
{
	void *ptr{::fast_io::win32::CreateIoCompletionPort(filehandle, existing_completionport, completionkey,
													   numberofconcurrentthreads)};
	if (ptr == nullptr) [[unlikely]]
	{
		throw_win32_error();
	}
	return ptr;
}

inline void *create_io_completion_port_impl()
	FAST_IO_HERBCEPTIONS_THROWS
{
	return create_io_completion_port(reinterpret_cast<void *>(static_cast<::std::size_t>(-1)), nullptr, 0, 0);
}

struct win32_open_mode
{
	::std::uint_least32_t dwDesiredAccess{};
	::std::uint_least32_t dwShareMode{};
	bool inherit{};
	::std::uint_least32_t dwCreationDisposition{}; // depends on EXCL
	::std::uint_least32_t dwFlagsAndAttributes{};  //=128|0x10000000;//FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS
};

template <win32_family family>
inline void *win32_family_create_file_internal_impl(
	::std::conditional_t<family == win32_family::wide_nt, char16_t, char> const *lpFileName,
	win32_open_mode const &mode)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if constexpr (family == win32_family::wide_nt)
	{
		::fast_io::win32::security_attributes sec_attr{sizeof(::fast_io::win32::security_attributes), nullptr, true};
		auto handle(::fast_io::win32::CreateFileW(lpFileName, mode.dwDesiredAccess, mode.dwShareMode,
												  mode.inherit ? __builtin_addressof(sec_attr) : nullptr,
												  mode.dwCreationDisposition, mode.dwFlagsAndAttributes, nullptr));
		if (handle == ((void *)(::std::ptrdiff_t)-1))
		{
			throw_win32_error();
		}
		return handle;
	}
	else
	{
		auto handle(::fast_io::win32::CreateFileA(lpFileName, mode.dwDesiredAccess, mode.dwShareMode,
												  nullptr, // 9x kernel does not support security attributes
												  mode.dwCreationDisposition, mode.dwFlagsAndAttributes, nullptr));
		if (handle == ((void *)(::std::ptrdiff_t)-1))
		{
			throw_win32_error();
		}
		return handle;
	}
}

inline constexpr win32_open_mode calculate_win32_open_mode(open_mode_perms ompm) noexcept
{
	open_mode value{ompm.om};
	perms pm{ompm.pm};
	win32_open_mode mode;
	if ((value & open_mode::no_shared_read) == open_mode::none)
	{
		mode.dwShareMode |= 1; // FILE_SHARE_READ
	}
	if ((value & open_mode::no_shared_write) == open_mode::none)
	{
		mode.dwShareMode |= 2; // FILE_SHARE_WRITE
	}
	if ((value & open_mode::shared_delete) != open_mode::none)
	{
		mode.dwShareMode |= 4; // FILE_SHARE_DELETE
	}
	if ((value & open_mode::app) != open_mode::none)
	{
		mode.dwDesiredAccess |= 4; // FILE_APPEND_DATA
	}
	else if ((value & open_mode::out) != open_mode::none)
	{
		mode.dwDesiredAccess |= 0x40000000; // GENERIC_WRITE
	}
	if (((value & open_mode::in) != open_mode::none) || ((value & open_mode::app) != open_mode::none))
	{
		mode.dwDesiredAccess |= 0x80000000; // GENERIC_READ
		if ((value & open_mode::out) != open_mode::none &&
			((value & open_mode::app) != open_mode::none && (value & open_mode::trunc) != open_mode::none))
		{
			mode.dwDesiredAccess |= 0x40000000; // GENERIC_WRITE
		}
	}
	/*
	Referenced partially from ReactOS
	https://github.com/changloong/msvcrt/blob/master/io/wopen.c



	https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilew

	CREATE_ALWAYS
	2
	Creates a new file, always.
	If the specified file exists and is writable, the function overwrites the file, the function succeeds, and
	last-error code is set to ERROR_ALREADY_EXISTS (183).

	If the specified file does not exist and is a valid path, a new file is created, the function succeeds, and the
	last-error code is set to zero.

	For more information, see the Remarks section of this topic.

	CREATE_NEW
	1
	Creates a new file, only if it does not already exist.
	If the specified file exists, the function fails and the last-error code is set to ERROR_FILE_EXISTS (80).

	If the specified file does not exist and is a valid path to a writable location, a new file is created.

	OPEN_ALWAYS
	4
	Opens a file, always.
	If the specified file exists, the function succeeds and the last-error code is set to ERROR_ALREADY_EXISTS (183).

	If the specified file does not exist and is a valid path to a writable location, the function creates a file and the
	last-error code is set to zero.

	OPEN_EXISTING
	3
	Opens a file or device, only if it exists.
	If the specified file or device does not exist, the function fails and the last-error code is set to
	ERROR_FILE_NOT_FOUND (2).

	For more information about devices, see the Remarks section.

	TRUNCATE_EXISTING
	5
	Opens a file and truncates it so that its size is zero bytes, only if it exists.
	If the specified file does not exist, the function fails and the last-error code is set to ERROR_FILE_NOT_FOUND (2).

	The calling process must open the file with the GENERIC_WRITE bit set as part of the dwDesiredAccess parameter.



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
			mode.dwCreationDisposition = 1; // CREATE_NEW
		}
		else
		{
			mode.dwCreationDisposition = 2; // CREATE_ALWAYS
		}
	}
	else if ((value & open_mode::in) == open_mode::none)
	{
		if ((value & open_mode::app) != open_mode::none)
		{
			mode.dwCreationDisposition = 4; // OPEN_ALWAYS
		}
		else if ((value & open_mode::out) != open_mode::none)
		{
			if ((value & open_mode::excl) != open_mode::none)
			{
				mode.dwCreationDisposition = 1; // CREATE_NEW
			}
			else
			{
				mode.dwCreationDisposition = 2; // CREATE_ALWAYS
			}
		}
	}
	else if ((value & open_mode::app) != open_mode::none)
	{
		mode.dwCreationDisposition = 4; // OPEN_ALWAYS
	}
	else
	{
		mode.dwCreationDisposition = 3; // OPEN_EXISTING
	}
	if ((value & open_mode::creat) != open_mode::none)
	{
		if ((value & open_mode::excl) != open_mode::none)
		{
			mode.dwCreationDisposition = 1; // CREATE_NEW
		}
		else
		{
			mode.dwCreationDisposition = 4; // OPEN_ALWAYS
		}
	}
	if ((value & open_mode::direct) != open_mode::none)
	{
		mode.dwFlagsAndAttributes |= 0x20000000; // FILE_FLAG_NO_BUFFERING
	}
	if ((value & open_mode::sync) != open_mode::none)
	{
		mode.dwFlagsAndAttributes |= 0x80000000; // FILE_FLAG_WRITE_THROUGH
	}
	if ((value & open_mode::no_block) != open_mode::none)
	{
		mode.dwFlagsAndAttributes |= 0x40000000; // FILE_FLAG_OVERLAPPED
	}
	if ((value & open_mode::follow) == open_mode::none)
	{
		mode.dwFlagsAndAttributes |= 0x00200000; // FILE_FLAG_OPEN_REPARSE_POINT
	}
	/*
	Referenced from:
	https://stackoverflow.com/questions/4495767/create-a-directory-and-get-the-handle-by-issuing-one-irp

	CreateFile actually can create a directory. Set the disposition to CREATE_NEW and the flags to
	FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_POSIX_SEMANTICS | FILE_ATTRIBUTE_DIRECTORY. Since this isn't documented, it
	probably isn't implemented in ReactOS and Wine. Note that if you're in a scripting environment such as Python, make
	sure to use the correct value of FILE_FLAG_POSIX_SEMANTICS from the headers (0x01000000). The value listed on MSDN
	has always been wrong. – Eryk Sun Jan 25 '17 at 4:14

	I tried this. Oh no. It cannot

	*/
	if ((value & open_mode::directory) != open_mode::none)
	{
		mode.dwFlagsAndAttributes |= 0x02000000; // FILE_FLAG_BACKUP_SEMANTICS
		mode.dwFlagsAndAttributes |= 0x10;       // FILE_ATTRIBUTE_DIRECTORY
		if (mode.dwCreationDisposition == 0)
		{
			mode.dwDesiredAccess |= UINT32_C(0x120116) | UINT32_C(0x120089); // GENERIC_WRITE|GENERIC_READ
			mode.dwCreationDisposition = 3;                                  // OPEN_EXISTING
		}
	}
	bool set_normal{true};
	if ((value & open_mode::archive) != open_mode::none)
	{
		mode.dwFlagsAndAttributes |= 0x20; // FILE_ATTRIBUTE_ARCHIVE
		set_normal = {};
	}
	if ((value & open_mode::encrypted) != open_mode::none)
	{
		mode.dwFlagsAndAttributes |= 0x4000; // FILE_ATTRIBUTE_ENCRYPTED
		set_normal = {};
	}
	if ((value & open_mode::hidden) != open_mode::none)
	{
		mode.dwFlagsAndAttributes |= 0x2; // FILE_ATTRIBUTE_HIDDEN
		set_normal = {};
	}
	if ((value & open_mode::compressed) != open_mode::none)
	{
		mode.dwFlagsAndAttributes |= 0x800; // FILE_ATTRIBUTE_COMPRESSED
		set_normal = {};
	}
	if ((value & open_mode::system) != open_mode::none)
	{
		mode.dwFlagsAndAttributes |= 0x4; // FILE_ATTRIBUTE_SYSTEM
		set_normal = {};
	}
	if ((value & open_mode::offline) != open_mode::none)
	{
		mode.dwFlagsAndAttributes |= 0x1000; // FILE_ATTRIBUTE_OFFLINE
		set_normal = {};
	}
	if (set_normal) [[likely]]
	{
		mode.dwFlagsAndAttributes |= 0x80; // FILE_ATTRIBUTE_NORMAL
	}
	if ((value & open_mode::random_access) == open_mode::none)
	{
		if ((value & open_mode::directory) == open_mode::none)
		{
			mode.dwFlagsAndAttributes |= 0x10000000; // FILE_FLAG_SEQUENTIAL_SCAN
		}
	}
	else
	{
		mode.dwFlagsAndAttributes |= 0x08000000; // FILE_FLAG_RANDOM_ACCESS
	}
	if ((value & open_mode::no_recall) != open_mode::none)
	{
		mode.dwFlagsAndAttributes |= 0x00100000; // FILE_FLAG_OPEN_NO_RECALL
	}
	if ((value & open_mode::case_insensitive) == open_mode::none)
	{
		mode.dwFlagsAndAttributes |= 0x01000000; // FILE_FLAG_POSIX_SEMANTICS
	}
	if ((value & open_mode::session_aware) != open_mode::none)
	{
		mode.dwFlagsAndAttributes |= 0x00800000; // FILE_FLAG_SESSION_AWARE
	}
	if ((value & open_mode::temporary) != open_mode::none)
	{
		mode.dwFlagsAndAttributes |= 0x04000000; // FILE_FLAG_DELETE_ON_CLOSE
		mode.dwFlagsAndAttributes |= 0x100;      // FILE_ATTRIBUTE_TEMPORARY
	}
	if ((pm & perms::owner_write) == perms::none)
	{
		mode.dwFlagsAndAttributes |= 1; // FILE_ATTRIBUTE_READONLY
	}
	return mode;
}

template <win32_family family>
inline void *win32_family_create_file_impl(
	::std::conditional_t<family == win32_family::wide_nt, char16_t, char> const *filename_c_str, open_mode_perms ompm)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_family_create_file_internal_impl<family>(filename_c_str, calculate_win32_open_mode(ompm));
}

template <win32_family family>
struct win32_family_open_file_parameter
{
	using family_char_type = ::std::conditional_t<family == win32_family::wide_nt, char16_t, char>;
	open_mode_perms ompm{};
	inline void *operator()(family_char_type const *filename) FAST_IO_HERBCEPTIONS_THROWS
	{
		return win32_family_create_file_impl<family>(filename, ompm);
	}
};

template <win32_family family, typename T>
	requires(::fast_io::constructible_to_os_c_str<T>)
inline void *win32_create_file_impl(T const &t, open_mode_perms ompm) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32_family_api_common<family>(t, win32_family_open_file_parameter<family>{ompm});
}

template <win32_family, typename T>
	requires(::fast_io::constructible_to_os_c_str<T>)
inline void *win32_create_file_at_impl(void *directory_handle, T const &t, open_mode_perms ompm)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::nt::details::nt_create_file_at_impl<false>(directory_handle, t, ompm);
}

template <win32_family, ::std::integral char_type>
inline void *win32_create_file_at_fs_dirent_impl(void *directory_handle, char_type const *filename_c_str,
												 ::std::size_t filename_c_str_len, open_mode_perms ompm)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::nt::details::nt_family_create_file_fs_dirent_impl<false>(directory_handle, filename_c_str,
																					  filename_c_str_len, ompm);
}
} // namespace details

} // namespace fast_io
