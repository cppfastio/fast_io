#pragma once

namespace fast_io
{

enum class win32_file_map_attribute
{
copy=0x00000001,write=0x00000002,read=0x00000004,all_access=0x000f001f,execute=0x00000020
};

constexpr win32_file_map_attribute operator&(win32_file_map_attribute x, win32_file_map_attribute y) noexcept
{
using utype = typename std::underlying_type<win32_file_map_attribute>::type;
return static_cast<win32_file_map_attribute>(static_cast<utype>(x) & static_cast<utype>(y));
}

constexpr win32_file_map_attribute operator|(win32_file_map_attribute x, win32_file_map_attribute y) noexcept
{
using utype = typename std::underlying_type<win32_file_map_attribute>::type;
return static_cast<win32_file_map_attribute>(static_cast<utype>(x) | static_cast<utype>(y));
}

constexpr win32_file_map_attribute operator^(win32_file_map_attribute x, win32_file_map_attribute y) noexcept
{
using utype = typename std::underlying_type<win32_file_map_attribute>::type;
return static_cast<win32_file_map_attribute>(static_cast<utype>(x) ^ static_cast<utype>(y));
}

constexpr win32_file_map_attribute operator~(win32_file_map_attribute x) noexcept
{
using utype = typename std::underlying_type<win32_file_map_attribute>::type;
return static_cast<win32_file_map_attribute>(~static_cast<utype>(x));
}

inline constexpr win32_file_map_attribute& operator&=(win32_file_map_attribute& x, win32_file_map_attribute y) noexcept{return x=x&y;}

inline constexpr win32_file_map_attribute& operator|=(win32_file_map_attribute& x, win32_file_map_attribute y) noexcept{return x=x|y;}

inline constexpr win32_file_map_attribute& operator^=(win32_file_map_attribute& x, win32_file_map_attribute y) noexcept{return x=x^y;}

inline constexpr win32_file_map_attribute to_win32_file_map_attribute(file_map_attribute x)
{
	switch(x)
	{
	case file_map_attribute::execute_read:return win32_file_map_attribute::execute|win32_file_map_attribute::read;
	case file_map_attribute::execute_read_write:return win32_file_map_attribute::execute|win32_file_map_attribute::read;
	case file_map_attribute::execute_write_copy:return win32_file_map_attribute::execute|win32_file_map_attribute::write|win32_file_map_attribute::copy;
	case file_map_attribute::read_only:return win32_file_map_attribute::read;
	case file_map_attribute::read_write:return win32_file_map_attribute::read|win32_file_map_attribute::write;
	case file_map_attribute::write_copy:return win32_file_map_attribute::write|win32_file_map_attribute::copy;
	default:
		throw_win32_error(0x000000A0);
	};
}

namespace win32::details
{

template<win32_family family>
inline void* create_file_mapping_impl(void* handle,file_map_attribute attr)
{
	if constexpr(family==win32_family::wide_nt)
	{
		void* addr{win32::CreateFileMappingW(handle,nullptr,static_cast<std::uint_least32_t>(attr),0,0,nullptr)};
		if(addr==nullptr)
			throw_win32_error();
		return addr;
	}
	else
	{
		void* addr{win32::CreateFileMappingA(handle,nullptr,static_cast<std::uint_least32_t>(attr),0,0,nullptr)};
		if(addr==nullptr)
			throw_win32_error();
		return addr;
	}
}

}

template<win32_family family>
class win32_family_memory_map_file
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
	constexpr win32_family_memory_map_file()=default;
	constexpr win32_family_memory_map_file(std::byte* addressbegin,std::byte* addressend):address_begin{addressbegin},address_end{addressend}{}
	win32_family_memory_map_file(nt_at_entry bf,file_map_attribute attr,std::size_t bytes,std::uintmax_t start_address=0)
	{
		basic_win32_family_file<family,char> mapping_file{win32::details::create_file_mapping_impl<family>(bf.handle,attr)};
		void *base_ptr{win32::MapViewOfFile(mapping_file.handle,static_cast<std::uint_least32_t>(to_win32_file_map_attribute(attr)),start_address>>32,static_cast<std::uint_least32_t>(start_address),bytes)};
		if(base_ptr==nullptr)
			throw_win32_error();
		this->address_begin=reinterpret_cast<std::byte*>(base_ptr);
		this->address_end=this->address_begin+bytes;
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
	win32_family_memory_map_file(win32_family_memory_map_file const&)=delete;
	win32_family_memory_map_file& operator=(win32_family_memory_map_file const&)=delete;
	constexpr win32_family_memory_map_file(win32_family_memory_map_file&& __restrict other) noexcept:address_begin{other.address_begin},address_end{other.address_end}
	{
		other.address_end=other.address_begin=nullptr;
	}
	win32_family_memory_map_file& operator=(win32_family_memory_map_file&& __restrict other) noexcept
	{
		if(this->address_begin)[[likely]]
			win32::UnmapViewOfFile(this->address_begin);
		this->address_begin=other.address_begin;
		this->address_end=other.address_end;
		other.address_end=other.address_begin=nullptr;
		return *this;
	}
	void close()
	{
		if(this->address_begin)[[likely]]
		{
			auto ret{win32::UnmapViewOfFile(this->address_begin)};
			this->address_end=this->address_begin=nullptr;
			if(!ret)
				throw_win32_error();
		}
	}
	~win32_family_memory_map_file()
	{
		if(this->address_begin)[[likely]]
			win32::UnmapViewOfFile(this->address_begin);
	}
};

using win32_memory_map_file_9xa = win32_family_memory_map_file<win32_family::ansi_9x>;
using win32_memory_map_file_ntw = win32_family_memory_map_file<win32_family::wide_nt>;
using win32_memory_map_file = win32_family_memory_map_file<win32_family::native>;

#ifndef __CYGWIN__
using native_memory_map_file = win32_memory_map_file;
#endif

}
