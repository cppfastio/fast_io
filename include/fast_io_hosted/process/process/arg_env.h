#pragma once
namespace fast_io
{
struct default_args_t
{
	inline explicit constexpr default_args_t() noexcept = default;
};
inline constexpr default_args_t default_args{};

#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)

namespace details
{
template <::std::integral replace_char_type, typename T>
inline constexpr void construct_win32_process_args_decay_singal(bool is_first, ::fast_io::containers::basic_string<replace_char_type, ::fast_io::native_global_allocator> &str, T t)
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
				if (is_first) [[unlikely]]
				{
					// The first argument of windows does not support double quotes
					throw_win32_error(13);
				}
				else
				{
					str.push_back_unchecked(::fast_io::char_literal_v<u8'\\', replace_char_type>);
					str.push_back_unchecked(::fast_io::char_literal_v<u8'\"', replace_char_type>);
				}
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
				if (is_first) [[unlikely]]
				{
					// The first argument of windows does not support double quotes
					throw_win32_error(13);
				}
				else
				{
					str.push_back_unchecked(::fast_io::char_literal_v<u8'\\', replace_char_type>);
					str.push_back_unchecked(::fast_io::char_literal_v<u8'\"', replace_char_type>);
				}
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
		static_assert(type_error, "some types are not printable or codecvt printable, so we cannot construct basic_win32_process_args");
	}
}

template <bool is_first, ::std::integral replace_char_type, typename T, typename... Args>
inline constexpr void construct_win32_process_args_decay(
	::fast_io::containers::basic_string<replace_char_type, ::fast_io::native_global_allocator> &str, T t, Args... args)
{
	construct_win32_process_args_decay_singal(is_first, str, t);

	if constexpr (sizeof...(Args) != 0)
	{
		construct_win32_process_args_decay<false>(str, args...);
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
		static_assert(type_error, "some types are not printable or codecvt printable, so we cannot construct basic_win32_process_envs");
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
struct basic_win32_process_args FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
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

	inline constexpr basic_win32_process_args() noexcept = default;

	inline constexpr basic_win32_process_args(default_args_t, char_type const *oscstr) noexcept
	{
		args = storage_type{::fast_io::mnp::os_c_str(oscstr)};
	}

	template <typename T, typename... Args>
		requires(!::std::same_as<::std::remove_cvref_t<T>, default_args_t>)
	inline constexpr basic_win32_process_args(T &&t, Args &&...as)
	{
		details::construct_win32_process_args_decay<true>(args,
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

	inline constexpr void append(basic_win32_process_args const &others) noexcept
	{
		args.append(others.args);
	}
};

template <::fast_io::win32_family family>
struct basic_win32_process_envs FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
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

	inline constexpr basic_win32_process_envs() noexcept = default;

	inline constexpr basic_win32_process_envs(default_args_t, char_type const *oscstr) noexcept
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
	inline constexpr basic_win32_process_envs(T &&t, Args &&...as)
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

	inline constexpr void append(basic_win32_process_envs const &others) noexcept
	{
		envs.append(others.envs);
	}
};

using win32_process_args_ntw = ::fast_io::basic_win32_process_args<::fast_io::win32_family::wide_nt>;
using win32_process_envs_ntw = ::fast_io::basic_win32_process_envs<::fast_io::win32_family::wide_nt>;

using win32_process_args_9xa = ::fast_io::basic_win32_process_args<::fast_io::win32_family::ansi_9x>;
using win32_process_envs_9xa = ::fast_io::basic_win32_process_envs<::fast_io::win32_family::ansi_9x>;

using win32_process_args = ::fast_io::basic_win32_process_args<::fast_io::win32_family::native>;
using win32_process_envs = ::fast_io::basic_win32_process_envs<::fast_io::win32_family::native>;

using nt_process_args = ::fast_io::basic_win32_process_args<::fast_io::win32_family::wide_nt>;
using nt_process_envs = ::fast_io::basic_win32_process_envs<::fast_io::win32_family::wide_nt>;

#else

// posix
namespace details
{

template <::std::integral char_type>
struct cstr_guard FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
{
	using Alloc = ::fast_io::native_typed_global_allocator<char_type>;

	char_type *cstr;

	inline constexpr cstr_guard() noexcept = default;

	inline constexpr cstr_guard(cstr_guard const &others) noexcept
	{
		::std::size_t str_size{::fast_io::cstr_len(others.cstr)};
		cstr = Alloc::allocate(str_size + 1);
		auto const lase_ptr{::fast_io::freestanding::non_overlapped_copy_n(others.cstr, str_size, cstr)};
		*lase_ptr = ::fast_io::char_literal_v<u8'\0', char_type>;
	}

	inline constexpr cstr_guard &operator=(cstr_guard const &others) noexcept
	{
		if (__builtin_addressof(others) == this) [[unlikely]]
		{
			return *this;
		}

		Alloc::deallocate(cstr);

		::std::size_t str_size{::fast_io::cstr_len(others.cstr)};
		cstr = Alloc::allocate(str_size + 1);
		auto const lase_ptr{::fast_io::freestanding::non_overlapped_copy_n(others.cstr, str_size, cstr)};
		*lase_ptr = ::fast_io::char_literal_v<u8'\0', char_type>;
	}

	inline constexpr cstr_guard(cstr_guard &&others) noexcept
	{
		cstr = others.cstr;
		others.cstr = nullptr;
	}

	inline constexpr cstr_guard &operator=(cstr_guard &&others) noexcept
	{
		if (__builtin_addressof(others) == this) [[unlikely]]
		{
			return *this;
		}

		Alloc::deallocate(cstr);

		cstr = others.cstr;
		others.cstr = nullptr;
	}

	inline constexpr ~cstr_guard()
	{
		Alloc::deallocate(cstr);
	}
};

} // namespace details

namespace freestanding
{
template <::std::integral char_type>
struct is_trivially_copyable_or_relocatable<details::cstr_guard<char_type>>
{
	inline static constexpr bool value = true;
};

template <::std::integral char_type>
struct is_zero_default_constructible<details::cstr_guard<char_type>>
{
	inline static constexpr bool value = true;
};

} // namespace freestanding

namespace details
{
template <::std::size_t N, ::std::integral replace_char_type, typename T>
inline constexpr void construct_posix_process_argenvs_decay_singal(
	::fast_io::containers::vector<cstr_guard<replace_char_type>, ::fast_io::native_global_allocator> &str, T t)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<replace_char_type>, T>};

	cstr_guard<replace_char_type> cstrg;

	if constexpr (type_error)
	{
		auto cstr{::fast_io::basic_general_concat<false, replace_char_type, ::fast_io::containers::basic_string<replace_char_type, ::fast_io::native_global_allocator>>(t)};
		cstrg.cstr = cstr.imp.begin_ptr;
		cstr.imp = {};
		str.push_back(::std::move(cstrg));
	}
	else if constexpr (requires { ::fast_io::mnp::code_cvt(t); })
	{
		auto cstr{::fast_io::basic_general_concat<false, replace_char_type, ::fast_io::containers::basic_string<replace_char_type, ::fast_io::native_global_allocator>>(::fast_io::mnp::code_cvt(t))};
		cstrg.cstr = cstr.imp.begin_ptr;
		cstr.imp = {};
		str.push_back(::std::move(cstrg));
	}
	else
	{
		static_assert(type_error, "some types are not printable or codecvt printable, so we cannot construct posix_process_envs");
	}
}

template <::std::size_t N = 0, ::std::integral replace_char_type, typename T, typename... Args>
inline constexpr void construct_posix_process_argenvs_decay(
	::fast_io::containers::vector<cstr_guard<replace_char_type>, ::fast_io::native_global_allocator> &str, T t, Args... args)
{
	construct_posix_process_argenvs_decay_singal<N>(str, t);

	if constexpr (sizeof...(Args) != 0)
	{
		construct_posix_process_argenvs_decay<N + 1>(str, args...);
	}
}
} // namespace details

struct posix_process_args FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
{
	using char_type = char;
	using replace_char_type = char;
	using storage_type = ::fast_io::containers::vector<details::cstr_guard<replace_char_type>, ::fast_io::native_global_allocator>;
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

	inline constexpr posix_process_args() noexcept
	{
		arg_envs.emplace_back(); // nullptr
	}

	inline constexpr posix_process_args(default_args_t, char const *const *args) noexcept
	{
		for (char const *const *curr{args}; *curr; ++curr)
		{
			details::cstr_guard<replace_char_type> str;
			::std::size_t str_size{::fast_io::cstr_len(*curr)};
			str.cstr = details::cstr_guard<replace_char_type>::Alloc::allocate(str_size + 1);
			auto const lase_ptr{::fast_io::freestanding::non_overlapped_copy_n(*curr, str_size, str.cstr)};
			*lase_ptr = ::fast_io::char_literal_v<u8'\0', replace_char_type>;
			arg_envs.push_back(::std::move(str));
		}
		arg_envs.emplace_back(); // nullptr
	}

	template <typename T, typename... Args>
		requires(!::std::same_as<::std::remove_cvref_t<T>, default_args_t>)
	inline constexpr posix_process_args(T &&t, Args &&...as)
	{
		details::construct_posix_process_argenvs_decay(arg_envs,
													   ::fast_io::io_print_forward<replace_char_type>(::fast_io::io_print_alias(t)),
													   ::fast_io::io_print_forward<replace_char_type>(::fast_io::io_print_alias(as))...);
		arg_envs.emplace_back(); // nullptr
	}

	inline char const *const *get() const noexcept
	{
		using char_const_p_const_p_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= char const *const *;

		return reinterpret_cast<char_const_p_const_p_may_alias_ptr>(arg_envs.data());
	}

	inline constexpr void append(posix_process_args const &others) noexcept
	{
		if (others.arg_envs.size() > 1) [[likely]]
		{
			arg_envs.pop_back(); // check and rm nullptr

			arg_envs.reserve(arg_envs.size() + others.arg_envs.size()); // sz + o.sz + 1, no need to check

			auto const end{others.arg_envs.cend() - 1}; // without nullptr
			for (auto curr{others.arg_envs.cbegin()}; curr != end; ++curr)
			{
				arg_envs.emplace_back_unchecked(*curr);
			}

			arg_envs.emplace_back_unchecked(); // nullptr
		}
	}
};

using posix_process_envs = posix_process_args;

#endif

namespace freestanding
{
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)

template <::fast_io::win32_family family>
struct is_trivially_copyable_or_relocatable<basic_win32_process_args<family>>
{
	inline static constexpr bool value = true;
};

template <::fast_io::win32_family family>
struct is_trivially_copyable_or_relocatable<basic_win32_process_envs<family>>
{
	inline static constexpr bool value = true;
};
#else

template <>
struct is_trivially_copyable_or_relocatable<posix_process_args>
{
	inline static constexpr bool value = true;
};
#endif
} // namespace freestanding
} // namespace fast_io
