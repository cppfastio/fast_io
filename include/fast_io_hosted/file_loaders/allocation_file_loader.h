#pragma once

namespace fast_io
{

namespace details
{

struct allocation_file_loader_closer_impl
{
	int fd{-1};
	char* address_begin{};
	explicit constexpr allocation_file_loader_closer_impl(int fdd,char* addressbegin):fd{fdd},address_begin{addressbegin}
	{
	}
	allocation_file_loader_closer_impl(allocation_file_loader_closer_impl const&)=delete;
	allocation_file_loader_closer_impl& operator=(allocation_file_loader_closer_impl const&)=delete;
	~allocation_file_loader_closer_impl()
	{
		if(fd!=-1)
		{
			::fast_io::noexcept_call(::close,fd);
		}
#if defined(__has_builtin)
#if __has_builtin(__builtin_free)
		__builtin_free(address_begin);
#else
		::std::free(address_begin);
#endif
#else
		::std::free(address_begin);
#endif
	}
};

inline void close_allocation_file_loader_impl(int fd, char* address_begin, char* address_end) noexcept
{
	allocation_file_loader_closer_impl closer(fd,address_begin);
	if(fd==-1)
	{
		return;
	}
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__)
	auto seekret = ::fast_io::noexcept_call(_lseeki64,fd,0,0);
	constexpr bool needshrinktoint32{true};
#else
#if defined(_LARGEFILE64_SOURCE)
	auto seekret = ::fast_io::noexcept_call(::lseek64,fd,0,0);
#else
	auto seekret = ::fast_io::noexcept_call(::lseek,fd,0,0);
#endif
	constexpr bool needshrinktoint32{};
#endif
	if(seekret == -1)
	{
		return;
	}
	using towritetype = ::std::conditional_t<needshrinktoint32,unsigned,::std::size_t>;
	while(address_begin!=address_end)
	{
		::std::size_t towrite{static_cast<::std::size_t>(address_end-address_begin)};
		if constexpr(sizeof(towritetype)<sizeof(::std::size_t))
		{
			::std::size_t towritemx{::std::numeric_limits<towritetype>::max()};
			if(towritemx<towrite)
			{
				towrite=towritemx;
			}
		}
		auto ret{::fast_io::noexcept_call(
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__)
			::_write
#else
			::write
#endif
			,fd,address_begin,static_cast<towritetype>(towrite))};
		if(ret==-1)
		{
#ifdef EAGAIN
			if(errno!=EAGAIN)
			{
				break;
			}
#else
			break;
#endif
		}
		address_begin += ret;
	}
}

struct passparm
{
	int fd;
	bool writeback;
};

struct allocation_file_loader_ret
{
	char* address_begin;
	char* address_end;
	char* address_capacity;
	int fd;
};


struct load_file_allocation_guard
{
	void* address{};
	explicit constexpr load_file_allocation_guard() noexcept = default;
	explicit load_file_allocation_guard(::std::size_t file_size):address(
#if defined(__has_builtin)
#if __has_builtin(__builtin_malloc)
__builtin_malloc
#else
::std::malloc
#endif
#else
::std::malloc
#endif
(file_size))
	{
		if(address==nullptr)
			throw_posix_error(EINVAL);
	}
	load_file_allocation_guard(load_file_allocation_guard const&)=delete;
	load_file_allocation_guard& operator=(load_file_allocation_guard const&)=delete;
	~load_file_allocation_guard()
	{
#if defined(__has_builtin)
#if __has_builtin(__builtin_free)
		__builtin_free(address);
#else
		::std::free(address);
#endif
#else
		::std::free(address);
#endif
	}
};

inline allocation_file_loader_ret allocation_load_address_impl(int fd)
{
	::std::size_t filesize{::fast_io::details::posix_loader_get_file_size(fd)};
	load_file_allocation_guard guard{filesize};
	posix_io_observer piob{fd};
	auto addr{reinterpret_cast<char*>(guard.address)};
	auto addr_ed{addr+filesize};
	::fast_io::operations::decay::read_all_bytes_decay(piob,
		reinterpret_cast<::std::byte*>(addr),
		reinterpret_cast<::std::byte*>(addr_ed));
	guard.address=nullptr;
	return {addr,addr_ed,addr_ed,-1};
}

template<typename... Args>
inline allocation_file_loader_ret allocation_load_file_impl(bool writeback,Args &&...args)
{
	::fast_io::posix_file pf(::fast_io::freestanding::forward<Args>(args)...);
	auto ret{allocation_load_address_impl(pf.fd)};
	if(writeback)
	{
		ret.fd=pf.release();
	}
	return ret;
}

inline allocation_file_loader_ret allocation_load_file_fd_impl(bool writeback,int fd)
{
	::fast_io::posix_file pf(::fast_io::io_dup, ::fast_io::posix_io_observer{fd});
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__)
	auto seekret = ::fast_io::noexcept_call(_lseeki64,pf.fd,0,0);
#else
#if defined(_LARGEFILE64_SOURCE)
	auto seekret = ::fast_io::noexcept_call(::lseek64,pf.fd,0,0);
#else
	auto seekret = ::fast_io::noexcept_call(::lseek,pf.fd,0,0);
#endif
#endif
	if(seekret == -1)
	{
		throw_posix_error();
	}
	auto ret{allocation_load_address_impl(pf.fd)};
	if(writeback)
	{
		ret.fd=pf.release();
	}
	return ret;
}

}

class allocation_file_loader
{
public:
	using file_type = posix_file;
	using value_type = char;
	using pointer = char*;
	using const_pointer = char const*;
	using const_iterator = const_pointer;
	using iterator = pointer;
	using reference = char&;
	using const_reference = char const&;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;
	using reverse_iterator = ::std::reverse_iterator<iterator>;

	pointer address_begin{};
	pointer address_end{};
	pointer address_capacity{};
	int fd{-1};
	explicit constexpr allocation_file_loader() noexcept=default;

	inline explicit allocation_file_loader(posix_at_entry pate)
	{
		auto ret{::fast_io::details::allocation_load_file_fd_impl(false,pate.fd)};
		address_begin=ret.address_begin;
		address_end=ret.address_end;
		address_capacity=ret.address_capacity;
	}
	inline explicit allocation_file_loader(native_fs_dirent fsdirent,open_mode om = open_mode::in, perms pm=static_cast<perms>(436))
	{
		auto ret{::fast_io::details::allocation_load_file_impl(false,fsdirent,om,pm)};
		address_begin=ret.address_begin;
		address_end=ret.address_end;
		address_capacity=ret.address_capacity;
	}
	template<::fast_io::constructible_to_os_c_str T>
	inline explicit allocation_file_loader(T const& filename,open_mode om = open_mode::in,perms pm=static_cast<perms>(436))
	{
		auto ret{::fast_io::details::allocation_load_file_impl(false,filename,om,pm)};
		address_begin=ret.address_begin;
		address_end=ret.address_end;
		address_capacity=ret.address_capacity;
	}
	template<::fast_io::constructible_to_os_c_str T>
	inline explicit allocation_file_loader(native_at_entry ent,T const& filename,open_mode om = open_mode::in,perms pm=static_cast<perms>(436))
	{
		auto ret{::fast_io::details::allocation_load_file_impl(false,ent,filename,om,pm)};
		address_begin=ret.address_begin;
		address_end=ret.address_end;
		address_capacity=ret.address_capacity;
	}
	inline explicit allocation_file_loader(allocation_mmap_options options, posix_at_entry pate)
	{
		auto ret{::fast_io::details::allocation_load_file_fd_impl(options.write_back,pate.fd)};
		address_begin=ret.address_begin;
		address_end=ret.address_end;
		address_capacity=ret.address_capacity;
		fd=ret.fd;
	}
	inline explicit allocation_file_loader(allocation_mmap_options options,native_fs_dirent fsdirent,open_mode om = open_mode::in, perms pm=static_cast<perms>(436))
	{
		auto ret{::fast_io::details::allocation_load_file_impl(options.write_back,fsdirent,om,pm)};
		address_begin=ret.address_begin;
		address_end=ret.address_end;
		address_capacity=ret.address_capacity;
		fd=ret.fd;
	}
	template<::fast_io::constructible_to_os_c_str T>
	inline explicit allocation_file_loader(allocation_mmap_options options,T const& filename,open_mode om = open_mode::in,perms pm=static_cast<perms>(436))
	{
		auto ret{::fast_io::details::allocation_load_file_impl(options.write_back,filename,om,pm)};
		address_begin=ret.address_begin;
		address_end=ret.address_end;
		address_capacity=ret.address_capacity;
		fd=ret.fd;
	}
	template<::fast_io::constructible_to_os_c_str T>
	inline explicit allocation_file_loader(allocation_mmap_options options,native_at_entry ent,T const& filename,open_mode om = open_mode::in,perms pm=static_cast<perms>(436))
	{
		auto ret{::fast_io::details::allocation_load_file_impl(options.write_back,ent,filename,om,pm)};
		address_begin=ret.address_begin;
		address_end=ret.address_end;
		address_capacity=ret.address_capacity;
		fd=ret.fd;
	}

	allocation_file_loader(allocation_file_loader const&)=delete;
	allocation_file_loader& operator=(allocation_file_loader const&)=delete;
	constexpr allocation_file_loader(allocation_file_loader&& __restrict other) noexcept:
		address_begin(other.address_begin),
		address_end(other.address_end),
		address_capacity(other.address_capacity),
		fd(other.fd)
	{
		other.address_capacity=other.address_end=other.address_begin=nullptr;
		other.fd=-1;
	}
	allocation_file_loader& operator=(allocation_file_loader && __restrict other) noexcept
	{
		address_begin=other.address_begin;
		address_end=other.address_end;
		address_capacity=other.address_capacity;
		other.address_capacity=other.address_end=other.address_begin=nullptr;
		other.fd=-1;
		return *this;
	}
	constexpr pointer data() const noexcept
	{
		return address_begin;
	}
	constexpr bool empty() const noexcept
	{
		return address_begin==address_end;
	}
	constexpr ::std::size_t size() const noexcept
	{
		return static_cast<::std::size_t>(address_end-address_begin);
	}
	constexpr const_iterator cbegin() const noexcept
	{
		return address_begin;
	}
	constexpr const_iterator begin() const noexcept
	{
		return address_begin;
	}
	constexpr iterator begin() noexcept
	{
		return address_begin;
	}
	constexpr const_iterator cend() const noexcept
	{
		return address_end;
	}
	constexpr const_iterator end() const noexcept
	{
		return address_end;
	}
	constexpr iterator end() noexcept
	{
		return address_end;
	}
	constexpr ::std::size_t max_size() const noexcept
	{
		return SIZE_MAX;
	}
	constexpr ::std::size_t capacity() const noexcept
	{
		return static_cast<::std::size_t>(this->address_capacity-this->address_begin);
	}
	constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator{address_end};
	}
	constexpr reverse_iterator rbegin() noexcept
	{
		return reverse_iterator{address_end};
	}
	constexpr const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator{address_end};
	}
	constexpr const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator{address_begin};
	}
	constexpr reverse_iterator rend() noexcept
	{
		return reverse_iterator{address_begin};
	}
	constexpr const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator{address_begin};
	}
	constexpr const_reference front() const noexcept
	{
		return *address_begin;
	}
	constexpr reference front() noexcept
	{
		return *address_begin;
	}
	constexpr const_reference back() const noexcept
	{
		return address_end[-1];
	}
	constexpr reference back() noexcept
	{
		return address_end[-1];
	}
	inline constexpr reference operator[](size_type size) noexcept
	{
		return address_begin[size];
	}
	inline constexpr const_reference operator[](size_type size) const noexcept
	{
		return address_begin[size];
	}
	inline void close()
	{
		::fast_io::details::close_allocation_file_loader_impl(fd,address_begin,address_end);
		address_capacity=address_end=address_begin=nullptr;
		fd=-1;
	}
#if __has_cpp_attribute(nodiscard)
	[[nodiscard]]
#endif
	inline constexpr pointer release() noexcept
	{
		pointer temp{address_begin};
		address_capacity=address_end=address_begin=nullptr;
		if(fd!=-1)
		{
			::fast_io::noexcept_call(::close,fd);
		}
		fd=-1;
		return temp;
	}
	~allocation_file_loader()
	{
		::fast_io::details::close_allocation_file_loader_impl(fd,address_begin,address_end);
	}
};

inline constexpr basic_io_scatter_t<char> print_alias_define(io_alias_t,allocation_file_loader const& load) noexcept
{
	return {load.data(),load.size()};
}

}
