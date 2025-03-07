#pragma once

namespace fast_io
{

namespace win32::details
{

struct win32_file_loader_return_value_t
{
	char *address_begin{};
	char *address_end{};
};

inline win32_file_loader_return_value_t win32_load_address_common_options_impl(void *hfilemappingobj,
																			   ::std::size_t file_size,
																			   ::std::uint_least32_t dwDesiredAccess)
{
	if (hfilemappingobj == nullptr)
	{
		throw_win32_error();
	}
	::fast_io::win32_file map_hd{hfilemappingobj};
	auto base_ptr{::fast_io::win32::MapViewOfFile(hfilemappingobj, dwDesiredAccess, 0, 0, file_size)};
	if (base_ptr == nullptr)
	{
		throw_win32_error();
	}
	return {reinterpret_cast<char *>(base_ptr), reinterpret_cast<char *>(base_ptr) + file_size};
}

inline win32_file_loader_return_value_t win32_load_address_common_impl(void *hfilemappingobj, ::std::size_t file_size)
{
	return win32_load_address_common_options_impl(hfilemappingobj, file_size, 1);
}

template <win32_family family>
inline win32_file_loader_return_value_t win32_load_address_impl(void *handle)
{
	::std::size_t file_size{win32_load_file_get_file_size(handle)};
	if (file_size == 0)
	{
		return {nullptr, nullptr};
	}
	if constexpr (family == win32_family::wide_nt)
	{
		return win32_load_address_common_impl(
			::fast_io::win32::CreateFileMappingW(handle, nullptr, 0x08, 0, 0, nullptr), file_size);
	}
	else
	{
		return win32_load_address_common_impl(
			::fast_io::win32::CreateFileMappingA(handle, nullptr, 0x08, 0, 0, nullptr), file_size);
	}
}

template <win32_family family, typename... Args>
inline auto win32_load_file_impl(Args &&...args)
{
	::fast_io::basic_win32_family_file<family, char> wf(::fast_io::freestanding::forward<Args>(args)...);
	return win32_load_address_impl<family>(wf.handle);
}

template <win32_family family>
inline win32_file_loader_return_value_t win32_load_address_options_impl(win32_mmap_options const &options, void *handle)
{
	::std::size_t file_size{win32_load_file_get_file_size(handle)};
	if (file_size == 0)
	{
		return {nullptr, nullptr};
	}
	using secattr_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= ::fast_io::win32::security_attributes *;
	if constexpr (family == win32_family::wide_nt)
	{
		using char16_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= char16_t const *;
		return win32_load_address_common_options_impl(
			::fast_io::win32::CreateFileMappingW(handle, reinterpret_cast<secattr_ptr>(options.lpFileMappingAttributes),
												 options.flProtect, 0, 0,
												 reinterpret_cast<char16_const_ptr>(options.lpName)),
			file_size, options.dwDesiredAccess);
	}
	else
	{
		return win32_load_address_common_options_impl(
			::fast_io::win32::CreateFileMappingA(handle, reinterpret_cast<secattr_ptr>(options.lpFileMappingAttributes),
												 options.flProtect, 0, 0,
												 reinterpret_cast<char const *>(options.lpName)),
			file_size, options.dwDesiredAccess);
	}
}

template <win32_family family, typename... Args>
inline auto win32_load_file_options_impl(win32_mmap_options const &options, Args &&...args)
{
	::fast_io::basic_win32_family_file<family, char> wf(::fast_io::freestanding::forward<Args>(args)...);
	return win32_load_address_options_impl<family>(options, wf.handle);
}

inline void win32_unload_address(void const *address) noexcept
{
	if (address)
	{
		::fast_io::win32::UnmapViewOfFile(address);
	}
}

} // namespace win32::details

template <win32_family family>
class win32_family_file_loader
{
public:
	using value_type = char;
	using pointer = char *;
	using const_pointer = char const *;
	using const_iterator = const_pointer;
	using iterator = pointer;
	using reference = char &;
	using const_reference = char const &;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;
	using reverse_iterator = ::std::reverse_iterator<iterator>;

	pointer address_begin{};
	pointer address_end{};
	inline constexpr win32_family_file_loader() noexcept = default;
	inline explicit win32_family_file_loader(nt_at_entry ent)
	{
		auto ret{::fast_io::win32::details::win32_load_address_impl<family>(ent.handle)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
	}
	inline explicit win32_family_file_loader(nt_fs_dirent fsdirent, open_mode om = open_mode::in,
											 perms pm = static_cast<perms>(436))
	{
		auto ret{::fast_io::win32::details::win32_load_file_impl<family>(fsdirent, om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
	}
	inline explicit win32_family_file_loader(win32_9xa_fs_dirent fsdirent, open_mode om = open_mode::in,
											 perms pm = static_cast<perms>(436))
	{
		auto ret{::fast_io::win32::details::win32_load_file_impl<family>(fsdirent, om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline explicit win32_family_file_loader(T const &filename, open_mode om = open_mode::in,
											 perms pm = static_cast<perms>(436))
	{
		auto ret{::fast_io::win32::details::win32_load_file_impl<family>(filename, om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline explicit win32_family_file_loader(nt_at_entry ent, T const &filename, open_mode om = open_mode::in,
											 perms pm = static_cast<perms>(436))
	{
		auto ret{::fast_io::win32::details::win32_load_file_impl<family>(ent, filename, om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline explicit win32_family_file_loader(win32_9xa_at_entry ent, T const &filename, open_mode om = open_mode::in,
											 perms pm = static_cast<perms>(436))
	{
		auto ret{::fast_io::win32::details::win32_load_file_impl<family>(ent, filename, om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
	}
	inline explicit win32_family_file_loader(win32_mmap_options const &options, nt_at_entry ent)
	{
		auto ret{::fast_io::win32::details::win32_load_address_options_impl<family>(options, ent.handle)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
	}
	inline explicit win32_family_file_loader(win32_mmap_options const &options, nt_fs_dirent fsdirent,
											 open_mode om = open_mode::in, perms pm = static_cast<perms>(436))
	{
		auto ret{::fast_io::win32::details::win32_load_file_options_impl<family>(options, fsdirent, om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline explicit win32_family_file_loader(win32_mmap_options const &options, T const &filename,
											 open_mode om = open_mode::in, perms pm = static_cast<perms>(436))
	{
		auto ret{::fast_io::win32::details::win32_load_file_options_impl<family>(options, filename, om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline explicit win32_family_file_loader(win32_mmap_options const &options, nt_at_entry ent, T const &filename,
											 open_mode om = open_mode::in, perms pm = static_cast<perms>(436))
	{
		auto ret{::fast_io::win32::details::win32_load_file_options_impl<family>(options, ent, filename, om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
	}

	inline win32_family_file_loader(win32_family_file_loader const &) = delete;
	inline win32_family_file_loader &operator=(win32_family_file_loader const &) = delete;
	inline constexpr win32_family_file_loader(win32_family_file_loader &&__restrict other) noexcept
		: address_begin(other.address_begin), address_end(other.address_end)
	{
		other.address_end = other.address_begin = nullptr;
	}
	inline win32_family_file_loader &operator=(win32_family_file_loader &&__restrict other) noexcept
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		::fast_io::win32::details::win32_unload_address(address_begin);
		address_begin = other.address_begin;
		address_end = other.address_end;
		other.address_end = other.address_begin = nullptr;
		return *this;
	}
	inline constexpr pointer data() noexcept
	{
		return address_begin;
	}
	inline constexpr const_pointer data() const noexcept
	{
		return address_begin;
	}
	inline constexpr bool empty() const noexcept
	{
		return address_begin == address_end;
	}
	inline constexpr bool is_empty() const noexcept
	{
		return address_begin == address_end;
	}
	inline constexpr ::std::size_t size() const noexcept
	{
		return static_cast<::std::size_t>(address_end - address_begin);
	}
	inline constexpr ::std::size_t max_size() const noexcept
	{
		return SIZE_MAX;
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
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]]
	inline constexpr const_reference front() const noexcept
	{
		if (address_begin == address_end) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return *address_begin;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]]
	inline constexpr reference front() noexcept
	{
		if (address_begin == address_end) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return *address_begin;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]]
	inline constexpr const_reference back() const noexcept
	{
		if (address_begin == address_end) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return address_end[-1];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]]
	inline constexpr reference back() noexcept
	{
		if (address_begin == address_end) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return address_end[-1];
	}

	inline constexpr const_reference front_unchecked() const noexcept
	{
		return *address_begin;
	}
	inline constexpr reference front_unchecked() noexcept
	{
		return *address_begin;
	}
	inline constexpr const_reference back_unchecked() const noexcept
	{
		return address_end[-1];
	}
	inline constexpr reference back_unchecked() noexcept
	{
		return address_end[-1];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]]
	inline constexpr reference operator[](size_type size) noexcept
	{
		if (static_cast<size_type>(address_end - address_begin) <= size) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return address_begin[size];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]]
	inline constexpr const_reference operator[](size_type size) const noexcept
	{
		if (static_cast<size_type>(address_end - address_begin) <= size) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return address_begin[size];
	}

	inline constexpr reference index_unchecked(size_type size) noexcept
	{
		return address_begin[size];
	}
	inline constexpr const_reference index_unchecked(size_type size) const noexcept
	{
		return address_begin[size];
	}
#if __has_cpp_attribute(nodiscard)
	[[nodiscard]]
#endif
	inline constexpr pointer release() noexcept
	{
		auto temp{this->address_begin};
		this->address_end = this->address_begin = nullptr;
		return temp;
	}

	inline void close()
	{
		::fast_io::win32::details::win32_unload_address(address_begin);
		address_end = address_begin = nullptr;
	}
	inline ~win32_family_file_loader()
	{
		win32::details::win32_unload_address(address_begin);
	}
};

template <win32_family family>
inline constexpr basic_io_scatter_t<char> print_alias_define(io_alias_t,
															 win32_family_file_loader<family> const &load) noexcept
{
	return {load.data(), load.size()};
}

using win32_file_loader_9xa = win32_family_file_loader<win32_family::ansi_9x>;
using win32_file_loader_ntw = win32_family_file_loader<win32_family::wide_nt>;
using win32_file_loader = win32_family_file_loader<win32_family::native>;

} // namespace fast_io
