#pragma once

namespace fast_io
{

namespace details
{

template <typename ch_type, typename T, typename... Args>
concept concat_may_throw =
	::std::integral<ch_type> &&
	(!::fast_io::nothrow_strlike<ch_type, T> ||
	 (::fast_io::buffer_strlike<ch_type, T> && ::fast_io::details::strlike_output_ops_may_throw<ch_type, T>) ||
	 (::fast_io::details::has_any_print_define_operations_may_throw<ch_type, Args> || ...));

template <bool line, ::std::integral output_char_type, typename T, typename... Args>
inline constexpr T basic_general_concat_phase1_decay1_impl(Args... args)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::details::concat_may_throw<output_char_type, T, Args...>)
{
	if constexpr ((::std::same_as<::std::remove_cvref_t<Args>,
								  ::fast_io::io_null_t> ||
				   ...))
	{
		constexpr auto seq =
			::fast_io::details::make_nonnull_index_sequence<Args...>();
		return [&]<::std::size_t... pos>(::std::index_sequence<pos...>)
				   FAST_IO_HERBCEPTIONS_THROWS_IF(
					   ::fast_io::details::concat_may_throw<output_char_type, T,
															::std::remove_cvref_t<Args...[pos]>...>) {
					   return ::fast_io::details::basic_general_concat_phase1_decay1_impl<line, output_char_type, T>(
						   args...[pos]...);
				   }(seq);
	}
	else if constexpr (sizeof...(Args) == 0)
	{
		if constexpr (line)
		{
			if constexpr (::fast_io::single_character_constructible_strlike<output_char_type, T>)
			{
				return strlike_construct_single_character_define(::fast_io::io_strlike_type<output_char_type, T>,
																 ::fast_io::char_literal_v<u8'\n', output_char_type>);
			}
			else
			{
				return strlike_construct_define(::fast_io::io_strlike_type<output_char_type, T>,
												__builtin_addressof(::fast_io::char_literal_v<u8'\n', output_char_type>),
												__builtin_addressof(::fast_io::char_literal_v<u8'\n', output_char_type>) + 1);
			}
		}
		else
		{
			return {};
		}
	}
	else if constexpr (!line && sizeof...(Args) == 1 &&
					   ((::std::same_as<Args,
										::fast_io::basic_io_scatter_t<output_char_type>> ||
						 ::fast_io::scatter_printable<output_char_type, Args>) &&
						...))
	{
		if constexpr (::std::same_as<Args...[0], ::fast_io::basic_io_scatter_t<output_char_type>>)
		{
			return strlike_construct_define(::fast_io::io_strlike_type<output_char_type, T>,
											args...[0].base, args...[0].base + args...[0].len);
		}
		else
		{
			auto scatter{
				print_scatter_define(::fast_io::io_reserve_type<output_char_type, ::std::remove_cvref_t<Args...[0]>>,
									 args...[0])};
			return strlike_construct_define(::fast_io::io_strlike_type<output_char_type, T>, scatter.base,
											scatter.base + scatter.len);
		}
	}
	else if constexpr (::fast_io::sso_buffer_strlike<output_char_type, T> &&
					   (::fast_io::reserve_printable<output_char_type, Args> && ...))
	{
		constexpr ::std::size_t sz{[]() consteval {
			::std::size_t total{};
			template for (constexpr auto i : ::fast_io::details::index_array_range<0zu, sizeof...(Args)>)
			{
				total = ::fast_io::details::intrinsics::add_or_overflow_die(
					total, print_reserve_size(io_reserve_type<output_char_type, Args...[i]>));
			}
			return total;
		}()};
		if constexpr (line)
		{
			static_assert(sz != SIZE_MAX, "overflow\n");
		}
		constexpr ::std::size_t sz_with_line{sz + static_cast<::std::size_t>(line)};
		constexpr ::std::size_t local_cap{strlike_sso_size(io_strlike_type<output_char_type, T>)};
		constexpr bool not_enough_space{(local_cap < sz_with_line)};
		T str;
		if constexpr (not_enough_space && sizeof...(Args) == 1 &&
					  (::fast_io::precise_reserve_printable<output_char_type, Args> && ...))
		{
			::std::size_t const precise_size{
				print_reserve_precise_size(io_reserve_type<output_char_type, ::std::remove_cvref_t<Args...[0]>>,
										   args...[0])};
			if (local_cap < precise_size + static_cast<::std::size_t>(line))
			{
				strlike_reserve(io_strlike_type<output_char_type, T>, str,
								precise_size + static_cast<::std::size_t>(line));
			}
			auto first{strlike_begin(io_strlike_type<output_char_type, T>, str)};
			print_reserve_precise_define(io_reserve_type<output_char_type, ::std::remove_cvref_t<Args...[0]>>,
										 first, precise_size, args...[0]);
			auto ptr{first + precise_size};
			if constexpr (line)
			{
				*ptr = ::fast_io::char_literal_v<u8'\n', output_char_type>;
				++ptr;
			}
			strlike_set_curr(io_strlike_type<output_char_type, T>, str, ptr);
			return str;
		}
		else
		{
			if constexpr (not_enough_space)
			{
				strlike_reserve(io_strlike_type<output_char_type, T>, str, sz_with_line);
			}
			auto ptr{strlike_begin(io_strlike_type<output_char_type, T>, str)};
			template for (constexpr auto i : ::fast_io::details::index_array_range<0zu, sizeof...(Args)>)
			{
				ptr = print_reserve_define(io_reserve_type<output_char_type, ::std::remove_cvref_t<Args...[i]>>,
										   ptr, args...[i]);
			}
			if constexpr (line)
			{
				*ptr = ::fast_io::char_literal_v<u8'\n', output_char_type>;
				++ptr;
			}
			strlike_set_curr(io_strlike_type<output_char_type, T>, str, ptr);
			return str;
		}
	}
	else if constexpr (::fast_io::buffer_strlike<output_char_type, T>)
	{
		T str FAST_IO_INDETERMINATE;
		::fast_io::operations::decay::print_freestanding_decay<line>(
			io_strlike_ref(::fast_io::io_alias, str), args...);
		return str;
	}
	else
	{
		::fast_io::details::basic_concat_buffer<output_char_type> buffer FAST_IO_INDETERMINATE;
		::fast_io::operations::decay::print_freestanding_decay<line>(
			io_strlike_ref(::fast_io::io_alias, buffer), args...);
		return strlike_construct_define(::fast_io::io_strlike_type<output_char_type, T>, buffer.buffer_begin,
										buffer.buffer_curr);
	}
}

namespace decay
{

template <::std::integral char_type, typename T>
inline constexpr ::std::size_t calculate_scatter_reserve_size_unit()
	FAST_IO_HERBCEPTIONS_THROWS
{
	using real_type = ::std::remove_cvref_t<T>;
	if constexpr (reserve_printable<char_type, real_type>)
	{
		constexpr ::std::size_t sz{print_reserve_size(io_reserve_type<char_type, real_type>)};
		return sz;
	}
	else
	{
		return 0;
	}
}

template <::std::integral char_type, typename... Args>
inline constexpr ::std::size_t calculate_scatter_reserve_size()
	FAST_IO_HERBCEPTIONS_THROWS
{
	::std::size_t total{};
	template for (constexpr auto i : ::fast_io::details::index_array_range<0zu, sizeof...(Args)>)
	{
		total = ::fast_io::details::intrinsics::add_or_overflow_die(
			total, calculate_scatter_reserve_size_unit<char_type, Args...[i]>());
	}
	return total;
}

template <bool line, ::std::integral ch_type, typename T, typename... Args>
inline constexpr T basic_general_concat_phase1_decay_impl(Args... args)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::details::concat_may_throw<ch_type, T, Args...>)
{
	return ::fast_io::details::basic_general_concat_phase1_decay1_impl<line, ch_type, T>(args...);
}

} // namespace decay

} // namespace details

template <bool line, ::std::integral char_type, typename T, typename... Args>
	requires strlike<char_type, T>
inline constexpr T basic_general_concat(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		::fast_io::details::concat_may_throw<char_type, T,
											 decltype(::fast_io::io_print_forward<char_type>(
												 ::fast_io::io_print_alias(args)))...>)
{
	return ::fast_io::details::basic_general_concat_phase1_decay1_impl<line, char_type, T>(
		::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args))...);
}

template <bool line, ::std::integral char_type, typename T, typename... Args>
	requires strlike<char_type, T>
inline constexpr T basic_concat(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		::fast_io::details::concat_may_throw<char_type, T, ::std::decay_t<Args>...>)
{
	return ::fast_io::details::basic_general_concat_phase1_decay1_impl<line, char_type, T>(
		static_cast<Args &&>(args)...);
}

} // namespace fast_io
