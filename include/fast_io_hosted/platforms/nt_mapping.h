#pragma once

namespace fast_io
{

enum class nt_file_map_attribute
{
	copy = 0x00000001,
	write = 0x00000002,
	read = 0x00000004,
	execute = 0x00000008,
	required = 0x000f0000
};

inline constexpr nt_file_map_attribute operator&(nt_file_map_attribute x, nt_file_map_attribute y) noexcept
{
	using utype = typename ::std::underlying_type<nt_file_map_attribute>::type;
	return static_cast<nt_file_map_attribute>(static_cast<utype>(x) & static_cast<utype>(y));
}

inline constexpr nt_file_map_attribute operator|(nt_file_map_attribute x, nt_file_map_attribute y) noexcept
{
	using utype = typename ::std::underlying_type<nt_file_map_attribute>::type;
	return static_cast<nt_file_map_attribute>(static_cast<utype>(x) | static_cast<utype>(y));
}

inline constexpr nt_file_map_attribute operator^(nt_file_map_attribute x, nt_file_map_attribute y) noexcept
{
	using utype = typename ::std::underlying_type<nt_file_map_attribute>::type;
	return static_cast<nt_file_map_attribute>(static_cast<utype>(x) ^ static_cast<utype>(y));
}

inline constexpr nt_file_map_attribute operator~(nt_file_map_attribute x) noexcept
{
	using utype = typename ::std::underlying_type<nt_file_map_attribute>::type;
	return static_cast<nt_file_map_attribute>(~static_cast<utype>(x));
}

inline constexpr nt_file_map_attribute &operator&=(nt_file_map_attribute &x, nt_file_map_attribute y) noexcept
{
	return x = x & y;
}

inline constexpr nt_file_map_attribute &operator|=(nt_file_map_attribute &x, nt_file_map_attribute y) noexcept
{
	return x = x | y;
}

inline constexpr nt_file_map_attribute &operator^=(nt_file_map_attribute &x, nt_file_map_attribute y) noexcept
{
	return x = x ^ y;
}

inline constexpr nt_file_map_attribute to_nt_file_map_attribute(file_map_attribute x)
{
	switch (x)
	{
	case file_map_attribute::execute_read:
		return nt_file_map_attribute::execute | nt_file_map_attribute::required | nt_file_map_attribute::read |
			   nt_file_map_attribute::copy;
	case file_map_attribute::execute_read_write:
		[[fallthrough]];
	case file_map_attribute::execute_write_copy:
		return nt_file_map_attribute::execute | nt_file_map_attribute::required | nt_file_map_attribute::read |
			   nt_file_map_attribute::copy | nt_file_map_attribute::write;
	case file_map_attribute::read_write:
		return nt_file_map_attribute::required | nt_file_map_attribute::read | nt_file_map_attribute::copy |
			   nt_file_map_attribute::write;
	case file_map_attribute::read_only:
		[[fallthrough]];
	case file_map_attribute::write_copy:
		return nt_file_map_attribute::required | nt_file_map_attribute::read | nt_file_map_attribute::copy;
	default:
		throw_nt_error(0x000000A0);
	};
}

namespace win32::nt::details
{

template <nt_family family>
inline void *create_file_mapping_impl(void *handle, file_map_attribute attr)
{
	::fast_io::win32::nt::object_attributes objAttr{};
	objAttr.Length = sizeof(::fast_io::win32::nt::object_attributes);
	void *h_section{};
	auto status{::fast_io::win32::nt::nt_create_section < family == ::fast_io::nt_family::zw > (__builtin_addressof(h_section), static_cast<::std::uint_least32_t>(to_nt_file_map_attribute(attr)),
																								__builtin_addressof(objAttr), nullptr, static_cast<::std::uint_least32_t>(attr), 0x08000000, handle)};
	if (status)
	{
		throw_nt_error(status);
	}
	return h_section;
}

} // namespace win32::nt::details

template <nt_family family>
	requires(family == nt_family::nt || family == nt_family::zw)
class nt_family_memory_map_file
{
public:
	using value_type = ::std::byte;
	using pointer = value_type *;
	using const_pointer = value_type const *;
	using const_iterator = const_pointer;
	using iterator = pointer;
	using reference = value_type &;
	using const_reference = value_type const &;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;
	using reverse_iterator = ::std::reverse_iterator<iterator>;
	pointer address_begin{}, address_end{};
	inline constexpr nt_family_memory_map_file() = default;
	inline constexpr nt_family_memory_map_file(::std::byte *addressbegin, ::std::byte *addressend)
		: address_begin{addressbegin}, address_end{addressend}
	{
	}
	inline nt_family_memory_map_file(nt_at_entry bf, file_map_attribute attr, ::std::size_t bytes,
							  ::std::uintmax_t start_address = 0)
	{
		basic_nt_family_file<family, char> mapping_file{
			win32::nt::details::create_file_mapping_impl<family>(bf.handle, attr)};
		void *base_ptr{};
		void *current_process_handle{reinterpret_cast<void *>(static_cast<::std::ptrdiff_t>(-1))};
		auto status{::fast_io::win32::nt::nt_map_view_of_section < family == ::fast_io::nt_family::zw > (mapping_file.handle, current_process_handle, __builtin_addressof(base_ptr), 0, 0,
																										 reinterpret_cast<::fast_io::win32::nt::large_integer const *>(__builtin_addressof(start_address)),
																										 __builtin_addressof(bytes), ::fast_io::win32::nt::section_inherit::ViewShare, 0,
																										 static_cast<::std::uint_least32_t>(attr))};
		if (status)
		{
			throw_nt_error(status);
		}
		this->address_begin = reinterpret_cast<::std::byte *>(base_ptr);
		this->address_end = this->address_begin + bytes;
	}
	inline constexpr pointer data() const noexcept
	{
		return address_begin;
	}
	inline constexpr bool empty() const noexcept
	{
		return address_begin == address_end;
	}
	inline constexpr ::std::size_t size() const noexcept
	{
		return static_cast<::std::size_t>(address_end - address_begin);
	}
	inline constexpr const_iterator cbegin() const noexcept
	{
		return address_begin;
	}
	inline constexpr const_iterator begin() const noexcept
	{
		return address_begin;
	}
	inline constexpr iterator begin() noexcept
	{
		return address_begin;
	}
	inline constexpr const_iterator cend() const noexcept
	{
		return address_end;
	}
	inline constexpr const_iterator end() const noexcept
	{
		return address_end;
	}
	inline constexpr iterator end() noexcept
	{
		return address_end;
	}
	inline constexpr ::std::size_t max_size() const noexcept
	{
		return SIZE_MAX;
	}
	inline constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator{address_end};
	}
	inline constexpr reverse_iterator rbegin() noexcept
	{
		return reverse_iterator{address_end};
	}
	inline constexpr const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator{address_end};
	}
	inline constexpr const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator{address_begin};
	}
	inline constexpr reverse_iterator rend() noexcept
	{
		return reverse_iterator{address_begin};
	}
	inline constexpr const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator{address_begin};
	}
	inline constexpr const_reference front() const noexcept
	{
		return *address_begin;
	}
	inline constexpr reference front() noexcept
	{
		return *address_begin;
	}
	inline constexpr const_reference back() const noexcept
	{
		return address_end[-1];
	}
	inline constexpr reference back() noexcept
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
	inline nt_family_memory_map_file(nt_family_memory_map_file const &) = delete;
	inline nt_family_memory_map_file &operator=(nt_family_memory_map_file const &) = delete;
	inline constexpr nt_family_memory_map_file(nt_family_memory_map_file &&__restrict other) noexcept
		: address_begin{other.address_begin}, address_end{other.address_end}
	{
		other.address_end = other.address_begin = nullptr;
	}
	inline nt_family_memory_map_file &operator=(nt_family_memory_map_file &&__restrict other) noexcept
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		if (this->address_begin) [[likely]]
		{
			void *current_process_handle{reinterpret_cast<void *>(static_cast<::std::ptrdiff_t>(-1))};
			// When destructuring, you shouldn't have to deal with return values like close.
			::fast_io::win32::nt::nt_unmap_view_of_section<family == ::fast_io::nt_family::zw>(current_process_handle,
																							   this->address_begin);
		}
		this->address_begin = other.address_begin;
		this->address_end = other.address_end;
		other.address_end = other.address_begin = nullptr;
		return *this;
	}
	inline void close()
	{
		if (this->address_begin) [[likely]]
		{
			void *current_process_handle{reinterpret_cast<void *>(static_cast<::std::ptrdiff_t>(-1))};
			auto ret{::fast_io::win32::nt::nt_unmap_view_of_section<family == ::fast_io::nt_family::zw>(current_process_handle, this->address_begin)};

			this->address_end = this->address_begin = nullptr;
			if (ret) [[unlikely]]
			{
				throw_nt_error(ret);
			}
		}
	}
	inline ~nt_family_memory_map_file()
	{
		if (this->address_begin) [[likely]]
		{
			void *current_process_handle{reinterpret_cast<void *>(static_cast<::std::ptrdiff_t>(-1))};
			// When destructuring, you shouldn't have to deal with return values like close.
			::fast_io::win32::nt::nt_unmap_view_of_section<family == ::fast_io::nt_family::zw>(current_process_handle,
																							   this->address_begin);
		}
	}
};

using nt_memory_map_file = nt_family_memory_map_file<nt_family::nt>;
using zw_memory_map_file = nt_family_memory_map_file<nt_family::zw>;

#if defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__) && !defined(__BIONIC__) && !defined(_WIN32_WINDOWS)
using native_memory_map_file = nt_memory_map_file;
#endif

} // namespace fast_io
