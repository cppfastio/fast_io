#pragma once
namespace fast_io
{
struct default_args_t
{
	explicit constexpr default_args_t() noexcept = default;
};
inline constexpr default_args_t default_args{};

#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)

namespace details
{
template <::std::integral replace_char_type, typename T>
inline constexpr void construct_win32_process_args_decay_singal(
	::fast_io::containers::basic_string<replace_char_type, ::fast_io::native_global_allocator> &str, T t)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<replace_char_type>, T>};

	if constexpr (type_error)
	{
		replace_char_type buf[32767];
		::fast_io::basic_obuffer_view<replace_char_type> obf{buf, buf + 32767};
		::fast_io::operations::decay::print_freestanding_decay<false>(::fast_io::operations::output_stream_ref(obf), t);
		str.reserve(str.size() + 3 + obf.size() * 2);
		str.push_back_unchecked(::fast_io::char_literal_v<u8'\"', replace_char_type>);
		for (auto const c : obf)
		{
			if (c == ::fast_io::char_literal_v<u8'\"', replace_char_type>)
			{
				str.push_back_unchecked(::fast_io::char_literal_v<u8'\\', replace_char_type>);
				str.push_back_unchecked(::fast_io::char_literal_v<u8'\"', replace_char_type>);
			}
			else
			{
				str.push_back_unchecked(c);
			}
		}
		str.push_back_unchecked(::fast_io::char_literal_v<u8'\"', replace_char_type>);
		str.push_back_unchecked(::fast_io::char_literal_v<u8' ', replace_char_type>);
	}
	else if constexpr (requires { ::fast_io::mnp::code_cvt(t); })
	{
		replace_char_type buf[32767];
		::fast_io::basic_obuffer_view<replace_char_type> obf{buf, buf + 32767};
		// need decay
		::fast_io::operations::print_freestanding<false>(obf, ::fast_io::mnp::code_cvt(t));
		str.reserve(str.size() + 3 + obf.size() * 2);
		str.push_back_unchecked(::fast_io::char_literal_v<u8'\"', replace_char_type>);
		for (auto const c : obf)
		{
			if (c == ::fast_io::char_literal_v<u8'\"', replace_char_type>)
			{
				str.push_back_unchecked(::fast_io::char_literal_v<u8'\\', replace_char_type>);
				str.push_back_unchecked(::fast_io::char_literal_v<u8'\"', replace_char_type>);
			}
			else
			{
				str.push_back_unchecked(c);
			}
		}
		str.push_back_unchecked(::fast_io::char_literal_v<u8'\"', replace_char_type>);
		str.push_back_unchecked(::fast_io::char_literal_v<u8' ', replace_char_type>);
	}
	else
	{
		static_assert(type_error, "some types are not printable or codecvt printable, so we cannot construct win32_process_args");
	}
}

template <::std::integral replace_char_type, typename T, typename... Args>
inline constexpr void construct_win32_process_args_decay(
	::fast_io::containers::basic_string<replace_char_type, ::fast_io::native_global_allocator> &str, T t, Args... args)
{
	construct_win32_process_args_decay_singal(str, t);

	if constexpr (sizeof...(Args) != 0)
	{
		construct_win32_process_args_decay(str, args...);
	}
}

template <::std::integral replace_char_type, typename T>
inline constexpr void construct_win32_process_envs_decay_singal(
	::fast_io::containers::basic_string<replace_char_type, ::fast_io::native_global_allocator> &str, T t)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<replace_char_type>, T>};

	if constexpr (type_error)
	{
		::fast_io::io_strlike_reference_wrapper<replace_char_type, ::fast_io::containers::basic_string<replace_char_type, ::fast_io::native_global_allocator>> wrapper{__builtin_addressof(str)};
		::fast_io::operations::print_freestanding<false>(wrapper, t, ::fast_io::mnp::chvw(::fast_io::char_literal_v<u8'\0', replace_char_type>));
	}
	else if constexpr (requires { ::fast_io::mnp::code_cvt(t); })
	{
		::fast_io::io_strlike_reference_wrapper<replace_char_type, ::fast_io::containers::basic_string<replace_char_type, ::fast_io::native_global_allocator>> wrapper{__builtin_addressof(str)};
		::fast_io::operations::print_freestanding<false>(wrapper, ::fast_io::mnp::code_cvt(t), ::fast_io::mnp::chvw(::fast_io::char_literal_v<u8'\0', replace_char_type>));
	}
	else
	{
		static_assert(type_error, "some types are not printable or codecvt printable, so we cannot construct win32_process_envs");
	}
}

template <::std::integral replace_char_type, typename T, typename... Args>
inline constexpr void construct_win32_process_envs_decay(
	::fast_io::containers::basic_string<replace_char_type, ::fast_io::native_global_allocator> &str, T t, Args... args)
{
	construct_win32_process_envs_decay_singal(str, t);

	if constexpr (sizeof...(Args) != 0)
	{
		construct_win32_process_envs_decay(str, args...);
	}

	// no need to push_back last '\0', str end with '\0'
}

} // namespace details

template <::fast_io::win32_family family>
struct win32_process_args
{
	inline static constexpr bool is_nt{family == ::fast_io::win32_family::wide_nt};
	using char_type = ::std::conditional_t<is_nt, char16_t, char>;
	using replace_char_type = ::std::conditional_t<is_nt, char16_t, char8_t>;
	using storage_type = ::fast_io::containers::basic_string<replace_char_type, ::fast_io::native_global_allocator>;
	using char_type_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char_type *;
	using char_type_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char_type const *;

	storage_type args{};

	inline constexpr win32_process_args() noexcept = default;

	inline constexpr win32_process_args(default_args_t, char_type const *oscstr) noexcept
	{
		args = storage_type{::fast_io::mnp::os_c_str(oscstr)};
	}

	template <typename T, typename... Args>
		requires(!::std::same_as<::std::remove_cvref_t<T>, default_args_t>)
	inline constexpr win32_process_args(T &&t, Args &&...as)
	{
		details::construct_win32_process_args_decay(args,
													::fast_io::io_print_forward<replace_char_type>(::fast_io::io_print_alias(t)),
													::fast_io::io_print_forward<replace_char_type>(::fast_io::io_print_alias(as))...);
	}

	inline constexpr char_type_may_alias_const_ptr get() const noexcept
	{
		if (args.empty())
		{
			return nullptr;
		}
		else
		{
			return reinterpret_cast<char_type_may_alias_const_ptr>(args.c_str());
		}
	}
};

template <::fast_io::win32_family family>
struct win32_process_envs
{
	inline static constexpr bool is_nt{family == ::fast_io::win32_family::wide_nt};
	using char_type = ::std::conditional_t<is_nt, char16_t, char>;
	using replace_char_type = ::std::conditional_t<is_nt, char16_t, char8_t>;
	using storage_type = ::fast_io::containers::basic_string<replace_char_type, ::fast_io::native_global_allocator>;
	using char_type_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char_type *;
	using char_type_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char_type const *;

	storage_type envs{};

	inline constexpr win32_process_envs() noexcept = default;

	inline constexpr win32_process_envs(default_args_t, char_type const *oscstr) noexcept
	{
		auto estr{oscstr};
		for (; *estr || estr[1]; ++estr)
		{
		}
		estr += 2; // "\0\0"
		envs = storage_type{oscstr, estr};
	}

	template <typename T, typename... Args>
		requires(!::std::same_as<::std::remove_cvref_t<T>, default_args_t>)
	inline constexpr win32_process_envs(T &&t, Args &&...as)
	{
		details::construct_win32_process_envs_decay(envs,
													::fast_io::io_print_forward<replace_char_type>(::fast_io::io_print_alias(t)),
													::fast_io::io_print_forward<replace_char_type>(::fast_io::io_print_alias(as))...);
	}

	inline constexpr char_type_may_alias_const_ptr get() const noexcept
	{
		if (envs.empty())
		{
			return nullptr;
		}
		else
		{
			return reinterpret_cast<char_type_may_alias_const_ptr>(envs.c_str());
		}
	}
};

using nt_process_args = ::fast_io::win32_process_args<::fast_io::win32_family::wide_nt>;
using nt_process_envs = ::fast_io::win32_process_envs<::fast_io::win32_family::wide_nt>;

#else

// posix
namespace details
{
template <::std::size_t N, ::std::integral replace_char_type, typename T>
inline constexpr void construct_posix_process_argenvs_decay_singal(
	::fast_io::containers::basic_string<replace_char_type, ::fast_io::native_global_allocator> &str, T t)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<replace_char_type>, T>};

	using ptrdiff_t_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= ::std::ptrdiff_t *;

	auto sz{static_cast<::std::ptrdiff_t>(str.size())};
	::fast_io::freestanding::my_memcpy(reinterpret_cast<ptrdiff_t_may_alias_ptr>(str.begin()) + N, __builtin_addressof(sz), sizeof(sz));

	if constexpr (type_error)
	{
		::fast_io::io_strlike_reference_wrapper<replace_char_type, ::fast_io::containers::basic_string<replace_char_type, ::fast_io::native_global_allocator>> wrapper{__builtin_addressof(str)};
		::fast_io::operations::print_freestanding<false>(wrapper, t, ::fast_io::mnp::chvw(::fast_io::char_literal_v<u8'\0', replace_char_type>));
	}
	else if constexpr (requires { ::fast_io::mnp::code_cvt(t); })
	{
		::fast_io::io_strlike_reference_wrapper<replace_char_type, ::fast_io::containers::basic_string<replace_char_type, ::fast_io::native_global_allocator>> wrapper{__builtin_addressof(str)};
		::fast_io::operations::print_freestanding<false>(wrapper, ::fast_io::mnp::code_cvt(t), ::fast_io::mnp::chvw(::fast_io::char_literal_v<u8'\0', replace_char_type>));
	}
	else
	{
		static_assert(type_error, "some types are not printable or codecvt printable, so we cannot construct posix_process_envs");
	}
}

template <::std::size_t N = 0, ::std::integral replace_char_type, typename T, typename... Args>
inline constexpr void construct_posix_process_argenvs_decay(
	::fast_io::containers::basic_string<replace_char_type, ::fast_io::native_global_allocator> &str, T t, Args... args)
{
	construct_posix_process_argenvs_decay_singal<N>(str, t);

	if constexpr (sizeof...(Args) != 0)
	{
		construct_posix_process_argenvs_decay<N + 1>(str, args...);
	}
}
} // namespace details

struct posix_process_args
{
	using char_type = char;
	using replace_char_type = char;
	using storage_type = ::fast_io::containers::basic_string<replace_char_type, ::fast_io::native_global_allocator>;
	using char_type_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char_type *;
	using char_type_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char_type const *;

	storage_type arg_envs{};

	inline constexpr posix_process_args() noexcept = default;

	inline constexpr posix_process_args(default_args_t, char const *const *args) noexcept
	{
		arg_envs = storage_type{sizeof(args) / sizeof(char_type)};
		::fast_io::freestanding::my_memcpy(arg_envs.begin(), __builtin_addressof(args), sizeof(args));
	}

	template <typename T, typename... Args>
		requires(!::std::same_as<::std::remove_cvref_t<T>, default_args_t>)
	inline constexpr posix_process_args(T &&t, Args &&...as)
	{
		auto sizeofptr{sizeof...(Args) + 2}; // T, Args..., nullptr
		arg_envs = ::fast_io::containers::basic_string<replace_char_type, ::fast_io::native_global_allocator>{sizeofptr * sizeof(::std::ptrdiff_t) / sizeof(replace_char_type)}; // calloc

		details::construct_posix_process_argenvs_decay(arg_envs,
													   ::fast_io::io_print_forward<replace_char_type>(::fast_io::io_print_alias(t)),
													   ::fast_io::io_print_forward<replace_char_type>(::fast_io::io_print_alias(as))...);
		
		using ptrdiff_t_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= ::std::ptrdiff_t *;

		auto const begin{reinterpret_cast<ptrdiff_t_may_alias_ptr>(arg_envs.begin())};
		auto const end{begin + sizeof...(Args) + 1};

		for (auto curr{begin}; curr != end; ++curr)
		{
			::std::ptrdiff_t sz{};
			::fast_io::freestanding::my_memcpy(__builtin_addressof(sz), curr, sizeof(sz));
			auto const ptr{arg_envs.cbegin() + sz};
			::fast_io::freestanding::my_memcpy(curr, __builtin_addressof(ptr), sizeof(ptr));
		}

	}

	inline constexpr char const *const *get() const noexcept
	{
		if (auto const szb{arg_envs.size_bytes()}; szb == 0)
		{
			return nullptr;
		}
		else if (szb == sizeof(::std::ptrdiff_t))
		{
			char const *const *ptr;
			::fast_io::freestanding::my_memcpy(__builtin_addressof(ptr), arg_envs.cbegin(), sizeof(ptr));
			return ptr;
		}
		else
		{
			return (char const *const *)(arg_envs.c_str());
		}
	}
};

using posix_process_envs = posix_process_args;

#endif
} // namespace fast_io
