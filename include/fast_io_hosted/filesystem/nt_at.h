#pragma once

namespace fast_io
{

enum class nt_at_flags:std::uint_least32_t
{
eaccess=static_cast<std::uint_least32_t>(1),
symlink_nofollow=static_cast<std::uint_least32_t>(1)<<1,
no_automount=static_cast<std::uint_least32_t>(1)<<2,
removedir=static_cast<std::uint_least32_t>(1)<<3,
empty_path=static_cast<std::uint_least32_t>(1)<<4
};


constexpr nt_at_flags operator&(nt_at_flags x, nt_at_flags y) noexcept
{
using utype = typename std::underlying_type<nt_at_flags>::type;
return static_cast<nt_at_flags>(static_cast<utype>(x) & static_cast<utype>(y));
}

constexpr nt_at_flags operator|(nt_at_flags x, nt_at_flags y) noexcept
{
using utype = typename std::underlying_type<nt_at_flags>::type;
return static_cast<nt_at_flags>(static_cast<utype>(x) | static_cast<utype>(y));
}

constexpr nt_at_flags operator^(nt_at_flags x, nt_at_flags y) noexcept
{
using utype = typename std::underlying_type<nt_at_flags>::type;
return static_cast<nt_at_flags>(static_cast<utype>(x) ^ static_cast<utype>(y));
}

constexpr nt_at_flags operator~(nt_at_flags x) noexcept
{
using utype = typename std::underlying_type<nt_at_flags>::type;
return static_cast<nt_at_flags>(~static_cast<utype>(x));
}

inline constexpr nt_at_flags& operator&=(nt_at_flags& x, nt_at_flags y) noexcept{return x=x&y;}

inline constexpr nt_at_flags& operator|=(nt_at_flags& x, nt_at_flags y) noexcept{return x=x|y;}

inline constexpr nt_at_flags& operator^=(nt_at_flags& x, nt_at_flags y) noexcept{return x=x^y;}

namespace win32::nt::details
{

inline constexpr nt_open_mode calculate_nt_delete_flag(nt_at_flags flags) noexcept
{
	nt_open_mode mode{
	.DesiredAccess=0x00010000,	//FILE_GENERIC_READ
	.FileAttributes=0x80,		//FILE_READ_ATTRIBUTES
	.ShareAccess=0x00000007,	//FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
	.CreateDisposition=0x00000001, /*OPEN_EXISTING	=>	FILE_OPEN*/
	.CreateOptions=0x00001000		/*FILE_DELETE_ON_CLOSE*/
	};
	if((flags&nt_at_flags::symlink_nofollow)!=nt_at_flags::symlink_nofollow)
	{
		mode.CreateOptions |= 0x00200000;//FILE_FLAG_OPEN_REPARSE_POINT => FILE_OPEN_REPARSE_POINT (0x00200000)
	}
	if((flags&nt_at_flags::removedir)==nt_at_flags::removedir)
	{
		mode.CreateOptions |= 0x00004000;		//FILE_OPEN_FOR_BACKUP_INTENT
		mode.CreateOptions |= 0x00000001;		//FILE_DIRECTORY_FILE
	}
	else
	{
		mode.CreateOptions |= 0x00000040;		//FILE_NON_DIRECTORY_FILE 0x00000040
	}
	return mode;
}

template<bool zw>
inline void nt_unlinkat_impl(void* dirhd,char16_t const* path_c_str,std::size_t path_size,nt_at_flags flags)
{
	auto status{nt_close<zw>(nt_call_callback(dirhd,path_c_str,path_size,nt_create_callback<zw>{calculate_nt_delete_flag(flags)}))};
	if(status)
		throw_nt_error(status);
}

template<bool zw>
inline void nt_mkdirat_impl(void* dirhd,char16_t const* path_c_str,std::size_t path_size,perms pm)
{
	constexpr fast_io::win32::nt::details::nt_open_mode create_dir_mode{fast_io::win32::nt::details::calculate_nt_open_mode({fast_io::open_mode::creat|fast_io::open_mode::directory})};
	auto m_dir_mode{create_dir_mode};
	if((pm&perms::owner_write)==perms::none)
		m_dir_mode.FileAttributes|=0x00000001;  //FILE_ATTRIBUTE_READONLY
	auto status{nt_close<zw>(nt_call_callback(dirhd,path_c_str,path_size,nt_create_callback<zw>{m_dir_mode}))};
	if(status)
		throw_nt_error(status);
}
inline constexpr nt_open_mode calculate_nt_link_flag(nt_at_flags flags) noexcept
{
	nt_open_mode mode{
	.DesiredAccess=0x00100000|0x0100,	//SYNCHRONIZE | FILE_WRITE_ATTRIBUTES
	.FileAttributes=0x80,		//FILE_READ_ATTRIBUTES
	.ShareAccess=0x00000007,	//FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
	.CreateDisposition=0x00200000 | 0x00000020, 	/*OPEN_EXISTING	=>	FILE_OPEN*/
	};
	if((flags&nt_at_flags::symlink_nofollow)!=nt_at_flags::symlink_nofollow)
	{
		mode.CreateOptions |= 0x00200000;//FILE_FLAG_OPEN_REPARSE_POINT => FILE_OPEN_REPARSE_POINT (0x00200000)
	}
	return mode;
}
#if 0
template<bool zw>
inline void nt_linkat_no_newpath_size_impl(void* olddirhd,char16_t const* oldpath_c_str,std::size_t oldpath_size,
	void* newdirhd,char16_t const* to_path_c_str,nt_at_flags flags)
{

}
#endif

struct file_link_information
{
	std::uint_least32_t ReplaceIfExists;
	void* RootDirectory;
	std::uint_least32_t FileNameLength;
};

template<bool zw>
inline void nt_linkat_impl(
	void* olddirhd,char16_t const* oldpath_c_str,std::size_t oldpath_size,
	void* newdirhd,char16_t const* newpath_c_str,std::size_t newpath_size,
	nt_at_flags flags)
{
	nt_open_mode const md{calculate_nt_link_flag(flags)};
	basic_nt_family_file<(zw?(nt_family::zw):(nt_family::nt)),char> file(nt_call_callback(olddirhd,oldpath_c_str,oldpath_size,nt_create_callback<zw>{md}));
	nt_call_callback(
		newdirhd,newpath_c_str,newpath_size,
		[&](void* directory_hd,win32::nt::unicode_string const* ustr)
	{
		char16_t const* pth_cstr{ustr->Buffer};
		std::uint_least32_t pth_size2{ustr->Length};
		::fast_io::details::local_operator_new_array_ptr<char> buffer(sizeof(file_link_information)+pth_size2); 
		file_link_information info{.ReplaceIfExists=false,
			.RootDirectory=directory_hd,
			.FileNameLength=static_cast<std::uint_least32_t>(pth_size2)};

		::fast_io::details::my_memcpy(buffer.ptr,__builtin_addressof(info),sizeof(file_link_information));
		::fast_io::details::my_memcpy(buffer.ptr+sizeof(file_link_information),pth_cstr,pth_size2);

		io_status_block block;
		using file_link_information_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= file_link_information*;
		std::uint_least32_t status{nt_set_information_file<zw>(
			file.handle,
			__builtin_addressof(block),
			reinterpret_cast<file_link_information_may_alias_ptr>(buffer.ptr),
			static_cast<std::uint_least32_t>(sizeof(info)),
			file_information_class::FileLinkInformation)};
		if(status)
			throw_nt_error(status);
	});
}

template<bool zw,::fast_io::details::posix_api_22 dsp,typename... Args>
inline auto nt22_api_dispatcher(void* olddirhd,char16_t const* oldpath_c_str,std::size_t oldpath_size,
	void* newdirhd,char16_t const* newpath_c_str,std::size_t newpath_size,Args... args)
{
#if 0
	if constexpr(dsp==::fast_io::details::posix_api_22::renameat)
	{
		static_assert(sizeof...(Args)==0);
		nt_renameat_impl<zw>(olddirfd,oldpath,newdirfd,newpath);
	}
	else
#endif
	if constexpr(dsp==::fast_io::details::posix_api_22::linkat)
	{
		nt_linkat_impl<zw>(olddirhd,oldpath_c_str,oldpath_size,newdirhd,newpath_c_str,newpath_size,args...);
	}
}

template<bool zw,::fast_io::details::posix_api_1x dsp,typename... Args>
inline auto nt1x_api_dispatcher(void* dir_handle,char16_t const* path_c_str,std::size_t path_size,Args... args)
{
#if 0
	if constexpr(dsp==::fast_io::details::posix_api_1x::faccessat)
		nt_faccessat_impl<zw>(dir_handle,path_c_str,path_size,args...);
	else if constexpr(dsp==::fast_io::details::posix_api_1x::fchmodat)
		nt_fchmodat_impl<zw>(dir_handle,path_c_str,path_size,args...);
	else if constexpr(dsp==::fast_io::details::posix_api_1x::fstatat)
		nt_fstatat_impl<zw>(dir_handle,path_c_str,path_size,args...);
	else
#endif
	if constexpr(dsp==::fast_io::details::posix_api_1x::mkdirat)
		nt_mkdirat_impl<zw>(dir_handle,path_c_str,path_size,args...);	
	else if constexpr(dsp==::fast_io::details::posix_api_1x::unlinkat)
		nt_unlinkat_impl<zw>(dir_handle,path_c_str,path_size,args...);
#if 0
	else if constexpr(dsp==::fast_io::details::posix_api_1x::unlinkat::utimensat)
		nt_utimensat_impl<zw>(dir_handle,path_c_str,path_size,args...);
#endif
}

template<nt_family family,::fast_io::details::posix_api_1x dsp,typename path_type,typename... Args>
inline auto nt_deal_with1x(
	void* dir_handle,
	path_type const& path,
	Args... args)
{
	return nt_api_common(path,[&](char16_t const* path_c_str,std::size_t path_size)
	{
		return nt1x_api_dispatcher<family==nt_family::zw,dsp>(dir_handle,path_c_str,path_size,args...);
	});
}

template<nt_family family,::fast_io::details::posix_api_22 dsp,typename oldpath_type,typename newpath_type>
inline auto nt_deal_with22(void* olddirhd,oldpath_type const& oldpath,
	void* newdirhd,newpath_type const& newpath,nt_at_flags )
{
	return nt_api_common(oldpath,[&](char16_t const* oldpath_c_str,std::size_t oldpath_size)
	{
		return nt_api_common(newpath,[&](char16_t const* newpath_c_str,std::size_t newpath_size)
		{
			return nt22_api_dispatcher<family==nt_family::zw,dsp>(olddirhd,
				oldpath_c_str,oldpath_size,
				newdirhd,
				newpath_c_str,newpath_size);
		});
	});
}


}


template<nt_family family,::fast_io::constructible_to_os_c_str path_type>
requires (family==nt_family::nt||family==nt_family::zw)
inline void nt_family_mkdirat(nt_at_entry ent,path_type const& path,perms pm=static_cast<perms>(436))
{
	::fast_io::win32::nt::details::nt_deal_with1x<family,details::posix_api_1x::mkdirat>(ent.handle,path,pm);
}

template<::fast_io::constructible_to_os_c_str path_type>
inline void nt_mkdirat(nt_at_entry ent,path_type const& path,perms pm=static_cast<perms>(436))
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt,details::posix_api_1x::mkdirat>(ent.handle,path,pm);
}

template<::fast_io::constructible_to_os_c_str path_type>
inline void zw_mkdirat(nt_at_entry ent,path_type const& path,perms pm=static_cast<perms>(436))
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::zw,details::posix_api_1x::mkdirat>(ent.handle,path,pm);
}

template<nt_family family,::fast_io::constructible_to_os_c_str path_type>
requires (family==nt_family::nt||family==nt_family::zw)
inline void nt_family_unlinkat(nt_at_entry ent,path_type const& path,nt_at_flags flags={})
{
	::fast_io::win32::nt::details::nt_deal_with1x<family,details::posix_api_1x::unlinkat>(ent.handle,path,flags);
}

template<::fast_io::constructible_to_os_c_str path_type>
inline void nt_unlinkat(nt_at_entry ent,path_type const& path,nt_at_flags flags={})
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt,details::posix_api_1x::unlinkat>(ent.handle,path,flags);
}

template<::fast_io::constructible_to_os_c_str path_type>
inline void zw_unlinkat(nt_at_entry ent,path_type const& path,nt_at_flags flags={})
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::zw,details::posix_api_1x::unlinkat>(ent.handle,path,flags);
}


#if !defined(__CYGWIN__) && !defined(__WINE__)
using native_at_flags = nt_at_flags;

template<::fast_io::constructible_to_os_c_str path_type>
inline void native_mkdirat(nt_at_entry ent,path_type const& path, perms pm=static_cast<perms>(436))
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt,details::posix_api_1x::mkdirat>(ent.handle,path,pm);
}

template<::fast_io::constructible_to_os_c_str path_type>
inline void native_unlinkat(nt_at_entry ent,path_type const& path,native_at_flags flags={})
{
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt,::fast_io::details::posix_api_1x::unlinkat>(ent.handle,path,flags);
}

template<::fast_io::constructible_to_os_c_str path_type>
inline void native_fchownat(nt_at_entry,path_type&&,std::uintptr_t,std::uintptr_t,[[maybe_unused]] nt_at_flags flags=nt_at_flags::symlink_nofollow)
{
//windows does not use POSIX user group system. stub it and it is perfectly fine. But nt_fchownat,zw_fchownat will not be provided since they do not exist.
}
#if 0
template<::fast_io::constructible_to_os_c_str old_path_type,::fast_io::constructible_to_os_c_str new_path_type>
inline void native_linkat(native_at_entry oldent,old_path_type&& oldpath,native_at_entry newent,new_path_type&& newpath,nt_at_flags flags=nt_at_flags::symlink_nofollow)
{
	auto oldvw{details::to_its_cstring_view(oldpath)};
	auto newvw{details::to_its_cstring_view(newpath)};
	::fast_io::win32::nt::details::nt_deal_with22<nt_family::nt,::fast_io::details::posix_api_22::linkat>(oldent.handle,oldvw.c_str(),oldvw.size(),
	newent.handle,newvw.c_str(),newvw.size(),flags);
}
#endif
#endif
}
