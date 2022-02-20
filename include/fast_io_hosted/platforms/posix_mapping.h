#pragma once
#include <sys/mman.h>
#include <sys/stat.h>

namespace fast_io
{

namespace details
{

inline std::byte* sys_mmap(void *addr, size_t len, int prot, int flags, int fd, std::uintmax_t offset)
{
#if defined(__linux__) && defined(__NR_mmap) && !defined(__NR_mmap2)
	if constexpr(sizeof(std::uintmax_t)>sizeof(off_t))
	{
		if(offset>static_cast<std::uintmax_t>(std::numeric_limits<off_t>::max()))
			throw_posix_error(EINVAL);
	}
	std::intptr_t ret{system_call<__NR_mmap,std::intptr_t>(addr,len,prot,flags,fd,offset)};
	system_call_throw_error(ret);
	return reinterpret_cast<std::byte*>(ret);
#elif defined(HAVE_MMAP64)
	if constexpr(sizeof(std::uintmax_t)>sizeof(off64_t))
	{
		if(offset>static_cast<std::uintmax_t>(std::numeric_limits<off64_t>::max()))
			throw_posix_error(EINVAL);
	}
	auto ret{reinterpret_cast<std::byte*>(mmap64(addr,len,prot,flags,fd,offset))};
	if(ret==MAP_FAILED)
		throw_posix_error();
	return ret;
#else
	if constexpr(sizeof(std::uintmax_t)>sizeof(off_t))
	{
		if(offset>static_cast<std::uintmax_t>(std::numeric_limits<off_t>::max()))
			throw_posix_error(EINVAL);
	}
	auto ret{reinterpret_cast<std::byte*>(mmap(addr,len,prot,flags,fd,static_cast<off_t>(static_cast<my_make_unsigned_t<off_t>>(offset))))};
	if(ret==MAP_FAILED)
		throw_posix_error();
	return ret;
#endif
} 

inline int sys_munmap(void *addr, size_t len)
{
	return
#if defined(__linux__) && defined(__NR_munmap)
	system_call<__NR_munmap,int>(addr,len);
#else
	munmap(addr,len);
#endif
}

inline void sys_munmap_throw_error(void *addr, std::size_t len)
{
	system_call_throw_error(sys_munmap(addr,len));
}
}

enum class posix_file_map_attribute
{
none=PROT_NONE,write=PROT_WRITE,execute=PROT_EXEC,read=PROT_READ
};

constexpr posix_file_map_attribute operator&(posix_file_map_attribute x, posix_file_map_attribute y) noexcept
{
using utype = typename std::underlying_type<posix_file_map_attribute>::type;
return static_cast<posix_file_map_attribute>(static_cast<utype>(x) & static_cast<utype>(y));
}

constexpr posix_file_map_attribute operator|(posix_file_map_attribute x, posix_file_map_attribute y) noexcept
{
using utype = typename std::underlying_type<posix_file_map_attribute>::type;
return static_cast<posix_file_map_attribute>(static_cast<utype>(x) | static_cast<utype>(y));
}

constexpr posix_file_map_attribute operator^(posix_file_map_attribute x, posix_file_map_attribute y) noexcept
{
using utype = typename std::underlying_type<posix_file_map_attribute>::type;
return static_cast<posix_file_map_attribute>(static_cast<utype>(x) ^ static_cast<utype>(y));
}

constexpr posix_file_map_attribute operator~(posix_file_map_attribute x) noexcept
{
using utype = typename std::underlying_type<posix_file_map_attribute>::type;
return static_cast<posix_file_map_attribute>(~static_cast<utype>(x));
}

inline posix_file_map_attribute& operator&=(posix_file_map_attribute& x, posix_file_map_attribute y) noexcept{return x=x&y;}

inline posix_file_map_attribute& operator|=(posix_file_map_attribute& x, posix_file_map_attribute y) noexcept{return x=x|y;}

inline posix_file_map_attribute& operator^=(posix_file_map_attribute& x, posix_file_map_attribute y) noexcept{return x=x^y;}

inline constexpr posix_file_map_attribute to_posix_file_map_attribute(file_map_attribute x)
{
	switch(x)
	{
	case file_map_attribute::execute_read:return posix_file_map_attribute::execute|posix_file_map_attribute::read;
	case file_map_attribute::execute_read_write:return posix_file_map_attribute::execute|posix_file_map_attribute::read;
	case file_map_attribute::execute_write_copy:return posix_file_map_attribute::execute|posix_file_map_attribute::write;
	case file_map_attribute::read_only:return posix_file_map_attribute::read;
	case file_map_attribute::read_write:return posix_file_map_attribute::read|posix_file_map_attribute::write;
	case file_map_attribute::write_copy:return posix_file_map_attribute::write;
	default:
		throw_posix_error(EINVAL);
	};
}

class posix_memory_map_file
{
public:
	using value_type = std::byte;
	using pointer = value_type*;
	using const_pointer = value_type const*;
	using const_iterator = const_pointer;
	using iterator = pointer;
	using reference = value_type&;
	using const_reference = value_type const&;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	pointer address_begin{},address_end{};
	constexpr posix_memory_map_file()=default;
	constexpr posix_memory_map_file(std::byte* addbg,std::byte* added):address_begin{addbg},address_end{added}
	{}
	posix_memory_map_file(posix_at_entry bf,file_map_attribute attr,std::size_t bytes,std::uintmax_t start_address=0)
		:address_begin{details::sys_mmap(nullptr,bytes,static_cast<int>(to_posix_file_map_attribute(attr)),MAP_SHARED,bf.fd,start_address)},
		address_end{address_begin+bytes}
	{}
	posix_memory_map_file(posix_memory_map_file const&)=delete;
	posix_memory_map_file& operator=(posix_memory_map_file const&)=delete;
	posix_memory_map_file(posix_memory_map_file&& __restrict other) noexcept:address_begin{other.address_begin},address_end{other.address_end}
	{
		other.address_end=other.address_begin=reinterpret_cast<std::byte*>(MAP_FAILED);
	}
	posix_memory_map_file& operator=(posix_memory_map_file&& __restrict other) noexcept
	{
		if(this->address_begin!=reinterpret_cast<std::byte*>(MAP_FAILED))[[likely]]
			details::sys_munmap(this->address_begin,static_cast<std::size_t>(address_end-address_begin));
		this->address_begin=other.address_begin;
		this->address_end=other.address_end;
		other.address_begin=reinterpret_cast<std::byte*>(MAP_FAILED);
		other.address_end=reinterpret_cast<std::byte*>(MAP_FAILED);
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
	constexpr std::size_t size() const noexcept
	{
		return static_cast<std::size_t>(address_end-address_begin);
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
	constexpr std::size_t max_size() const noexcept
	{
		return SIZE_MAX;
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
	constexpr reference operator[](size_type pos) noexcept
	{
		return address_begin[pos];
	}
	constexpr const_reference operator[](size_type pos) const noexcept
	{
		return address_begin[pos];
	}
	void close() noexcept
	{
		if(this->address_begin!=MAP_FAILED)[[likely]]
		{
			auto ret{details::sys_munmap(this->address_begin,static_cast<std::size_t>(address_end-address_begin))};
			this->address_end=this->address_begin=reinterpret_cast<std::byte*>(MAP_FAILED);
			system_call_throw_error(ret);
		}
	}
	~posix_memory_map_file()
	{
		if(this->address_begin!=MAP_FAILED)[[likely]]
			details::sys_munmap(this->address_begin,static_cast<std::size_t>(address_end-address_begin));
	}
};

using native_memory_map_file = posix_memory_map_file;

}
