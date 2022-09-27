#pragma once
/*
Unfinished due to some issues
*/
namespace fast_io
{

namespace details
{

template<typename Allocator>
inline constexpr posix_file_loader_return_value_t allocation_load_file_common_impl(posix_at_entry pate)
{
	auto fsize{posix_loader_get_file_size(pate.fd)};
	buffer_alloc_arr_ptr<char,Allocator> ptr;
	return {};
}

#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)

template<typename Allocator>
inline constexpr posix_file_loader_return_value_t allocation_load_file_common_impl( pate)
{
	auto filesize{posix_loader_get_file_size(pate.fd)};
	
	
	posix_io_observer piob(pate);
	
	read_all(piob,);
}

template<typename Filetype,typename Allocator>
inline constexpr posix_file_loader_return_value_t allocation_load_file_impl(native_fs_dirent fsdirent,open_mode om,perms pm)
{

}

#endif

}

template<typename Filetype,typename Allocator>
class basic_allocation_file_loader
{
public:
	using file_type = file_type;
	using allocator_type = Allocator;
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
	inline constexpr basic_allocation_file_loader() noexcept=default;

	inline explicit basic_allocation_file_loader(native_fs_dirent fsdirent,open_mode om = open_mode::in, perms pm=static_cast<perms>(436))
	{
		auto ret{posix_load_file_impl<allocation>(fsdirent,om,pm)};
		address_begin=ret.address_begin;
		address_end=ret.address_end;
	}
	template<::fast_io::constructible_to_os_c_str T>
	inline explicit basic_allocation_file_loader(T const& filename,open_mode om = open_mode::in,perms pm=static_cast<perms>(436))
	{
		auto ret{posix_load_file_impl<allocation>(filename,om,pm)};
		address_begin=ret.address_begin;
		address_end=ret.address_end;
	}
	template<::fast_io::constructible_to_os_c_str T>
	inline explicit basic_allocation_file_loader(posix_at_entry ent,T const& filename,open_mode om = open_mode::in,perms pm=static_cast<perms>(436))
	{
		auto ret{posix_load_file_impl<allocation>(ent,filename,om,pm)};
		address_begin=ret.address_begin;
		address_end=ret.address_end;
	}
	basic_allocation_file_loader(basic_allocation_file_loader const&)=delete;
	basic_allocation_file_loader& operator=(basic_allocation_file_loader const&)=delete;
	constexpr basic_allocation_file_loader(posix_file_loader_impl&& __restrict other) noexcept:address_begin(other.address_begin),address_end(other.address_end)
	{
		other.address_end=other.address_begin=nullptr;
	}
	basic_allocation_file_loader& operator=(basic_allocation_file_loader && __restrict other) noexcept
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
}

}
