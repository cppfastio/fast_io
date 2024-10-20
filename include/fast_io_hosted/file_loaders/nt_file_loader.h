#pragma once

namespace fast_io
{

namespace win32::nt::details
{

struct nt_file_loader_return_value_t
{
	char *address_begin{};
	char *address_end{};
};

template <::fast_io::nt_family family>
inline void nt_unload_address(void *address) noexcept
{
	if (address)
	{
		void *current_process_handle{reinterpret_cast<void *>(static_cast<::std::ptrdiff_t>(-1))};
		::fast_io::win32::nt::nt_unmap_view_of_section<family == ::fast_io::nt_family::zw>(current_process_handle,
																						   address);
	}
}

template <bool zw>
inline ::std::size_t nt_file_size_impl(void *handle)
{
	::fast_io::win32::nt::file_standard_information fsi;
	::fast_io::win32::nt::io_status_block block;
	auto status{::fast_io::win32::nt::nt_query_information_file<zw>(
		handle, __builtin_addressof(block), __builtin_addressof(fsi),
		static_cast<::std::uint_least32_t>(sizeof(::fast_io::win32::nt::file_standard_information)),
		::fast_io::win32::nt::file_information_class::FileStandardInformation)};
	if (status)
	{
		throw_nt_error(status);
	}
	if constexpr (sizeof(::std::size_t) < sizeof(::std::uint_least64_t))
	{
		if (SIZE_MAX < fsi.end_of_file)
		{
			throw_nt_error(0xC0000040 /*STATUS_SECTION_TOO_BIG*/);
		}
	}
	return static_cast<::std::size_t>(fsi.end_of_file);
}

template <::fast_io::nt_family family>
inline nt_file_loader_return_value_t nt_load_address_options_impl(::fast_io::nt_mmap_options const &options,
																  void *handle)
{
	using secattr_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= ::fast_io::win32::nt::object_attributes *;

	void *h_section{};
	::fast_io::win32::nt::object_attributes objAttr;
	secattr_ptr pobjattr{reinterpret_cast<secattr_ptr>(options.objAttr)};
	if (pobjattr == nullptr)
	{
		objAttr = {};
		objAttr.Length = sizeof(::fast_io::win32::nt::object_attributes);
		pobjattr = __builtin_addressof(objAttr);
	}
	::std::uint_least32_t status{};
	status = ::fast_io::win32::nt::nt_create_section<(family == ::fast_io::nt_family::zw)>(__builtin_addressof(h_section), options.dwDesiredAccess, pobjattr, nullptr, options.flProtect, options.attributes, handle);
	if (status)
	{
		throw_nt_error(status);
	}
	::fast_io::basic_nt_family_file<family, char> map_hd{h_section};
	void *p_map_address{};
	::std::size_t fsz{::fast_io::win32::nt::details::nt_file_size_impl<(family == ::fast_io::nt_family::zw)>(handle)};
	::std::size_t view_size{fsz};
	void *current_process_handle{reinterpret_cast<void *>(static_cast<::std::ptrdiff_t>(-1))};
	status = ::fast_io::win32::nt::nt_map_view_of_section<(family == ::fast_io::nt_family::zw)>(h_section, current_process_handle, __builtin_addressof(p_map_address), 0u, 0u, nullptr, __builtin_addressof(view_size), static_cast<::fast_io::win32::nt::section_inherit>(options.viewShare), 0u, options.flProtect);
	if (status)
	{
		throw_nt_error(status);
	}
	return {reinterpret_cast<char *>(p_map_address), reinterpret_cast<char *>(p_map_address) + fsz};
}

inline constexpr auto create_nt_default_load_address_option() noexcept
{
	::fast_io::nt_mmap_options options;
	options.dwDesiredAccess = 0x000F0000 | 0x0001 | 0x0004;
	options.flProtect = 0x08;
	options.attributes = 0x08000000;
	options.viewShare = 1;
	return options;
}

template <::fast_io::nt_family family>
inline nt_file_loader_return_value_t nt_load_address_impl(void *handle)
{
	constexpr auto nt_file_loader_default_option{create_nt_default_load_address_option()};
	return nt_load_address_options_impl<family>(nt_file_loader_default_option, handle);
}

template <::fast_io::nt_family family, typename... Args>
inline auto nt_load_file_impl(Args &&...args)
{
	::fast_io::basic_nt_family_file<family, char> nf(::fast_io::freestanding::forward<Args>(args)...);
	return nt_load_address_impl<family>(nf.handle);
}

template <::fast_io::nt_family family, typename... Args>
inline auto nt_load_file_options_impl(nt_mmap_options const &options, Args &&...args)
{
	::fast_io::basic_nt_family_file<family, char> wf(::fast_io::freestanding::forward<Args>(args)...);
	return nt_load_address_options_impl<family>(options, wf.handle);
}

} // namespace win32::nt::details

template <::fast_io::nt_family family>
	requires(family == ::fast_io::nt_family::nt || family == ::fast_io::nt_family::zw)
class nt_family_file_loader
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

	inline constexpr nt_family_file_loader() noexcept = default;
	inline explicit nt_family_file_loader(::fast_io::nt_at_entry ent)
	{
		auto ret{::fast_io::win32::nt::details::nt_load_address_impl<family>(ent.handle)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
	}
	inline explicit nt_family_file_loader(::fast_io::nt_fs_dirent fsdirent,
										  ::fast_io::open_mode om = ::fast_io::open_mode::in,
										  ::fast_io::perms pm = static_cast<::fast_io::perms>(436))
	{
		auto ret{::fast_io::win32::nt::details::nt_load_file_impl<family>(fsdirent, om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline explicit nt_family_file_loader(T const &filename, ::fast_io::open_mode om = ::fast_io::open_mode::in,
										  ::fast_io::perms pm = static_cast<::fast_io::perms>(436))
	{
		auto ret{::fast_io::win32::nt::details::nt_load_file_impl<family>(filename, om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline explicit nt_family_file_loader(::fast_io::nt_at_entry ent, T const &filename,
										  ::fast_io::open_mode om = ::fast_io::open_mode::in,
										  ::fast_io::perms pm = static_cast<::fast_io::perms>(436))
	{
		auto ret{::fast_io::win32::nt::details::nt_load_file_impl<family>(ent, filename, om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline explicit nt_family_file_loader(::fast_io::io_kernel_t, T const &t, ::fast_io::open_mode om,
										  ::fast_io::perms pm = static_cast<::fast_io::perms>(436))
	{
		auto ret{::fast_io::win32::nt::details::nt_load_file_impl<family>(::fast_io::io_kernel, t, om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline explicit nt_family_file_loader(::fast_io::io_kernel_t, ::fast_io::nt_at_entry ent, T const &t,
										  ::fast_io::open_mode om,
										  ::fast_io::perms pm = static_cast<::fast_io::perms>(436))
	{
		auto ret{::fast_io::win32::nt::details::nt_load_file_impl<family>(::fast_io::io_kernel, ent, t, om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
	}

	inline explicit nt_family_file_loader(nt_mmap_options const &options, ::fast_io::nt_at_entry ent)
	{
		auto ret{::fast_io::win32::nt::details::nt_load_address_options_impl<family>(options, ent.handle)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
	}
	inline explicit nt_family_file_loader(nt_mmap_options const &options, ::fast_io::nt_fs_dirent fsdirent,
										  ::fast_io::open_mode om = ::fast_io::open_mode::in,
										  ::fast_io::perms pm = static_cast<::fast_io::perms>(436))
	{
		auto ret{::fast_io::win32::nt::details::nt_load_file_options_impl<family>(options, fsdirent, om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline explicit nt_family_file_loader(nt_mmap_options const &options, T const &filename,
										  ::fast_io::open_mode om = ::fast_io::open_mode::in,
										  ::fast_io::perms pm = static_cast<::fast_io::perms>(436))
	{
		auto ret{::fast_io::win32::nt::details::nt_load_file_options_impl<family>(options, filename, om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline explicit nt_family_file_loader(nt_mmap_options const &options, ::fast_io::nt_at_entry ent, T const &filename,
										  ::fast_io::open_mode om = ::fast_io::open_mode::in,
										  ::fast_io::perms pm = static_cast<::fast_io::perms>(436))
	{
		auto ret{::fast_io::win32::nt::details::nt_load_file_options_impl<family>(options, ent, filename, om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline explicit nt_family_file_loader(nt_mmap_options const &options, ::fast_io::io_kernel_t, T const &t,
										  ::fast_io::open_mode om,
										  ::fast_io::perms pm = static_cast<::fast_io::perms>(436))
	{
		auto ret{
			::fast_io::win32::nt::details::nt_load_file_options_impl<family>(options, ::fast_io::io_kernel, t, om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline explicit nt_family_file_loader(nt_mmap_options const &options, ::fast_io::io_kernel_t,
										  ::fast_io::nt_at_entry ent, T const &t, ::fast_io::open_mode om,
										  ::fast_io::perms pm = static_cast<::fast_io::perms>(436))
	{
		auto ret{::fast_io::win32::nt::details::nt_load_file_options_impl<family>(options, ::fast_io::io_kernel, ent, t,
																				  om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
	}
	nt_family_file_loader(nt_family_file_loader const &) = delete;
	nt_family_file_loader &operator=(nt_family_file_loader const &) = delete;
	constexpr nt_family_file_loader(nt_family_file_loader &&__restrict other) noexcept
		: address_begin(other.address_begin), address_end(other.address_end)
	{
		other.address_end = other.address_begin = nullptr;
	}
	nt_family_file_loader &operator=(nt_family_file_loader &&__restrict other) noexcept
	{
		::fast_io::win32::nt::details::nt_unload_address<family>(address_begin);
		address_begin = other.address_begin;
		address_end = other.address_end;
		other.address_end = other.address_begin = nullptr;
		return *this;
	}
	constexpr pointer data() const noexcept
	{
		return address_begin;
	}
	constexpr bool empty() const noexcept
	{
		return address_begin == address_end;
	}
	constexpr bool is_empty() const noexcept
	{
		return address_begin == address_end;
	}
	constexpr ::std::size_t size() const noexcept
	{
		return static_cast<::std::size_t>(address_end - address_begin);
	}
	constexpr ::std::size_t max_size() const noexcept
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
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]]
	constexpr const_reference front() const noexcept
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
	constexpr reference front() noexcept
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
	constexpr const_reference back() const noexcept
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
	constexpr reference back() noexcept
	{
		if (address_begin == address_end) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return address_end[-1];
	}

	constexpr const_reference front_unchecked() const noexcept
	{
		return *address_begin;
	}
	constexpr reference front_unchecked() noexcept
	{
		return *address_begin;
	}
	constexpr const_reference back_unchecked() const noexcept
	{
		return address_end[-1];
	}
	constexpr reference back_unchecked() noexcept
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
	constexpr pointer release() noexcept
	{
		pointer temp{this->address_begin};
		this->address_end = this->address_begin = nullptr;
		return temp;
	}

	inline void close()
	{
		::fast_io::win32::nt::details::nt_unload_address<family>(address_begin);
		address_end = address_begin = nullptr;
	}
	~nt_family_file_loader()
	{
		::fast_io::win32::nt::details::nt_unload_address<family>(address_begin);
	}
};

template <::fast_io::nt_family family>
inline constexpr basic_io_scatter_t<char> print_alias_define(::fast_io::io_alias_t,
															 nt_family_file_loader<family> const &load) noexcept
{
	return {load.data(), load.size()};
}

using nt_file_loader = nt_family_file_loader<::fast_io::nt_family::nt>;
using zw_file_loader = nt_family_file_loader<::fast_io::nt_family::zw>;

} // namespace fast_io
