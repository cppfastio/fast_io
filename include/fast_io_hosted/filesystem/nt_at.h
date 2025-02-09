#pragma once

namespace fast_io
{

enum class nt_at_flags : ::std::uint_least32_t
{
	eaccess = static_cast<::std::uint_least32_t>(1),
	symlink_nofollow = static_cast<::std::uint_least32_t>(1) << 1,
	no_automount = static_cast<::std::uint_least32_t>(1) << 2,
	removedir = static_cast<::std::uint_least32_t>(1) << 3,
	empty_path = static_cast<::std::uint_least32_t>(1) << 4
};

inline constexpr nt_at_flags operator&(nt_at_flags x, nt_at_flags y) noexcept
{
	using utype = typename ::std::underlying_type<nt_at_flags>::type;
	return static_cast<nt_at_flags>(static_cast<utype>(x) & static_cast<utype>(y));
}

inline constexpr nt_at_flags operator|(nt_at_flags x, nt_at_flags y) noexcept
{
	using utype = typename ::std::underlying_type<nt_at_flags>::type;
	return static_cast<nt_at_flags>(static_cast<utype>(x) | static_cast<utype>(y));
}

inline constexpr nt_at_flags operator^(nt_at_flags x, nt_at_flags y) noexcept
{
	using utype = typename ::std::underlying_type<nt_at_flags>::type;
	return static_cast<nt_at_flags>(static_cast<utype>(x) ^ static_cast<utype>(y));
}

inline constexpr nt_at_flags operator~(nt_at_flags x) noexcept
{
	using utype = typename ::std::underlying_type<nt_at_flags>::type;
	return static_cast<nt_at_flags>(~static_cast<utype>(x));
}

inline constexpr nt_at_flags &operator&=(nt_at_flags &x, nt_at_flags y) noexcept
{
	return x = x & y;
}

inline constexpr nt_at_flags &operator|=(nt_at_flags &x, nt_at_flags y) noexcept
{
	return x = x | y;
}

inline constexpr nt_at_flags &operator^=(nt_at_flags &x, nt_at_flags y) noexcept
{
	return x = x ^ y;
}

namespace win32::nt::details
{

inline constexpr nt_open_mode calculate_nt_delete_flag(nt_at_flags flags) noexcept
{
	nt_open_mode mode{
		.DesiredAccess = 0x00010000,     // FILE_GENERIC_READ
		.FileAttributes = 0x80,          // FILE_ATTRIBUTE_NORMAL
		.ShareAccess = 0x00000007,       // FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
		.CreateDisposition = 0x00000001, /*OPEN_EXISTING	=>	FILE_OPEN*/
		.CreateOptions = 0x00001000      /*FILE_DELETE_ON_CLOSE*/
	};
	if ((flags & nt_at_flags::symlink_nofollow) != nt_at_flags::symlink_nofollow)
	{
		mode.CreateOptions |= 0x00200000; // FILE_FLAG_OPEN_REPARSE_POINT => FILE_OPEN_REPARSE_POINT (0x00200000)
	}
	if ((flags & nt_at_flags::removedir) == nt_at_flags::removedir)
	{
		mode.CreateOptions |= 0x00004000; // FILE_OPEN_FOR_BACKUP_INTENT
		mode.CreateOptions |= 0x00000001; // FILE_DIRECTORY_FILE
	}
	else
	{
		mode.CreateOptions |= 0x00000040; // FILE_NON_DIRECTORY_FILE 0x00000040
	}
	return mode;
}

template <bool zw>
inline void nt_unlinkat_impl(void *dirhd, char16_t const *path_c_str, ::std::size_t path_size, nt_at_flags flags, bool kernel)
{
	auto status{nt_close<zw>(
		nt_call_determine_kernel_callback(dirhd, path_c_str, path_size, kernel, nt_create_callback<zw>{calculate_nt_delete_flag(flags)}))};
	if (status)
	{
		throw_nt_error(status);
	}
}

template <bool zw>
inline void nt_mkdirat_impl(void *dirhd, char16_t const *path_c_str, ::std::size_t path_size, perms pm, bool kernel)
{
	constexpr fast_io::win32::nt::details::nt_open_mode create_dir_mode{
		fast_io::win32::nt::details::calculate_nt_open_mode(
			{fast_io::open_mode::creat | fast_io::open_mode::directory})};
	auto m_dir_mode{create_dir_mode};
	if ((pm & perms::owner_write) == perms::none)
	{
		m_dir_mode.FileAttributes |= 0x00000001; // FILE_ATTRIBUTE_READONLY
	}
	auto status{nt_close<zw>(nt_call_determine_kernel_callback(dirhd, path_c_str, path_size, kernel, nt_create_callback<zw>{m_dir_mode}))};
	if (status)
	{
		throw_nt_error(status);
	}
}

template <bool zw>
inline void nt_faccessat_impl(void *dirhd, char16_t const *path_c_str, ::std::size_t path_size, access_how mode, nt_at_flags flags, bool kernel)
{
	switch (mode)
	{
	case ::fast_io::access_how::f_ok:
		[[fallthrough]];
	case ::fast_io::access_how::x_ok:
		[[fallthrough]];
	case ::fast_io::access_how::w_ok:
		[[fallthrough]];
	case ::fast_io::access_how::r_ok:
		break;
	default:
		throw_nt_error(0xC0000003);
	}

	nt_open_mode md{
		.DesiredAccess = 0x00100000 | 0x0080, // SYNCHRONIZE | FILE_READ_ATTRIBUTES
		.FileAttributes = 0x80,               // FILE_ATTRIBUTE_NORMAL
		.ShareAccess = 0x00000007,            // FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
		.CreateDisposition = 0x00000001,      // OPEN_EXISTING => FILE_OPEN
	};

	if ((flags & nt_at_flags::symlink_nofollow) != nt_at_flags::symlink_nofollow)
	{
		md.CreateOptions |= 0x00200000; // FILE_FLAG_OPEN_REPARSE_POINT => FILE_OPEN_REPARSE_POINT (0x00200000)
	}

	::fast_io::basic_nt_family_file<(zw ? nt_family::zw : nt_family::nt), char> file{
		nt_call_determine_kernel_callback(dirhd, path_c_str, path_size, kernel, nt_create_callback<zw>{md})};

	switch (mode)
	{
	case ::fast_io::access_how::f_ok:
		return;
	case ::fast_io::access_how::x_ok:
		// Like mounted ntfs in wsl, all files and dirs are executable
		return;
	case ::fast_io::access_how::w_ok:
	{
		::fast_io::win32::nt::file_basic_information fbi;
		::fast_io::win32::nt::io_status_block isb;

		auto status{::fast_io::win32::nt::nt_query_information_file<zw>(
			file.native_handle(), __builtin_addressof(isb), __builtin_addressof(fbi),
			static_cast<::std::uint_least32_t>(sizeof(fbi)),
			::fast_io::win32::nt::file_information_class::FileBasicInformation)};

		if (status) [[unlikely]]
		{
			throw_nt_error(status);
		}

		if ((fbi.FileAttributes & 0x1) == 0x1) // READ_ONLY
		{
			throw_nt_error(0xC000001E); // no access
		}
		return;
	}
	case ::fast_io::access_how::r_ok:
		return;
	default:
		::fast_io::unreachable();
	}
}

template <bool zw>
inline void nt_fchmodat_impl(void *dirhd, char16_t const *path_c_str, ::std::size_t path_size, perms pm, nt_at_flags flags, bool kernel)
{
	nt_open_mode md{
		.DesiredAccess = 0x00100000 | 0x00000100 | 0x0080, // SYNCHRONIZE | FILE_WRITE_ATTRIBUTES | FILE_READ_ATTRIBUTES
		.FileAttributes = 0x80,                            // FILE_ATTRIBUTE_NORMAL
		.ShareAccess = 0x00000007,                         // FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
		.CreateDisposition = 0x00000001,                   // OPEN_EXISTING => FILE_OPEN
	};

	if ((flags & nt_at_flags::symlink_nofollow) != nt_at_flags::symlink_nofollow)
	{
		md.CreateOptions |= 0x00200000; // FILE_FLAG_OPEN_REPARSE_POINT => FILE_OPEN_REPARSE_POINT (0x00200000)
	}

	::fast_io::basic_nt_family_file<(zw ? nt_family::zw : nt_family::nt), char> file{
		nt_call_determine_kernel_callback(dirhd, path_c_str, path_size, kernel, nt_create_callback<zw>{md})};

	::fast_io::win32::nt::file_basic_information fbi;
	::fast_io::win32::nt::io_status_block isb;

	auto status{::fast_io::win32::nt::nt_query_information_file<zw>(
		file.native_handle(), __builtin_addressof(isb), __builtin_addressof(fbi),
		static_cast<::std::uint_least32_t>(sizeof(fbi)),
		::fast_io::win32::nt::file_information_class::FileBasicInformation)};

	if (status) [[unlikely]]
	{
		throw_nt_error(status);
	}

	if ((pm & perms::owner_write) == perms::none)
	{
		fbi.FileAttributes |= 0x00000001; // FILE_ATTRIBUTE_READONLY
	}
	else
	{
		fbi.FileAttributes &= ~static_cast<::std::uint_least32_t>(0x00000001);
	}

	// If this is set to 0 in a FILE_BASIC_INFO structure passed to func then none of the attributes are changed.
	fbi.CreationTime = 0;
	fbi.LastAccessTime = 0;
	fbi.LastWriteTime = 0;
	fbi.ChangeTime = 0;

	status = ::fast_io::win32::nt::nt_set_information_file<zw>(
		file.native_handle(), __builtin_addressof(isb), __builtin_addressof(fbi),
		static_cast<::std::uint_least32_t>(sizeof(fbi)),
		::fast_io::win32::nt::file_information_class::FileBasicInformation);

	if (status) [[unlikely]]
	{
		throw_nt_error(status);
	}
}

template <bool zw>
[[noreturn]] inline void nt_fchownat_impl(void *dirhd, char16_t const *path_c_str, ::std::size_t path_size,
										  [[maybe_unused]] ::std::uintmax_t owner, [[maybe_unused]] ::std::uintmax_t group, nt_at_flags flags, bool kernel)
{
	nt_open_mode md{
		.DesiredAccess = 0x00100000 | 0x0080, // SYNCHRONIZE | FILE_READ_ATTRIBUTES
		.FileAttributes = 0x80,               // FILE_ATTRIBUTE_NORMAL
		.ShareAccess = 0x00000007,            // FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
		.CreateDisposition = 0x00000001,      // OPEN_EXISTING => FILE_OPEN
	};

	if ((flags & nt_at_flags::symlink_nofollow) != nt_at_flags::symlink_nofollow)
	{
		md.CreateOptions |= 0x00200000; // FILE_FLAG_OPEN_REPARSE_POINT => FILE_OPEN_REPARSE_POINT (0x00200000)
	}

	// check if file exist
	::fast_io::basic_nt_family_file<(zw ? nt_family::zw : nt_family::nt), char> file{
		nt_call_determine_kernel_callback(dirhd, path_c_str, path_size, kernel, nt_create_callback<zw>{md})};

	// Windows does not use POSIX user group system. stub it and it is perfectly fine.
	// But nt_fchownat, zw_fchownat will not be provided since they do not exist.
	throw_nt_error(0xC0000002);
}

template <bool zw>
inline posix_file_status nt_fstatat_impl(void *dirhd, char16_t const *path_c_str, ::std::size_t path_size, nt_at_flags flags, bool kernel)
{
	nt_open_mode md{
		.DesiredAccess = 0x00100000 | 0x0080, // SYNCHRONIZE | FILE_READ_ATTRIBUTES
		.FileAttributes = 0x80,               // FILE_ATTRIBUTE_NORMAL
		.ShareAccess = 0x00000007,            // FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
		.CreateDisposition = 0x00000001,      // OPEN_EXISTING => FILE_OPEN
	};

	if ((flags & nt_at_flags::symlink_nofollow) != nt_at_flags::symlink_nofollow)
	{
		md.CreateOptions |= 0x00200000; // FILE_FLAG_OPEN_REPARSE_POINT => FILE_OPEN_REPARSE_POINT (0x00200000)
	}

	::fast_io::basic_nt_family_file<(zw ? nt_family::zw : nt_family::nt), char> file{
		nt_call_determine_kernel_callback(dirhd, path_c_str, path_size, kernel, nt_create_callback<zw>{md})};

	return ::fast_io::win32::nt::details::nt_status_impl<(zw ? nt_family::zw : nt_family::nt)>(file.native_handle());
}

template <bool zw>
inline void nt_utimensat_impl(void *dirhd, char16_t const *path_c_str, ::std::size_t path_size, unix_timestamp_option creation_time,
							  unix_timestamp_option last_access_time, unix_timestamp_option last_modification_time, nt_at_flags flags, bool kernel)
{
	nt_open_mode md{
		.DesiredAccess = 0x00100000 | 0x00000100 | 0x0080, // SYNCHRONIZE | FILE_WRITE_ATTRIBUTES | FILE_READ_ATTRIBUTES
		.FileAttributes = 0x80,                            // FILE_ATTRIBUTE_NORMAL
		.ShareAccess = 0x00000007,                         // FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
		.CreateDisposition = 0x00000001,                   // OPEN_EXISTING => FILE_OPEN
	};

	if ((flags & nt_at_flags::symlink_nofollow) != nt_at_flags::symlink_nofollow)
	{
		md.CreateOptions |= 0x00200000; // FILE_FLAG_OPEN_REPARSE_POINT => FILE_OPEN_REPARSE_POINT (0x00200000)
	}

	::fast_io::basic_nt_family_file<(zw ? nt_family::zw : nt_family::nt), char> file{
		nt_call_determine_kernel_callback(dirhd, path_c_str, path_size, kernel, nt_create_callback<zw>{md})};

	::fast_io::win32::nt::file_basic_information fbi;
	::fast_io::win32::nt::io_status_block isb;

	::std::uint_least64_t current_time;

	constexpr bool is_nt_above_win7{
#if (!defined(_WIN32_WINNT) || _WIN32_WINNT >= 0x0602) && !defined(_WIN32_WINDOWS)
		true
#endif
	};

	if constexpr (is_nt_above_win7)
	{
		current_time = static_cast<::std::uint_least64_t>(::fast_io::win32::nt::RtlGetSystemTimePrecise());
	}
	else
	{
		::fast_io::win32::filetime ftm;
		::fast_io::win32::GetSystemTimeAsFileTime(__builtin_addressof(ftm));
		current_time = (static_cast<::std::uint_least64_t>(ftm.dwHighDateTime) << 32u) | ftm.dwLowDateTime;
	}

	constexpr ::std::uint_least64_t mul_factor{::fast_io::uint_least64_subseconds_per_second / 10000000ULL};

	switch (creation_time.flags)
	{
	case ::fast_io::utime_flags::none:
	{
		auto const win32_time{static_cast<::fast_io::win32_timestamp>(creation_time.timestamp)};
		fbi.CreationTime = static_cast<::std::uint_least64_t>(win32_time.seconds) * 10000000ULL +
						   win32_time.subseconds / mul_factor;
		break;
	}
	case ::fast_io::utime_flags::now:
		fbi.CreationTime = current_time;
		break;
	case ::fast_io::utime_flags::omit:
		fbi.CreationTime = 0;
		break;
	default:
		throw_nt_error(0xC0000003);
	}

	switch (last_access_time.flags)
	{
	case ::fast_io::utime_flags::none:
	{
		auto const win32_time{static_cast<::fast_io::win32_timestamp>(last_access_time.timestamp)};
		fbi.LastAccessTime = static_cast<::std::uint_least64_t>(win32_time.seconds) * 10000000ULL +
							 win32_time.subseconds / mul_factor;
		break;
	}
	case ::fast_io::utime_flags::now:
		fbi.LastAccessTime = current_time;
		break;
	case ::fast_io::utime_flags::omit:
		fbi.LastAccessTime = 0;
		break;
	default:
		throw_nt_error(0xC0000003);
	}

	switch (last_modification_time.flags)
	{
	case ::fast_io::utime_flags::none:
	{
		auto const win32_time{static_cast<::fast_io::win32_timestamp>(last_modification_time.timestamp)};
		fbi.LastWriteTime = static_cast<::std::uint_least64_t>(win32_time.seconds) * 10000000ULL +
							win32_time.subseconds / mul_factor;
		break;
	}
	case ::fast_io::utime_flags::now:
		fbi.LastWriteTime = current_time;
		break;
	case ::fast_io::utime_flags::omit:
		fbi.LastWriteTime = 0;
		break;
	default:
		throw_nt_error(0xC0000003);
	}

	// If this is set to 0 in a FILE_BASIC_INFO structure passed to func then none of the attributes are changed.
	fbi.ChangeTime = 0;
	fbi.FileAttributes = 0;

	auto status{::fast_io::win32::nt::nt_set_information_file<zw>(
		file.native_handle(), __builtin_addressof(isb), __builtin_addressof(fbi),
		static_cast<::std::uint_least32_t>(sizeof(fbi)),
		::fast_io::win32::nt::file_information_class::FileBasicInformation)};

	if (status) [[unlikely]]
	{
		throw_nt_error(status);
	}
}

struct nt_create_file_nothrow_common_return_t
{
	void *handle;
	::std::uint_least32_t status;
};

template <bool zw>
inline nt_create_file_nothrow_common_return_t nt_create_file_nothrow_common(void *directory, ::fast_io::win32::nt::unicode_string *relative_path, nt_open_mode const &mode) noexcept
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
	return {handle, status};
}

template <bool zw>
struct nt_create_nothrow_callback
{
	nt_open_mode const &mode;
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline auto operator()(void *directory_handle, ::fast_io::win32::nt::unicode_string *relative_path) const
	{
		return nt_create_file_nothrow_common<zw>(directory_handle, relative_path, mode); // get rid of this pointer
	}
};

template <bool zw>
inline void nt_symlinkat_impl(char16_t const *oldpath_c_str, ::std::size_t oldpath_size,
							  void *newdirhd, char16_t const *newpath_c_str, ::std::size_t newpath_size, bool kernel)
{
#if !defined(_WIN32_WINNT) || _WIN32_WINNT > 0x0600

	bool is_root{};

	char16_t const *oldpath_real_c_str{oldpath_c_str};
	::std::size_t oldpath_real_size{oldpath_size};

	// use nt path in root
	::fast_io::win32::nt::unicode_string us{};
	::fast_io::win32::nt::rtl_unicode_string_unique_ptr us_guard{};

	if (kernel)
	{
		is_root = true;
	}
	else
	{
		auto rpt{::fast_io::win32::nt::RtlDetermineDosPathNameType_U(oldpath_c_str)};
		switch (rpt)
		{
		case rtl_path_type::RtlPathTypeUnknown:
			[[fallthrough]];
		case rtl_path_type::RtlPathTypeRooted:
			[[fallthrough]];
		case rtl_path_type::RtlPathTypeRelative:
			break;
		case rtl_path_type::RtlPathTypeDriveRelative:
		{
			char16_t buf[32767];
			::std::size_t end{::fast_io::win32::nt::RtlGetFullPathName_U(oldpath_c_str, 32767 * sizeof(char16_t), buf, nullptr)};
			if (end == 0) [[unlikely]]
			{
				throw_nt_error(0xC0000004);
			}

			is_root = true;
			char16_t const *tmp_part_name{};
			win32::nt::rtl_relative_name_u tmp_relative_name{};
			::fast_io::win32::nt::details::nt_file_rtl_path(buf, us, tmp_part_name, tmp_relative_name);

			oldpath_real_c_str = us.Buffer;
			oldpath_real_size = us.Length / sizeof(char16_t);
			us_guard.heap_ptr = __builtin_addressof(us); // need free

			break;
		}
		case rtl_path_type::RtlPathTypeUncAbsolute:
			[[fallthrough]];
		case rtl_path_type::RtlPathTypeDriveAbsolute:
			[[fallthrough]];
		case rtl_path_type::RtlPathTypeLocalDevice:
			[[fallthrough]];
		case rtl_path_type::RtlPathTypeRootLocalDevice:
			[[fallthrough]];
		default:
		{
			is_root = true;
			char16_t const *tmp_part_name{};
			win32::nt::rtl_relative_name_u tmp_relative_name{};
			::fast_io::win32::nt::details::nt_file_rtl_path(
				oldpath_real_c_str, us, tmp_part_name, tmp_relative_name);

			oldpath_real_c_str = us.Buffer;
			oldpath_real_size = us.Length / sizeof(char16_t);
			us_guard.heap_ptr = __builtin_addressof(us); // need free
			break;
		}
		}
	}

	nt_open_mode md{
		.DesiredAccess = 0x00100000 | 0x0080, // SYNCHRONIZE | FILE_READ_ATTRIBUTES
		.FileAttributes = 0x80,               // FILE_ATTRIBUTE_NORMAL
		.ShareAccess = 0x00000007,            // FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
		.CreateDisposition = 0x00000001,      // OPEN_EXISTING => FILE_OPEN
		.CreateOptions = 0x00200000           // FILE_FLAG_OPEN_REPARSE_POINT => FILE_OPEN_REPARSE_POINT (0x00200000)
	};

	void *olddirhd{newdirhd};

	if (is_root)
	{
		olddirhd = reinterpret_cast<void *>(::std::ptrdiff_t(-3));
	}

	// Check if exists to get the target file type
	auto [handle, status]{nt_call_determine_kernel_callback(olddirhd, oldpath_real_c_str, oldpath_real_size, kernel, nt_create_nothrow_callback<zw>{md})};

	::fast_io::basic_nt_family_file<(zw ? nt_family::zw : nt_family::nt), char> check_file{};

	if (status)
	{
		if (status != 0xC0000034) [[unlikely]]
		{
			throw_nt_error(status);
		}
	}
	else
	{
		check_file.handle = handle; // need to close
	}

	// Check if exists to set the file type
	::fast_io::win32::nt::io_status_block isb;
	::std::uint_least32_t attribute{};
	if (!status)
	{

		::fast_io::win32::nt::file_basic_information fbi;
		status = ::fast_io::win32::nt::nt_query_information_file<zw>(handle, __builtin_addressof(isb), __builtin_addressof(fbi),
																	 static_cast<::std::uint_least32_t>(sizeof(fbi)),
																	 ::fast_io::win32::nt::file_information_class::FileBasicInformation);
		if (status) [[unlikely]]
		{
			throw_nt_error(status);
		}

		check_file.close();

		attribute = fbi.FileAttributes;
	}

	nt_open_mode symbol_mode{
		.DesiredAccess = 0x00100000 | 0x0100 | 0x00010000, // SYNCHRONIZE | FILE_WRITE_ATTRIBUTES | DELETE
		.FileAttributes = 0x80,                            // FILE_ATTRIBUTE_NORMAL
		.ShareAccess = 0x00000007,                         // FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
		.CreateDisposition = 0x00000002,                   // FILE_OVERWRITE_IF
		.CreateOptions = 0x00000020 | 0x00200000           // FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_REPARSE_POINT
	};

	if ((attribute & 0x10) == 0x10)
	{
		symbol_mode.CreateOptions |= 0x00000001; // directory
	}
	else
	{
		symbol_mode.CreateOptions |= 0x00000040; // file
	}

	constexpr ::std::size_t pathbufferoffset{
		__builtin_offsetof(reparse_data_buffer, u.SymbolicLinkReparseBuffer.PathBuffer)};
	::std::size_t const cbReparseData{pathbufferoffset + oldpath_real_size * sizeof(char16_t) * 2};

	using reparse_data_buffer_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= reparse_data_buffer *;

	::fast_io::details::local_operator_new_array_ptr<char> buffer(cbReparseData);

	reparse_data_buffer_may_alias_ptr pReparseData{reinterpret_cast<reparse_data_buffer_may_alias_ptr>(buffer.get())};

	auto pBufTail{reinterpret_cast<char16_t *>(pReparseData->u.SymbolicLinkReparseBuffer.PathBuffer)};

	pReparseData->ReparseTag = 0xA000000CL; // IO_REPARSE_TAG_SYMLINK
	constexpr ::std::size_t reparsebufferoffset{__builtin_offsetof(reparse_data_buffer, u.GenericReparseBuffer)};
	pReparseData->ReparseDataLength = static_cast<::std::uint_least16_t>(cbReparseData - reparsebufferoffset);
	pReparseData->Reserved = 0;

	pReparseData->u.SymbolicLinkReparseBuffer.SubstituteNameOffset = 0;
	pReparseData->u.SymbolicLinkReparseBuffer.SubstituteNameLength = static_cast<::std::uint_least16_t>(oldpath_real_size * sizeof(char16_t));
	::fast_io::freestanding::non_overlapped_copy_n(oldpath_real_c_str, oldpath_real_size, pBufTail);

	pReparseData->u.SymbolicLinkReparseBuffer.PrintNameOffset = static_cast<::std::uint_least16_t>(oldpath_real_size * sizeof(char16_t));
	pReparseData->u.SymbolicLinkReparseBuffer.PrintNameLength = static_cast<::std::uint_least16_t>(oldpath_real_size * sizeof(char16_t));
	pBufTail += oldpath_real_size;
	::fast_io::freestanding::non_overlapped_copy_n(oldpath_real_c_str, oldpath_real_size, pBufTail);

	// Check whether it is the root directory
	pReparseData->u.SymbolicLinkReparseBuffer.Flags = static_cast<::std::uint_least32_t>(!is_root);

	// You need to check whether or not the authorization process has SeCreateSymbolicLinkPrivilege privilege

	::fast_io::basic_nt_family_file<(zw ? nt_family::zw : nt_family::nt), char> new_file(
		nt_call_determine_kernel_callback(newdirhd, newpath_c_str, newpath_size, kernel, nt_create_callback<zw>{symbol_mode}));

	status = ::fast_io::win32::nt::nt_fs_control_file<zw>(
		new_file.native_handle(),
		nullptr,
		nullptr,
		nullptr,
		__builtin_addressof(isb),
		589988,
		pReparseData,
		static_cast<::std::uint_least32_t>(cbReparseData),
		nullptr,
		0);

	if (status) [[unlikely]]
	{
		::fast_io::win32::nt::file_disposition_information DispInfo{1};
		::fast_io::win32::nt::nt_set_information_file<zw>(new_file.native_handle(), __builtin_addressof(isb), __builtin_addressof(DispInfo),
														  static_cast<::std::uint_least32_t>(sizeof(DispInfo)), ::fast_io::win32::nt::file_information_class::FileDispositionInformation);
		throw_nt_error(status);
	}
#else
	constexpr nt_open_mode symbol_mode{::fast_io::win32::nt::details::calculate_nt_open_mode({::fast_io::open_mode::out, static_cast<perms>(436)})};

	::fast_io::basic_nt_family_file<(zw ? nt_family::zw : nt_family::nt), char8_t> new_file(
		nt_call_determine_kernel_callback(newdirhd, newpath_c_str, newpath_size, kernel, nt_create_callback<zw>{symbol_mode}));

	char8_t buffer[510];
	::fast_io::freestanding::my_memset(buffer, u8' ', 510);
	::fast_io::u8obuffer_view u8obv{buffer, buffer + 510};
	::fast_io::operations::print_freestanding<false>(u8obv, u8"!<symlink>",
													 ::fast_io::mnp::code_cvt(::fast_io::mnp::strvw(oldpath_c_str, oldpath_c_str + oldpath_size)),
													 u8"\nThis is just a text to force symlink file to be 510 bytes long. Do not delete it nor spaces following it.");

	::fast_io::operations::write_all(new_file, buffer, buffer + 510);
#endif
}

template <bool zw>
inline void nt_renameat_impl(void *olddirhd, char16_t const *oldpath_c_str, ::std::size_t oldpath_size,
							 void *newdirhd, char16_t const *newpath_c_str, ::std::size_t newpath_size, bool kernel)
{
	constexpr nt_open_mode md{
		.DesiredAccess = 0x00100000 | 0x0080 | 0x00010000, // SYNCHRONIZE | FILE_READ_ATTRIBUTES | DELETE
		.FileAttributes = 0x80,                            // FILE_ATTRIBUTE_NORMAL
		.ShareAccess = 0x00000007,                         // FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
		.CreateDisposition = 0x00000001,                   // OPEN_EXISTING => FILE_OPEN
	};

	::fast_io::basic_nt_family_file<(zw ? nt_family::zw : nt_family::nt), char> file(
		nt_call_determine_kernel_callback(olddirhd, oldpath_c_str, oldpath_size, kernel, nt_create_callback<zw>{md}));

	nt_call_determine_kernel_callback(
		newdirhd, newpath_c_str, newpath_size, kernel,
		[&](void *directory_hd, win32::nt::unicode_string const *ustr) {
			char16_t const *pth_cstr{ustr->Buffer};
			::std::uint_least32_t pth_size2{ustr->Length};
			::fast_io::details::local_operator_new_array_ptr<char> buffer(sizeof(::fast_io::win32::nt::file_rename_information) + pth_size2 + sizeof(char16_t));

			using file_rename_information_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
				[[__gnu__::__may_alias__]]
#endif
				= file_rename_information *;

			::fast_io::win32::nt::file_rename_information *info{reinterpret_cast<file_rename_information_may_alias_ptr>(buffer.get())};
			info->ReplaceIfExists = 1;
			info->RootDirectory = directory_hd;
			info->FileNameLength = pth_size2;
			::fast_io::freestanding::my_memcpy(info->FileName, pth_cstr, pth_size2 + sizeof(char16_t));

			::fast_io::win32::nt::io_status_block block;
			::std::uint_least32_t status{::fast_io::win32::nt::nt_set_information_file<zw>(
				file.handle, __builtin_addressof(block), info,
				static_cast<::std::uint_least32_t>(sizeof(::fast_io::win32::nt::file_rename_information) + pth_size2 + sizeof(char16_t)), file_information_class::FileRenameInformation)};
			if (status) [[unlikely]]
			{
				throw_nt_error(status);
			}
		});
}

inline constexpr nt_open_mode calculate_nt_link_flag(nt_at_flags flags) noexcept
{
	nt_open_mode mode{
		.DesiredAccess = 0x00100000 | 0x0080, // SYNCHRONIZE | FILE_WRITE_ATTRIBUTES | FILE_READ_ATTRIBUTES
		.FileAttributes = 0x80,               // FILE_ATTRIBUTE_NORMAL
		.ShareAccess = 0x00000007,            // FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
		.CreateDisposition = 0x00000001,      /*OPEN_EXISTING	=>	FILE_OPEN*/
	};
	if ((flags & nt_at_flags::symlink_nofollow) != nt_at_flags::symlink_nofollow)
	{
		mode.CreateOptions |= 0x00200000; // FILE_FLAG_OPEN_REPARSE_POINT => FILE_OPEN_REPARSE_POINT (0x00200000)
	}
	return mode;
}

template <bool zw>
inline void nt_linkat_impl(void *olddirhd, char16_t const *oldpath_c_str, ::std::size_t oldpath_size, void *newdirhd,
						   char16_t const *newpath_c_str, ::std::size_t newpath_size, nt_at_flags flags, bool kernel)
{
	nt_open_mode const md{calculate_nt_link_flag(flags)};
	::fast_io::basic_nt_family_file<(zw ? nt_family::zw : nt_family::nt), char> basic_file{};
	::fast_io::basic_nt_family_io_observer<(zw ? nt_family::zw : nt_family::nt), char> file{};

	if ((flags & nt_at_flags::empty_path) == nt_at_flags::empty_path && oldpath_size == 0)
	{
		file = ::fast_io::basic_nt_family_io_observer<(zw ? nt_family::zw : nt_family::nt), char>{olddirhd};
	}
	else
	{
		basic_file = ::fast_io::basic_nt_family_file<(zw ? nt_family::zw : nt_family::nt), char>{
			nt_call_determine_kernel_callback(olddirhd, oldpath_c_str, oldpath_size, kernel, nt_create_callback<zw>{md})};
		file = basic_file;
	}

	nt_call_determine_kernel_callback(
		newdirhd, newpath_c_str, newpath_size, kernel,
		[&](void *directory_hd, win32::nt::unicode_string const *ustr) {
			char16_t const *pth_cstr{ustr->Buffer};
			::std::uint_least32_t pth_size2{ustr->Length};
			::fast_io::details::local_operator_new_array_ptr<char> buffer(sizeof(::fast_io::win32::nt::file_link_information) + pth_size2 + sizeof(char16_t));

			using file_link_information_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
				[[__gnu__::__may_alias__]]
#endif
				= file_link_information *;

			::fast_io::win32::nt::file_link_information *info{reinterpret_cast<file_link_information_may_alias_ptr>(buffer.get())};
			info->ReplaceIfExists = 0;
			info->RootDirectory = directory_hd;
			info->FileNameLength = pth_size2;

			::fast_io::freestanding::my_memcpy(info->FileName, pth_cstr, pth_size2 + sizeof(char16_t));

			::fast_io::win32::nt::io_status_block block;

			::std::uint_least32_t status{::fast_io::win32::nt::nt_set_information_file<zw>(
				file.handle, __builtin_addressof(block), info,
				static_cast<::std::uint_least32_t>(sizeof(::fast_io::win32::nt::file_link_information) + pth_size2 + sizeof(char16_t)), file_information_class::FileLinkInformation)};

			if (status) [[unlikely]]
			{
				throw_nt_error(status);
			}
		});
}

template <bool zw, ::fast_io::details::posix_api_22 dsp, typename... Args>
inline auto nt22_api_dispatcher(void *olddirhd, char16_t const *oldpath_c_str, ::std::size_t oldpath_size,
								void *newdirhd, char16_t const *newpath_c_str, ::std::size_t newpath_size, Args... args)
{
	if constexpr (dsp == ::fast_io::details::posix_api_22::renameat)
	{
		nt_renameat_impl<zw>(olddirhd, oldpath_c_str, oldpath_size, newdirhd, newpath_c_str, newpath_size, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_22::linkat)
	{
		nt_linkat_impl<zw>(olddirhd, oldpath_c_str, oldpath_size, newdirhd, newpath_c_str, newpath_size, args...);
	}
}

template <bool zw, ::fast_io::details::posix_api_12 dsp, typename... Args>
inline auto nt12_api_dispatcher(char16_t const *oldpath_c_str, ::std::size_t oldpath_size,
								void *newdirhd, char16_t const *newpath_c_str, ::std::size_t newpath_size, Args... args)
{
	if constexpr (dsp == ::fast_io::details::posix_api_12::symlinkat)
	{
		nt_symlinkat_impl<zw>(oldpath_c_str, oldpath_size, newdirhd, newpath_c_str, newpath_size, args...);
	}
}

template <bool zw, ::fast_io::details::posix_api_1x dsp, typename... Args>
inline auto nt1x_api_dispatcher(void *dir_handle, char16_t const *path_c_str, ::std::size_t path_size, Args... args)
{

	if constexpr (dsp == ::fast_io::details::posix_api_1x::faccessat)
	{
		nt_faccessat_impl<zw>(dir_handle, path_c_str, path_size, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::fchmodat)
	{
		nt_fchmodat_impl<zw>(dir_handle, path_c_str, path_size, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::fchownat)
	{
		nt_fchownat_impl<zw>(dir_handle, path_c_str, path_size, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::fstatat)
	{
		return nt_fstatat_impl<zw>(dir_handle, path_c_str, path_size, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::mkdirat)
	{
		nt_mkdirat_impl<zw>(dir_handle, path_c_str, path_size, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::unlinkat)
	{
		nt_unlinkat_impl<zw>(dir_handle, path_c_str, path_size, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::utimensat)
	{
		nt_utimensat_impl<zw>(dir_handle, path_c_str, path_size, args...);
	}
}

template <nt_family family, ::fast_io::details::posix_api_1x dsp, typename path_type, typename... Args>
inline auto nt_deal_with1x(void *dir_handle, path_type const &path, Args... args)
{
	return nt_api_common(
		path, [&](char16_t const *path_c_str, ::std::size_t path_size) {
			return nt1x_api_dispatcher<family == nt_family::zw, dsp>(dir_handle, path_c_str, path_size, args...);
		});
}

template <nt_family family, ::fast_io::details::posix_api_12 dsp, ::fast_io::constructible_to_os_c_str old_path_type,
		  ::fast_io::constructible_to_os_c_str new_path_type, typename... Args>
inline auto nt_deal_with12(old_path_type const &oldpath, void *newdirfd, new_path_type const &newpath, Args... args)
{
	return nt_api_common(
		oldpath,
		[&](char16_t const *oldpath_c_str, ::std::size_t oldpath_size) {
			return nt_api_common(
				newpath, [&](char16_t const *newpath_c_str, ::std::size_t newpath_size) {
					return nt12_api_dispatcher<family == nt_family::zw, dsp>(oldpath_c_str, oldpath_size, newdirfd, newpath_c_str, newpath_size, args...);
				});
		});
}

template <nt_family family, ::fast_io::details::posix_api_22 dsp, typename oldpath_type, typename newpath_type, typename... Args>
inline auto nt_deal_with22(void *olddirhd, oldpath_type const &oldpath, void *newdirhd, newpath_type const &newpath, Args... args)
{
	return nt_api_common(oldpath,
						 [&](char16_t const *oldpath_c_str, ::std::size_t oldpath_size) {
							 return nt_api_common(newpath,
												  [&](char16_t const *newpath_c_str, ::std::size_t newpath_size) {
													  return nt22_api_dispatcher<family == nt_family::zw, dsp>(olddirhd, oldpath_c_str, oldpath_size, newdirhd,
																											   newpath_c_str, newpath_size, args...);
												  });
						 });
}

} // namespace win32::nt::details

// 1x
template <nt_family family, bool kernel, ::fast_io::constructible_to_os_c_str path_type>
	requires(family == nt_family::nt || family == nt_family::zw)
inline void nt_family_nt_faccessat(nt_at_entry ent, path_type const &path, access_how mode, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<family, details::posix_api_1x::faccessat>(ent.handle, path, mode, flags, kernel);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void nt_faccessat(nt_at_entry ent, path_type const &path, access_how mode, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt, details::posix_api_1x::faccessat>(ent.handle, path, mode, flags, false);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void zw_faccessat(nt_at_entry ent, path_type const &path, access_how mode, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::zw, details::posix_api_1x::faccessat>(ent.handle, path, mode, flags, false);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void nt_faccessat(io_kernel_t, nt_at_entry ent, path_type const &path, access_how mode, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt, details::posix_api_1x::faccessat>(ent.handle, path, mode, flags, true);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void zw_faccessat(io_kernel_t, nt_at_entry ent, path_type const &path, access_how mode, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::zw, details::posix_api_1x::faccessat>(ent.handle, path, mode, flags, true);
}

template <nt_family family, bool kernel, ::fast_io::constructible_to_os_c_str path_type>
	requires(family == nt_family::nt || family == nt_family::zw)
inline void nt_family_nt_fchmodat(nt_at_entry ent, path_type const &path, perms mode, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<family, details::posix_api_1x::fchmodat>(ent.handle, path, mode, flags, kernel);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void nt_fchmodat(nt_at_entry ent, path_type const &path, perms mode, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt, details::posix_api_1x::fchmodat>(ent.handle, path, mode, flags, false);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void zw_fchmodat(nt_at_entry ent, path_type const &path, perms mode, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::zw, details::posix_api_1x::fchmodat>(ent.handle, path, mode, flags, false);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void nt_fchmodat(io_kernel_t, nt_at_entry ent, path_type const &path, perms mode, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt, details::posix_api_1x::fchmodat>(ent.handle, path, mode, flags, true);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void zw_fchmodat(io_kernel_t, nt_at_entry ent, path_type const &path, perms mode, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::zw, details::posix_api_1x::fchmodat>(ent.handle, path, mode, flags, true);
}

template <nt_family family, bool kernel, ::fast_io::constructible_to_os_c_str path_type>
	requires(family == nt_family::nt || family == nt_family::zw)
inline void nt_family_nt_fchownat(nt_at_entry ent, path_type const &path, ::std::uintmax_t owner, ::std::uintmax_t group, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<family, details::posix_api_1x::fchownat>(ent.handle, path, owner, group, flags, kernel);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void nt_fchownat(nt_at_entry ent, path_type const &path, ::std::uintmax_t owner, ::std::uintmax_t group, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt, details::posix_api_1x::fchownat>(ent.handle, path, owner, group, flags, false);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void zw_fchownat(nt_at_entry ent, path_type const &path, ::std::uintmax_t owner, ::std::uintmax_t group, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::zw, details::posix_api_1x::fchownat>(ent.handle, path, owner, group, flags, false);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void nt_fchownat(io_kernel_t, nt_at_entry ent, path_type const &path, ::std::uintmax_t owner, ::std::uintmax_t group, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt, details::posix_api_1x::fchownat>(ent.handle, path, owner, group, flags, true);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void zw_fchownat(io_kernel_t, nt_at_entry ent, path_type const &path, ::std::uintmax_t owner, ::std::uintmax_t group, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::zw, details::posix_api_1x::fchownat>(ent.handle, path, owner, group, flags, true);
}

template <nt_family family, bool kernel, ::fast_io::constructible_to_os_c_str path_type>
	requires(family == nt_family::nt || family == nt_family::zw)
inline posix_file_status nt_family_nt_fstatat(nt_at_entry ent, path_type const &path, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	return ::fast_io::win32::nt::details::nt_deal_with1x<family, details::posix_api_1x::fstatat>(ent.handle, path, flags, kernel);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline posix_file_status nt_fstatat(nt_at_entry ent, path_type const &path, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	return ::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt, details::posix_api_1x::fstatat>(ent.handle, path, flags, false);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline posix_file_status zw_fstatat(nt_at_entry ent, path_type const &path, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	return ::fast_io::win32::nt::details::nt_deal_with1x<nt_family::zw, details::posix_api_1x::fstatat>(ent.handle, path, flags, false);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline posix_file_status nt_fstatat(io_kernel_t, nt_at_entry ent, path_type const &path, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	return ::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt, details::posix_api_1x::fstatat>(ent.handle, path, flags, true);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline posix_file_status zw_fstatat(io_kernel_t, nt_at_entry ent, path_type const &path, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	return ::fast_io::win32::nt::details::nt_deal_with1x<nt_family::zw, details::posix_api_1x::fstatat>(ent.handle, path, flags, true);
}

template <nt_family family, bool kernel, ::fast_io::constructible_to_os_c_str path_type>
	requires(family == nt_family::nt || family == nt_family::zw)
inline void nt_family_nt_utimensat(nt_at_entry ent, path_type const &path, unix_timestamp_option creation_time,
								   unix_timestamp_option last_access_time, unix_timestamp_option last_modification_time,
								   nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<family, details::posix_api_1x::utimensat>(ent.handle, path, creation_time, last_access_time, last_modification_time, flags, kernel);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void nt_utimensat(nt_at_entry ent, path_type const &path, unix_timestamp_option creation_time,
						 unix_timestamp_option last_access_time, unix_timestamp_option last_modification_time,
						 nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt, details::posix_api_1x::utimensat>(ent.handle, path, creation_time, last_access_time, last_modification_time, flags, false);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void zw_utimensat(nt_at_entry ent, path_type const &path, unix_timestamp_option creation_time,
						 unix_timestamp_option last_access_time, unix_timestamp_option last_modification_time,
						 nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::zw, details::posix_api_1x::utimensat>(ent.handle, path, creation_time, last_access_time, last_modification_time, flags, false);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void nt_utimensat(io_kernel_t, nt_at_entry ent, path_type const &path, unix_timestamp_option creation_time,
						 unix_timestamp_option last_access_time, unix_timestamp_option last_modification_time,
						 nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt, details::posix_api_1x::utimensat>(ent.handle, path, creation_time, last_access_time, last_modification_time, flags, true);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void zw_utimensat(io_kernel_t, nt_at_entry ent, path_type const &path, unix_timestamp_option creation_time,
						 unix_timestamp_option last_access_time, unix_timestamp_option last_modification_time,
						 nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::zw, details::posix_api_1x::utimensat>(ent.handle, path, creation_time, last_access_time, last_modification_time, flags, true);
}

template <nt_family family, bool kernel, ::fast_io::constructible_to_os_c_str path_type>
	requires(family == nt_family::nt || family == nt_family::zw)
inline void nt_family_mkdirat(nt_at_entry ent, path_type &&path, perms pm = static_cast<perms>(436))
{
	::fast_io::win32::nt::details::nt_deal_with1x<family, details::posix_api_1x::mkdirat>(ent.handle, path, pm, kernel);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void nt_mkdirat(nt_at_entry ent, path_type &&path, perms pm = static_cast<perms>(436))
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt, details::posix_api_1x::mkdirat>(ent.handle, path, pm, false);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void zw_mkdirat(nt_at_entry ent, path_type &&path, perms pm = static_cast<perms>(436))
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::zw, details::posix_api_1x::mkdirat>(ent.handle, path, pm, false);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void nt_mkdirat(io_kernel_t, nt_at_entry ent, path_type &&path, perms pm = static_cast<perms>(436))
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt, details::posix_api_1x::mkdirat>(ent.handle, path, pm, true);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void zw_mkdirat(io_kernel_t, nt_at_entry ent, path_type &&path, perms pm = static_cast<perms>(436))
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::zw, details::posix_api_1x::mkdirat>(ent.handle, path, pm, true);
}

template <nt_family family, bool kernel, ::fast_io::constructible_to_os_c_str path_type>
	requires(family == nt_family::nt || family == nt_family::zw)
inline void nt_family_unlinkat(nt_at_entry ent, path_type &&path, nt_at_flags flags = {})
{
	::fast_io::win32::nt::details::nt_deal_with1x<family, details::posix_api_1x::unlinkat>(ent.handle, path, flags, kernel);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void nt_unlinkat(nt_at_entry ent, path_type &&path, nt_at_flags flags = {})
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt, details::posix_api_1x::unlinkat>(ent.handle, path, flags, false);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void zw_unlinkat(nt_at_entry ent, path_type &&path, nt_at_flags flags = {})
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::zw, details::posix_api_1x::unlinkat>(ent.handle, path, flags, false);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void nt_unlinkat(io_kernel_t, nt_at_entry ent, path_type &&path, nt_at_flags flags = {})
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt, details::posix_api_1x::unlinkat>(ent.handle, path, flags, true);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void zw_unlinkat(io_kernel_t, nt_at_entry ent, path_type &&path, nt_at_flags flags = {})
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::zw, details::posix_api_1x::unlinkat>(ent.handle, path, flags, true);
}

// 12
template <nt_family family, bool kernel, ::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
	requires(family == nt_family::nt || family == nt_family::zw)
inline void nt_family_symlinkat(old_path_type &&oldpath, nt_at_entry newdirfd, new_path_type &&newpath)
{
	::fast_io::win32::nt::details::nt_deal_with12<family, details::posix_api_12::symlinkat>(oldpath, newdirfd.handle, newpath, kernel);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void nt_symlinkat(old_path_type &&oldpath, nt_at_entry newdirfd, new_path_type &&newpath)
{
	::fast_io::win32::nt::details::nt_deal_with12<nt_family::nt, details::posix_api_12::symlinkat>(oldpath, newdirfd.handle, newpath, false);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void zw_symlinkat(old_path_type &&oldpath, nt_at_entry newdirfd, new_path_type &&newpath)
{
	::fast_io::win32::nt::details::nt_deal_with12<nt_family::zw, details::posix_api_12::symlinkat>(oldpath, newdirfd.handle, newpath, false);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void nt_symlinkat(io_kernel_t, old_path_type &&oldpath, nt_at_entry newdirfd, new_path_type &&newpath)
{
	::fast_io::win32::nt::details::nt_deal_with12<nt_family::nt, details::posix_api_12::symlinkat>(oldpath, newdirfd.handle, newpath, true);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void zw_symlinkat(io_kernel_t, old_path_type &&oldpath, nt_at_entry newdirfd, new_path_type &&newpath)
{
	::fast_io::win32::nt::details::nt_deal_with12<nt_family::zw, details::posix_api_12::symlinkat>(oldpath, newdirfd.handle, newpath, true);
}

// 22
template <nt_family family, bool kernel, ::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void nt_family_renameat(nt_at_entry oldent, old_path_type &&oldpath, nt_at_entry newent, new_path_type &&newpath)
{
	::fast_io::win32::nt::details::nt_deal_with22<family, ::fast_io::details::posix_api_22::renameat>(oldent.handle, oldpath,
																									  newent.handle, newpath, kernel);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void nt_renameat(nt_at_entry oldent, old_path_type &&oldpath, nt_at_entry newent, new_path_type &&newpath)
{
	::fast_io::win32::nt::details::nt_deal_with22<nt_family::nt, ::fast_io::details::posix_api_22::renameat>(oldent.handle, oldpath,
																											 newent.handle, newpath, false);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void zw_renameat(nt_at_entry oldent, old_path_type &&oldpath, nt_at_entry newent, new_path_type &&newpath)
{
	::fast_io::win32::nt::details::nt_deal_with22<nt_family::zw, ::fast_io::details::posix_api_22::renameat>(oldent.handle, oldpath,
																											 newent.handle, newpath, false);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void nt_renameat(io_kernel_t, nt_at_entry oldent, old_path_type &&oldpath, nt_at_entry newent, new_path_type &&newpath)
{
	::fast_io::win32::nt::details::nt_deal_with22<nt_family::nt, ::fast_io::details::posix_api_22::renameat>(oldent.handle, oldpath,
																											 newent.handle, newpath, true);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void zw_renameat(io_kernel_t, nt_at_entry oldent, old_path_type &&oldpath, nt_at_entry newent, new_path_type &&newpath)
{
	::fast_io::win32::nt::details::nt_deal_with22<nt_family::zw, ::fast_io::details::posix_api_22::renameat>(oldent.handle, oldpath,
																											 newent.handle, newpath, true);
}

template <nt_family family, bool kernel, ::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void nt_family_linkat(nt_at_entry oldent, old_path_type &&oldpath, nt_at_entry newent, new_path_type &&newpath, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with22<family, ::fast_io::details::posix_api_22::linkat>(oldent.handle, oldpath,
																									newent.handle, newpath, flags, kernel);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void nt_linkat(nt_at_entry oldent, old_path_type &&oldpath, nt_at_entry newent, new_path_type &&newpath, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with22<nt_family::nt, ::fast_io::details::posix_api_22::linkat>(oldent.handle, oldpath,
																										   newent.handle, newpath, flags, false);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void zw_linkat(nt_at_entry oldent, old_path_type &&oldpath, nt_at_entry newent, new_path_type &&newpath, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with22<nt_family::zw, ::fast_io::details::posix_api_22::linkat>(oldent.handle, oldpath,
																										   newent.handle, newpath, flags, false);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void nt_linkat(io_kernel_t, nt_at_entry oldent, old_path_type &&oldpath, nt_at_entry newent, new_path_type &&newpath, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with22<nt_family::nt, ::fast_io::details::posix_api_22::linkat>(oldent.handle, oldpath,
																										   newent.handle, newpath, flags, true);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void zw_linkat(io_kernel_t, nt_at_entry oldent, old_path_type &&oldpath, nt_at_entry newent, new_path_type &&newpath, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with22<nt_family::zw, ::fast_io::details::posix_api_22::linkat>(oldent.handle, oldpath,
																										   newent.handle, newpath, flags, true);
}

#if !defined(_WIN32_WINDOWS) && !defined(__CYGWIN__) && !defined(__WINE__)
using native_at_flags = nt_at_flags;

template <::fast_io::constructible_to_os_c_str path_type>
inline void native_faccessat(nt_at_entry ent, path_type const &path, access_how mode, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt, details::posix_api_1x::faccessat>(ent.handle, path, mode, flags, false);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void native_fchmodat(nt_at_entry ent, path_type const &path, perms mode, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt, details::posix_api_1x::fchmodat>(ent.handle, path, mode, flags, false);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void native_fchownat(nt_at_entry ent, path_type const &path, ::std::uintmax_t owner, ::std::uintmax_t group, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt, details::posix_api_1x::fchownat>(ent.handle, path, owner, group, flags, false);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline posix_file_status native_fstatat(nt_at_entry ent, path_type const &path, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	return ::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt, details::posix_api_1x::fstatat>(ent.handle, path, flags, false);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void native_utimensat(nt_at_entry ent, path_type const &path, unix_timestamp_option creation_time,
							 unix_timestamp_option last_access_time, unix_timestamp_option last_modification_time,
							 nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt, details::posix_api_1x::utimensat>(ent.handle, path, creation_time, last_access_time, last_modification_time, flags, false);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void native_mkdirat(nt_at_entry ent, path_type &&path, perms pm = static_cast<perms>(436))
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt, details::posix_api_1x::mkdirat>(ent.handle, path, pm, false);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void native_unlinkat(nt_at_entry ent, path_type &&path, nt_at_flags flags = {})
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt, ::fast_io::details::posix_api_1x::unlinkat>(
		ent.handle, path, flags, false);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void native_symlinkat(old_path_type &&oldpath, nt_at_entry newdirfd, new_path_type &&newpath)
{
	::fast_io::win32::nt::details::nt_deal_with12<nt_family::nt, details::posix_api_12::symlinkat>(oldpath, newdirfd.handle, newpath, false);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void native_renameat(nt_at_entry oldent, old_path_type &&oldpath, nt_at_entry newent, new_path_type &&newpath)
{
	::fast_io::win32::nt::details::nt_deal_with22<nt_family::nt, ::fast_io::details::posix_api_22::renameat>(oldent.handle, oldpath,
																											 newent.handle, newpath, false);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void native_linkat(nt_at_entry oldent, old_path_type &&oldpath, nt_at_entry newent, new_path_type &&newpath, nt_at_flags flags = nt_at_flags::symlink_nofollow)
{
	::fast_io::win32::nt::details::nt_deal_with22<nt_family::nt, ::fast_io::details::posix_api_22::linkat>(oldent.handle, oldpath,
																										   newent.handle, newpath, flags, false);
}
#endif
} // namespace fast_io
