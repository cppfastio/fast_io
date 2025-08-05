#pragma once

namespace fast_io
{

namespace posix
{

struct utimbuf
{
	time_t actime;  /* access time (unused on FAT filesystems) */
	time_t modtime; /* modification time */
};

extern int my_dos_access(char const *filename, int flags) noexcept __asm__("_access");
extern int my_dos_rename(char const *oldname, char const *newname) noexcept __asm__("_rename");
// Because of limitations of MS-DOS, this function doesn't really link two files together.
// However, it simulates a real link by copying the file at exists to new.
extern int my_dos_link(char const *exists, char const *newname) noexcept __asm__("_link");
extern int my_dos_symlink(char const *exists, char const *newname) noexcept __asm__("_symlink");
extern int my_dos_chmod(char const *path, mode_t mode) noexcept __asm__("_chmod");
extern int my_dos_utime(char const *file, utimbuf const *time) noexcept __asm__("_utime");
// This function does nothing under MS-DOS
extern int my_dos_chown(char const *file, int owner, int group) noexcept __asm__("_chown");
extern int my_dos_stat(char const *file, struct stat *sbuf) noexcept __asm__("_stat");
extern int my_dos_mkdir(char const *path, mode_t mode) noexcept __asm__("_mkdir");
// It always returns an error condition.
extern int my_dos_mknod(char const *path, mode_t mode, dev_t dev) noexcept __asm__("_mknod");
extern int my_dos_unlink(char const *file) noexcept __asm__("_unlink");
} // namespace posix

enum class dos_at_flags
{
	eaccess =
#ifdef AT_EACCESS
		AT_EACCESS
#else
		0
#endif
		,
	symlink_nofollow =
#ifdef AT_SYMLINK_NOFOLLOW
		AT_SYMLINK_NOFOLLOW
#else
		0
#endif
		,
	no_automount =
#ifdef AT_NO_AUTOMOUNT
		AT_NO_AUTOMOUNT
#else
		0
#endif
		,
	removedir =
#ifdef AT_REMOVEDIR
		AT_REMOVEDIR
#else
		0
#endif
		,
	empty_path =
#ifdef AT_EMPTY_PATH
		AT_EMPTY_PATH
#else
		0x1000
#endif
};

using native_at_flags = dos_at_flags;

inline constexpr dos_at_flags operator&(dos_at_flags x, dos_at_flags y) noexcept
{
	using utype = typename ::std::underlying_type<dos_at_flags>::type;
	return static_cast<dos_at_flags>(static_cast<utype>(x) & static_cast<utype>(y));
}

inline constexpr dos_at_flags operator|(dos_at_flags x, dos_at_flags y) noexcept
{
	using utype = typename ::std::underlying_type<dos_at_flags>::type;
	return static_cast<dos_at_flags>(static_cast<utype>(x) | static_cast<utype>(y));
}

inline constexpr dos_at_flags operator^(dos_at_flags x, dos_at_flags y) noexcept
{
	using utype = typename ::std::underlying_type<dos_at_flags>::type;
	return static_cast<dos_at_flags>(static_cast<utype>(x) ^ static_cast<utype>(y));
}

inline constexpr dos_at_flags operator~(dos_at_flags x) noexcept
{
	using utype = typename ::std::underlying_type<dos_at_flags>::type;
	return static_cast<dos_at_flags>(~static_cast<utype>(x));
}

inline constexpr dos_at_flags &operator&=(dos_at_flags &x, dos_at_flags y) noexcept
{
	return x = x & y;
}

inline constexpr dos_at_flags &operator|=(dos_at_flags &x, dos_at_flags y) noexcept
{
	return x = x | y;
}

inline constexpr dos_at_flags &operator^=(dos_at_flags &x, dos_at_flags y) noexcept
{
	return x = x ^ y;
}

namespace details
{

inline void dos_renameat_impl(int olddirfd, char const *oldpath, int newdirfd, char const *newpath)
{
	::fast_io::system_call_throw_error(::fast_io::posix::my_dos_rename(::fast_io::details::my_dos_concat_tlc_path(olddirfd, oldpath).c_str(),
																	   ::fast_io::details::my_dos_concat_tlc_path(newdirfd, newpath).c_str()));
}

inline void dos_linkat_impl(int olddirfd, char const *oldpath, int newdirfd, char const *newpath)
{
	::fast_io::system_call_throw_error(::fast_io::posix::my_dos_link(::fast_io::details::my_dos_concat_tlc_path(olddirfd, oldpath).c_str(),
																	 ::fast_io::details::my_dos_concat_tlc_path(newdirfd, newpath).c_str()));
}

template <posix_api_22 dsp, typename... Args>
inline auto dos22_api_dispatcher(int olddirfd, char const *oldpath, int newdirfd, char const *newpath, Args... args)
{
	if constexpr (dsp == ::fast_io::details::posix_api_22::renameat)
	{
		dos_renameat_impl(olddirfd, oldpath, newdirfd, newpath, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_22::linkat)
	{
		dos_linkat_impl(olddirfd, oldpath, newdirfd, newpath, args...);
	}
}

inline void dos_symlinkat_impl(char const *oldpath, int newdirfd, char const *newpath)
{
	::fast_io::system_call_throw_error(::fast_io::posix::my_dos_symlink(oldpath, ::fast_io::details::my_dos_concat_tlc_path(newdirfd, newpath).c_str()));
}

template <posix_api_12 dsp, typename... Args>
inline auto dos12_api_dispatcher(char const *oldpath, int newdirfd, char const *newpath, Args... args)
{
	if constexpr (dsp == ::fast_io::details::posix_api_12::symlinkat)
	{
		dos_symlinkat_impl(oldpath, newdirfd, newpath, args...);
	}
}

inline void dos_faccessat_impl(int dirfd, char const *pathname, int flags)
{
	::fast_io::system_call_throw_error(::fast_io::posix::my_dos_access(::fast_io::details::my_dos_concat_tlc_path(dirfd, pathname).c_str(), flags));
}

inline void dos_fchownat_impl(int dirfd, char const *pathname, uintmax_t owner, uintmax_t group)
{
	// chown does nothing under MS-DOS, so just check is_valid filename
	::fast_io::system_call_throw_error(::fast_io::posix::my_dos_chown(::fast_io::details::my_dos_concat_tlc_path(dirfd, pathname).c_str(),
																	  static_cast<int>(owner), static_cast<int>(group)));
}

inline void dos_fchmodat_impl(int dirfd, char const *pathname, mode_t mode)
{
	::fast_io::system_call_throw_error(::fast_io::posix::my_dos_chmod(::fast_io::details::my_dos_concat_tlc_path(dirfd, pathname).c_str(), mode));
}

inline posix_file_status dos_fstatat_impl(int dirfd, char const *pathname)
{
	struct stat buf;

	::fast_io::system_call_throw_error(::fast_io::posix::my_dos_stat(::fast_io::details::my_dos_concat_tlc_path(dirfd, pathname).c_str(), __builtin_addressof(buf)));
	return ::fast_io::details::struct_stat_to_posix_file_status(buf);
}

inline void dos_mkdirat_impl(int dirfd, char const *pathname, mode_t mode)
{
	::fast_io::system_call_throw_error(::fast_io::posix::my_dos_mkdir(::fast_io::details::my_dos_concat_tlc_path(dirfd, pathname).c_str(), mode));
}

inline void dos_unlinkat_impl(int dirfd, char const *pathname)
{
	::fast_io::system_call_throw_error(::fast_io::posix::my_dos_unlink(::fast_io::details::my_dos_concat_tlc_path(dirfd, pathname).c_str()));
}

inline constexpr ::std::time_t unix_timestamp_to_time_t(unix_timestamp stmp) noexcept
{
	return static_cast<int>(static_cast<::std::time_t>(stmp.seconds));
}

inline
#if defined(UTIME_NOW) && defined(UTIME_OMIT)
	constexpr
#endif
	::std::time_t
	unix_timestamp_to_time_t(unix_timestamp_option opt) noexcept
{
	switch (opt.flags)
	{
	case utime_flags::now:
		[[fallthrough]];
	case utime_flags::omit:
		throw_posix_error(EINVAL);
		::fast_io::unreachable();
	default:
		return ::fast_io::details::unix_timestamp_to_time_t(opt.timestamp);
	}
}

inline void dos_utimensat_impl(int dirfd, char const *pathname, unix_timestamp_option creation_time,
							   unix_timestamp_option last_access_time, unix_timestamp_option last_modification_time)
{
	if (creation_time.flags != utime_flags::omit)
	{
		throw_posix_error(EINVAL);
	}

	::fast_io::posix::utimbuf ts{
		::fast_io::details::unix_timestamp_to_time_t(last_access_time),
		::fast_io::details::unix_timestamp_to_time_t(last_modification_time),
	};

	::fast_io::system_call_throw_error(::fast_io::posix::my_dos_utime(::fast_io::details::my_dos_concat_tlc_path(dirfd, pathname).c_str(), __builtin_addressof(ts)));
}

template <posix_api_1x dsp, typename... Args>
inline auto dos1x_api_dispatcher(int dirfd, char const *path, Args... args)
{
	if constexpr (dsp == ::fast_io::details::posix_api_1x::faccessat)
	{
		dos_faccessat_impl(dirfd, path, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::fchownat)
	{
		dos_fchownat_impl(dirfd, path, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::fchmodat)
	{
		dos_fchmodat_impl(dirfd, path, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::fstatat)
	{
		return dos_fstatat_impl(dirfd, path, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::mkdirat)
	{
		dos_mkdirat_impl(dirfd, path, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::unlinkat)
	{
		dos_unlinkat_impl(dirfd, path, args...);
	}
	else if constexpr (dsp == ::fast_io::details::posix_api_1x::utimensat)
	{
		dos_utimensat_impl(dirfd, path, args...);
	}
}

template <posix_api_22 dsp, ::fast_io::constructible_to_os_c_str old_path_type,
		  ::fast_io::constructible_to_os_c_str new_path_type, typename... Args>
inline auto dos_deal_with22(int olddirfd, old_path_type const &oldpath, int newdirfd, new_path_type const &newpath, Args... args)
{
	return ::fast_io::posix_api_common(
		oldpath,
		[&](char const *oldpath_c_str) {
			return ::fast_io::posix_api_common(
				newpath, [&](char const *newpath_c_str) { return dos22_api_dispatcher<dsp>(olddirfd, oldpath_c_str, newdirfd, newpath_c_str, args...); });
		});
}

template <posix_api_12 dsp, ::fast_io::constructible_to_os_c_str old_path_type,
		  ::fast_io::constructible_to_os_c_str new_path_type, typename... Args>
inline auto dos_deal_with12(old_path_type const &oldpath, int newdirfd, new_path_type const &newpath, Args... args)
{
	return ::fast_io::posix_api_common(
		oldpath,
		[&](char const *oldpath_c_str) {
			return ::fast_io::posix_api_common(
				newpath, [&](char const *newpath_c_str) { return dos12_api_dispatcher<dsp>(oldpath_c_str, newdirfd, newpath_c_str, args...); });
		});
}

template <posix_api_1x dsp, ::fast_io::constructible_to_os_c_str path_type, typename... Args>
inline auto dos_deal_with1x(int dirfd, path_type const &path, Args... args)
{
	return ::fast_io::posix_api_common(path, [&](char const *path_c_str) { return dos1x_api_dispatcher<dsp>(dirfd, path_c_str, args...); });
}

} // namespace details

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void dos_renameat(posix_at_entry oldent, old_path_type const &oldpath, posix_at_entry newent,
						 new_path_type const &newpath)
{
	::fast_io::details::dos_deal_with22<::fast_io::details::posix_api_22::renameat>(oldent.fd, oldpath, newent.fd, newpath);
}

template <::fast_io::constructible_to_os_c_str new_path_type>
inline void dos_renameat(posix_fs_dirent fs_dirent, posix_at_entry newent, new_path_type const &newpath)
{
	::fast_io::details::dos_deal_with22<::fast_io::details::posix_api_22::renameat>(
		fs_dirent.fd, ::fast_io::manipulators::os_c_str(fs_dirent.filename), newent.fd, newpath);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void dos_symlinkat(old_path_type const &oldpath, posix_at_entry newent, new_path_type const &newpath)
{
	::fast_io::details::dos_deal_with12<::fast_io::details::posix_api_12::symlinkat>(oldpath, newent.fd, newpath);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void native_renameat(posix_at_entry oldent, old_path_type const &oldpath, posix_at_entry newent,
							new_path_type const &newpath)
{
	::fast_io::details::dos_deal_with22<::fast_io::details::posix_api_22::renameat>(oldent.fd, oldpath, newent.fd, newpath);
}

template <::fast_io::constructible_to_os_c_str new_path_type>
inline void native_renameat(posix_fs_dirent fs_dirent, posix_at_entry newent, new_path_type const &newpath)
{
	::fast_io::details::dos_deal_with22<::fast_io::details::posix_api_22::renameat>(
		fs_dirent.fd, ::fast_io::manipulators::os_c_str(fs_dirent.filename), newent.fd, newpath);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void native_symlinkat(old_path_type const &oldpath, posix_at_entry newent, new_path_type const &newpath)
{
	::fast_io::details::dos_deal_with12<::fast_io::details::posix_api_12::symlinkat>(oldpath, newent.fd, newpath);
}


template <::fast_io::constructible_to_os_c_str path_type>
inline void dos_faccessat(posix_at_entry ent, path_type const &path, [[maybe_unused]] access_how mode,
						  dos_at_flags flags = dos_at_flags::symlink_nofollow)
{
	::fast_io::details::dos_deal_with1x<::fast_io::details::posix_api_1x::faccessat>(ent.fd, path, static_cast<int>(flags));
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void native_faccessat(posix_at_entry ent, path_type const &path, [[maybe_unused]] access_how mode,
							 dos_at_flags flags = dos_at_flags::symlink_nofollow)
{
	::fast_io::details::dos_deal_with1x<::fast_io::details::posix_api_1x::faccessat>(ent.fd, path, static_cast<int>(flags));
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void dos_fchmodat(posix_at_entry ent, path_type const &path, perms mode,
						 [[maybe_unused]] dos_at_flags flags = dos_at_flags::symlink_nofollow)
{
	::fast_io::details::dos_deal_with1x<::fast_io::details::posix_api_1x::fchmodat>(ent.fd, path, static_cast<int>(mode));
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void native_fchmodat(posix_at_entry ent, path_type const &path, perms mode,
							[[maybe_unused]] dos_at_flags flags = dos_at_flags::symlink_nofollow)
{
	::fast_io::details::dos_deal_with1x<::fast_io::details::posix_api_1x::fchmodat>(ent.fd, path, static_cast<int>(mode));
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void dos_fchownat(posix_at_entry ent, path_type const &path, ::std::uintmax_t owner, ::std::uintmax_t group,
						 [[maybe_unused]] dos_at_flags flags = dos_at_flags::symlink_nofollow)
{
	::fast_io::details::dos_deal_with1x<::fast_io::details::posix_api_1x::fchownat>(ent.fd, path, owner, group);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void native_fchownat(posix_at_entry ent, path_type const &path, ::std::uintmax_t owner, ::std::uintmax_t group,
							[[maybe_unused]] dos_at_flags flags = dos_at_flags::symlink_nofollow)
{
	::fast_io::details::dos_deal_with1x<::fast_io::details::posix_api_1x::fchownat>(ent.fd, path, owner, group);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline posix_file_status dos_fstatat(posix_at_entry ent, path_type const &path,
									 [[maybe_unused]] dos_at_flags flags = dos_at_flags::symlink_nofollow)
{
	return ::fast_io::details::dos_deal_with1x<::fast_io::details::posix_api_1x::fstatat>(ent.fd, path);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline posix_file_status native_fstatat(posix_at_entry ent, path_type const &path,
										[[maybe_unused]] dos_at_flags flags = dos_at_flags::symlink_nofollow)
{
	return ::fast_io::details::dos_deal_with1x<::fast_io::details::posix_api_1x::fstatat>(ent.fd, path);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void dos_mkdirat(posix_at_entry ent, path_type const &path, perms perm = static_cast<perms>(509))
{
	return ::fast_io::details::dos_deal_with1x<::fast_io::details::posix_api_1x::mkdirat>(ent.fd, path, static_cast<mode_t>(perm));
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void native_mkdirat(posix_at_entry ent, path_type const &path, perms perm = static_cast<perms>(509))
{
	return ::fast_io::details::dos_deal_with1x<::fast_io::details::posix_api_1x::mkdirat>(ent.fd, path, static_cast<mode_t>(perm));
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void dos_unlinkat(posix_at_entry ent, path_type const &path, [[maybe_unused]] dos_at_flags flags = {})
{
	::fast_io::details::dos_deal_with1x<::fast_io::details::posix_api_1x::unlinkat>(ent.fd, path);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void native_unlinkat(posix_at_entry ent, path_type const &path, [[maybe_unused]] dos_at_flags flags = {})
{
	::fast_io::details::dos_deal_with1x<::fast_io::details::posix_api_1x::unlinkat>(ent.fd, path);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void dos_linkat(posix_at_entry oldent, old_path_type const &oldpath, posix_at_entry newent,
					   new_path_type const &newpath, [[maybe_unused]] dos_at_flags flags = dos_at_flags::symlink_nofollow)
{
	::fast_io::details::dos_deal_with22<::fast_io::details::posix_api_22::linkat>(oldent.fd, oldpath, newent.fd, newpath);
}

template <::fast_io::constructible_to_os_c_str old_path_type, ::fast_io::constructible_to_os_c_str new_path_type>
inline void native_linkat(posix_at_entry oldent, old_path_type const &oldpath, posix_at_entry newent,
						  new_path_type const &newpath, [[maybe_unused]] dos_at_flags flags = dos_at_flags::symlink_nofollow)
{
	::fast_io::details::dos_deal_with22<::fast_io::details::posix_api_22::linkat>(oldent.fd, oldpath, newent.fd, newpath);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void dos_utimensat(posix_at_entry ent, path_type const &path, unix_timestamp_option creation_time,
						  unix_timestamp_option last_access_time, unix_timestamp_option last_modification_time,
						  [[maybe_unused]] dos_at_flags flags = dos_at_flags::symlink_nofollow)
{
	::fast_io::details::dos_deal_with1x<::fast_io::details::posix_api_1x::utimensat>(ent.fd, path, creation_time, last_access_time,
																					 last_modification_time);
}

template <::fast_io::constructible_to_os_c_str path_type>
inline void native_utimensat(posix_at_entry ent, path_type const &path, unix_timestamp_option creation_time,
							 unix_timestamp_option last_access_time, unix_timestamp_option last_modification_time,
							 [[maybe_unused]] dos_at_flags flags = dos_at_flags::symlink_nofollow)
{
	::fast_io::details::dos_deal_with1x<::fast_io::details::posix_api_1x::utimensat>(ent.fd, path, creation_time, last_access_time,
																					 last_modification_time);
}

} // namespace fast_io
