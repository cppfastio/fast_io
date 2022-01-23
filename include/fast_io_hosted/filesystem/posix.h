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
	constexpr auto& native_handle() noexcept
	{
		return dirp;
	}
	constexpr auto& native_handle() const noexcept
	{
		return dirp;
	}
	explicit constexpr operator bool() const noexcept
	{
		return dirp;
	}
	operator basic_posix_io_observer<char>() const noexcept
	{
		return {details::dirp_to_fd(dirp)};
	}
	constexpr native_handle_type release() noexcept
	{
		auto temp{dirp};
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
	constexpr posix_directory_file()=default;

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
	constexpr posix_directory_file(posix_directory_file&& other) noexcept:
		posix_directory_io_observer{other.release()}{}

	posix_directory_file& operator=(posix_directory_file&& other) noexcept
	{
		if(this==__builtin_addressof(other))
			return *this;
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

struct posix_directory_entry
{
	using native_char_type = char;
	using char_type = char8_t;
	DIR* dirp{};
	struct dirent* entry{};
	std::size_t d_namlen{};
	explicit operator posix_io_observer() const noexcept
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

inline constexpr cstring_view native_filename(posix_directory_entry pioe) noexcept
{
	return cstring_view(null_terminated,pioe.entry->d_name,pioe.d_namlen);
}

inline u8cstring_view filename(posix_directory_entry pioe) noexcept
{
	using char8_may_alias_const_ptr
#if __has_cpp_attribute(gnu::may_alias)
	[[gnu::may_alias]]
#endif
	=char8_t const*;
	return u8cstring_view(null_terminated,
		reinterpret_cast<char8_may_alias_const_ptr>(pioe.entry->d_name),
		pioe.d_namlen);
}

inline constexpr std::uintmax_t inode(posix_directory_entry pioe) noexcept
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
	if(entry==nullptr&&errno)
		throw_posix_error();
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

inline ::fast_io::freestanding::default_sentinel_t end(posix_directory_generator const&) noexcept
{
	return {};
}

inline constexpr bool operator==(::fast_io::freestanding::default_sentinel_t, posix_directory_iterator const& b) noexcept
{
	return b.entry == nullptr;
}
inline constexpr bool operator==(posix_directory_iterator const& b, ::fast_io::freestanding::default_sentinel_t) noexcept
{
	return b.entry == nullptr;
}
inline constexpr bool operator!=(::fast_io::freestanding::default_sentinel_t, posix_directory_iterator const& b) noexcept
{
	return b.entry;
}
inline constexpr bool operator!=(posix_directory_iterator const& b, ::fast_io::freestanding::default_sentinel_t) noexcept
{
	return b.entry;
}

inline posix_directory_generator current(posix_at_entry pate)
{
	return {.dir_fl=posix_directory_file(posix_file(details::sys_dup(pate.fd)))};
}

struct posix_recursive_directory_iterator
{
	DIR* dirp{};
	struct dirent* entry{};
	std::size_t d_namlen{};
	details::naive_vector<posix_directory_file> stack;
	posix_recursive_directory_iterator()=default;
	posix_recursive_directory_iterator(DIR* dp):dirp(dp){}
	posix_recursive_directory_iterator(posix_recursive_directory_iterator const&)=delete;
	posix_recursive_directory_iterator& operator=(posix_recursive_directory_iterator const&)=delete;
	posix_recursive_directory_iterator(posix_recursive_directory_iterator&&) noexcept=default;
	posix_recursive_directory_iterator& operator=(posix_recursive_directory_iterator&&) noexcept=default;
};

struct posix_recursive_directory_generator
{
	posix_directory_file dir_fl;
};

inline std::size_t depth(posix_recursive_directory_iterator const& prdit) noexcept
{
	return prdit.stack.size();
}

inline posix_recursive_directory_iterator& operator++(posix_recursive_directory_iterator& prdit)
{
	for(;;)
	{
		errno=0;
		if(prdit.stack.empty())
		{
			auto entry{readdir(prdit.dirp)};
			if(entry==nullptr)
			{
				if(errno)
					throw_posix_error();
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
				if(errno)
					throw_posix_error();
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

inline void pop(posix_recursive_directory_iterator& prdit)
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

inline posix_recursive_directory_iterator begin(posix_recursive_directory_generator const& pdg)
{
	auto dirp{pdg.dir_fl.dirp};
	::rewinddir(dirp);
	posix_recursive_directory_iterator pdit{dirp};
	++pdit;
	return pdit;
}

inline ::fast_io::freestanding::default_sentinel_t end(posix_recursive_directory_generator const&) noexcept
{
	return {};
}

inline posix_directory_entry operator*(posix_recursive_directory_iterator const& prdit) noexcept
{
	return {prdit.stack.empty()?prdit.dirp:prdit.stack.back().dirp,prdit.entry,prdit.d_namlen};
}

inline bool operator==(::fast_io::freestanding::default_sentinel_t, posix_recursive_directory_iterator const& b) noexcept
{
	return b.stack.empty()&&b.entry == nullptr;
}

inline bool operator==(posix_recursive_directory_iterator const& b, ::fast_io::freestanding::default_sentinel_t sntnl) noexcept
{
	return sntnl==b;
}

inline bool operator!=(::fast_io::freestanding::default_sentinel_t sntnl, posix_recursive_directory_iterator const& b) noexcept
{
	return !(sntnl==b);
}

inline bool operator!=(posix_recursive_directory_iterator const& b, ::fast_io::freestanding::default_sentinel_t sntnl) noexcept
{
	return sntnl!=b;
}

inline posix_recursive_directory_generator recursive(posix_at_entry pate)
{
	return {.dir_fl=posix_directory_file(posix_file(details::sys_dup(pate.fd)))};
}

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,posix_directory_entry>,
	posix_directory_entry ent) noexcept
{
	if constexpr(std::same_as<char_type,typename posix_directory_entry::native_char_type>)
		return native_filename(ent).size();
	else if constexpr(std::same_as<char_type,char8_t>)
		return filename(ent).size();
	else
		return details::cal_full_reserve_size<
			sizeof(typename posix_directory_entry::native_char_type),
			sizeof(char_type)>(native_filename(ent).size());
}

inline u8cstring_view extension(posix_directory_entry ent) noexcept
{
	auto fnm{filename(ent)};
	auto pos{fnm.rfind(u8'.')};
	if(pos==static_cast<std::size_t>(-1))
		return {};
	if(pos==0)
		return {};
	if(2<fnm.size()&&pos==1&&fnm.front()==u8'.')
		return {};
	return u8cstring_view(null_terminated,fnm.data()+pos,fnm.data()+fnm.size());
}

inline ::fast_io::freestanding::u8string_view stem(posix_directory_entry ent) noexcept
{
	auto fnm{filename(ent)};
	auto pos{fnm.rfind(u8'.')};
	if(pos==static_cast<std::size_t>(-1))
		return ::fast_io::freestanding::u8string_view(fnm.data(),fnm.size());
	if(pos==0)
		return ::fast_io::freestanding::u8string_view(fnm.data(),fnm.size());
	if(2<fnm.size()&&pos==1&&fnm.front()==u8'.')
		return ::fast_io::freestanding::u8string_view(fnm.data(),fnm.size());
	return ::fast_io::freestanding::u8string_view(fnm.data(),pos);
}

template<std::integral char_type>
requires ((std::same_as<char_type,char8_t>)||(std::same_as<char_type,posix_directory_entry::native_char_type>))
inline basic_io_scatter_t<char_type> print_scatter_define(io_reserve_type_t<char_type,posix_directory_entry>,posix_directory_entry pth)
{
	if constexpr(std::same_as<char_type,char8_t>)
	{
		auto name{filename(pth)};
		return {name.data(),name.size()};
	}
	else
	{
		auto name{native_filename(pth)};
		return {name.data(),name.size()};
	}
}

template<::fast_io::freestanding::random_access_iterator Iter>
inline constexpr Iter print_reserve_define(io_reserve_type_t<::fast_io::freestanding::iter_value_t<Iter>,posix_directory_entry>,
	Iter iter,posix_directory_entry ent) noexcept
{
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
	if constexpr(std::same_as<char_type,typename posix_directory_entry::native_char_type>)
	{
		auto nfnm{native_filename(ent)};
		return details::non_overlapped_copy_n(nfnm.data(),nfnm.size(),iter);
	}
	else if constexpr(std::same_as<char_type,char8_t>)
	{
		auto fnm{filename(ent)};
		return details::non_overlapped_copy_n(fnm.data(),fnm.size(),iter);
	}
	else
	{
		auto fnm{filename(ent)};
		if constexpr(std::is_pointer_v<Iter>)
			return details::codecvt::general_code_cvt_full<encoding_scheme::utf>(fnm.data(),fnm.data()+fnm.size(),iter);
		else
			return iter+(details::codecvt::general_code_cvt_full<encoding_scheme::utf>(fnm.data(),fnm.data()+fnm.size(),::fast_io::freestanding::to_address(iter))-::fast_io::freestanding::to_address(iter));
	}
}

using native_directory_entry = posix_directory_entry;

}