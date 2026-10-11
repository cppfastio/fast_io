#pragma once

namespace fast_io
{

namespace win32::details
{
using win32_9xa_dir_handle_path_str =
	::fast_io::containers::basic_string<char8_t, ::fast_io::generic_allocator_adapter<::fast_io::win32_heapalloc_allocator>>;

template <typename... Args>
constexpr inline win32_9xa_dir_handle_path_str concat_win32_9xa_dir_handle_path_str(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char8_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char8_t, win32_9xa_dir_handle_path_str>(
			::fast_io::io_print_forward<char8_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::win32::details::win32_9xa_dir_handle_path_str");
		return {};
	}
}

} // namespace win32::details

struct win32_9xa_dir_handle
{
	win32::details::win32_9xa_dir_handle_path_str path;
};

namespace win32::details
{
struct find_struct_guard
{
	void *file_struct{};

	inline explicit constexpr find_struct_guard(void *fs) noexcept : file_struct{fs}
	{}

	find_struct_guard(find_struct_guard const &) = delete;
	find_struct_guard &operator=(find_struct_guard const &) = delete;

	inline ~find_struct_guard()
	{
		if (file_struct && file_struct != reinterpret_cast<void *>(static_cast<::std::ptrdiff_t>(-1))) [[likely]]
		{
			::fast_io::win32::FindClose(file_struct);
		}
	}
};

inline void check_win32_9xa_dir_is_valid(win32_9xa_dir_handle const &h)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::win32_find_dataa wfda{};
	win32_9xa_dir_handle_path_str temp_find_path{concat_win32_9xa_dir_handle_path_str(h.path, u8"\\*")};
	auto find_struct{::fast_io::win32::FindFirstFileA(reinterpret_cast<char const *>(temp_find_path.c_str()), __builtin_addressof(wfda))};
	if (find_struct == reinterpret_cast<void *>(static_cast<::std::ptrdiff_t>(-1))) [[unlikely]]
	{
		throw_win32_error(0x2);
	}
	else
	{
		::fast_io::win32::FindClose(find_struct);
	}
}

[[nodiscard]] inline bool get_win32_9xa_dir_validity(win32_9xa_dir_handle const &h)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::win32_find_dataa wfda{};
	win32_9xa_dir_handle_path_str temp_find_path{concat_win32_9xa_dir_handle_path_str(h.path, u8"\\*")};
	auto find_struct{::fast_io::win32::FindFirstFileA(reinterpret_cast<char const *>(temp_find_path.c_str()), __builtin_addressof(wfda))};
	if (find_struct == reinterpret_cast<void *>(static_cast<::std::ptrdiff_t>(-1))) [[unlikely]]
	{
		return false;
	}
	else
	{
		::fast_io::win32::FindClose(find_struct);
		return true;
	}
}

template <bool throw_eh = false>
inline void close_win32_9xa_dir_handle(win32_9xa_dir_handle &h) FAST_IO_HERBCEPTIONS_THROWS_IF(throw_eh)
{
	if constexpr (throw_eh)
	{
		// Make sure to successfully close even if an exception is thrown.
		bool const is_win32_9xa_dir_valid{get_win32_9xa_dir_validity(h)};
		h.path.clear();
		if (!is_win32_9xa_dir_valid) [[unlikely]]
		{
			throw_win32_error(0x5);
		}
	}
	else
	{
		h.path.clear();
	}
}

inline win32_9xa_dir_handle win32_9xa_dir_dup_impl(win32_9xa_dir_handle const &h)
	FAST_IO_HERBCEPTIONS_THROWS
{
	check_win32_9xa_dir_is_valid(h);
	return {h.path};
}

inline win32_9xa_dir_handle win32_9xa_dir_dup2_impl(win32_9xa_dir_handle const &h1, win32_9xa_dir_handle &h2)
	FAST_IO_HERBCEPTIONS_THROWS
{
	auto temp{win32_9xa_dir_dup_impl(h1)};
	close_win32_9xa_dir_handle(h2);
	return temp;
}

inline win32_9xa_dir_handle basic_win32_9xa_create_dir_file_impl(char const *filename_c_str, ::std::size_t filename_c_str_len)
	FAST_IO_HERBCEPTIONS_THROWS
{
	using char8_t_const_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char8_t const *;

	win32_9xa_dir_handle_path_str path{concat_win32_9xa_dir_handle_path_str(::fast_io::mnp::os_c_str_with_known_size(
		reinterpret_cast<char8_t_const_may_alias_ptr>(filename_c_str), filename_c_str_len))};

	for (auto &c : path)
	{
		if (c == u8'/')
		{
			c = u8'\\';
		}
	}
	if (path.back() == u8'\\')
	{
		path.pop_back_unchecked();
	}

	win32_9xa_dir_handle ret{::std::move(path)};

	check_win32_9xa_dir_is_valid(ret);

	return ret;
}

inline win32_9xa_dir_handle basic_win32_9xa_create_dir_file_at_fs_dirent_impl(win32_9xa_dir_handle const *directory_handle, char const *filename_c_str,
																			  ::std::size_t filename_c_str_len)
	FAST_IO_HERBCEPTIONS_THROWS
{
	using char8_t_const_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char8_t const *;

	auto const beg{reinterpret_cast<char8_t_const_may_alias_ptr>(filename_c_str)};

#if 0
	if (::fast_io::details::is_invalid_dos_pathname_with_size(beg, filename_c_str_len)) [[unlikely]]
{
		throw_win32_error(3);
	}
#endif

	check_win32_9xa_dir_is_valid(*directory_handle);
	win32_9xa_dir_handle ret{concat_win32_9xa_dir_handle_path_str(directory_handle->path, u8"\\", ::fast_io::mnp::os_c_str_with_known_size(beg, filename_c_str_len))};
	check_win32_9xa_dir_is_valid(ret);

	return ret;
}

inline void *basic_win32_9xa_create_file_at_fs_dirent_impl(win32_9xa_dir_handle const *directory_handle, char const *filename_c_str,
														   ::std::size_t filename_c_str_len, open_mode_perms ompm)
	FAST_IO_HERBCEPTIONS_THROWS
{
	using char8_t_const_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char8_t const *;

	auto const beg{reinterpret_cast<char8_t_const_may_alias_ptr>(filename_c_str)};

#if 0
	if (::fast_io::details::is_invalid_dos_pathname_with_size(beg, filename_c_str_len)) [[unlikely]]
{
		throw_win32_error(3);
	}
#endif

	check_win32_9xa_dir_is_valid(*directory_handle);
	win32_9xa_dir_handle_path_str str{concat_win32_9xa_dir_handle_path_str(directory_handle->path, u8"\\", ::fast_io::mnp::os_c_str_with_known_size(beg, filename_c_str_len))};
	auto handle{::fast_io::details::win32_create_file_impl<win32_family::ansi_9x>(str, ompm)};
	return handle;
}

inline ::fast_io::win32::details::win32_9xa_dir_handle_path_str concat_win32_9xa_path_uncheck_whether_exist(::fast_io::win32_9xa_dir_handle const &dirhd, char8_t const *path_c_str, ::std::size_t path_size)
	FAST_IO_HERBCEPTIONS_THROWS
{
	auto const beg{path_c_str};

#if 0
	if (::fast_io::details::is_invalid_dos_pathname_with_size(beg, path_size)) [[unlikely]]
{
		throw_win32_error(3);
	}
#endif

	return ::fast_io::win32::details::concat_win32_9xa_dir_handle_path_str(dirhd.path, u8"\\", ::fast_io::mnp::os_c_str_with_known_size(beg, path_size));
}

struct win32_9xa_create_dir_file
{
	using family_char_type = char;
	inline win32_9xa_dir_handle operator()(family_char_type const *filename, ::std::size_t filename_c_str_len)
		FAST_IO_HERBCEPTIONS_THROWS
	{
		return basic_win32_9xa_create_dir_file_impl(filename, filename_c_str_len);
	}
};

struct win32_9xa_create_dir_file_at_fs_dirent
{
	using family_char_type = char;
	win32_9xa_dir_handle const *directory_handle{};
	inline win32_9xa_dir_handle operator()(family_char_type const *filename, ::std::size_t filename_c_str_len)
		FAST_IO_HERBCEPTIONS_THROWS
	{
		return basic_win32_9xa_create_dir_file_at_fs_dirent_impl(directory_handle, filename, filename_c_str_len);
	}
};

struct win32_9xa_create_file_at_fs_dirent
{
	using family_char_type = char;
	win32_9xa_dir_handle const *directory_handle{};
	open_mode_perms ompm{};
	inline void *operator()(family_char_type const *filename, ::std::size_t filename_c_str_len)
		FAST_IO_HERBCEPTIONS_THROWS
	{
		return basic_win32_9xa_create_file_at_fs_dirent_impl(directory_handle, filename, filename_c_str_len, ompm);
	}
};

template <typename T>
	requires(::fast_io::constructible_to_os_c_str<T>)
inline win32_9xa_dir_handle win32_9xa_create_dir_file_impl(T const &t) FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_api_common_9xa(t, win32_9xa_create_dir_file{});
}

template <::std::integral char_type>
inline win32_9xa_dir_handle win32_9xa_create_dir_file_at_fs_dirent_impl(win32_9xa_dir_handle const &directory_handle, char_type const *filename_c_str,
																		::std::size_t filename_c_str_len)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_api_common_9xa(::fast_io::mnp::os_c_str_with_known_size(filename_c_str, filename_c_str_len),
								win32_9xa_create_dir_file_at_fs_dirent{__builtin_addressof(directory_handle)});
}

template <typename T>
	requires(::fast_io::constructible_to_os_c_str<T>)
inline win32_9xa_dir_handle win32_9xa_create_dir_file_at_impl(win32_9xa_dir_handle const &directory_handle, T const &t)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_api_common_9xa(t, win32_9xa_create_dir_file_at_fs_dirent{__builtin_addressof(directory_handle)});
}

template <::std::integral char_type>
inline void *win32_9xa_create_file_at_fs_dirent_impl(win32_9xa_dir_handle const &directory_handle, char_type const *filename_c_str,
													 ::std::size_t filename_c_str_len, open_mode_perms ompm)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_api_common_9xa(::fast_io::mnp::os_c_str_with_known_size(filename_c_str, filename_c_str_len),
								win32_9xa_create_file_at_fs_dirent{__builtin_addressof(directory_handle), ompm});
}

template <typename T>
	requires(::fast_io::constructible_to_os_c_str<T>)
inline void *win32_9xa_create_file_at_impl(win32_9xa_dir_handle const &directory_handle, T const &t, open_mode_perms ompm)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_api_common_9xa(t, win32_9xa_create_file_at_fs_dirent{__builtin_addressof(directory_handle), ompm});
}

} // namespace win32::details

struct win32_9xa_fs_dirent
{
	win32_9xa_dir_handle handle{};                                               // path
	::fast_io::manipulators::basic_os_c_str_with_known_size<char8_t> filename{}; // file
};

struct win32_9xa_at_entry
{
	using native_handle_type = win32_9xa_dir_handle;
	native_handle_type handle{};
	inline explicit constexpr win32_9xa_at_entry() noexcept = default;
	inline explicit constexpr win32_9xa_at_entry(native_handle_type mhandle) noexcept
		: handle(::std::move(mhandle))
	{}
};

#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline win32_9xa_at_entry win32_9xa_at_fdcwd()
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_9xa_at_entry{{win32::details::concat_win32_9xa_dir_handle_path_str(u8".")}};
}

#if !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__) && defined(_WIN32_WINDOWS)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline win32_9xa_at_entry at_fdcwd()
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_9xa_at_fdcwd();
}
#endif

struct win32_9xa_dir_file_factory FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
{
	using native_handle_type = win32_9xa_dir_handle;
	win32_9xa_dir_handle handle{};
	inline explicit constexpr win32_9xa_dir_file_factory(native_handle_type hd) noexcept
		: handle(::std::move(hd)) {};
	inline win32_9xa_dir_file_factory(win32_9xa_dir_file_factory const &) = delete;
	inline win32_9xa_dir_file_factory &operator=(win32_9xa_dir_file_factory const &) = delete;
	inline ~win32_9xa_dir_file_factory()
	{
		if (!handle.path.is_empty()) [[likely]]
		{
			::fast_io::win32::details::close_win32_9xa_dir_handle(handle);
		}
	}
};

class win32_9xa_dir_io_observer
{
public:
	using native_handle_type = win32_9xa_dir_handle;
	using char_type = char8_t;
	using input_char_type = char_type;
	using output_char_type = char_type;
	native_handle_type handle{};
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return handle;
	}
	inline explicit constexpr operator bool() const noexcept
	{
		return !handle.path.is_empty();
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{::std::move(handle)};
		return temp;
	}
};

inline constexpr bool operator==(win32_9xa_dir_io_observer const &a,
								 win32_9xa_dir_io_observer const &b) noexcept
{
	return a.handle.path == b.handle.path;
}

#if __cpp_impl_three_way_comparison >= 201907L
inline constexpr auto operator<=>(win32_9xa_dir_io_observer const &a,
								  win32_9xa_dir_io_observer const &b) noexcept
{
	return a.handle.path <=> b.handle.path;
}
#endif

class win32_9xa_dir_file : public win32_9xa_dir_io_observer
{
public:
	using typename win32_9xa_dir_io_observer::char_type;
	using typename win32_9xa_dir_io_observer::input_char_type;
	using typename win32_9xa_dir_io_observer::output_char_type;
	using typename win32_9xa_dir_io_observer::native_handle_type;
	using win32_9xa_dir_io_observer::native_handle;
	using file_factory_type = win32_9xa_dir_file_factory;
	inline explicit constexpr win32_9xa_dir_file() noexcept = default;

	inline constexpr win32_9xa_dir_file(win32_9xa_dir_io_observer) noexcept = delete;
	inline constexpr win32_9xa_dir_file &operator=(win32_9xa_dir_io_observer) noexcept = delete;

	inline win32_9xa_dir_file(win32_9xa_dir_file const &other)
		FAST_IO_HERBCEPTIONS_THROWS
		: win32_9xa_dir_io_observer{win32::details::win32_9xa_dir_dup_impl(other.handle)}
	{
	}
	inline win32_9xa_dir_file &operator=(win32_9xa_dir_file const &other)
		FAST_IO_HERBCEPTIONS_THROWS
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		this->handle = win32::details::win32_9xa_dir_dup2_impl(other.handle, this->handle);
		return *this;
	}
	inline win32_9xa_dir_file(win32_9xa_dir_file &&__restrict b) noexcept
		: win32_9xa_dir_io_observer{b.release()}
	{
	}
	inline win32_9xa_dir_file &operator=(win32_9xa_dir_file &&__restrict b) noexcept
	{
		if (__builtin_addressof(b) == this) [[unlikely]]
		{
			return *this;
		}
		if (*this) [[likely]]
		{
			win32::details::close_win32_9xa_dir_handle(this->handle);
		}
		this->handle = b.release();
		return *this;
	}
	inline void reset(native_handle_type newhandle = {}) noexcept
	{
		if (*this) [[likely]]
		{
			::fast_io::win32::details::close_win32_9xa_dir_handle(this->handle);
		}
		this->handle = ::std::move(newhandle);
	}
	inline void close()
		FAST_IO_HERBCEPTIONS_THROWS
	{
		if (*this) [[likely]]
		{
			::fast_io::win32::details::close_win32_9xa_dir_handle<true>(this->handle);
		}
	}

	template <typename native_hd>
		requires ::std::same_as<native_handle_type, ::std::remove_cvref_t<native_hd>>
	inline explicit constexpr win32_9xa_dir_file(native_hd handle1) noexcept
		: win32_9xa_dir_io_observer{::std::move(handle1)}
	{
	}

	inline win32_9xa_dir_file(io_dup_t, win32_9xa_dir_io_observer wiob)
		FAST_IO_HERBCEPTIONS_THROWS
		: win32_9xa_dir_io_observer{::fast_io::win32::details::win32_9xa_dir_dup_impl(wiob.handle)}
	{
	}

	inline explicit constexpr win32_9xa_dir_file(win32_9xa_dir_file_factory &&fact) noexcept
		: win32_9xa_dir_io_observer{::std::move(fact.handle)}
	{
	}

	inline explicit constexpr win32_9xa_dir_file(decltype(nullptr)) noexcept = delete;

	inline explicit win32_9xa_dir_file(win32_9xa_fs_dirent fsdirent, [[maybe_unused]] open_mode om = open_mode::directory, [[maybe_unused]] perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
		: win32_9xa_dir_io_observer{
			  ::fast_io::win32::details::win32_9xa_create_dir_file_at_fs_dirent_impl(
				  fsdirent.handle, fsdirent.filename.c_str(), fsdirent.filename.size())}
	{
	}

	template <::fast_io::constructible_to_os_c_str T>
	inline explicit win32_9xa_dir_file(T const &filename, [[maybe_unused]] open_mode om = open_mode::directory, [[maybe_unused]] perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
		: win32_9xa_dir_io_observer{
			  ::fast_io::win32::details::win32_9xa_create_dir_file_impl(filename)}
	{
	}

	template <::fast_io::constructible_to_os_c_str T>
	inline explicit win32_9xa_dir_file(win32_9xa_at_entry nate, T const &filename, [[maybe_unused]] open_mode om = open_mode::directory, [[maybe_unused]] perms pm = static_cast<perms>(436))
		FAST_IO_HERBCEPTIONS_THROWS
		: win32_9xa_dir_io_observer{
			  ::fast_io::win32::details::win32_9xa_create_dir_file_at_impl(nate.handle, filename)}
	{
	}

	inline ~win32_9xa_dir_file()
	{
		if (*this) [[likely]]
		{
			::fast_io::win32::details::close_win32_9xa_dir_handle(this->handle);
		}
	}
};

inline win32_9xa_at_entry at(win32_9xa_dir_io_observer const &wiob) noexcept
{
	return win32_9xa_at_entry{wiob.handle};
}

} // namespace fast_io
