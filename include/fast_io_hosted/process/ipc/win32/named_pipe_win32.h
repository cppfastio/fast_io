#pragma once

namespace fast_io
{

namespace win32::details
{
template <win32_family family>
using win32_named_pipe_char_type = ::std::conditional_t<family == win32_family::wide_nt, char16_t, char>;

template <win32_family family>
using win32_named_pipe_internal_char_type = ::std::conditional_t<family == win32_family::wide_nt, char16_t, char8_t>;

template <win32_family family>
using win32_named_pipe_internal_str = ::fast_io::containers::basic_string<win32_named_pipe_internal_char_type<family>, ::fast_io::native_global_allocator>;

template <win32_family family>
using win32_named_pipe_internal_tlc_str = ::fast_io::containers::basic_string<win32_named_pipe_internal_char_type<family>, ::fast_io::native_thread_local_allocator>;

template <win32_family family>
using win32_named_pipe_internal_strvw = ::fast_io::containers::basic_string_view<win32_named_pipe_internal_char_type<family>>;

template <win32_family family, typename... Args>
constexpr inline win32_named_pipe_internal_str<family> concat_win32_named_pipe_internal_str(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<win32_named_pipe_internal_char_type<family>>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, win32_named_pipe_internal_char_type<family>, win32_named_pipe_internal_str<family>>(
			::fast_io::io_print_forward<win32_named_pipe_internal_char_type<family>>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::win32::details::win32_named_pipe_internal_str");
		return {};
	}
}

template <win32_family family, typename... Args>
constexpr inline win32_named_pipe_internal_tlc_str<family> concat_win32_named_pipe_internal_tlc_str(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<win32_named_pipe_internal_char_type<family>>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, win32_named_pipe_internal_char_type<family>, win32_named_pipe_internal_tlc_str<family>>(
			::fast_io::io_print_forward<win32_named_pipe_internal_char_type<family>>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::win32::details::win32_named_pipe_internal_tlc_str");
		return {};
	}
}

using win32_client_connection_handle = void;

// SERVER
template <win32_family family>
inline void *win32_family_create_named_pipe_ipc_server_impl(win32_named_pipe_char_type<family> const *server_name, ::std::size_t server_name_size, ::fast_io::ipc_mode mode)
{
	// check filename
	using family_internal_char_type = win32_named_pipe_internal_char_type<family>;
#if 0
	using family_internal_str = win32_named_pipe_internal_str<family>;
#endif
	using family_internal_char_type_const_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= family_internal_char_type const *;
#if 0
	{
		// Prohibit including superior paths, otherwise it may cause vulnerability

		auto const beg{reinterpret_cast<family_internal_char_type_const_may_alias_ptr>(server_name)};
		auto const end{beg + server_name_size};
		auto curr{beg};

		bool is_new_dir{true};
		::std::size_t point_counter{};

		for (; curr != end; ++curr)
		{
			auto fc{*curr};
			if (fc == ::fast_io::char_literal_v<u8'.', family_internal_char_type>)
			{
				if (is_new_dir)
				{
					++point_counter;
				}
			}
			else if (fc == ::fast_io::char_literal_v<u8'/', family_internal_char_type> ||
					 fc == ::fast_io::char_literal_v<u8'\\', family_internal_char_type>)
			{
				if (point_counter == 2)
				{
					if constexpr (family == win32_family::wide_nt)
					{
						throw_win32_error(3);
					}
					else if constexpr (family == win32_family::ansi_9x)
					{
						throw_win32_error(3221225530);
					}
				}
				is_new_dir = true;
				point_counter = 0;
			}
			else
			{
				is_new_dir = false;
				point_counter = 0;
			}
		}

		if (is_new_dir && point_counter == 2)
		{
			if constexpr (family == win32_family::wide_nt)
			{
				throw_win32_error(3);
			}
			else if constexpr (family == win32_family::ansi_9x)
			{
				throw_win32_error(3221225530);
			}
		}
	}
#else
	auto const beg{reinterpret_cast<family_internal_char_type_const_may_alias_ptr>(server_name)};

	if (::fast_io::details::is_invalid_dos_filename_with_size(beg, server_name_size)) [[unlikely]]
	{
		if constexpr (family == win32_family::wide_nt)
		{
			throw_win32_error(3);
		}
		else if constexpr (family == win32_family::ansi_9x)
		{
			throw_win32_error(3221225530);
		}
	}
#endif

	using win32_named_pipe_server_internal_char_type_tlc_str = win32_named_pipe_internal_tlc_str<family>;
	win32_named_pipe_server_internal_char_type_tlc_str temp_pipe_file_name_tlc_str{};

	if constexpr (::std::same_as<win32_named_pipe_char_type<family>, char8_t>)
	{
		temp_pipe_file_name_tlc_str = concat_win32_named_pipe_internal_tlc_str<family>(u8"\\\\.\\pipe\\fast_io_ipc\\", ::fast_io::mnp::os_c_str_with_known_size(beg, server_name_size));
	}
	else if constexpr (::std ::same_as<win32_named_pipe_char_type<family>, char16_t>)
	{
		temp_pipe_file_name_tlc_str = concat_win32_named_pipe_internal_tlc_str<family>(u"\\\\.\\pipe\\fast_io_ipc\\", ::fast_io::mnp::os_c_str_with_known_size(beg, server_name_size));
	}

	::std::uint_least32_t dwOpenMode{};

	if ((mode & ::fast_io::ipc_mode::in) == ::fast_io::ipc_mode::in)
	{
		dwOpenMode |= 0x00000001; // PIPE_ACCESS_INBOUND
	}
	if ((mode & ::fast_io::ipc_mode::out) == ::fast_io::ipc_mode::out)
	{
		dwOpenMode |= 0x00000002; // PIPE_ACCESS_OUTBOUND
	}
	if ((mode & ::fast_io::ipc_mode::sync) != ::fast_io::ipc_mode::none)
	{
		dwOpenMode |= 0x80000000; // FILE_FLAG_WRITE_THROUGH
	}
	if ((mode & ::fast_io::ipc_mode::no_block) != ::fast_io::ipc_mode::none)
	{
		dwOpenMode |= 0x40000000; // FILE_FLAG_OVERLAPPED
	}

	::std::uint_least32_t dwPipeMode{};

	if ((mode & ::fast_io::ipc_mode::message) != ::fast_io::ipc_mode::none)
	{
		dwPipeMode |= 0x00000002 /*PIPE_READMODE_MESSAGE*/ | 0x00000004 /*PIPE_TYPE_MESSAGE*/;
	}

	void *handle{};

	if constexpr (family == win32_family::wide_nt)
	{
		handle = ::fast_io::win32::CreateNamedPipeW(
			temp_pipe_file_name_tlc_str.c_str(),
			dwOpenMode,
			dwPipeMode,
			static_cast<::std::uint_least32_t>(255) /*PIPE_UNLIMITED_INSTANCES */,
			static_cast<::std::uint_least32_t>(0x4000),
			static_cast<::std::uint_least32_t>(0x4000),
			0 /*default: 50ms*/,
			nullptr);
	}
	else if constexpr (family == win32_family::ansi_9x)
	{
		handle = ::fast_io::win32::CreateNamedPipeA(
			reinterpret_cast<char const *>(temp_pipe_file_name_tlc_str.c_str()),
			dwOpenMode,
			dwPipeMode,
			static_cast<::std::uint_least32_t>(255) /*PIPE_UNLIMITED_INSTANCES */,
			static_cast<::std::uint_least32_t>(0x4000),
			static_cast<::std::uint_least32_t>(0x4000),
			0 /*default: 50ms*/,
			nullptr);
	}

	if (handle == reinterpret_cast<void *>(static_cast<::std::ptrdiff_t>(-1))) [[unlikely]]
	{
		throw_win32_error();
	}

	return handle;
}

template <win32_family family>
struct win32_family_create_named_pipe_ipc_server_paramenter
{
	using family_char_type = ::std::conditional_t<family == win32_family::wide_nt, char16_t, char>;
	ipc_mode im{};
	inline void *operator()(family_char_type const *filename, ::std::size_t filename_size)
	{
		return win32_family_create_named_pipe_ipc_server_impl<family>(filename, filename_size, im);
	}
};

template <win32_family family, typename T>
	requires(::fast_io::constructible_to_os_c_str<T>)
inline void *win32_create_named_pipe_ipc_server_impl(T const &t, ipc_mode im)
{
	return ::fast_io::win32_family_api_common<family>(t, win32_family_create_named_pipe_ipc_server_paramenter<family>{im});
}

inline void win32_family_named_pipe_ipc_server_wait_for_connect_impl(void *pipe_handle)
{
	if (!::fast_io::win32::ConnectNamedPipe(pipe_handle, nullptr)) [[unlikely]]
	{
		throw_win32_error();
	}
}

inline void win32_family_named_pipe_ipc_server_disconnect_impl(void *pipe_handle)
{
	if (!::fast_io::win32::DisconnectNamedPipe(pipe_handle)) [[unlikely]]
	{
		throw_win32_error();
	}
}

// CLIENT
template <win32_family family>
inline void *win32_family_ipc_named_pipe_client_connect_impl(win32_named_pipe_char_type<family> const *server_name, ::std::size_t server_name_size, ::fast_io::ipc_mode mode)
{
	// check filename
	using family_char_type = win32_named_pipe_char_type<family>;
	using family_internal_char_type = win32_named_pipe_internal_char_type<family>;
#if 0
	using family_internal_str = win32_named_pipe_internal_str<family>;
#endif
	using family_internal_char_type_const_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= family_internal_char_type const *;
#if 0
	{
		// Prohibit including superior paths, otherwise it may cause vulnerability

		auto const beg{reinterpret_cast<family_internal_char_type_const_may_alias_ptr>(server_name)};
		auto const end{beg + server_name_size};
		auto curr{beg};

		bool is_new_dir{true};
		::std::size_t point_counter{};

		for (; curr != end; ++curr)
		{
			auto fc{*curr};
			if (fc == ::fast_io::char_literal_v<u8'.', family_internal_char_type>)
			{
				if (is_new_dir)
				{
					++point_counter;
				}
			}
			else if (fc == ::fast_io::char_literal_v<u8'/', family_internal_char_type> ||
					 fc == ::fast_io::char_literal_v<u8'\\', family_internal_char_type>)
			{
				if (point_counter == 2)
				{
					if constexpr (family == win32_family::wide_nt)
					{
						throw_win32_error(3);
					}
					else if constexpr (family == win32_family::ansi_9x)
					{
						throw_win32_error(3221225530);
					}
				}
				is_new_dir = true;
				point_counter = 0;
			}
			else
			{
				is_new_dir = false;
				point_counter = 0;
			}
		}

		if (is_new_dir && point_counter == 2)
		{
			if constexpr (family == win32_family::wide_nt)
			{
				throw_win32_error(3);
			}
			else if constexpr (family == win32_family::ansi_9x)
			{
				throw_win32_error(3221225530);
			}
		}
	}
#else
	auto const beg{reinterpret_cast<family_internal_char_type_const_may_alias_ptr>(server_name)};

	if (::fast_io::details::is_invalid_dos_filename_with_size(beg, server_name_size)) [[unlikely]]
	{
		if constexpr (family == win32_family::wide_nt)
		{
			throw_win32_error(3);
		}
		else if constexpr (family == win32_family::ansi_9x)
		{
			throw_win32_error(3221225530);
		}
	}
#endif

	using win32_named_pipe_server_internal_char_type_tlc_str = win32_named_pipe_internal_tlc_str<family>;
	win32_named_pipe_server_internal_char_type_tlc_str temp_pipe_file_name_tlc_str{};

	if constexpr (::std::same_as<win32_named_pipe_char_type<family>, char8_t>)
	{
		temp_pipe_file_name_tlc_str = concat_win32_named_pipe_internal_tlc_str<family>(u8"\\\\.\\pipe\\fast_io_ipc\\", ::fast_io::mnp::os_c_str_with_known_size(beg, server_name_size));
	}
	else if constexpr (::std ::same_as<win32_named_pipe_char_type<family>, char16_t>)
	{
		temp_pipe_file_name_tlc_str = concat_win32_named_pipe_internal_tlc_str<family>(u"\\\\.\\pipe\\fast_io_ipc\\", ::fast_io::mnp::os_c_str_with_known_size(beg, server_name_size));
	}

	::fast_io::open_mode om{};
	if ((mode & ::fast_io::ipc_mode::in) == ::fast_io::ipc_mode::in)
	{
		om |= ::fast_io::open_mode::in;
	}
	if ((mode & ::fast_io::ipc_mode::out) == ::fast_io::ipc_mode::out)
	{
		om |= ::fast_io::open_mode::out;
	}
	if ((mode & ::fast_io::ipc_mode::sync) != ::fast_io::ipc_mode::none)
	{
		om |= ::fast_io::open_mode::sync;
	}
	if ((mode & ::fast_io::ipc_mode::no_block) != ::fast_io::ipc_mode::none)
	{
		om |= ::fast_io::open_mode::no_block;
	}

	auto handle{::fast_io::details::win32_family_create_file_impl<family>(
		reinterpret_cast<family_char_type const *>(temp_pipe_file_name_tlc_str.c_str()),
		{om, static_cast<perms>(436)})};

	if (handle == reinterpret_cast<void *>(static_cast<::std::ptrdiff_t>(-1))) [[unlikely]]
	{
		throw_win32_error();
	}

	return handle;
}

template <win32_family family>
struct win32_family_create_named_pipe_ipc_client_paramenter
{
	using family_char_type = ::std::conditional_t<family == win32_family::wide_nt, char16_t, char>;
	ipc_mode im{};
	inline void *operator()(family_char_type const *filename, ::std::size_t filename_size)
	{
		return win32_family_ipc_named_pipe_client_connect_impl<family>(filename, filename_size, im);
	}
};

template <win32_family family, typename T>
	requires(::fast_io::constructible_to_os_c_str<T>)
inline void *win32_create_named_pipe_ipc_client_impl(T const &t, ipc_mode im)
{
	return ::fast_io::win32_family_api_common<family>(t, win32_family_create_named_pipe_ipc_client_paramenter<family>{im});
}

} // namespace win32::details

// SERVER
template <win32_family family, ::std::integral ch_type>
using basic_win32_family_named_pipe_ipc_server_observer = basic_win32_family_io_observer<family, ch_type>;

template <win32_family family, ::std::integral ch_type>
class basic_win32_family_named_pipe_ipc_server FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE : public basic_win32_family_named_pipe_ipc_server_observer<family, ch_type>
{
public:
	using typename basic_win32_family_named_pipe_ipc_server_observer<family, ch_type>::char_type;
	using typename basic_win32_family_named_pipe_ipc_server_observer<family, ch_type>::input_char_type;
	using typename basic_win32_family_named_pipe_ipc_server_observer<family, ch_type>::output_char_type;
	using typename basic_win32_family_named_pipe_ipc_server_observer<family, ch_type>::native_handle_type;
	using basic_win32_family_named_pipe_ipc_server_observer<family, ch_type>::native_handle;

	inline explicit constexpr basic_win32_family_named_pipe_ipc_server() noexcept = default;

	inline constexpr basic_win32_family_named_pipe_ipc_server(basic_win32_family_named_pipe_ipc_server_observer<family, ch_type>) noexcept = delete;
	inline constexpr basic_win32_family_named_pipe_ipc_server &operator=(basic_win32_family_named_pipe_ipc_server_observer<family, ch_type>) noexcept = delete;

	inline basic_win32_family_named_pipe_ipc_server(basic_win32_family_named_pipe_ipc_server const &other)
		: basic_win32_family_named_pipe_ipc_server_observer<family, ch_type>{::fast_io::win32::details::win32_dup_impl(other.handle)}
	{
	}
	inline basic_win32_family_named_pipe_ipc_server &operator=(basic_win32_family_named_pipe_ipc_server const &other)
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		this->handle = ::fast_io::win32::details::win32_dup2_impl(other.handle, this->handle);
		return *this;
	}
	inline basic_win32_family_named_pipe_ipc_server(basic_win32_family_named_pipe_ipc_server &&__restrict b) noexcept
		: basic_win32_family_named_pipe_ipc_server_observer<family, ch_type>{b.release()}
	{
	}
	inline basic_win32_family_named_pipe_ipc_server &operator=(basic_win32_family_named_pipe_ipc_server &&__restrict b) noexcept
	{
		if (__builtin_addressof(b) == this) [[unlikely]]
		{
			return *this;
		}
		if (*this) [[likely]]
		{
			::fast_io::win32::CloseHandle(this->handle);
		}
		this->handle = b.handle;
		b.handle = nullptr;
		return *this;
	}
	inline void reset(native_handle_type newhandle = {}) noexcept
	{
		if (*this) [[likely]]
		{
			::fast_io::win32::CloseHandle(this->handle);
		}
		this->handle = newhandle;
	}
	inline void close()
	{
		if (*this) [[likely]]
		{
			auto error{::fast_io::win32::CloseHandle(this->handle)};
			this->handle = nullptr; // POSIX standard says we should never call close(2) again even close syscall fails
			if (!error) [[unlikely]]
			{
				throw_win32_error();
			}
		}
	}

	template <typename native_hd>
		requires ::std::same_as<native_handle_type, ::std::remove_cvref_t<native_hd>>
	inline explicit constexpr basic_win32_family_named_pipe_ipc_server(native_hd handle1) noexcept
		: basic_win32_family_named_pipe_ipc_server_observer<family, ch_type>{handle1}
	{
	}

	inline basic_win32_family_named_pipe_ipc_server(io_dup_t, basic_win32_family_named_pipe_ipc_server_observer<family, ch_type> wiob)
		: basic_win32_family_named_pipe_ipc_server_observer<family, ch_type>{::fast_io::win32::details::win32_dup_impl(wiob.handle)}
	{
	}

	template <::fast_io::constructible_to_os_c_str T>
	inline explicit basic_win32_family_named_pipe_ipc_server(T const &server_name, ipc_mode im)
		: basic_win32_family_named_pipe_ipc_server_observer<family, char_type>{
			  ::fast_io::win32::details::win32_create_named_pipe_ipc_server_impl<family>(server_name, im)}
	{
	}

	inline ~basic_win32_family_named_pipe_ipc_server()
	{
		if (*this) [[likely]]
		{
			::fast_io::win32::CloseHandle(this->handle);
		}
	}
};

// CLIENT
template <win32_family family, ::std::integral ch_type>
using basic_win32_family_named_pipe_ipc_client_observer = basic_win32_family_io_observer<family, ch_type>;

template <win32_family family, ::std::integral ch_type>
class basic_win32_family_named_pipe_ipc_client FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE : public basic_win32_family_named_pipe_ipc_client_observer<family, ch_type>
{
public:
	using typename basic_win32_family_named_pipe_ipc_client_observer<family, ch_type>::char_type;
	using typename basic_win32_family_named_pipe_ipc_client_observer<family, ch_type>::input_char_type;
	using typename basic_win32_family_named_pipe_ipc_client_observer<family, ch_type>::output_char_type;
	using typename basic_win32_family_named_pipe_ipc_client_observer<family, ch_type>::native_handle_type;
	using basic_win32_family_named_pipe_ipc_client_observer<family, ch_type>::native_handle;

	inline explicit constexpr basic_win32_family_named_pipe_ipc_client() noexcept = default;

	inline constexpr basic_win32_family_named_pipe_ipc_client(basic_win32_family_named_pipe_ipc_client_observer<family, ch_type>) noexcept = delete;
	inline constexpr basic_win32_family_named_pipe_ipc_client &operator=(basic_win32_family_named_pipe_ipc_client_observer<family, ch_type>) noexcept = delete;

	inline basic_win32_family_named_pipe_ipc_client(basic_win32_family_named_pipe_ipc_client const &other)
		: basic_win32_family_named_pipe_ipc_client_observer<family, ch_type>{::fast_io::win32::details::win32_dup_impl(other.handle)}
	{
	}
	inline basic_win32_family_named_pipe_ipc_client &operator=(basic_win32_family_named_pipe_ipc_client const &other)
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		this->handle = ::fast_io::win32::details::win32_dup2_impl(other.handle, this->handle);
		return *this;
	}
	inline basic_win32_family_named_pipe_ipc_client(basic_win32_family_named_pipe_ipc_client &&__restrict b) noexcept
		: basic_win32_family_named_pipe_ipc_client_observer<family, ch_type>{b.release()}
	{
	}
	inline basic_win32_family_named_pipe_ipc_client &operator=(basic_win32_family_named_pipe_ipc_client &&__restrict b) noexcept
	{
		if (__builtin_addressof(b) == this) [[unlikely]]
		{
			return *this;
		}
		if (*this) [[likely]]
		{
			::fast_io::win32::CloseHandle(this->handle);
		}
		this->handle = b.handle;
		b.handle = nullptr;
		return *this;
	}
	inline void reset(native_handle_type newhandle = {}) noexcept
	{
		if (*this) [[likely]]
		{
			::fast_io::win32::CloseHandle(this->handle);
		}
		this->handle = newhandle;
	}
	inline void close()
	{
		if (*this) [[likely]]
		{
			auto error{::fast_io::win32::CloseHandle(this->handle)};
			this->handle = nullptr; // POSIX standard says we should never call close(2) again even close syscall fails
			if (!error) [[unlikely]]
			{
				throw_win32_error();
			}
		}
	}

	template <typename native_hd>
		requires ::std::same_as<native_handle_type, ::std::remove_cvref_t<native_hd>>
	inline explicit constexpr basic_win32_family_named_pipe_ipc_client(native_hd handle1) noexcept
		: basic_win32_family_named_pipe_ipc_client_observer<family, ch_type>{handle1}
	{
	}

	inline basic_win32_family_named_pipe_ipc_client(io_dup_t, basic_win32_family_named_pipe_ipc_client_observer<family, ch_type> wiob)
		: basic_win32_family_named_pipe_ipc_client_observer<family, ch_type>{::fast_io::win32::details::win32_dup_impl(wiob.handle)}
	{
	}

	template <::fast_io::constructible_to_os_c_str T>
	inline explicit basic_win32_family_named_pipe_ipc_client(T const &client_name, ipc_mode im)
		: basic_win32_family_named_pipe_ipc_client_observer<family, char_type>{
			  ::fast_io::win32::details::win32_create_named_pipe_ipc_client_impl<family>(client_name, im)}
	{
	}

	inline ~basic_win32_family_named_pipe_ipc_client()
	{
		if (*this) [[likely]]
		{
			::fast_io::win32::CloseHandle(this->handle);
		}
	}
};

template <win32_family server_family, ::std::integral server_ch_type, win32_family client_family = win32_family::native, ::std::integral client_ch_type = char>
inline basic_win32_family_named_pipe_ipc_client<client_family, client_ch_type> wait_for_connect(basic_win32_family_named_pipe_ipc_server_observer<server_family, server_ch_type> server)
{
	win32::details::win32_family_named_pipe_ipc_server_wait_for_connect_impl(server.handle);
	return basic_win32_family_named_pipe_ipc_client<client_family, client_ch_type>{};
}

template <win32_family server_family, ::std::integral server_ch_type, win32_family client_family = win32_family::native, ::std::integral client_ch_type = char>
inline void accept_connect(
	basic_win32_family_named_pipe_ipc_server_observer<server_family, server_ch_type>,
	basic_win32_family_named_pipe_ipc_client_observer<client_family, client_ch_type>,
	bool)
{
	// do nothing
}

template <win32_family server_family, ::std::integral server_ch_type>
inline void disconnect(basic_win32_family_named_pipe_ipc_server_observer<server_family, server_ch_type> server) noexcept
{
	win32::details::win32_family_named_pipe_ipc_server_disconnect_impl(server.handle);
}

// USING
template <::std::integral ch_type>
using basic_win32_named_pipe_ipc_server_observer = basic_win32_family_named_pipe_ipc_server_observer<win32_family::native, ch_type>;
template <::std::integral ch_type>
using basic_win32_named_pipe_ipc_server_observer_ntw = basic_win32_family_named_pipe_ipc_server_observer<win32_family::wide_nt, ch_type>;
template <::std::integral ch_type>
using basic_win32_named_pipe_ipc_server_observer_9xa = basic_win32_family_named_pipe_ipc_server_observer<win32_family::ansi_9x, ch_type>;

using win32_named_pipe_ipc_server_observer = basic_win32_named_pipe_ipc_server_observer<char>;
using win32_named_pipe_ipc_server_observer_ntw = basic_win32_named_pipe_ipc_server_observer_ntw<char>;
using win32_named_pipe_ipc_server_observer_9xa = basic_win32_named_pipe_ipc_server_observer_9xa<char>;

using wwin32_named_pipe_ipc_server_observer = basic_win32_named_pipe_ipc_server_observer<wchar_t>;
using wwin32_named_pipe_ipc_server_observer_ntw = basic_win32_named_pipe_ipc_server_observer_ntw<wchar_t>;
using wwin32_named_pipe_ipc_server_observer_9xa = basic_win32_named_pipe_ipc_server_observer_9xa<wchar_t>;

using u8win32_named_pipe_ipc_server_observer = basic_win32_named_pipe_ipc_server_observer<char8_t>;
using u8win32_named_pipe_ipc_server_observer_ntw = basic_win32_named_pipe_ipc_server_observer_ntw<char8_t>;
using u8win32_named_pipe_ipc_server_observer_9xa = basic_win32_named_pipe_ipc_server_observer_9xa<char8_t>;

using u16win32_named_pipe_ipc_server_observer = basic_win32_named_pipe_ipc_server_observer<char16_t>;
using u16win32_named_pipe_ipc_server_observer_ntw = basic_win32_named_pipe_ipc_server_observer_ntw<char16_t>;
using u16win32_named_pipe_ipc_server_observer_9xa = basic_win32_named_pipe_ipc_server_observer_9xa<char16_t>;

using u32win32_named_pipe_ipc_server_observer = basic_win32_named_pipe_ipc_server_observer<char32_t>;
using u32win32_named_pipe_ipc_server_observer_ntw = basic_win32_named_pipe_ipc_server_observer_ntw<char32_t>;
using u32win32_named_pipe_ipc_server_observer_9xa = basic_win32_named_pipe_ipc_server_observer_9xa<char32_t>;

template <::std::integral ch_type>
using basic_win32_named_pipe_ipc_server = basic_win32_family_named_pipe_ipc_server<win32_family::native, ch_type>;
template <::std::integral ch_type>
using basic_win32_named_pipe_ipc_server_ntw = basic_win32_family_named_pipe_ipc_server<win32_family::wide_nt, ch_type>;
template <::std::integral ch_type>
using basic_win32_named_pipe_ipc_server_9xa = basic_win32_family_named_pipe_ipc_server<win32_family::ansi_9x, ch_type>;

using win32_named_pipe_ipc_server = basic_win32_named_pipe_ipc_server<char>;
using win32_named_pipe_ipc_server_ntw = basic_win32_named_pipe_ipc_server_ntw<char>;
using win32_named_pipe_ipc_server_9xa = basic_win32_named_pipe_ipc_server_9xa<char>;

using wwin32_named_pipe_ipc_server = basic_win32_named_pipe_ipc_server<wchar_t>;
using wwin32_named_pipe_ipc_server_ntw = basic_win32_named_pipe_ipc_server_ntw<wchar_t>;
using wwin32_named_pipe_ipc_server_9xa = basic_win32_named_pipe_ipc_server_9xa<wchar_t>;

using u8win32_named_pipe_ipc_server = basic_win32_named_pipe_ipc_server<char8_t>;
using u8win32_named_pipe_ipc_server_ntw = basic_win32_named_pipe_ipc_server_ntw<char8_t>;
using u8win32_named_pipe_ipc_server_9xa = basic_win32_named_pipe_ipc_server_9xa<char8_t>;

using u16win32_named_pipe_ipc_server = basic_win32_named_pipe_ipc_server<char16_t>;
using u16win32_named_pipe_ipc_server_ntw = basic_win32_named_pipe_ipc_server_ntw<char16_t>;
using u16win32_named_pipe_ipc_server_9xa = basic_win32_named_pipe_ipc_server_9xa<char16_t>;

using u32win32_named_pipe_ipc_server = basic_win32_named_pipe_ipc_server<char32_t>;
using u32win32_named_pipe_ipc_server_ntw = basic_win32_named_pipe_ipc_server_ntw<char32_t>;
using u32win32_named_pipe_ipc_server_9xa = basic_win32_named_pipe_ipc_server_9xa<char32_t>;

template <::std::integral ch_type>
using basic_win32_named_pipe_ipc_client_observer = basic_win32_family_named_pipe_ipc_client_observer<win32_family::native, ch_type>;
template <::std::integral ch_type>
using basic_win32_named_pipe_ipc_client_observer_ntw = basic_win32_family_named_pipe_ipc_client_observer<win32_family::wide_nt, ch_type>;
template <::std::integral ch_type>
using basic_win32_named_pipe_ipc_client_observer_9xa = basic_win32_family_named_pipe_ipc_client_observer<win32_family::ansi_9x, ch_type>;

using win32_named_pipe_ipc_client_observer = basic_win32_named_pipe_ipc_client_observer<char>;
using win32_named_pipe_ipc_client_observer_ntw = basic_win32_named_pipe_ipc_client_observer_ntw<char>;
using win32_named_pipe_ipc_client_observer_9xa = basic_win32_named_pipe_ipc_client_observer_9xa<char>;

using wwin32_named_pipe_ipc_client_observer = basic_win32_named_pipe_ipc_client_observer<wchar_t>;
using wwin32_named_pipe_ipc_client_observer_ntw = basic_win32_named_pipe_ipc_client_observer_ntw<wchar_t>;
using wwin32_named_pipe_ipc_client_observer_9xa = basic_win32_named_pipe_ipc_client_observer_9xa<wchar_t>;

using u8win32_named_pipe_ipc_client_observer = basic_win32_named_pipe_ipc_client_observer<char8_t>;
using u8win32_named_pipe_ipc_client_observer_ntw = basic_win32_named_pipe_ipc_client_observer_ntw<char8_t>;
using u8win32_named_pipe_ipc_client_observer_9xa = basic_win32_named_pipe_ipc_client_observer_9xa<char8_t>;

using u16win32_named_pipe_ipc_client_observer = basic_win32_named_pipe_ipc_client_observer<char16_t>;
using u16win32_named_pipe_ipc_client_observer_ntw = basic_win32_named_pipe_ipc_client_observer_ntw<char16_t>;
using u16win32_named_pipe_ipc_client_observer_9xa = basic_win32_named_pipe_ipc_client_observer_9xa<char16_t>;

using u32win32_named_pipe_ipc_client_observer = basic_win32_named_pipe_ipc_client_observer<char32_t>;
using u32win32_named_pipe_ipc_client_observer_ntw = basic_win32_named_pipe_ipc_client_observer_ntw<char32_t>;
using u32win32_named_pipe_ipc_client_observer_9xa = basic_win32_named_pipe_ipc_client_observer_9xa<char32_t>;

template <::std::integral ch_type>
using basic_win32_named_pipe_ipc_client = basic_win32_family_named_pipe_ipc_client<win32_family::native, ch_type>;
template <::std::integral ch_type>
using basic_win32_named_pipe_ipc_client_ntw = basic_win32_family_named_pipe_ipc_client<win32_family::wide_nt, ch_type>;
template <::std::integral ch_type>
using basic_win32_named_pipe_ipc_client_9xa = basic_win32_family_named_pipe_ipc_client<win32_family::ansi_9x, ch_type>;

using win32_named_pipe_ipc_client = basic_win32_named_pipe_ipc_client<char>;
using win32_named_pipe_ipc_client_ntw = basic_win32_named_pipe_ipc_client_ntw<char>;
using win32_named_pipe_ipc_client_9xa = basic_win32_named_pipe_ipc_client_9xa<char>;

using wwin32_named_pipe_ipc_client = basic_win32_named_pipe_ipc_client<wchar_t>;
using wwin32_named_pipe_ipc_client_ntw = basic_win32_named_pipe_ipc_client_ntw<wchar_t>;
using wwin32_named_pipe_ipc_client_9xa = basic_win32_named_pipe_ipc_client_9xa<wchar_t>;

using u8win32_named_pipe_ipc_client = basic_win32_named_pipe_ipc_client<char8_t>;
using u8win32_named_pipe_ipc_client_ntw = basic_win32_named_pipe_ipc_client_ntw<char8_t>;
using u8win32_named_pipe_ipc_client_9xa = basic_win32_named_pipe_ipc_client_9xa<char8_t>;

using u16win32_named_pipe_ipc_client = basic_win32_named_pipe_ipc_client<char16_t>;
using u16win32_named_pipe_ipc_client_ntw = basic_win32_named_pipe_ipc_client_ntw<char16_t>;
using u16win32_named_pipe_ipc_client_9xa = basic_win32_named_pipe_ipc_client_9xa<char16_t>;

using u32win32_named_pipe_ipc_client = basic_win32_named_pipe_ipc_client<char32_t>;
using u32win32_named_pipe_ipc_client_ntw = basic_win32_named_pipe_ipc_client_ntw<char32_t>;
using u32win32_named_pipe_ipc_client_9xa = basic_win32_named_pipe_ipc_client_9xa<char32_t>;

namespace freestanding
{
template <win32_family fm, ::std::integral char_type>
struct is_trivially_copyable_or_relocatable<basic_win32_family_named_pipe_ipc_server<fm, char_type>>
{
	inline static constexpr bool value = true;
};

template <win32_family fm, ::std::integral char_type>
struct is_zero_default_constructible<basic_win32_family_named_pipe_ipc_server<fm, char_type>>
{
	inline static constexpr bool value = true;
};

template <win32_family fm, ::std::integral char_type>
struct is_trivially_copyable_or_relocatable<basic_win32_family_named_pipe_ipc_client<fm, char_type>>
{
	inline static constexpr bool value = true;
};

template <win32_family fm, ::std::integral char_type>
struct is_zero_default_constructible<basic_win32_family_named_pipe_ipc_client<fm, char_type>>
{
	inline static constexpr bool value = true;
};

} // namespace freestanding

} // namespace fast_io
