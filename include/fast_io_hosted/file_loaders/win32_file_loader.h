#pragma once

namespace fast_io
{

namespace win32::details
{

struct win32_file_loader_return_value_t
{
	char* address_begin;
	char* address_end;
};

inline win32_file_loader_return_value_t win32_load_address_common_impl(void* hfilemappingobj,std::size_t file_size)
{
	if(hfilemappingobj==nullptr)
		throw_win32_error();
	win32_file map_hd{hfilemappingobj};
	auto base_ptr{MapViewOfFile(hfilemappingobj,1,0,0,file_size)};
	if(base_ptr==nullptr)
		throw_win32_error();
	return {reinterpret_cast<char*>(base_ptr),reinterpret_cast<char*>(base_ptr)+file_size};
}

template<win32_family family>
inline win32_file_loader_return_value_t win32_load_address_impl(void* handle)
{
	std::size_t file_size{win32_load_file_get_file_size(handle)};
	if(file_size==0)
		return {nullptr,nullptr};
	if constexpr(family==win32_family::wide_nt)
		return win32_load_address_common_impl(CreateFileMappingW(handle,nullptr,0x08,0,0,nullptr),file_size);
	else
		return win32_load_address_common_impl(CreateFileMappingA(handle,nullptr,0x08,0,0,nullptr),file_size);
}

template<win32_family family>
inline auto win32_load_file_impl(nt_fs_dirent fsdirent,open_mode om,perms pm)
{
	basic_win32_family_file<family,char> wf(fsdirent,om,pm);
	return win32_load_address_impl<family>(wf.handle);
}

template<win32_family family,::fast_io::constructible_to_os_c_str T>
inline auto win32_load_file_impl(T const& str,open_mode om,perms pm)
{
	basic_win32_family_file<family,char> wf(str,om,pm);
	return win32_load_address_impl<family>(wf.handle);
}

template<win32_family family,::fast_io::constructible_to_os_c_str T>
inline auto win32_load_file_impl(nt_at_entry ent,T const& str,open_mode om,perms pm)
{
	basic_win32_family_file<family,char> wf(ent,str,om,pm);
	return win32_load_address_impl<family>(wf.handle);
}

inline void win32_unload_address(void const* address) noexcept
{
	if(address)
		UnmapViewOfFile(address);
}

}

template<win32_family family>
class win32_family_file_loader
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
	inline constexpr win32_family_file_loader() noexcept=default;
	inline explicit win32_family_file_loader(nt_at_entry ent)
	{
		auto ret{win32::details::win32_load_address_impl<family>(ent.handle)};
		address_begin=ret.address_begin;
		address_end=ret.address_end;
	}
	inline explicit win32_family_file_loader(nt_fs_dirent fsdirent,open_mode om = open_mode::in,perms pm=static_cast<perms>(436))
	{
		auto ret{win32::details::win32_load_file_impl<family>(fsdirent,om,pm)};
		address_begin=ret.address_begin;
		address_end=ret.address_end;
	}
	template<::fast_io::constructible_to_os_c_str T>
	inline explicit win32_family_file_loader(T const& filename,open_mode om = open_mode::in,perms pm=static_cast<perms>(436))
	{
		auto ret{win32::details::win32_load_file_impl<family>(filename,om,pm)};
		address_begin=ret.address_begin;
		address_end=ret.address_end;
	}
	template<::fast_io::constructible_to_os_c_str T>
	inline explicit win32_family_file_loader(nt_at_entry ent,T const& filename,open_mode om = open_mode::in,perms pm=static_cast<perms>(436))
	{
		auto ret{win32::details::win32_load_file_impl<family>(ent,filename,om,pm)};
		address_begin=ret.address_begin;
		address_end=ret.address_end;
	}
	win32_family_file_loader(win32_family_file_loader const&)=delete;
	win32_family_file_loader& operator=(win32_family_file_loader const&)=delete;
	constexpr win32_family_file_loader(win32_family_file_loader&& __restrict other) noexcept:address_begin(other.address_begin),address_end(other.address_end)
	{
		other.address_end=other.address_begin=nullptr;
	}
	win32_family_file_loader& operator=(win32_family_file_loader&& __restrict other) noexcept
	{
		win32::details::win32_unload_address(address_begin);
		address_begin=other.address_begin;
		address_end=other.address_end;
		other.address_end=other.address_begin=nullptr;
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
	constexpr std::size_t max_size() const noexcept
	{
		return SIZE_MAX;
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
#if __has_cpp_attribute(nodiscard)
	[[nodiscard]]
#endif
	constexpr pointer release() noexcept
	{
		auto temp{this->address_begin};
		this->address_end=this->address_begin = nullptr;
		return temp;
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
		win32::details::win32_unload_address(address_begin);
		address_end=address_begin=nullptr;
	}
	~win32_family_file_loader()
	{
		win32::details::win32_unload_address(address_begin);
	}
};

template<win32_family family>
inline constexpr basic_io_scatter_t<char> print_alias_define(io_alias_t,win32_family_file_loader<family> const& load) noexcept
{
	return {load.data(),load.size()};
}

using win32_file_loader_9xa=win32_family_file_loader<win32_family::ansi_9x>;
using win32_file_loader_ntw=win32_family_file_loader<win32_family::wide_nt>;
using win32_file_loader=win32_family_file_loader<win32_family::native>;

}
