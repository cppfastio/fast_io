#pragma once

namespace fast_io
{

namespace details
{

template <typename allocator>
struct allocation_file_loader_closer_impl
{
	int fd{-1};
	char *address_begin{};
	::std::size_t size{};
	inline explicit constexpr allocation_file_loader_closer_impl(int fdd, char *addressbegin, ::std::size_t sz) noexcept
		: fd{fdd}, address_begin{addressbegin}, size{sz}
	{
	}
	inline allocation_file_loader_closer_impl(allocation_file_loader_closer_impl const &) = delete;
	inline allocation_file_loader_closer_impl &operator=(allocation_file_loader_closer_impl const &) = delete;
	inline ~allocation_file_loader_closer_impl() noexcept
	{
		if (fd != -1)
		{
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__)
			::fast_io::noexcept_call(::_close, fd);
#else
			::fast_io::noexcept_call(::close, fd);
#endif
		}
		if (address_begin != nullptr)
		{
			::fast_io::typed_generic_allocator_adapter<allocator, char>::deallocate_n(address_begin, size);
		}
	}
};

template <typename allocator>
inline void close_allocation_file_loader_impl(int fd, char *address_begin, char *address_end) noexcept
{
	::std::size_t sz{};
	if (address_begin != nullptr)
	{
		sz = static_cast<::std::size_t>(address_end - address_begin);
	}
	allocation_file_loader_closer_impl<allocator> closer(fd, address_begin, sz);
	if (fd == -1)
	{
		return;
	}
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__)
	constexpr bool needshrinktoint32{true};
#else
	constexpr bool needshrinktoint32{};
#endif
	using towritetype = ::std::conditional_t<needshrinktoint32, unsigned, ::std::size_t>;
	while (address_begin != address_end)
	{
		::std::size_t towrite{static_cast<::std::size_t>(address_end - address_begin)};
		if constexpr (sizeof(towritetype) < sizeof(::std::size_t))
		{
			constexpr ::std::size_t towritemx{::std::numeric_limits<towritetype>::max()};
			if (towritemx < towrite)
			{
				towrite = towritemx;
			}
		}
		auto ret{::fast_io::noexcept_call(
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__)
			::_write
#else
			::write
#endif
			,
			fd, address_begin, static_cast<towritetype>(towrite))};
		if (ret == -1)
		{
#ifdef EAGAIN
			if (errno != EAGAIN)
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
	char *address_begin;
	char *address_end;
	char *address_capacity;
	int fd;
};

template <typename allocator>
struct load_file_allocation_guard
{
	using typed_allocator = ::fast_io::typed_generic_allocator_adapter<allocator, char>;
	char *address{};
	::std::size_t size{};
	inline explicit constexpr load_file_allocation_guard() noexcept = default;
	inline explicit load_file_allocation_guard(::std::size_t file_size) FAST_IO_HERBCEPTIONS_THROWS
		: address(typed_allocator::allocate_try(file_size)),
		  size(file_size)
	{
	}
	inline load_file_allocation_guard(load_file_allocation_guard const &) = delete;
	inline load_file_allocation_guard &operator=(load_file_allocation_guard const &) = delete;
	inline ~load_file_allocation_guard() noexcept
	{
		if (address != nullptr)
		{
			typed_allocator::deallocate_n(address, size);
		}
	}
};

template <typename allocator>
inline allocation_file_loader_ret allocation_load_address_impl(int fd)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::std::size_t filesize{::fast_io::details::posix_loader_get_file_size(fd)};
	load_file_allocation_guard<allocator> guard{filesize};
	posix_io_observer piob{fd};
	auto addr{guard.address};
	auto addr_ed{addr + filesize};
	::fast_io::operations::decay::read_all_bytes_decay(piob, reinterpret_cast<::std::byte *>(addr), filesize);
	guard.address = nullptr;
	return {addr, addr_ed, addr_ed, -1};
}

inline void rewind_allocation_file_loader(int fd)
	FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__)
	auto seekret = ::fast_io::noexcept_call(::_lseeki64, fd, 0, 0);
#else
#if defined(_LARGEFILE64_SOURCE)
	auto seekret = ::fast_io::noexcept_call(::lseek64, fd, 0, 0);
#else
	auto seekret = ::fast_io::noexcept_call(::lseek, fd, 0, 0);
#endif
#endif
	if (seekret == -1)
	{
		throw_posix_error();
	}
}

template <typename allocator, typename... Args>
inline allocation_file_loader_ret allocation_load_file_impl(bool writeback, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::posix_file pf(::fast_io::freestanding::forward<Args>(args)...);
	auto ret{allocation_load_address_impl<allocator>(pf.fd)};

	if (writeback)
	{
		load_file_allocation_guard<allocator> loader;
		loader.address = ret.address_begin;
		loader.size = static_cast<::std::size_t>(ret.address_end - ret.address_begin);
		rewind_allocation_file_loader(pf.fd);
		loader.address = nullptr;
		ret.fd = pf.release();
	}
	return ret;
}

template <typename allocator>
inline allocation_file_loader_ret allocation_load_file_fd_impl(bool writeback, int fd)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::posix_file pf(::fast_io::io_dup, ::fast_io::posix_io_observer{fd});
	rewind_allocation_file_loader(pf.fd);
	auto ret{allocation_load_address_impl<allocator>(pf.fd)};
	if (writeback)
	{
		load_file_allocation_guard<allocator> loader;
		loader.address = ret.address_begin;
		loader.size = static_cast<::std::size_t>(ret.address_end - ret.address_begin);
		rewind_allocation_file_loader(pf.fd);
		loader.address = nullptr;
		ret.fd = pf.release();
	}
	return ret;
}

} // namespace details

template <typename allocator>
class basic_allocation_file_loader
{
public:
	using allocator_type = allocator;
	using file_type = posix_file;
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
	pointer address_capacity{};
	int fd{-1};
	inline explicit constexpr basic_allocation_file_loader() noexcept = default;

	inline explicit basic_allocation_file_loader(posix_at_entry pate)
		FAST_IO_HERBCEPTIONS_THROWS
	{
		auto ret{::fast_io::details::allocation_load_file_fd_impl<allocator>(false, pate.fd)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
		address_capacity = ret.address_capacity;
	}
	inline explicit basic_allocation_file_loader(native_fs_dirent fsdirent, open_mode om = open_mode::in,
												 perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
	{
		auto ret{::fast_io::details::allocation_load_file_impl<allocator>(false, fsdirent, om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
		address_capacity = ret.address_capacity;
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline explicit basic_allocation_file_loader(T const &filename, open_mode om = open_mode::in,
												 perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
	{
		auto ret{::fast_io::details::allocation_load_file_impl<allocator>(false, filename, om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
		address_capacity = ret.address_capacity;
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline explicit basic_allocation_file_loader(native_at_entry ent, T const &filename, open_mode om = open_mode::in,
												 perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
	{
		auto ret{::fast_io::details::allocation_load_file_impl<allocator>(false, ent, filename, om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
		address_capacity = ret.address_capacity;
	}
	inline explicit basic_allocation_file_loader(allocation_mmap_options options, posix_at_entry pate)
		FAST_IO_HERBCEPTIONS_THROWS
	{
		auto ret{::fast_io::details::allocation_load_file_fd_impl<allocator>(options.write_back, pate.fd)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
		address_capacity = ret.address_capacity;
		fd = ret.fd;
	}
	inline explicit basic_allocation_file_loader(allocation_mmap_options options, native_fs_dirent fsdirent,
												 open_mode om = open_mode::in, perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
	{
		auto ret{::fast_io::details::allocation_load_file_impl<allocator>(options.write_back, fsdirent, om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
		address_capacity = ret.address_capacity;
		fd = ret.fd;
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline explicit basic_allocation_file_loader(allocation_mmap_options options, T const &filename,
												 open_mode om = open_mode::in, perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
	{
		auto ret{::fast_io::details::allocation_load_file_impl<allocator>(options.write_back, filename, om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
		address_capacity = ret.address_capacity;
		fd = ret.fd;
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline explicit basic_allocation_file_loader(allocation_mmap_options options, native_at_entry ent, T const &filename,
												 open_mode om = open_mode::in, perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
	{
		auto ret{::fast_io::details::allocation_load_file_impl<allocator>(options.write_back, ent, filename, om, pm)};
		address_begin = ret.address_begin;
		address_end = ret.address_end;
		address_capacity = ret.address_capacity;
		fd = ret.fd;
	}

	inline basic_allocation_file_loader(basic_allocation_file_loader const &) = delete;
	inline basic_allocation_file_loader &operator=(basic_allocation_file_loader const &) = delete;
	inline constexpr basic_allocation_file_loader(basic_allocation_file_loader &&__restrict other) noexcept
		: address_begin(other.address_begin), address_end(other.address_end), address_capacity(other.address_capacity),
		  fd(other.fd)
	{
		other.address_capacity = other.address_end = other.address_begin = nullptr;
		other.fd = -1;
	}
	inline basic_allocation_file_loader &operator=(basic_allocation_file_loader &&__restrict other) noexcept
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}

		::fast_io::details::close_allocation_file_loader_impl<allocator>(fd, address_begin, address_end);

		// There is no need to check the 'this' pointer as there are no side effects
		address_begin = other.address_begin;
		address_end = other.address_end;
		address_capacity = other.address_capacity;
		other.address_capacity = other.address_end = other.address_begin = nullptr;
		other.fd = -1;
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
	inline constexpr bool is_empty() const noexcept
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
	inline constexpr ::std::size_t capacity() const noexcept
	{
		return static_cast<::std::size_t>(this->address_capacity - this->address_begin);
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
	inline void close() noexcept
	{
		::fast_io::details::close_allocation_file_loader_impl<allocator>(fd, address_begin, address_end);
		address_capacity = address_end = address_begin = nullptr;
		fd = -1;
	}
#if __has_cpp_attribute(nodiscard)
	[[nodiscard]]
#endif
	inline constexpr pointer release() noexcept
	{
		pointer temp{address_begin};
		address_capacity = address_end = address_begin = nullptr;
		if (fd != -1)
		{
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__)
			::fast_io::noexcept_call(::_close, fd);
#else
			::fast_io::noexcept_call(::close, fd);
#endif
		}
		fd = -1;
		return temp;
	}
	inline ~basic_allocation_file_loader() noexcept
	{
		::fast_io::details::close_allocation_file_loader_impl<allocator>(fd, address_begin, address_end);
	}
};

using allocation_file_loader = basic_allocation_file_loader<
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__)
#if defined(_WIN32_WINDOWS)
	::fast_io::generic_allocator_adapter<::fast_io::win32_heapalloc_allocator>
#else
	::fast_io::generic_allocator_adapter<::fast_io::nt_rtlallocateheap_allocator>
#endif
#else
	::fast_io::native_thread_local_allocator
#endif
	>;

template <typename allocator>
inline constexpr basic_io_scatter_t<char> print_alias_define(io_alias_t, basic_allocation_file_loader<allocator> const &load) noexcept
{
	return {load.data(), load.size()};
}

} // namespace fast_io
