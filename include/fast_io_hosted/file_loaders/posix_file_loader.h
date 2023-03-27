#pragma once
#if defined(__linux__) && defined(__NR_statx) && !defined(__statx_defined)
#include<linux/stat.h>
#endif

#if defined(__has_builtin)
#if !__has_builtin(__builtin_malloc) || !__has_builtin(__builtin_free)
#include<cstdlib>
#endif
#elif !defined(_MSC_VER)
#include<cstdlib>
#endif

namespace fast_io
{
#if (defined(_WIN32) && !defined(__BIONIC__)) || defined(__CYGWIN__)
namespace win32::details
{
inline std::size_t win32_load_file_get_file_size(void* handle)
{
	by_handle_file_information bhdi;
	if(!GetFileInformationByHandle(handle,__builtin_addressof(bhdi)))
		throw_win32_error();
	if constexpr(sizeof(std::size_t)<sizeof(std::uint_least64_t))
	{
		if(bhdi.nFileSizeHigh)
			throw_win32_error(0x000000DF);
		if constexpr(sizeof(std::size_t)<sizeof(std::uint_least32_t))
		{
			if(bhdi.nFileSizeLow>static_cast<std::uint_least32_t>(SIZE_MAX))
				throw_win32_error(0x000000DF);
		}
		return static_cast<std::size_t>(bhdi.nFileSizeLow);
	}
	else
	{
		return static_cast<std::size_t>((static_cast<std::uint_least64_t>(bhdi.nFileSizeHigh)<<32)|bhdi.nFileSizeLow);
	}
}
}
#endif

namespace details
{

inline std::size_t posix_loader_get_file_size(int fd)
{
#if defined(_WIN32) && !defined(__BIONIC__)&&!defined(__WINE__)
//windows 95 and windows 98 msvcrt do not provide struct __stat64. Directly invoke win32 api
	return ::fast_io::win32::details::win32_load_file_get_file_size(reinterpret_cast<void*>(noexcept_call(_get_osfhandle,fd)));
#elif defined(__linux__) && defined(__NR_statx)
//Linux kernel provides new __NR_statx syscall. That allows us to only extract part of information and avoid libc braindeath beneath.
	struct statx statxbuf;
	system_call_throw_error(system_call<__NR_statx,int>(fd,reinterpret_cast<char const*>(u8""),
#if defined(AT_EMPTY_PATH)
		AT_EMPTY_PATH
#else
		0x1000	//AT_EMPTY_PATH requires _GNU_SOURCE gets defined which might not be available for all libcs
#endif
		,STATX_SIZE,__builtin_addressof(statxbuf)));
	using stx_size_unsigned_type = std::make_unsigned_t<decltype(statxbuf.stx_size)>;
	if constexpr(sizeof(stx_size_unsigned_type)>sizeof(std::size_t))
	{
		if(static_cast<stx_size_unsigned_type>(statxbuf.stx_size)>static_cast<stx_size_unsigned_type>(SIZE_MAX))
			throw_posix_error(EINVAL);
	}
	return static_cast<std::size_t>(statxbuf.stx_size);
#else
#if defined(__linux__) && !defined(__MLIBC_O_CLOEXEC)
	struct stat64 st;
#else
	struct stat st;
#endif
	if(
#if defined(__linux__) && !defined(__MLIBC_O_CLOEXEC)
fstat64
#else
fstat
#endif
	(fd,__builtin_addressof(st))<0)
		throw_posix_error();
	using st_size_unsigned_type = std::make_unsigned_t<decltype(st.st_size)>;
	if constexpr(sizeof(st_size_unsigned_type)>sizeof(std::size_t))
	{
		if(static_cast<st_size_unsigned_type>(st.st_size)>static_cast<st_size_unsigned_type>(SIZE_MAX))
			throw_posix_error(EINVAL);
	}
	return static_cast<std::size_t>(st.st_size);
#endif
}


struct load_file_allocation_guard
{
	void* address{};
	explicit load_file_allocation_guard(std::size_t file_size):address(
#if defined(__has_builtin)
#if __has_builtin(__builtin_malloc)
__builtin_malloc
#else
malloc
#endif
#else
malloc
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
		free(address);
#endif
#else
		free(address);
#endif
	}
};

template<bool allocation>
inline char* posix_load_address(int fd,std::size_t file_size)
{
	if constexpr(allocation)
	{
		if(file_size==0)
			return nullptr;
		load_file_allocation_guard guard{file_size};
		posix_io_observer piob{fd};
		auto addr{reinterpret_cast<char*>(guard.address)};
		auto addr_ed{addr+file_size};
		for(auto i{addr};i!=addr_ed;)
		{
			auto after{read(piob,i,addr_ed)};
			if(after==i)
				throw_posix_error();
			i=after;
		}
		guard.address=nullptr;
		return addr;
	}
	else
	{
#if defined(_WIN32) || (defined(__NEWLIB__)&&!defined(__CYGWIN__)) || defined(__MSDOS__) || defined(_PICOLIBC__) || (defined(__wasm__)&&!defined(_WASI_EMULATED_MMAN))
	static_assert(allocation);
#else
	if(file_size==0)
		return (char*)-1;
	return reinterpret_cast<char*>(
sys_mmap(nullptr,file_size,PROT_READ|PROT_WRITE,MAP_PRIVATE
#if defined(MAP_POPULATE)
|MAP_POPULATE
#endif
,fd,0));
#endif
	}
}

inline char* posix_load_address_extra(int fd,std::size_t file_size,std::size_t exsz)
{
	::std::size_t mxsz{SIZE_MAX-exsz};
	if(file_size>mxsz)
	{
		throw_posix_error(EINVAL);
	}
	load_file_allocation_guard guard{file_size+exsz};
	posix_io_observer piob{fd};
	auto addr{reinterpret_cast<char*>(guard.address)};
	auto addr_ed{addr+file_size};
	for(auto i{addr};i!=addr_ed;)
	{
		auto after{read(piob,i,addr_ed)};
		if(after==i)
			throw_posix_error();
		i=after;
	}
#if defined(__has_builtin)
#if __has_builtin(__builtin_memset)
__builtin_memset
#else
memset
#endif
#else
memset
#endif
	(addr_ed,0,exsz);
	guard.address=nullptr;
	return addr;
}


template<bool allocation>
inline void posix_unload_address(void* address,[[maybe_unused]] std::size_t file_size) noexcept
{
	if constexpr(allocation)
	{
#if defined(__has_builtin)
#if __has_builtin(__builtin_free)
		__builtin_free(address);
#else
		free(address);
#endif
#else
		free(address);
#endif
	}
	else
	{
#if defined(_WIN32) || (defined(__NEWLIB__)&&!defined(__CYGWIN__)) || defined(_PICOLIBC__) || defined(__MSDOS__) || defined(__wasm__)
		static_assert(allocation);
#else
		if(address!=(void*)-1)[[likely]]
			sys_munmap(address,file_size);
#endif
	}
}

struct posix_file_loader_return_value_t
{
	char* address_begin;
	char* address_end;
};

template<bool allocation>
inline posix_file_loader_return_value_t posix_load_address_impl(int fd)
{
	std::size_t size{posix_loader_get_file_size(fd)};
	auto add{posix_load_address<allocation>(fd,size)};
	return {add,add+size};
}

template<bool allocation>
inline auto posix_load_file_impl(native_fs_dirent fsdirent,open_mode om,perms pm)
{
	posix_file pf(fsdirent,om,pm);
	return posix_load_address_impl<allocation>(pf.fd);
}

template<bool allocation,::fast_io::constructible_to_os_c_str T>
inline auto posix_load_file_impl(T const& str,open_mode om,perms pm)
{
	posix_file pf(str,om,pm);
	return posix_load_address_impl<allocation>(pf.fd);
}

template<bool allocation,::fast_io::constructible_to_os_c_str T>
inline auto posix_load_file_impl(native_at_entry ent,T const& str,open_mode om,perms pm)
{
	posix_file pf(ent,str,om,pm);
	return posix_load_address_impl<allocation>(pf.fd);
}


inline posix_file_loader_return_value_t posix_load_address_allocation_extra_impl(::std::size_t exsz,int fd)
{
	std::size_t size{posix_loader_get_file_size(fd)};
	auto add{posix_load_address_extra(fd,size,exsz)};
	return {add,add+size};
}

inline auto posix_load_file_allocation_extra_impl(::std::size_t exsz,native_fs_dirent fsdirent,open_mode om,perms pm)
{
	posix_file pf(fsdirent,om,pm);
	return posix_load_address_allocation_extra_impl(exsz,pf.fd);
}

template<::fast_io::constructible_to_os_c_str T>
inline auto posix_load_file_allocation_extra_impl(::std::size_t exsz,T const& str,open_mode om,perms pm)
{
	posix_file pf(str,om,pm);
	return posix_load_address_allocation_extra_impl(exsz,pf.fd);
}

template<::fast_io::constructible_to_os_c_str T>
inline auto posix_load_file_allocation_extra_impl(::std::size_t exsz,native_at_entry ent,T const& str,open_mode om,perms pm)
{
	posix_file pf(ent,str,om,pm);
	return posix_load_address_allocation_extra_impl(exsz,pf.fd);
}



template<bool allocation=false>
class posix_file_loader_impl
{
public:
	using value_type = char;
	using pointer = char*;
	using const_pointer = char const*;
	using const_iterator = const_pointer;
	using iterator = pointer;
	using reference = char&;
	using const_reference = char const&;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	using reverse_iterator = std::reverse_iterator<iterator>;

	pointer address_begin{};
	pointer address_end{};
	inline constexpr posix_file_loader_impl() noexcept requires(allocation)=default;
	inline constexpr posix_file_loader_impl() noexcept : address_begin((char*)-1),address_end((char*)-1){}
	inline explicit posix_file_loader_impl(posix_at_entry pate)
	{
		auto ret{posix_load_address_impl<allocation>(pate.fd)};
		address_begin=ret.address_begin;
		address_end=ret.address_end;
	}
	inline explicit posix_file_loader_impl(native_fs_dirent fsdirent,open_mode om = open_mode::in, perms pm=static_cast<perms>(436))
	{
		auto ret{posix_load_file_impl<allocation>(fsdirent,om,pm)};
		address_begin=ret.address_begin;
		address_end=ret.address_end;
	}
	template<::fast_io::constructible_to_os_c_str T>
	inline explicit posix_file_loader_impl(T const& filename,open_mode om = open_mode::in,perms pm=static_cast<perms>(436))
	{
		auto ret{posix_load_file_impl<allocation>(filename,om,pm)};
		address_begin=ret.address_begin;
		address_end=ret.address_end;
	}
	template<::fast_io::constructible_to_os_c_str T>
	inline explicit posix_file_loader_impl(native_at_entry ent,T const& filename,open_mode om = open_mode::in,perms pm=static_cast<perms>(436))
	{
		auto ret{posix_load_file_impl<allocation>(ent,filename,om,pm)};
		address_begin=ret.address_begin;
		address_end=ret.address_end;
	}
	posix_file_loader_impl(posix_file_loader_impl const&)=delete;
	posix_file_loader_impl& operator=(posix_file_loader_impl const&)=delete;
	constexpr posix_file_loader_impl(posix_file_loader_impl&& __restrict other) noexcept:address_begin(other.address_begin),address_end(other.address_end)
	{
		if constexpr(allocation)
			other.address_end=other.address_begin=nullptr;
		else
			other.address_end=other.address_begin=(char*)-1;
	}
	posix_file_loader_impl& operator=(posix_file_loader_impl && __restrict other) noexcept
	{
		posix_unload_address<allocation>(address_begin,static_cast<std::size_t>(address_end-address_begin));
		address_begin=other.address_begin;
		address_end=other.address_end;
		if constexpr(allocation)
			other.address_end=other.address_begin=nullptr;
		else
			other.address_end=other.address_begin=(char*)-1;
		return *this;
	}


	inline explicit posix_file_loader_impl(file_loader_extra_bytes exb,posix_at_entry pate) requires(allocation)
	{
		auto ret{posix_load_address_allocation_extra_impl(exb.n,pate.fd)};
		address_begin=ret.address_begin;
		address_end=ret.address_end;
	}
	inline explicit posix_file_loader_impl(file_loader_extra_bytes exb,native_fs_dirent fsdirent,open_mode om = open_mode::in, perms pm=static_cast<perms>(436)) requires(allocation)
	{
		auto ret{posix_load_file_allocation_extra_impl(exb.n,fsdirent,om,pm)};
		address_begin=ret.address_begin;
		address_end=ret.address_end;
	}
	template<::fast_io::constructible_to_os_c_str T>
	inline explicit posix_file_loader_impl(file_loader_extra_bytes exb,T const& filename,open_mode om = open_mode::in,perms pm=static_cast<perms>(436)) requires(allocation)
	{
		auto ret{posix_load_file_allocation_extra_impl(exb.n,filename,om,pm)};
		address_begin=ret.address_begin;
		address_end=ret.address_end;
	}
	template<::fast_io::constructible_to_os_c_str T>
	inline explicit posix_file_loader_impl(file_loader_extra_bytes exb,native_at_entry ent,T const& filename,open_mode om = open_mode::in,perms pm=static_cast<perms>(436)) requires(allocation)
	{
		auto ret{posix_load_file_allocation_extra_impl(exb.n,ent,filename,om,pm)};
		address_begin=ret.address_begin;
		address_end=ret.address_end;
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
		posix_unload_address<allocation>(address_begin,static_cast<std::size_t>(address_end-address_begin));
		if constexpr(allocation)
			address_end=address_begin=nullptr;
		else
			address_end=address_begin=(char*)-1;
	}
#if __has_cpp_attribute(nodiscard)
	[[nodiscard]]
#endif
	inline constexpr pointer release() noexcept
	{
		pointer temp{address_begin};
		if constexpr(allocation)
			address_end=address_begin=nullptr;
		else
			address_end=address_begin=(char*)-1;
		return temp;
	}
	~posix_file_loader_impl()
	{
		posix_unload_address<allocation>(address_begin,static_cast<std::size_t>(address_end-address_begin));
	}
};

template<bool h>
inline constexpr basic_io_scatter_t<char> print_alias_define(io_alias_t,posix_file_loader_impl<h> const& load) noexcept
{
	return {load.data(),load.size()};
}

}

#if (!defined(_WIN32) || defined(__WINE__)) && (!defined(__NEWLIB__)||defined(__CYGWIN__)) && !defined(__MSDOS__) && !defined(__wasm__) && !defined(_PICOLIBC__)
using posix_file_loader = details::posix_file_loader_impl<false>;
#endif

using allocation_file_loader = details::posix_file_loader_impl<true>;

}
