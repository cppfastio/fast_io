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
inline void nt_unlinkat_impl(void* dirhd,wchar_t const* path_c_str,std::size_t path_size,nt_at_flags flags)
{
	auto status{nt_close<zw>(nt_call_callback(dirhd,path_c_str,path_size,nt_create_callback<zw>{calculate_nt_delete_flag(flags)}))};
	if(status)
		throw_nt_error(status);
}

template<bool zw>
inline void nt_mkdirat_impl(void* dirhd,wchar_t const* path_c_str,std::size_t path_size,perms pm)
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
inline void nt_linkat_no_newpath_size_impl(void* olddirhd,wchar_t const* oldpath_c_str,std::size_t oldpath_size,
	void* newdirhd,wchar_t const* to_path_c_str,nt_at_flags flags)
{

}
#endif

struct file_link_information
{
	std::uint32_t ReplaceIfExists;
	void* RootDirectory;
	std::uint32_t FileNameLength;
};

template<bool zw>
inline void nt_linkat_impl(
	void* olddirhd,wchar_t const* oldpath_c_str,std::size_t oldpath_size,
	void* newdirhd,wchar_t const* newpath_c_str,std::size_t newpath_size,
	nt_at_flags flags)
{
	nt_open_mode const md{calculate_nt_link_flag(flags)};
	basic_nt_family_file<(zw?(nt_family::zw):(nt_family::nt)),char> file(nt_call_callback(olddirhd,oldpath_c_str,oldpath_size,nt_create_callback<zw>{md}));
	nt_call_callback(
		newdirhd,newpath_c_str,newpath_size,
		[&](void* directory_hd,win32::nt::unicode_string const* ustr)
	{
		wchar_t const* pth_cstr{ustr->Buffer};
		std::uint32_t pth_size2{ustr->Length};
		::fast_io::details::local_operator_new_array_ptr<char> buffer(sizeof(file_link_information)+pth_size2); 
		file_link_information info{.ReplaceIfExists=false,
			.RootDirectory=directory_hd,
			.FileNameLength=static_cast<std::uint32_t>(pth_size2)};

		::fast_io::details::my_memcpy(buffer.ptr,__builtin_addressof(info),sizeof(file_link_information));
		::fast_io::details::my_memcpy(buffer.ptr+sizeof(file_link_information),pth_cstr,pth_size2);

		io_status_block block;
		using file_link_information_may_alias_ptr
#if __has_cpp_attribute(gnu::may_alias)
		[[gnu::may_alias]]
#endif
		= file_link_information*;
		std::uint32_t status{nt_set_information_file<zw>(
			file.handle,
			__builtin_addressof(block),
			reinterpret_cast<file_link_information_may_alias_ptr>(buffer.ptr),
			static_cast<std::uint32_t>(sizeof(info)),
			file_information_class::FileLinkInformation)};
		if(status)
			throw_nt_error(status);
	});
}

template<bool zw,::fast_io::details::posix_api_22 dsp,typename... Args>
inline auto nt22_api_dispatcher(void* olddirhd,wchar_t const* oldpath_c_str,std::size_t oldpath_size,
	void* newdirhd,wchar_t const* newpath_c_str,std::size_t newpath_size,Args... args)
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
inline auto nt1x_api_dispatcher(void* dir_handle,wchar_t const* path_c_str,std::size_t path_size,Args... args)
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

template<nt_family family,::fast_io::details::posix_api_1x dsp,std::integral char_type,typename... Args>
inline auto nt_deal_with1x(
	void* dir_handle,
	char_type const* path_c_str,
	std::size_t path_size,
	Args... args)
{
	using wchar_t_may_alias_const_ptr
#if __has_cpp_attribute(gnu::may_alias)
	[[gnu::may_alias]]
#endif
	= wchar_t const*;
	if constexpr(sizeof(char_type)==2)
	{
		return nt1x_api_dispatcher<family==nt_family::zw,dsp>(dir_handle,reinterpret_cast<wchar_t_may_alias_const_ptr>(path_c_str),path_size,args...);
	}
	else
	{
		nt_api_encoding_converter converter(path_c_str,path_size);
		return nt1x_api_dispatcher<family==nt_family::zw,dsp>(dir_handle,converter.native_c_str(),converter.size(),args...);
	}
}

template<nt_family family,::fast_io::details::posix_api_22 dsp,std::integral char_type1,std::integral char_type2>
inline auto nt_deal_with22(void* olddirhd,char_type1 const* oldpath_c_str,std::size_t oldpath_size,
	void* newdirhd,char_type2 const* newpath_c_str,std::size_t newpath_size,nt_at_flags flags)
{
	using wchar_t_may_alias_const_ptr
#if __has_cpp_attribute(gnu::may_alias)
	[[gnu::may_alias]]
#endif
	= wchar_t const*;
	if constexpr(sizeof(char_type1)==2&&sizeof(char_type2)==2)
	{
		return nt22_api_dispatcher<family==nt_family::zw,dsp>(olddirhd,reinterpret_cast<wchar_t_may_alias_const_ptr>(oldpath_c_str),oldpath_size,
		newdirhd,reinterpret_cast<wchar_t_may_alias_const_ptr>(newpath_c_str),newpath_size,flags);
	}
	else if constexpr(sizeof(char_type1)==2&&sizeof(char_type2)!=2)
	{
		nt_api_encoding_converter converter(newpath_c_str,newpath_size);
		return nt22_api_dispatcher<family==nt_family::zw,dsp>(olddirhd,reinterpret_cast<wchar_t_may_alias_const_ptr>(oldpath_c_str),oldpath_size,
		newdirhd,converter.native_c_str(),converter.size(),flags);
	}
	else if constexpr(sizeof(char_type1)!=2&&sizeof(char_type2)==2)
	{
		nt_api_encoding_converter op_converter(oldpath_c_str,oldpath_size);
		return nt22_api_dispatcher<family==nt_family::zw,dsp>(olddirhd,op_converter.native_c_str(),op_converter.size(),
		newdirhd,reinterpret_cast<wchar_t_may_alias_const_ptr>(newpath_c_str),newpath_size,flags);
	}
	else
	{
		nt_api_encoding_converter opdealer(oldpath_c_str,oldpath_size);
		nt_api_encoding_converter newpdealer(newpath_c_str,newpath_size);
		return nt22_api_dispatcher<family==nt_family::zw,dsp>(olddirhd,opdealer.native_c_str(),opdealer.size(),
		newdirhd,newpdealer.native_c_str(),newpdealer.size());
	}
}


}


template<nt_family family,constructible_to_path path_type>
requires (family==nt_family::nt||family==nt_family::zw)
inline void nt_family_mkdirat(nt_at_entry ent,path_type&& path,perms pm=static_cast<perms>(436))
{
	auto vw{::fast_io::details::to_its_cstring_view(path)};
	::fast_io::win32::nt::details::nt_deal_with1x<family,details::posix_api_1x::mkdirat>(ent.handle,vw.c_str(),vw.size(),pm);
}

template<constructible_to_path path_type>
inline void nt_mkdirat(nt_at_entry ent,path_type&& path,perms pm=static_cast<perms>(436))
{
	auto vw{::fast_io::details::to_its_cstring_view(path)};
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt,details::posix_api_1x::mkdirat>(ent.handle,vw.c_str(),vw.size(),pm);
}

template<constructible_to_path path_type>
inline void zw_mkdirat(nt_at_entry ent,path_type&& path,perms pm=static_cast<perms>(436))
{
	auto vw{::fast_io::details::to_its_cstring_view(path)};
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::zw,details::posix_api_1x::mkdirat>(ent.handle,vw.c_str(),vw.size(),pm);
}

template<nt_family family,constructible_to_path path_type>
requires (family==nt_family::nt||family==nt_family::zw)
inline void nt_family_unlinkat(nt_at_entry ent,path_type&& path,nt_at_flags flags={})
{
	auto vw{::fast_io::details::to_its_cstring_view(path)};
	::fast_io::win32::nt::details::nt_deal_with1x<family,details::posix_api_1x::unlinkat>(ent.handle,vw.c_str(),vw.size(),flags);
}

template<constructible_to_path path_type>
inline void nt_unlinkat(nt_at_entry ent,path_type&& path,nt_at_flags flags={})
{
	auto vw{::fast_io::details::to_its_cstring_view(path)};
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt,details::posix_api_1x::unlinkat>(ent.handle,vw.c_str(),vw.size(),flags);
}

template<constructible_to_path path_type>
inline void zw_unlinkat(nt_at_entry ent,path_type&& path,nt_at_flags flags={})
{
	auto vw{::fast_io::details::to_its_cstring_view(path)};
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::zw,details::posix_api_1x::unlinkat>(ent.handle,vw.c_str(),vw.size(),flags);
}


#if !defined(__CYGWIN__) && !defined(__WINE__)
using native_at_flags = nt_at_flags;

template<constructible_to_path path_type>
inline void native_mkdirat(nt_at_entry ent,path_type&& path, perms pm=static_cast<perms>(436))
{
	auto vw{::fast_io::details::to_its_cstring_view(path)};
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt,details::posix_api_1x::mkdirat>(ent.handle,vw.c_str(),vw.size(),pm);
}

template<constructible_to_path path_type>
inline void native_unlinkat(nt_at_entry ent,path_type&& path,native_at_flags flags={})
{
	auto vw{::fast_io::details::to_its_cstring_view(path)};
	::fast_io::win32::nt::details::nt_deal_with1x<nt_family::nt,::fast_io::details::posix_api_1x::unlinkat>(ent.handle,vw.c_str(),vw.size(),flags);
}

template<constructible_to_path path_type>
inline void native_fchownat(nt_at_entry,path_type&&,std::uintptr_t,std::uintptr_t,[[maybe_unused]] nt_at_flags flags=nt_at_flags::symlink_nofollow)
{
//windows does not use POSIX user group system. stub it and it is perfectly fine. But nt_fchownat,zw_fchownat will not be provided since they do not exist.
}
#if 0
template<constructible_to_path old_path_type,constructible_to_path new_path_type>
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
