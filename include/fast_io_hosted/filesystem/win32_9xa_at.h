#pragma once

namespace fast_io
{
enum class win32_9xa_at_flags : ::std::uint_least32_t
{
	eaccess = static_cast<::std::uint_least32_t>(1),
	symlink_nofollow = static_cast<::std::uint_least32_t>(1) << 1,
	no_automount = static_cast<::std::uint_least32_t>(1) << 2,
	removedir = static_cast<::std::uint_least32_t>(1) << 3,
	empty_path = static_cast<::std::uint_least32_t>(1) << 4
};

inline constexpr win32_9xa_at_flags operator&(win32_9xa_at_flags x, win32_9xa_at_flags y) noexcept
{
	using utype = typename ::std::underlying_type<win32_9xa_at_flags>::type;
	return static_cast<win32_9xa_at_flags>(static_cast<utype>(x) & static_cast<utype>(y));
}

inline constexpr win32_9xa_at_flags operator|(win32_9xa_at_flags x, win32_9xa_at_flags y) noexcept
{
	using utype = typename ::std::underlying_type<win32_9xa_at_flags>::type;
	return static_cast<win32_9xa_at_flags>(static_cast<utype>(x) | static_cast<utype>(y));
}

inline constexpr win32_9xa_at_flags operator^(win32_9xa_at_flags x, win32_9xa_at_flags y) noexcept
{
	using utype = typename ::std::underlying_type<win32_9xa_at_flags>::type;
	return static_cast<win32_9xa_at_flags>(static_cast<utype>(x) ^ static_cast<utype>(y));
}

inline constexpr win32_9xa_at_flags operator~(win32_9xa_at_flags x) noexcept
{
	using utype = typename ::std::underlying_type<win32_9xa_at_flags>::type;
	return static_cast<win32_9xa_at_flags>(~static_cast<utype>(x));
}

inline constexpr win32_9xa_at_flags &operator&=(win32_9xa_at_flags &x, win32_9xa_at_flags y) noexcept
{
	return x = x & y;
}

inline constexpr win32_9xa_at_flags &operator|=(win32_9xa_at_flags &x, win32_9xa_at_flags y) noexcept
{
	return x = x | y;
}

inline constexpr win32_9xa_at_flags &operator^=(win32_9xa_at_flags &x, win32_9xa_at_flags y) noexcept
{
	return x = x ^ y;
}

namespace win32::details
{
inline void win32_9xa_unlinkat_impl(::fast_io::win32_9xa_dir_handle const &dirhd, char8_t const *path_c_str, ::std::size_t path_size, win32_9xa_at_flags flags)
{
	auto file_or_path{concat_tlc_win32_9xa_path_uncheck_whether_exist(dirhd, path_c_str, path_size)};

	switch (flags)
	{
	case ::fast_io::win32_9xa_at_flags::removedir:
		if (!::fast_io::win32::RemoveDirectoryA(reinterpret_cast<char const *>(file_or_path.c_str()))) [[unlikely]]
		{
			throw_win32_error();
		}
		break;
	default:
		if (!::fast_io::win32::DeleteFileA(reinterpret_cast<char const *>(file_or_path.c_str()))) [[unlikely]]
		{
			throw_win32_error();
		}
		break;
	}
}

inline void win32_9xa_mkdirat_impl(::fast_io::win32_9xa_dir_handle const &dirhd, char8_t const *path_c_str, ::std::size_t path_size, perms pm)
{
	auto path{concat_tlc_win32_9xa_path_uncheck_whether_exist(dirhd, path_c_str, path_size)};

	if (!::fast_io::win32::CreateDirectoryA(reinterpret_cast<char const *>(path.c_str()), nullptr)) [[unlikely]]
	{
		throw_win32_error();
	}

	auto const attr{::fast_io::win32::GetFileAttributesA(reinterpret_cast<char const *>(path.c_str()))};

	if (attr == static_cast<::std::uint_least32_t>(-1)) [[unlikely]]
	{
		throw_win32_error(0x2);
	}

	if ((pm & perms::owner_write) == perms::none)
	{
		if (!::fast_io::win32::SetFileAttributesA(reinterpret_cast<char const *>(path.c_str()), attr | 0x00000001)) [[unlikely]]
		{
			throw_win32_error();
		}
	}
	else
	{
		if (!::fast_io::win32::SetFileAttributesA(reinterpret_cast<char const *>(path.c_str()), attr & ~static_cast<::std::uint_least32_t>(0x00000001))) [[unlikely]]
		{
			throw_win32_error();
		}
	}
}

inline void win32_9xa_faccessat_impl(::fast_io::win32_9xa_dir_handle const &dirhd, char8_t const *path_c_str, ::std::size_t path_size, access_how mode)
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
		throw_win32_error(0x57);
	}

	auto path{concat_tlc_win32_9xa_path_uncheck_whether_exist(dirhd, path_c_str, path_size)};

	auto const attr{::fast_io::win32::GetFileAttributesA(reinterpret_cast<char const *>(path.c_str()))};

	if (attr == static_cast<::std::uint_least32_t>(-1)) [[unlikely]]
	{
		throw_win32_error(0x2);
	}

	switch (mode)
	{
	case ::fast_io::access_how::f_ok:
		return;
	case ::fast_io::access_how::x_ok:
		// Like mounted ntfs in wsl, all files and dirs are executable
		return;
	case ::fast_io::access_how::w_ok:
	{
		if ((attr & 0x00000001) == 0x00000001) [[unlikely]]
		{
			throw_win32_error(0x57); // no access
		}
		return;
	}
	case ::fast_io::access_how::r_ok:
		return;
	default:
		::fast_io::unreachable();
	}
}

inline void win32_9xa_fchmodat_impl(::fast_io::win32_9xa_dir_handle const &dirhd, char8_t const *path_c_str, ::std::size_t path_size, perms pm)
{
	auto path{concat_tlc_win32_9xa_path_uncheck_whether_exist(dirhd, path_c_str, path_size)};

	auto const attr{::fast_io::win32::GetFileAttributesA(reinterpret_cast<char const *>(path.c_str()))};

	if (attr == static_cast<::std::uint_least32_t>(-1)) [[unlikely]]
	{
		throw_win32_error(0x2);
	}

	if ((pm & perms::owner_write) == perms::none)
	{
		if (!::fast_io::win32::SetFileAttributesA(reinterpret_cast<char const *>(path.c_str()), attr | 0x00000001))
		{
			throw_win32_error();
		}
	}
	else
	{
		if (!::fast_io::win32::SetFileAttributesA(reinterpret_cast<char const *>(path.c_str()), attr & ~static_cast<::std::uint_least32_t>(0x00000001)))
		{
			throw_win32_error();
		}
	}
}

[[noreturn]] inline void win32_9xa_fchownat_impl(::fast_io::win32_9xa_dir_handle const &dirhd, char8_t const *path_c_str, ::std::size_t path_size,
												 [[maybe_unused]] ::std::uintmax_t owner, [[maybe_unused]] ::std::uintmax_t group)
{
	auto path{concat_tlc_win32_9xa_path_uncheck_whether_exist(dirhd, path_c_str, path_size)};

	auto const attr{::fast_io::win32::GetFileAttributesA(reinterpret_cast<char const *>(path.c_str()))};

	if (attr == static_cast<::std::uint_least32_t>(-1)) [[unlikely]]
	{
		throw_win32_error(0x2);
	}

	// Windows does not use POSIX user group system. stub it and it is perfectly fine.
	// But nt_fchownat, zw_fchownat will not be provided since they do not exist.
	throw_win32_error(0x1);
}

inline constexpr auto calculate_win32_9xa_readonly_open_mode(bool write_attribute, bool fsymlink_nofollow) noexcept
{
	::fast_io::details::win32_open_mode md;
	md.dwShareMode = 3;
	md.dwDesiredAccess = 0x40000000 | 0x80000000 | 0x0080;
	md.dwCreationDisposition = 3;

	if (write_attribute)
	{
		md.dwDesiredAccess |= 0x0100;
	}

	if (fsymlink_nofollow)
	{
		md.dwFlagsAndAttributes = 0x00200000;
	}
	return md;
}

inline posix_file_status win32_9xa_fstatat_impl(::fast_io::win32_9xa_dir_handle const &dirhd, char8_t const *path_c_str, ::std::size_t path_size, win32_9xa_at_flags flags)
{
	auto path{concat_tlc_win32_9xa_path_uncheck_whether_exist(dirhd, path_c_str, path_size)};
	auto md{calculate_win32_9xa_readonly_open_mode(false, (flags & win32_9xa_at_flags::symlink_nofollow) != win32_9xa_at_flags::symlink_nofollow)};
	::fast_io::win32_file_9xa f{::fast_io::details::win32_family_create_file_internal_impl<win32_family::ansi_9x>(reinterpret_cast<char const *>(path.c_str()), md)};

	return ::fast_io::win32::details::win32_status_impl(f.native_handle());
}

inline void win32_9xa_utimensat_impl(::fast_io::win32_9xa_dir_handle const &dirhd, char8_t const *path_c_str, ::std::size_t path_size, unix_timestamp_option creation_time,
									 unix_timestamp_option last_access_time, unix_timestamp_option last_modification_time, win32_9xa_at_flags flags)
{
	auto path{concat_tlc_win32_9xa_path_uncheck_whether_exist(dirhd, path_c_str, path_size)};
	auto md{calculate_win32_9xa_readonly_open_mode(true, (flags & win32_9xa_at_flags::symlink_nofollow) != win32_9xa_at_flags::symlink_nofollow)};
	::fast_io::win32_file_9xa f{::fast_io::details::win32_family_create_file_internal_impl<win32_family::ansi_9x>(reinterpret_cast<char const *>(path.c_str()), md)};

	::fast_io::win32::filetime ftm;
	::fast_io::win32::GetSystemTimeAsFileTime(__builtin_addressof(ftm));

	// ::std::uint_least64_t current_time{(static_cast<::std::uint_least64_t>(ftm.dwHighDateTime) << 32) | ftm.dwLowDateTime};

	// constexpr ::std::uint_least64_t mul_factor{::fast_io::uint_least64_subseconds_per_second / 10000000ULL};

	::fast_io::win32::filetime CreationTime;
	::fast_io::win32::filetime *pCreationTime{__builtin_addressof(CreationTime)};
	::fast_io::win32::filetime LastAccessTime;
	::fast_io::win32::filetime *pLastAccessTime{__builtin_addressof(LastAccessTime)};
	::fast_io::win32::filetime LastWriteTime;
	::fast_io::win32::filetime *pLastWriteTime{__builtin_addressof(LastWriteTime)};

	switch (creation_time.flags)
	{
	case ::fast_io::utime_flags::none:
	{
		auto const win32_time{static_cast<::fast_io::win32_timestamp>(creation_time.timestamp)};
		CreationTime = ::fast_io::win32::win32_timestamp_to_filetime(win32_time);
		break;
	}
	case ::fast_io::utime_flags::now:
		CreationTime = ftm;
		break;
	case ::fast_io::utime_flags::omit:
		pCreationTime = nullptr;
		break;
	default:
		throw_win32_error(0x57);
	}

	switch (last_access_time.flags)
	{
	case ::fast_io::utime_flags::none:
	{
		auto const win32_time{static_cast<::fast_io::win32_timestamp>(last_access_time.timestamp)};
		LastAccessTime = ::fast_io::win32::win32_timestamp_to_filetime(win32_time);
		break;
	}
	case ::fast_io::utime_flags::now:
		LastAccessTime = ftm;
		break;
	case ::fast_io::utime_flags::omit:
		pLastAccessTime = nullptr;
		break;
	default:
		throw_win32_error(0x57);
	}

	switch (last_modification_time.flags)
	{
	case ::fast_io::utime_flags::none:
	{
		auto const win32_time{static_cast<::fast_io::win32_timestamp>(last_modification_time.timestamp)};
		LastWriteTime = ::fast_io::win32::win32_timestamp_to_filetime(win32_time);
		break;
	}
	case ::fast_io::utime_flags::now:
		LastWriteTime = ftm;
		break;
	case ::fast_io::utime_flags::omit:
		pLastWriteTime = nullptr;
		break;
	default:
		throw_win32_error(0x57);
	}

	if (!::fast_io::win32::SetFileTime(f.native_handle(), pCreationTime, pLastAccessTime, pLastWriteTime))
	{
		throw_win32_error();
	}
}

inline void win32_9xa_symlinkat_impl(char8_t const *oldpath_c_str, ::std::size_t oldpath_size,
									 ::fast_io::win32_9xa_dir_handle const &newdirhd, char8_t const *newpath_c_str, ::std::size_t newpath_size)
{
	::fast_io::containers::basic_cstring_view<char8_t> path{::fast_io::containers::null_terminated, oldpath_c_str, oldpath_size};

	::fast_io::u8win32_file_9xa f{::fast_io::win32::details::basic_win32_9xa_create_file_at_fs_dirent_impl(
		__builtin_addressof(newdirhd), reinterpret_cast<char const *>(newpath_c_str), newpath_size,
		{::fast_io::open_mode::out | ::fast_io::open_mode::excl, static_cast<perms>(436)})};

	char8_t buffer[510];
	::fast_io::freestanding::my_memset(buffer, u8' ', 510);
	::fast_io::u8obuffer_view u8obv{buffer, buffer + 510};

	::fast_io::operations::print_freestanding<false>(u8obv, u8"!<symlink>", path, u8"\nThis is just a text to force symlink file to be 510 bytes long. Do not delete it nor spaces following it.");

	::fast_io::operations::write_all(f, buffer, buffer + 510);
}

inline void win32_9xa_linkat_impl(::fast_io::win32_9xa_dir_handle const &olddirhd, char8_t const *oldpath_c_str, ::std::size_t oldpath_size,
								  ::fast_io::win32_9xa_dir_handle const &newdirhd, char8_t const *newpath_c_str, ::std::size_t newpath_size)
{
	auto oldpath{concat_tlc_win32_9xa_path_uncheck_whether_exist(olddirhd, oldpath_c_str, oldpath_size)};
	auto newpath{concat_tlc_win32_9xa_path_uncheck_whether_exist(newdirhd, newpath_c_str, newpath_size)};

	// Because of limitations of win32_9xa, this function doesn't really link two files together.
	// However, it simulates a real link by copying the file at exists to new.

	if (!::fast_io::win32::CopyFileA(reinterpret_cast<char const *>(oldpath.c_str()), reinterpret_cast<char const *>(newpath.c_str()), 1))
	{
		throw_win32_error();
	}
}

inline void win32_9xa_renameat_impl(::fast_io::win32_9xa_dir_handle const &olddirhd, char8_t const *oldpath_c_str, ::std::size_t oldpath_size,
									::fast_io::win32_9xa_dir_handle const &newdirhd, char8_t const *newpath_c_str, ::std::size_t newpath_size)
{
	auto oldpath{concat_tlc_win32_9xa_path_uncheck_whether_exist(olddirhd, oldpath_c_str, oldpath_size)};
	auto newpath{concat_tlc_win32_9xa_path_uncheck_whether_exist(newdirhd, newpath_c_str, newpath_size)};

	if (!::fast_io::win32::MoveFileA(reinterpret_cast<char const *>(oldpath.c_str()), reinterpret_cast<char const *>(newpath.c_str())))
	{
		throw_win32_error();
	}
}

template <::fast_io::details::posix_api_22 dsp, typename... Args>
inline auto win32_9xa_22_api_dispatcher(::fast_io::win32_9xa_dir_handle const &olddirhd, char8_t const *oldpath_c_str, ::std::size_t oldpath_size,
										::fast_io::win32_9xa_dir_handle const &newdirhd, char8_t const *newpath_c_str, ::std::size_t newpath_size, Args... args)
{
	if constexpr (dsp == ::fast_io::details::posix_api_22::renameat)
	{
		win32_9xa_renameat_impl(olddirhd, oldpath_c_str, oldpath_size, newdirhd, newpath_c_str, newpath_size, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_22::linkat)
	{
		win32_9xa_linkat_impl(olddirhd, oldpath_c_str, oldpath_size, newdirhd, newpath_c_str, newpath_size, args...);
	}
}

template <::fast_io::details::posix_api_12 dsp>
inline auto win32_9xa_12_api_dispatcher(char8_t const *oldpath_c_str, ::std::size_t oldpath_size,
										::fast_io::win32_9xa_dir_handle const &newdirhd, char8_t const *newpath_c_str, ::std::size_t newpath_size)
{
	if constexpr (dsp == ::fast_io::details::posix_api_12::symlinkat)
	{
		win32_9xa_symlinkat_impl(oldpath_c_str, oldpath_size, newdirhd, newpath_c_str, newpath_size);
	}
}

template <::fast_io::details::posix_api_1x dsp, typename... Args>
inline auto win32_9xa_1x_api_dispatcher(::fast_io::win32_9xa_dir_handle const &dir_handle, char8_t const *path_c_str, ::std::size_t path_size, Args... args)
{

	if constexpr (dsp == ::fast_io::details::posix_api_1x::faccessat)
	{
		win32_9xa_faccessat_impl(dir_handle, path_c_str, path_size, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::fchmodat)
	{
		win32_9xa_fchmodat_impl(dir_handle, path_c_str, path_size, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::fchownat)
	{
		win32_9xa_fchownat_impl(dir_handle, path_c_str, path_size, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::fstatat)
	{
		return win32_9xa_fstatat_impl(dir_handle, path_c_str, path_size, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::mkdirat)
	{
		win32_9xa_mkdirat_impl(dir_handle, path_c_str, path_size, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::unlinkat)
	{
		win32_9xa_unlinkat_impl(dir_handle, path_c_str, path_size, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::utimensat)
	{
		win32_9xa_utimensat_impl(dir_handle, path_c_str, path_size, args...);
	}
}

template <::fast_io::details::posix_api_1x dsp, typename path_type, typename... Args>
inline auto win32_9xa_deal_with1x(::fast_io::win32_9xa_dir_handle const &dir_handle, path_type const &path, Args... args)
{
	using char8_t_const_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char8_t const *;

	return win32_api_common_9xa(
		path, [&](char const *path_c_str, ::std::size_t path_size) { return win32_9xa_1x_api_dispatcher<dsp>(dir_handle, reinterpret_cast<char8_t_const_may_alias_ptr>(path_c_str), path_size, args...); });
}

template <::fast_io::details::posix_api_12 dsp, ::fast_io::constructible_to_os_c_str old_path_type,
		  ::fast_io::constructible_to_os_c_str new_path_type, typename... Args>
inline auto win32_9xa_deal_with12(old_path_type const &oldpath, ::fast_io::win32_9xa_dir_handle const &newdirfd, new_path_type const &newpath, Args... args)
{
	using char8_t_const_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char8_t const *;

	return win32_api_common_9xa(
		oldpath,
		[&](char const *oldpath_c_str, ::std::size_t oldpath_size) {
			return win32_api_common_9xa(
				newpath, [&](char const *newpath_c_str, ::std::size_t newpath_size) {
					return win32_9xa_12_api_dispatcher<dsp>(reinterpret_cast<char8_t_const_may_alias_ptr>(oldpath_c_str), oldpath_size, newdirfd, reinterpret_cast<char8_t_const_may_alias_ptr>(newpath_c_str), newpath_size, args...);
				});
		});
}

template <::fast_io::details::posix_api_22 dsp, typename oldpath_type, typename newpath_type, typename... Args>
inline auto win32_9xa_deal_with22(::fast_io::win32_9xa_dir_handle const &olddirhd, oldpath_type const &oldpath, ::fast_io::win32_9xa_dir_handle const &newdirhd, newpath_type const &newpath, Args... args)
{
	using char8_t_const_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char8_t const *;

	return win32_api_common_9xa(oldpath,
								[&](char const *oldpath_c_str, ::std::size_t oldpath_size) {
									return win32_api_common_9xa(newpath,
																[&](char const *newpath_c_str, ::std::size_t newpath_size) {
																	return win32_9xa_22_api_dispatcher<dsp>(olddirhd, reinterpret_cast<char8_t_const_may_alias_ptr>(oldpath_c_str), oldpath_size, newdirhd,
																											reinterpret_cast<char8_t_const_may_alias_ptr>(newpath_c_str), newpath_size, args...);
																});
								});
}

} // namespace win32::details

// 1x
template <::fast_io::constructible_to_os_c_str path_type>
inline void win32_9xa_unlinkat(::fast_io::win32_9xa_at_entry const &ent, path_type &&path, win32_9xa_at_flags flags = {})
{
	::fast_io::win32::details::win32_9xa_deal_with1x<details::posix_api_1x::unlinkat>(ent.handle, path, flags);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void win32_9xa_mkdirat(::fast_io::win32_9xa_at_entry const &ent, path_type &&path, perms pm = static_cast<perms>(436))
{
	::fast_io::win32::details::win32_9xa_deal_with1x<details::posix_api_1x::mkdirat>(ent.handle, path, pm);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void win32_9xa_faccessat(::fast_io::win32_9xa_at_entry const &ent, path_type const &path, access_how mode, [[maybe_unused]] win32_9xa_at_flags flags = win32_9xa_at_flags::symlink_nofollow)
{
	::fast_io::win32::details::win32_9xa_deal_with1x<details::posix_api_1x::faccessat>(ent.handle, path, mode);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void win32_9xa_fchownat(::fast_io::win32_9xa_at_entry const &ent, path_type const &path, ::std::uintmax_t owner, ::std::uintmax_t group, [[maybe_unused]] win32_9xa_at_flags flags = win32_9xa_at_flags::symlink_nofollow)
{
	::fast_io::win32::details::win32_9xa_deal_with1x<details::posix_api_1x::fchownat>(ent.handle, path, owner, group);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline posix_file_status win32_9xa_fstatat(::fast_io::win32_9xa_at_entry const &ent, path_type const &path, win32_9xa_at_flags flags = win32_9xa_at_flags::symlink_nofollow)
{
	return ::fast_io::win32::details::win32_9xa_deal_with1x<details::posix_api_1x::fstatat>(ent.handle, path, flags);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void win32_9xa_utimensat(::fast_io::win32_9xa_at_entry const &ent, path_type const &path, unix_timestamp_option creation_time,
								unix_timestamp_option last_access_time, unix_timestamp_option last_modification_time,
								win32_9xa_at_flags flags = win32_9xa_at_flags::symlink_nofollow)
{
	::fast_io::win32::details::win32_9xa_deal_with1x<details::posix_api_1x::utimensat>(ent.handle, path, creation_time, last_access_time, last_modification_time, flags);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void win32_9xa_symlinkat(old_path_type &&oldpath, win32_9xa_at_entry const &newdirfd, new_path_type &&newpath)
{
	::fast_io::win32::details::win32_9xa_deal_with12<details::posix_api_12::symlinkat>(oldpath, newdirfd.handle, newpath);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void win32_9xa_linkat(win32_9xa_at_entry const &oldent, old_path_type &&oldpath, win32_9xa_at_entry const &newent, new_path_type &&newpath, [[maybe_unused]] win32_9xa_at_flags flags = win32_9xa_at_flags::symlink_nofollow)
{
	::fast_io::win32::details::win32_9xa_deal_with22<::fast_io::details::posix_api_22::linkat>(oldent.handle, oldpath, newent.handle, newpath);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void win32_9xa_renameat(win32_9xa_at_entry const &oldent, old_path_type &&oldpath, win32_9xa_at_entry const &newent, new_path_type &&newpath)
{
	::fast_io::win32::details::win32_9xa_deal_with22<::fast_io::details::posix_api_22::renameat>(oldent.handle, oldpath, newent.handle, newpath);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void win32_9xa_fchmodat(::fast_io::win32_9xa_at_entry const &ent, path_type const &path, perms mode, [[maybe_unused]] win32_9xa_at_flags flags = win32_9xa_at_flags::symlink_nofollow)
{
	::fast_io::win32::details::win32_9xa_deal_with1x<details::posix_api_1x::fchmodat>(ent.handle, path, mode);
}

#if defined(_WIN32_WINDOWS) && !defined(__CYGWIN__) && !defined(__WINE__)

using native_at_flags = win32_9xa_at_flags;

template <::fast_io::constructible_to_os_c_str path_type>
inline void native_unlinkat(::fast_io::win32_9xa_at_entry const &ent, path_type &&path, win32_9xa_at_flags flags = {})
{
	::fast_io::win32::details::win32_9xa_deal_with1x<details::posix_api_1x::unlinkat>(ent.handle, path, flags);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void native_mkdirat(::fast_io::win32_9xa_at_entry const &ent, path_type &&path, perms pm = static_cast<perms>(436))
{
	::fast_io::win32::details::win32_9xa_deal_with1x<details::posix_api_1x::mkdirat>(ent.handle, path, pm);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void native_faccessat(::fast_io::win32_9xa_at_entry const &ent, path_type const &path, access_how mode, [[maybe_unused]] win32_9xa_at_flags flags = win32_9xa_at_flags::symlink_nofollow)
{
	::fast_io::win32::details::win32_9xa_deal_with1x<details::posix_api_1x::faccessat>(ent.handle, path, mode);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void native_fchmodat(::fast_io::win32_9xa_at_entry const &ent, path_type const &path, perms mode, [[maybe_unused]] win32_9xa_at_flags flags = win32_9xa_at_flags::symlink_nofollow)
{
	::fast_io::win32::details::win32_9xa_deal_with1x<details::posix_api_1x::fchmodat>(ent.handle, path, mode);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void native_fchownat(::fast_io::win32_9xa_at_entry const &ent, path_type const &path, ::std::uintmax_t owner, ::std::uintmax_t group, [[maybe_unused]] win32_9xa_at_flags flags = win32_9xa_at_flags::symlink_nofollow)
{
	::fast_io::win32::details::win32_9xa_deal_with1x<details::posix_api_1x::fchownat>(ent.handle, path, owner, group);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline posix_file_status native_fstatat(::fast_io::win32_9xa_at_entry const &ent, path_type const &path, win32_9xa_at_flags flags = win32_9xa_at_flags::symlink_nofollow)
{
	return ::fast_io::win32::details::win32_9xa_deal_with1x<details::posix_api_1x::fstatat>(ent.handle, path, flags);
}
template <::fast_io::constructible_to_os_c_str path_type>
inline void native_utimensat(::fast_io::win32_9xa_at_entry const &ent, path_type const &path, unix_timestamp_option creation_time,
							 unix_timestamp_option last_access_time, unix_timestamp_option last_modification_time,
							 win32_9xa_at_flags flags = win32_9xa_at_flags::symlink_nofollow)
{
	::fast_io::win32::details::win32_9xa_deal_with1x<details::posix_api_1x::utimensat>(ent.handle, path, creation_time, last_access_time, last_modification_time, flags);
}

// 12

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void native_symlinkat(old_path_type &&oldpath, win32_9xa_at_entry const &newdirfd, new_path_type &&newpath)
{
	::fast_io::win32::details::win32_9xa_deal_with12<details::posix_api_12::symlinkat>(oldpath, newdirfd.handle, newpath);
}

// 22

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void native_linkat(win32_9xa_at_entry const &oldent, old_path_type &&oldpath, win32_9xa_at_entry const &newent, new_path_type &&newpath, [[maybe_unused]] win32_9xa_at_flags flags = win32_9xa_at_flags::symlink_nofollow)
{
	::fast_io::win32::details::win32_9xa_deal_with22<::fast_io::details::posix_api_22::linkat>(oldent.handle, oldpath, newent.handle, newpath);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void native_renameat(win32_9xa_at_entry const &oldent, old_path_type &&oldpath, win32_9xa_at_entry const &newent, new_path_type &&newpath)
{
	::fast_io::win32::details::win32_9xa_deal_with22<::fast_io::details::posix_api_22::renameat>(oldent.handle, oldpath, newent.handle, newpath);
}

#endif

} // namespace fast_io
