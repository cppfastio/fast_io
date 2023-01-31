#pragma once

namespace fast_io
{

/*
https://github.com/reactos/reactos/blob/7ffb6a09c370ef51de554a1ce913f871d72b76ca/dll/win32/kernel32/client/file/find.c#L647

HANDLE
WINAPI
FindFirstFileExW(IN LPCWSTR lpFileName,
                 IN FINDEX_INFO_LEVELS fInfoLevelId,
                 OUT LPVOID lpFindFileData,
                 IN FINDEX_SEARCH_OPS fSearchOp,
                 LPVOID lpSearchFilter,
                 IN DWORD dwAdditionalFlags)

*/

struct nt_dirent
{
	void* d_handle{};
	file_type d_type{};
	char16_t native_d_name[0x2001];
	std::size_t native_d_namlen{};
	char8_t u8d_name[0x8004];
	std::size_t u8d_namlen{};
};

namespace win32::nt::details
{

template<typename Allocator,typename T>
struct nt_dirent_space_guard
{
	using pointer = T*;
	pointer ptr{};
	constexpr nt_dirent_space_guard() noexcept = default;
	nt_dirent_space_guard(nt_dirent_space_guard const&)=delete;
	nt_dirent_space_guard& operator=(nt_dirent_space_guard const&)=delete;
	constexpr T* release() noexcept
	{
		pointer temp{ptr};
		this->ptr=nullptr;
		return temp;
	}
	~nt_dirent_space_guard()
	{
		if(ptr)
		{
			typed_generic_allocator_adapter<Allocator,T>::deallocate_n(ptr,1);
		}
	}
};

template<typename Allocator>
inline
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
constexpr
#endif
nt_dirent* new_nt_dirent() noexcept
{
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if(__builtin_is_constant_evaluated())
#endif
	{
		return new nt_dirent;
	}
	else
#endif
	{
		nt_dirent_space_guard<Allocator,nt_dirent> guard;
		guard.ptr=typed_generic_allocator_adapter<Allocator,nt_dirent>::allocate(1);
		::new (guard.ptr) nt_dirent;
		return guard.release();
	}
}

template<typename Allocator>
inline
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
constexpr
#endif
void delete_nt_dirent(nt_dirent* ptr) noexcept
{
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if(__builtin_is_constant_evaluated())
#endif
	{
		delete ptr;
	}
	else
#endif
	{
		if(ptr==nullptr)
		{
			return;
		}
		ptr->~nt_dirent();
		typed_generic_allocator_adapter<Allocator,nt_dirent>::deallocate_n(ptr, 1);
	}
}

template<nt_family family>
inline nt_dirent* set_nt_dirent(nt_dirent* entry,bool start)
{
	io_status_block block{};
	constexpr std::uint_least32_t ul32_buffer_size{0x4000};
	std::byte buffer[ul32_buffer_size];
	::fast_io::win32::nt::dir_information d_info{buffer};
	auto status{nt_query_directory_file<family==nt_family::zw>(entry->d_handle,nullptr,nullptr,nullptr,
	__builtin_addressof(block),d_info.DirInfo,
	ul32_buffer_size,file_information_class::FileFullDirectoryInformation,
	true,nullptr,start)};
	if(status)
	{
		if(status==2147483654)[[likely]]
			return nullptr;
		throw_nt_error(status);
	}
	auto ful_dir_info{d_info.FullDirInfo};
	entry->native_d_namlen=ful_dir_info->FileNameLength/sizeof(char16_t);

	::fast_io::freestanding::nonoverlapped_bytes_copy_n(
		reinterpret_cast<std::byte const*>(ful_dir_info->FileName),ful_dir_info->FileNameLength,
		reinterpret_cast<std::byte*>(entry->native_d_name));
	entry->native_d_name[entry->native_d_namlen]=0;

	char16_t const* enstart{entry->native_d_name};
	char16_t const* enend{enstart+entry->native_d_namlen};
	entry->u8d_namlen=static_cast<std::size_t>(::fast_io::details::codecvt::general_code_cvt_full(enstart,enend,entry->u8d_name)-entry->u8d_name);
	entry->u8d_name[entry->u8d_namlen]=0;
/*
Referenced from win32 port dirent.h
https://github.com/win32ports/dirent_h/blob/5a40afce928f1780058f44e0dda37553c662a8a7/dirent.h#L249
		if (((w32fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) == FILE_ATTRIBUTE_REPARSE_POINT) && __islink(wname, buffer))
			data->entries[data->index].d_type = DT_LNK;
		else if ((w32fd.dwFileAttributes & FILE_ATTRIBUTE_DEVICE) == FILE_ATTRIBUTE_DEVICE)
			data->entries[data->index].d_type = DT_CHR;
		else if ((w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			data->entries[data->index].d_type = DT_DIR;
		else
			data->entries[data->index].d_type = DT_REG;
*/

	std::uint_least32_t attribute{ful_dir_info->FileAttributes};
	if(attribute&0x400)
		entry->d_type=file_type::symlink;
	else if(attribute&0x40)
		entry->d_type=file_type::character;
	else if(attribute&0x10)
		entry->d_type=file_type::directory;
	else if(attribute&0x1000)
		entry->d_type=file_type::remote;
	else
		entry->d_type=file_type::regular;
	return entry;
}

template<nt_family family>
inline nt_dirent* set_nt_dirent_first(nt_dirent* entry)
{
	return set_nt_dirent<family>(entry,true);
}

template<nt_family family>
inline nt_dirent* nt_dirent_next(nt_dirent* entry)
{
	return set_nt_dirent<family>(entry,false);
}

}

struct nt_directory_entry
{
	using native_char_type = char16_t;
	using char_type = char8_t;
	nt_dirent* entry{};
	template<nt_family family,std::integral ch_type>
	explicit constexpr operator basic_nt_family_io_observer<family,ch_type>() const noexcept
	{
		return {entry->d_handle};
	}
	template<win32_family family,std::integral ch_type>
	explicit constexpr operator basic_win32_family_io_observer<family,ch_type>() const noexcept
	{
		return {entry->d_handle};
	}
};

inline constexpr nt_at_entry at(nt_directory_entry ndet) noexcept
{
	return nt_at_entry{ndet.entry->d_handle};
}

inline constexpr ::fast_io::manipulators::basic_os_c_str_with_known_size<char16_t> native_filename(nt_directory_entry pioe) noexcept
{
	auto& ent{*pioe.entry};
	return {ent.native_d_name,ent.native_d_namlen};
}

inline constexpr nt_fs_dirent drt(nt_directory_entry pioe) noexcept
{
	auto& ent{*pioe.entry};
	return nt_fs_dirent{ent.d_handle,{ent.native_d_name,ent.native_d_namlen}};
}

inline constexpr ::fast_io::manipulators::basic_os_c_str_with_known_size<char8_t> u8filename(nt_directory_entry pioe) noexcept
{
	auto& ent{*pioe.entry};
	return {ent.u8d_name,ent.u8d_namlen};
}

inline constexpr std::uint_least64_t inode_ul64(nt_directory_entry) noexcept
{
	return 0;
}

inline constexpr file_type type(nt_directory_entry pioe) noexcept
{
	return pioe.entry->d_type;
}

template<nt_family family>
struct nt_family_directory_iterator
{
	nt_dirent* entry{};
};

template<nt_family family>
inline nt_directory_entry operator*(nt_family_directory_iterator<family> pdit) noexcept
{
	return {pdit.entry};
}

template<nt_family family>
inline nt_family_directory_iterator<family>& operator++(nt_family_directory_iterator<family>& pdit)
{
	pdit.entry=win32::nt::details::nt_dirent_next<family>(pdit.entry);
	return pdit;
}

template<nt_family family>
inline constexpr bool operator==(::std::default_sentinel_t, nt_family_directory_iterator<family> b) noexcept
{
	return b.entry==nullptr;
}

template<nt_family family>
inline constexpr bool operator==(nt_family_directory_iterator<family> b, ::std::default_sentinel_t other) noexcept
{
	return other==b;
}

template<nt_family family>
inline constexpr bool operator!=(::std::default_sentinel_t other,nt_family_directory_iterator<family> b) noexcept
{
	return !(other==b);
}

template<nt_family family>
inline constexpr bool operator!=(nt_family_directory_iterator<family> b, ::std::default_sentinel_t other) noexcept
{
	return !(other==b);
}

template<nt_family family,typename Allocator>
struct basic_nt_family_directory_generator
{
	using allocator_type = Allocator;
	nt_dirent* entry{};
	constexpr basic_nt_family_directory_generator() noexcept=default;
	explicit constexpr basic_nt_family_directory_generator(void* directory_handle) noexcept:entry(::fast_io::win32::nt::details::new_nt_dirent<Allocator>())
	{
		entry->d_handle=directory_handle;
	}

	basic_nt_family_directory_generator(basic_nt_family_directory_generator const&)=delete;
	basic_nt_family_directory_generator& operator=(basic_nt_family_directory_generator const&)=delete;
	constexpr basic_nt_family_directory_generator(basic_nt_family_directory_generator&& __restrict other) noexcept:entry(other.entry)
	{
		other.entry=nullptr;
	}
	constexpr basic_nt_family_directory_generator& operator=(basic_nt_family_directory_generator&& __restrict other) noexcept
	{
		::fast_io::win32::nt::details::delete_nt_dirent<Allocator>(entry);
		entry=other.entry;
		other.entry=nullptr;
		return *this;
	}
	constexpr ~basic_nt_family_directory_generator()
	{
		::fast_io::win32::nt::details::delete_nt_dirent<Allocator>(entry);
	}
};

template<nt_family family>
using nt_family_directory_generator = basic_nt_family_directory_generator<family,typename nt_api_encoding_converter::allocator_type>;

template<nt_family family,typename Allocator>
inline nt_family_directory_iterator<family> begin(basic_nt_family_directory_generator<family,Allocator> const& pdg)
{
	return {win32::nt::details::set_nt_dirent_first<family>(pdg.entry)};
}

template<nt_family family,typename Allocator>
inline ::std::default_sentinel_t end(basic_nt_family_directory_generator<family,Allocator> const&) noexcept
{
	return {};
}

using nt_directory_generator = nt_family_directory_generator<nt_family::nt>;
using zw_directory_generator = nt_family_directory_generator<nt_family::zw>;

inline nt_directory_generator current(nt_at_entry nate)
{
	return nt_directory_generator(nate.handle);
}

inline zw_directory_generator current(zw_at_entry nate)
{
	return zw_directory_generator(nate.handle);
}

template<nt_family family,typename StackType>
struct basic_nt_family_recursive_directory_iterator
{
	using stack_type = StackType;
	void* root_handle{};
	nt_dirent* entry{};
	stack_type stack;
	constexpr basic_nt_family_recursive_directory_iterator()=default;
	basic_nt_family_recursive_directory_iterator(void* root_han,nt_dirent* ent):root_handle(root_han),entry(ent){}
	basic_nt_family_recursive_directory_iterator(basic_nt_family_recursive_directory_iterator const&)=delete;
	basic_nt_family_recursive_directory_iterator& operator=(basic_nt_family_recursive_directory_iterator const&)=delete;
	basic_nt_family_recursive_directory_iterator(basic_nt_family_recursive_directory_iterator&& __restrict) noexcept=default;
	basic_nt_family_recursive_directory_iterator& operator=(basic_nt_family_recursive_directory_iterator&& __restrict) noexcept=default;
};

template<nt_family family,typename Allocator,typename StackType>
struct basic_nt_family_recursive_directory_generator
{
	using allocator_type = Allocator;
	using stack_type = StackType;
	void* root_handle{};
	nt_dirent* entry{};
	constexpr basic_nt_family_recursive_directory_generator()=default;
	explicit constexpr basic_nt_family_recursive_directory_generator(void* rhd):root_handle(rhd),entry(::fast_io::win32::nt::details::new_nt_dirent<Allocator>()){}
	basic_nt_family_recursive_directory_generator(basic_nt_family_recursive_directory_generator const&)=delete;
	basic_nt_family_recursive_directory_generator& operator=(basic_nt_family_recursive_directory_generator const&)=delete;
	constexpr basic_nt_family_recursive_directory_generator(basic_nt_family_recursive_directory_generator&& __restrict other) noexcept:root_handle(other.root_handle),entry(other.entry)
	{
		other.root_handle=nullptr;
		entry=nullptr;
	}
	constexpr basic_nt_family_recursive_directory_generator& operator=(basic_nt_family_recursive_directory_generator&& __restrict other) noexcept
	{
		::fast_io::win32::nt::details::delete_nt_dirent<Allocator>(this->entry);
		root_handle=other.root_handle;
		entry=other.entry;
		return *this;
	}
	constexpr ~basic_nt_family_recursive_directory_generator()
	{
		::fast_io::win32::nt::details::delete_nt_dirent<Allocator>(this->entry);
	}
};

template<nt_family family,typename StackType>
inline std::size_t depth(basic_nt_family_recursive_directory_iterator<family,StackType> const& prdit) noexcept
{
	return prdit.stack.size();
}

template<nt_family family,typename StackType>
inline basic_nt_family_recursive_directory_iterator<family,StackType>& operator++(basic_nt_family_recursive_directory_iterator<family,StackType>& prdit)
{
	for(;;)
	{
		if(prdit.stack.empty())
		{
			prdit.entry->d_handle=prdit.root_handle;
			prdit.entry=win32::nt::details::nt_dirent_next<family>(prdit.entry);
			if(prdit.entry==nullptr)
				return prdit;
		}
		else
		{
			prdit.entry->d_handle=prdit.stack.back().handle;
			auto entry=win32::nt::details::nt_dirent_next<family>(prdit.entry);
			if(entry==nullptr)
			{
				prdit.stack.pop_back();
				continue;
			}
			prdit.entry=entry;
		}
		if(prdit.entry->d_type==file_type::directory)
		{
			std::size_t const native_d_namlen{prdit.entry->native_d_namlen};
			char16_t const *native_d_name_ptr{prdit.entry->native_d_name};
			if((native_d_namlen==1&&*native_d_name_ptr==u'.')||(native_d_namlen==2&&*native_d_name_ptr==u'.'&&native_d_name_ptr[1]==u'.'))
				continue;
			prdit.stack.emplace_back(nt_at_entry{prdit.stack.empty()?prdit.root_handle:prdit.stack.back().handle},
				::fast_io::manipulators::basic_os_c_str_with_known_size<char16_t>{native_d_name_ptr,native_d_namlen},
				open_mode::directory);
		}
		return prdit;
	}
	return prdit;
}

template<nt_family family,typename StackType>
inline void pop(basic_nt_family_recursive_directory_iterator<family,StackType>& prdit)
{
	if(prdit.stack.empty())
		prdit.entry=nullptr;
	else
	{
		prdit.stack.pop_back();
		++prdit;
	}
}

template<nt_family family,typename AllocatorType,typename StackType>
inline basic_nt_family_recursive_directory_iterator<family,StackType> begin(basic_nt_family_recursive_directory_generator<family,AllocatorType,StackType> const& prg) noexcept
{
	basic_nt_family_recursive_directory_iterator<family,StackType> prdit{prg.root_handle,prg.entry};
	prdit.entry->d_handle=prg.root_handle;
	prdit.entry=win32::nt::details::set_nt_dirent_first<family>(prdit.entry);
	if(prdit.entry&&prdit.entry->d_type==file_type::directory)
	{
		auto& ent{*prdit.entry};
		char16_t const *native_d_name_ptr{ent.native_d_name};
		std::size_t const native_d_namlen{ent.native_d_namlen};
		if((native_d_namlen==1&&*native_d_name_ptr==u'.')||(native_d_namlen==2&&*native_d_name_ptr==u'.'&&native_d_name_ptr[1]==u'.'))
		{
			++prdit;
		}
		else
		{
			prdit.stack.emplace_back(nt_at_entry{prdit.root_handle},
				::fast_io::manipulators::basic_os_c_str_with_known_size<char16_t>{native_d_name_ptr,native_d_namlen},
				open_mode::directory);
		}
	}
	return prdit;
}

template<nt_family family,typename AllocatorType,typename StackType>
inline ::std::default_sentinel_t end(basic_nt_family_recursive_directory_generator<family,AllocatorType,StackType> const&) noexcept
{
	return {};
}

template<nt_family family,typename StackType>
inline nt_directory_entry operator*(basic_nt_family_recursive_directory_iterator<family,StackType> const& prdit) noexcept
{
	return {prdit.entry};
}

template<nt_family family,typename StackType>
inline bool operator==(::std::default_sentinel_t, basic_nt_family_recursive_directory_iterator<family,StackType> const& b) noexcept
{
	return b.stack.empty()&&b.entry == nullptr;
}

template<nt_family family,typename StackType>
inline bool operator==(basic_nt_family_recursive_directory_iterator<family,StackType> const& b, ::std::default_sentinel_t sntnl) noexcept
{
	return sntnl==b;
}

template<nt_family family,typename StackType>
inline bool operator!=(::std::default_sentinel_t sntnl, basic_nt_family_recursive_directory_iterator<family,StackType> const& b) noexcept
{
	return !(sntnl==b);
}

template<nt_family family,typename StackType>
inline bool operator!=(basic_nt_family_recursive_directory_iterator<family,StackType> const& b, ::std::default_sentinel_t sntnl) noexcept
{
	return sntnl!=b;
}

template<nt_family family>
using nt_family_recursive_directory_generator = basic_nt_family_recursive_directory_generator<family,
	typename nt_directory_generator::allocator_type,
	::fast_io::containers::vector<basic_nt_family_file<family,char>,typename nt_directory_generator::allocator_type>>;

using nt_recursive_directory_generator = nt_family_recursive_directory_generator<nt_family::nt>;
using zw_recursive_directory_generator = nt_family_recursive_directory_generator<nt_family::zw>;

inline nt_recursive_directory_generator recursive(nt_at_entry nate)
{
	return nt_recursive_directory_generator{nate.handle};
}

inline zw_recursive_directory_generator recursive(zw_at_entry zate)
{
	return zw_recursive_directory_generator{zate.handle};
}

inline auto native_extension(nt_directory_entry ent) noexcept
{
	auto& et{*ent.entry};
	return ::fast_io::details::find_dot_and_sep<false,char16_t,char16_t>(et.native_d_name,et.native_d_namlen);
}

inline auto native_stem(nt_directory_entry ent) noexcept
{
	auto& et{*ent.entry};
	return ::fast_io::details::find_dot_and_sep<true,char16_t,char16_t>(et.native_d_name,et.native_d_namlen);
}

inline auto u8extension(nt_directory_entry ent) noexcept
{
	auto& et{*ent.entry};
	return ::fast_io::details::find_dot_and_sep<false,char8_t,char8_t>(et.u8d_name,et.u8d_namlen);
}

inline auto u8stem(nt_directory_entry ent) noexcept
{
	auto& et{*ent.entry};
	return ::fast_io::details::find_dot_and_sep<true,char8_t,char8_t>(et.u8d_name,et.u8d_namlen);
}

template<std::integral char_type>
inline constexpr auto status_io_print_forward(io_alias_type_t<char_type>,nt_directory_entry ent) noexcept
{
	auto& et{*ent.entry};
	if constexpr(sizeof(char_type)==1||(std::same_as<char_type,char>&&
		::fast_io::execution_charset_encoding_scheme<char>()==::fast_io::encoding_scheme::utf))
	{
		return status_io_print_forward(io_alias_type<char_type>,
			::fast_io::cross_code_cvt_t<char8_t>{{et.u8d_name,et.u8d_namlen}});
	}
	else
	{
		return status_io_print_forward(io_alias_type<char_type>,
			::fast_io::cross_code_cvt_t<char16_t>{{et.native_d_name,et.native_d_namlen}});
	}
}


#ifndef __CYGWIN__
using native_directory_entry = nt_directory_entry;
#endif

}
