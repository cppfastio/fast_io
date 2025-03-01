#pragma once

#include <libc/fd_props.h>
#include <dirent.h>

namespace fast_io
{

struct dos_DIR
{
	DIR *dirp{};
	int fd{-1};
};

namespace details
{
inline int dirp_to_fd(dos_DIR dirp) noexcept
{
	if (dirp.dirp == nullptr) [[unlikely]]
	{
		return -1;
	}
	return dirp.fd;
}

} // namespace details

class dos_directory_io_observer
{
public:
	using native_handle_type = dos_DIR;
	native_handle_type dirp{};

	inline constexpr native_handle_type native_handle() const noexcept
	{
		return dirp;
	}

	inline explicit constexpr operator bool() const noexcept
	{
		return dirp.dirp;
	}

	template <::std::integral char_type>
	inline operator basic_posix_io_observer<char_type>() const noexcept
	{
		return {details::dirp_to_fd(dirp)};
	}

	inline constexpr native_handle_type release() noexcept
	{
		native_handle_type temp{dirp};
		dirp = {};
		return temp;
	}
};

namespace posix
{
// extern char const* my_dos_get_fd_name(int) noexcept __asm__("___get_fd_name");
extern DIR *my_dos_opendir(char const *) noexcept __asm__("_opendir");

inline DIR *my_dos_fdopendir(int fd) noexcept
{
	return my_dos_opendir(::fast_io::noexcept_call(::__get_fd_name, fd));
}
} // namespace posix

namespace details
{

inline dos_DIR sys_dup_dir(dos_DIR dirp)
{
	if (dirp.dirp == nullptr) [[unlikely]]
	{
		throw_posix_error(EBADF);
	}
	auto const fd{dirp.fd};
	if (fd == -1) [[unlikely]]
	{
		throw_posix_error();
	}
	auto newfd{details::sys_dup(fd)};
	auto newdir{::fast_io::posix::my_dos_fdopendir(newfd)};
	if (newdir == nullptr) [[unlikely]]
	{
		details::sys_close(newfd);
		throw_posix_error();
	}
	return {newdir, newfd};
}

} // namespace details

class dos_directory_file FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE : public dos_directory_io_observer
{
public:
	using native_handle_type = dos_DIR;
	inline constexpr dos_directory_file() noexcept = default;

	inline explicit constexpr dos_directory_file(dos_directory_io_observer) noexcept = delete;
	inline constexpr dos_directory_file &operator=(dos_directory_io_observer) noexcept = delete;

	template <typename native_hd>
		requires ::std::same_as<native_handle_type, ::std::remove_cvref_t<native_hd>>
	inline constexpr dos_directory_file(native_hd dirp1)
		: dos_directory_io_observer{dirp1}
	{
	}

	inline dos_directory_file(posix_file &&pioh)
		: dos_directory_io_observer{posix::my_dos_fdopendir(pioh.fd), pioh.fd}
	{
		if (this->dirp.dirp == nullptr)
		{
			throw_posix_error();
		}
		pioh.release();
	}

	inline dos_directory_file &operator=(dos_directory_file const &other)
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		auto newdir{details::sys_dup_dir(other.dirp)};
		if (this->dirp.dirp) [[likely]]
		{
			noexcept_call(::closedir, this->dirp.dirp);
		}
		this->dirp = newdir;
		return *this;
	}

	inline constexpr dos_directory_file(dos_directory_file &&__restrict other) noexcept
		: dos_directory_io_observer{other.release()}
	{}

	inline dos_directory_file &operator=(dos_directory_file &&__restrict other) noexcept
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		if (this->dirp.dirp) [[likely]]
		{
			noexcept_call(::closedir, this->dirp.dirp);
		}
		this->dirp = other.release();
		return *this;
	}

	inline void reset(native_handle_type dirp1 = {}) noexcept
	{
		if (this->dirp.dirp) [[likely]]
		{
			noexcept_call(::closedir, this->dirp.dirp);
		}
		this->dirp = dirp1;
	}

	inline void close()
	{
		if (*this) [[likely]]
		{
			int ret{noexcept_call(::closedir, this->dirp.dirp)};
			this->dirp.dirp = nullptr;
			this->dirp.fd = -1;
			if (ret == -1)
			{
				throw_posix_error();
			}
		}
	}

	inline ~dos_directory_file()
	{
		if (this->dirp.dirp) [[likely]]
		{
			noexcept_call(::closedir, this->dirp.dirp);
		}
	}
};

namespace freestanding
{
template <>
struct is_zero_default_constructible<dos_directory_io_observer>
{
	inline static constexpr bool value = true;
};

template <>
struct is_trivially_copyable_or_relocatable<dos_directory_file>
{
	inline static constexpr bool value = true;
};

template <>
struct is_zero_default_constructible<dos_directory_file>
{
	inline static constexpr bool value = true;
};

} // namespace freestanding

struct dos_directory_entry
{
	using native_char_type = char;
	using char_type = char8_t;
	dos_DIR dirp{};
	struct dirent *entry{};
	::std::size_t d_namlen{};

	template <::std::integral ch_type>
	inline explicit operator basic_posix_io_observer<ch_type>() const noexcept
	{
		return {details::dirp_to_fd(dirp)};
	}
};

inline posix_fs_dirent drt(dos_directory_entry ndet) noexcept
{
	return posix_fs_dirent{details::dirp_to_fd(ndet.dirp), ndet.entry->d_name};
}

inline posix_at_entry at(dos_directory_entry ndet) noexcept
{
	return posix_at_entry{details::dirp_to_fd(ndet.dirp)};
}

inline constexpr ::fast_io::manipulators::basic_os_c_str_with_known_size<char> native_filename(dos_directory_entry pioe) noexcept
{
	return {pioe.entry->d_name, pioe.d_namlen};
}

inline ::fast_io::manipulators::basic_os_c_str_with_known_size<char8_t> u8filename(dos_directory_entry pioe) noexcept
{
	using char8_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char8_t const *;
	return {reinterpret_cast<char8_may_alias_const_ptr>(pioe.entry->d_name), pioe.d_namlen};
}

inline constexpr ::std::uint_least64_t inode_ul64(dos_directory_entry) noexcept
{
	return 0;
}

inline constexpr file_type type(dos_directory_entry pioe) noexcept
{
	switch (pioe.entry->d_type)
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
}

inline bool is_dot(dos_directory_entry ent) noexcept
{
	auto name{ent.entry->d_name};
	return (*name == u8'.' && (name[1] == 0 || (name[1] == u8'.' && name[2] == 0)));
}

struct dos_directory_iterator
{
	dos_DIR dirp{};
	struct dirent *entry{};
	::std::size_t d_namlen{};
};

struct posix_directory_generator
{
	dos_directory_file dir_fl;
};

inline dos_directory_entry operator*(dos_directory_iterator pdit) noexcept
{
	return {pdit.dirp, pdit.entry, pdit.d_namlen};
}

inline dos_directory_iterator &operator++(dos_directory_iterator &pdit)
{
	/*
	To fix: avoid setting errno
	*/
	errno = 0;
	auto entry{readdir(pdit.dirp.dirp)};
	auto en{errno};
	if (entry == nullptr && en)
	{
		throw_posix_error(en);
	}
	pdit.entry = entry;
	if (entry == nullptr)
	{
		pdit.d_namlen = 0;
	}
	else
	{
#if defined(_DIRENT_HAVE_D_NAMLEN)
		pdit.d_namlen = entry->d_namlen;
#else
		pdit.d_namlen = cstr_len(entry->d_name);
#endif
	}
	return pdit;
}

inline dos_directory_iterator begin(posix_directory_generator const &pdg)
{
	auto dirp{pdg.dir_fl.dirp.dirp};
	::rewinddir(dirp);
	dos_directory_iterator pdit{dirp};
	++pdit;
	return pdit;
}

inline ::std::default_sentinel_t end(posix_directory_generator const &) noexcept
{
	return {};
}

inline constexpr bool operator==(::std::default_sentinel_t, dos_directory_iterator const &b) noexcept
{
	return b.entry == nullptr;
}

inline constexpr bool operator==(dos_directory_iterator const &b, ::std::default_sentinel_t) noexcept
{
	return b.entry == nullptr;
}

inline constexpr bool operator!=(::std::default_sentinel_t, dos_directory_iterator const &b) noexcept
{
	return b.entry;
}

inline constexpr bool operator!=(dos_directory_iterator const &b, ::std::default_sentinel_t) noexcept
{
	return b.entry;
}

inline posix_directory_generator current(posix_at_entry pate)
{
	return {.dir_fl = dos_directory_file(posix_file(details::sys_dup(pate.fd)))};
}

template <typename StackType>
struct basic_dos_recursive_directory_iterator
{
	using stack_type = StackType;
	dos_DIR dirp{};
	struct dirent *entry{};
	::std::size_t d_namlen{};
	stack_type stack;
	inline constexpr basic_dos_recursive_directory_iterator() = default;

	inline explicit constexpr basic_dos_recursive_directory_iterator(dos_DIR dp)
		: dirp(dp)
	{}

	inline basic_dos_recursive_directory_iterator(basic_dos_recursive_directory_iterator const &) = delete;
	inline basic_dos_recursive_directory_iterator &operator=(basic_dos_recursive_directory_iterator const &) = delete;
	inline basic_dos_recursive_directory_iterator(basic_dos_recursive_directory_iterator &&) noexcept = default;
	inline basic_dos_recursive_directory_iterator &operator=(basic_dos_recursive_directory_iterator &&) noexcept = default;
};

template <typename StackType>
struct basic_posix_recursive_directory_generator
{
	using stack_type = StackType;
	dos_directory_file dir_fl;
};

using posix_recursive_directory_generator =
	basic_posix_recursive_directory_generator<::fast_io::containers::vector<dos_directory_file, ::fast_io::posix_api_encoding_converter::allocator_type>>;

template <typename StackType>
inline ::std::size_t depth(basic_dos_recursive_directory_iterator<StackType> const &prdit) noexcept
{
	return prdit.stack.size();
}

template <typename StackType>
inline basic_dos_recursive_directory_iterator<StackType> &operator++(basic_dos_recursive_directory_iterator<StackType> &prdit)
{
	for (;;)
	{
		errno = 0;
		if (prdit.stack.empty())
		{
			auto entry{readdir(prdit.dirp.dirp)};
			if (entry == nullptr)
			{
				auto en{errno};
				if (en)
				{
					throw_posix_error(en);
				}
				prdit.entry = nullptr;
				return prdit;
			}
			prdit.entry = entry;
		}
		else
		{
			auto entry = readdir(prdit.stack.back().dirp.dirp);
			if (entry == nullptr)
			{
				auto en{errno};
				if (en)
				{
					throw_posix_error(en);
				}
				prdit.stack.pop_back();
				continue;
			}
			prdit.entry = entry;
		}
		if (prdit.entry->d_type ==
#if defined(__CYGWIN__)
			4
#else
			DT_DIR
#endif
		)
		{
			auto name{prdit.entry->d_name};
			if ((*name == u8'.' && name[1] == 0) || (*name == u8'.' && name[1] == u8'.' && name[2] == 0))
			{
				continue;
			}
			prdit.stack.emplace_back(
				::fast_io::posix_file(::fast_io::posix_fs_dirent{details::dirp_to_fd(prdit.stack.empty() ? prdit.dirp : prdit.stack.back().dirp), name},
									  ::fast_io::open_mode::directory));
		}
		if (prdit.entry) [[likely]]
		{
#if defined(_DIRENT_HAVE_D_NAMLEN)
			prdit.d_namlen = prdit.entry->d_namlen;
#else
			prdit.d_namlen = cstr_len(prdit.entry->d_name);
#endif
		}
		else
		{
			prdit.d_namlen = 0;
		}
		return prdit;
	}
}

template <typename StackType>
inline void pop(basic_dos_recursive_directory_iterator<StackType> &prdit) noexcept
{
	if (prdit.stack.empty())
	{
		prdit.entry = nullptr;
		prdit.d_namlen = 0;
	}
	else
	{
		prdit.stack.pop_back();
		++prdit;
	}
}

template <typename StackType>
inline basic_dos_recursive_directory_iterator<StackType> begin(basic_posix_recursive_directory_generator<StackType> const &pdg)
{
	::rewinddir(pdg.dir_fl.dirp.dirp);
	basic_dos_recursive_directory_iterator<StackType> pdit{pdg.dir_fl.dirp};
	++pdit;
	return pdit;
}

template <typename StackType>
inline ::std::default_sentinel_t end(basic_posix_recursive_directory_generator<StackType> const &) noexcept
{
	return {};
}

template <typename StackType>
inline dos_directory_entry operator*(basic_dos_recursive_directory_iterator<StackType> const &prdit) noexcept
{
	return {prdit.stack.empty() ? prdit.dirp : prdit.stack.back().dirp, prdit.entry, prdit.d_namlen};
}

template <typename StackType>
inline bool operator==(::std::default_sentinel_t, basic_dos_recursive_directory_iterator<StackType> const &b) noexcept
{
	return b.stack.empty() && b.entry == nullptr;
}

template <typename StackType>
inline bool operator==(basic_dos_recursive_directory_iterator<StackType> const &b, ::std::default_sentinel_t sntnl) noexcept
{
	return sntnl == b;
}

template <typename StackType>
inline bool operator!=(::std::default_sentinel_t sntnl, basic_dos_recursive_directory_iterator<StackType> const &b) noexcept
{
	return !(sntnl == b);
}

template <typename StackType>
inline bool operator!=(basic_dos_recursive_directory_iterator<StackType> const &b, ::std::default_sentinel_t sntnl) noexcept
{
	return sntnl != b;
}

inline posix_recursive_directory_generator recursive(posix_at_entry pate)
{
	return {.dir_fl = dos_directory_file(posix_file(details::sys_dup(pate.fd)))};
}

inline auto native_extension(dos_directory_entry ent) noexcept
{
	return ::fast_io::details::find_dot_and_sep<false, char, char>(ent.entry->d_name, ent.d_namlen);
}

inline auto native_stem(dos_directory_entry ent) noexcept
{
	return ::fast_io::details::find_dot_and_sep<true, char, char>(ent.entry->d_name, ent.d_namlen);
}

inline auto u8extension(dos_directory_entry ent) noexcept
{
	return ::fast_io::details::find_dot_and_sep<false, char8_t, char>(ent.entry->d_name, ent.d_namlen);
}

inline auto u8stem(dos_directory_entry ent) noexcept
{
	return ::fast_io::details::find_dot_and_sep<true, char8_t, char>(ent.entry->d_name, ent.d_namlen);
}

inline cross_code_cvt_t<char8_t> print_alias_define(io_alias_t, dos_directory_entry pth) noexcept
{
	using char8_const_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char8_t const *;
	return {
		{reinterpret_cast<char8_const_may_alias_ptr>(pth.entry->d_name), pth.d_namlen}};
}

using native_directory_entry = dos_directory_entry;

} // namespace fast_io
