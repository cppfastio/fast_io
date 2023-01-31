#pragma once
#include <dirent.h>

namespace fast_io
{

namespace details
{
inline int dirp_to_fd(DIR* dirp) noexcept
{
	if(dirp==nullptr)
		return -1;
#if defined(__CYGWIN__)
	return dirp->__d_fd;
#else
	return dirfd(dirp);
#endif
}

}

class posix_directory_io_observer
{
public:
	using native_handle_type = DIR*;
	native_handle_type dirp{};
	constexpr native_handle_type native_handle() const noexcept
	{
		return dirp;
	}
	explicit constexpr operator bool() const noexcept
	{
		return dirp;
	}
	template<std::integral char_type>
	operator basic_posix_io_observer<char_type>() const noexcept
	{
		return {details::dirp_to_fd(dirp)};
	}
	constexpr native_handle_type release() noexcept
	{
		native_handle_type temp{dirp};
		dirp=nullptr;
		return temp;
	}
};

namespace details
{

inline DIR* sys_dup_dir(DIR* dirp)
{
	if(dirp==nullptr)
		throw_posix_error(EBADF);
	auto fd{
#if defined(__CYGWIN__)
	dirp->__d_fd
#else
	dirfd(dirp)
#endif
	};
	if(fd==-1)
		throw_posix_error();
	auto newfd{details::sys_dup(fd)};
	auto newdir{::fdopendir(newfd)};
	if(newdir==nullptr)
	{
		details::sys_close(newfd);
		throw_posix_error();
	}
	return newdir;
}

}

class posix_directory_file:public posix_directory_io_observer
{
public:
	using native_handle_type = DIR*;
	constexpr posix_directory_file() noexcept=default;

	explicit constexpr posix_directory_file(posix_directory_io_observer) noexcept=delete;
	constexpr posix_directory_file& operator=(posix_directory_io_observer) noexcept=delete;
	template<typename native_hd>
	requires std::same_as<native_handle_type,std::remove_cvref_t<native_hd>>
	constexpr posix_directory_file(native_hd dirp1):posix_directory_io_observer{dirp1}{}
	posix_directory_file(posix_file&& pioh):posix_directory_io_observer{noexcept_call(::fdopendir,pioh.fd)}
	{
		if(this->dirp==nullptr)
			throw_posix_error();
		pioh.release();
	}
	posix_directory_file& operator=(posix_directory_file const& other)
	{
		auto newdir{details::sys_dup_dir(other.dirp)};
		if(this->dirp)[[likely]]
			noexcept_call(::closedir,this->dirp);
		this->dirp=newdir;
		return *this;
	}
	constexpr posix_directory_file(posix_directory_file&& __restrict other) noexcept:
		posix_directory_io_observer{other.release()}{}

	posix_directory_file& operator=(posix_directory_file&& __restrict other) noexcept
	{
		if(this->dirp)[[likely]]
			noexcept_call(::closedir,this->dirp);
		this->dirp=other.release();
		return *this;
	}
	inline void reset(native_handle_type dirp1=nullptr) noexcept
	{
		if(this->dirp)[[likely]]
			noexcept_call(::closedir,this->dirp);
		this->dirp=dirp1;
	}
	void close()
	{
		if(*this)[[likely]]
		{
			int ret{noexcept_call(::closedir,this->dirp)};
			this->dirp=nullptr;
			if(ret==-1)
				throw_posix_error();
		}
	}
	~posix_directory_file()
	{
		if(this->dirp)[[likely]]
			noexcept_call(::closedir,this->dirp);
	}
};

namespace freestanding
{

template<>
struct is_trivially_relocatable<posix_directory_file>
{
	inline static constexpr bool value = true;
};

template<>
struct is_zero_default_constructible<posix_directory_file>
{
	inline static constexpr bool value = true;
};

}

struct posix_directory_entry
{
	using native_char_type = char;
	using char_type = char8_t;
	DIR* dirp{};
	struct dirent* entry{};
	std::size_t d_namlen{};
	template<std::integral ch_type>
	explicit operator basic_posix_io_observer<ch_type>() const noexcept
	{
		return {details::dirp_to_fd(dirp)};
	}
};

inline posix_fs_dirent drt(posix_directory_entry ndet) noexcept
{
	return posix_fs_dirent{details::dirp_to_fd(ndet.dirp),ndet.entry->d_name};
}

inline posix_at_entry at(posix_directory_entry ndet) noexcept
{
	return posix_at_entry{details::dirp_to_fd(ndet.dirp)};
}

inline constexpr ::fast_io::manipulators::basic_os_c_str_with_known_size<char> native_filename(posix_directory_entry pioe) noexcept
{
	return {pioe.entry->d_name,pioe.d_namlen};
}

inline ::fast_io::manipulators::basic_os_c_str_with_known_size<char8_t> u8filename(posix_directory_entry pioe) noexcept
{
	using char8_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	=char8_t const*;
	return {reinterpret_cast<char8_may_alias_const_ptr>(pioe.entry->d_name),pioe.d_namlen};
}

inline constexpr std::uint_least64_t inode_ul64(posix_directory_entry pioe) noexcept
{
	return pioe.entry->d_ino;
}

inline constexpr file_type type(posix_directory_entry pioe) noexcept
{
#ifdef __CYGWIN__
	switch(pioe.entry->d_type)
	{
	case 6:
		return file_type::block;
	case 2:
		return file_type::character;
	case 4:
		return file_type::directory;
	case 1:
		return file_type::fifo;
	case 10:
		return file_type::symlink;
	case 8:
		return file_type::regular;
	case 12:
		return file_type::socket;
	case 0:
		return file_type::unknown;
	default:
		return file_type::not_found;
	};
#else
	switch(pioe.entry->d_type)
	{
	case DT_BLK:
		return file_type::block;
	case DT_CHR:
		return file_type::character;
	case DT_DIR:
		return file_type::directory;
	case DT_FIFO:
		return file_type::fifo;
	case DT_LNK:
		return file_type::symlink;
	case DT_REG:
		return file_type::regular;
#if defined(DT_SOCK)
	case DT_SOCK:
		return file_type::socket;
#endif
	case DT_UNKNOWN:
		return file_type::unknown;
	default:
		return file_type::not_found;
	};
#endif
}

struct posix_directory_iterator
{
	DIR* dirp{};
	struct dirent* entry{};
	std::size_t d_namlen{};
};

struct posix_directory_generator
{
	posix_directory_file dir_fl;
};

inline posix_directory_entry operator*(posix_directory_iterator pdit) noexcept
{
	return {pdit.dirp,pdit.entry,pdit.d_namlen};
}

inline posix_directory_iterator& operator++(posix_directory_iterator& pdit)
{
/*
To fix: avoid setting errno
*/
	errno=0;
	auto entry{readdir(pdit.dirp)};
	auto en{errno};
	if(entry==nullptr&&en)
		throw_posix_error(en);
	pdit.entry=entry;
	if(entry==nullptr)
		pdit.d_namlen=0;
	else
	{
#if defined(_DIRENT_HAVE_D_NAMLEN)
		pdit.d_namlen=entry->d_namlen;
#else
		pdit.d_namlen=cstr_len(entry->d_name);
#endif
	}
	return pdit;
}

inline posix_directory_iterator begin(posix_directory_generator const& pdg)
{
	auto dirp{pdg.dir_fl.dirp};
	::rewinddir(dirp);
	posix_directory_iterator pdit{dirp};
	++pdit;
	return pdit;
}

inline ::std::default_sentinel_t end(posix_directory_generator const&) noexcept
{
	return {};
}

inline constexpr bool operator==(::std::default_sentinel_t, posix_directory_iterator const& b) noexcept
{
	return b.entry == nullptr;
}
inline constexpr bool operator==(posix_directory_iterator const& b, ::std::default_sentinel_t) noexcept
{
	return b.entry == nullptr;
}
inline constexpr bool operator!=(::std::default_sentinel_t, posix_directory_iterator const& b) noexcept
{
	return b.entry;
}
inline constexpr bool operator!=(posix_directory_iterator const& b, ::std::default_sentinel_t) noexcept
{
	return b.entry;
}

inline posix_directory_generator current(posix_at_entry pate)
{
	return {.dir_fl=posix_directory_file(posix_file(details::sys_dup(pate.fd)))};
}

template<typename StackType>
struct basic_posix_recursive_directory_iterator
{
	using stack_type = StackType;
	DIR* dirp{};
	struct dirent* entry{};
	std::size_t d_namlen{};
	stack_type stack;
	constexpr basic_posix_recursive_directory_iterator()=default;
	explicit constexpr basic_posix_recursive_directory_iterator(DIR* dp):dirp(dp){}
	basic_posix_recursive_directory_iterator(basic_posix_recursive_directory_iterator const&)=delete;
	basic_posix_recursive_directory_iterator& operator=(basic_posix_recursive_directory_iterator const&)=delete;
	basic_posix_recursive_directory_iterator(basic_posix_recursive_directory_iterator&&) noexcept=default;
	basic_posix_recursive_directory_iterator& operator=(basic_posix_recursive_directory_iterator&&) noexcept=default;
};

template<typename StackType>
struct basic_posix_recursive_directory_generator
{
	using stack_type = StackType;
	posix_directory_file dir_fl;
};

using posix_recursive_directory_generator = basic_posix_recursive_directory_generator<::fast_io::containers::vector<posix_directory_file,::fast_io::posix_api_encoding_converter::allocator_type>>;

template<typename StackType>
inline std::size_t depth(basic_posix_recursive_directory_iterator<StackType> const& prdit) noexcept
{
	return prdit.stack.size();
}

template<typename StackType>
inline basic_posix_recursive_directory_iterator<StackType>& operator++(basic_posix_recursive_directory_iterator<StackType>& prdit)
{
	for(;;)
	{
		errno=0;
		if(prdit.stack.empty())
		{
			auto entry{readdir(prdit.dirp)};
			if(entry==nullptr)
			{
				auto en{errno};
				if(en)
					throw_posix_error(en);
				prdit.entry=nullptr;
				return prdit;
			}
			prdit.entry=entry;
		}
		else
		{
			auto entry=readdir(prdit.stack.back().dirp);
			if(entry==nullptr)
			{
				auto en{errno};
				if(en)
					throw_posix_error(en);
				prdit.stack.pop_back();
				continue;
			}
			prdit.entry=entry;
		}
		if(prdit.entry->d_type==
#if defined(__CYGWIN__)
		4
#else
		DT_DIR
#endif
		)
		{
			auto name{prdit.entry->d_name};
			if((*name==u8'.'&&name[1]==0)||(*name==u8'.'&&name[1]==u8'.'&&name[2]==0))
				continue;
			prdit.stack.emplace_back(::fast_io::posix_file(::fast_io::posix_fs_dirent{details::dirp_to_fd(prdit.stack.empty()?prdit.dirp:prdit.stack.back().dirp),name},::fast_io::open_mode::directory));
		}
		if(prdit.entry)[[likely]]
		{
#if defined(_DIRENT_HAVE_D_NAMLEN)
			prdit.d_namlen=prdit.entry->d_namlen;
#else
			prdit.d_namlen=cstr_len(prdit.entry->d_name);
#endif
		}
		else
		{
			prdit.d_namlen=0;
		}
		return prdit;
	}
}

template<typename StackType>
inline void pop(basic_posix_recursive_directory_iterator<StackType>& prdit) noexcept
{
	if(prdit.stack.empty())
	{
		prdit.entry=nullptr;
		prdit.d_namlen=0;
	}
	else
	{
		prdit.stack.pop_back();
		++prdit;
	}
}

template<typename StackType>
inline basic_posix_recursive_directory_iterator<StackType> begin(basic_posix_recursive_directory_generator<StackType> const& pdg)
{
	auto dirp{pdg.dir_fl.dirp};
	::rewinddir(dirp);
	basic_posix_recursive_directory_iterator<StackType> pdit{dirp};
	++pdit;
	return pdit;
}

template<typename StackType>
inline ::std::default_sentinel_t end(basic_posix_recursive_directory_generator<StackType> const&) noexcept
{
	return {};
}

template<typename StackType>
inline posix_directory_entry operator*(basic_posix_recursive_directory_iterator<StackType> const& prdit) noexcept
{
	return {prdit.stack.empty()?prdit.dirp:prdit.stack.back().dirp,prdit.entry,prdit.d_namlen};
}

template<typename StackType>
inline bool operator==(::std::default_sentinel_t, basic_posix_recursive_directory_iterator<StackType> const& b) noexcept
{
	return b.stack.empty()&&b.entry == nullptr;
}

template<typename StackType>
inline bool operator==(basic_posix_recursive_directory_iterator<StackType> const& b, ::std::default_sentinel_t sntnl) noexcept
{
	return sntnl==b;
}

template<typename StackType>
inline bool operator!=(::std::default_sentinel_t sntnl, basic_posix_recursive_directory_iterator<StackType> const& b) noexcept
{
	return !(sntnl==b);
}

template<typename StackType>
inline bool operator!=(basic_posix_recursive_directory_iterator<StackType> const& b, ::std::default_sentinel_t sntnl) noexcept
{
	return sntnl!=b;
}

inline posix_recursive_directory_generator recursive(posix_at_entry pate)
{
	return {.dir_fl=posix_directory_file(posix_file(details::sys_dup(pate.fd)))};
}

inline auto native_extension(posix_directory_entry ent) noexcept
{
	return ::fast_io::details::find_dot_and_sep<false,char,char>(ent.entry->d_name,ent.d_namlen);
}

inline auto native_stem(posix_directory_entry ent) noexcept
{
	return ::fast_io::details::find_dot_and_sep<true,char,char>(ent.entry->d_name,ent.d_namlen);
}

inline auto u8extension(posix_directory_entry ent) noexcept
{
	return ::fast_io::details::find_dot_and_sep<false,char8_t,char>(ent.entry->d_name,ent.d_namlen);
}

inline auto u8stem(posix_directory_entry ent) noexcept
{
	return ::fast_io::details::find_dot_and_sep<true,char8_t,char>(ent.entry->d_name,ent.d_namlen);
}

inline cross_code_cvt_t<char8_t> print_alias_define(io_alias_t,posix_directory_entry pth) noexcept
{
	using char8_const_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
	= char8_t const*;
	return {{reinterpret_cast<char8_const_may_alias_ptr>(pth.entry->d_name),pth.d_namlen}};
}

using native_directory_entry = posix_directory_entry;

}