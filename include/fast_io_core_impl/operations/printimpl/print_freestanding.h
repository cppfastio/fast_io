#pragma once

namespace fast_io::details
{

template <typename char_type, typename T>
concept print_define_operations_nothrow = ::std::integral<char_type> && ((::fast_io::printable<char_type, T> && !requires(::fast_io::details::dummy_buffer_output_stream<char_type> out, T t) {
																			 { print_define(::fast_io::io_reserve_type<char_type, ::std::remove_cvref_t<T>>, out, t) } noexcept;
																		 }) || (::fast_io::reserve_printable<char_type, T> && !requires(char_type *ptr, T t) {
																			 { print_reserve_define(::fast_io::io_reserve_type<char_type, ::std::remove_cvref_t<T>>, ptr, t) } noexcept;
																		 }) || (::fast_io::dynamic_reserve_printable<char_type, T> && !requires(char_type *ptr, T t) {
																			 { print_reserve_size(::fast_io::io_reserve_type<char_type, ::std::remove_cvref_t<T>>, t) } noexcept;
																			 { print_reserve_define(::fast_io::io_reserve_type<char_type, ::std::remove_cvref_t<T>>, ptr, t) } noexcept;
																		 }) || (::fast_io::scatter_printable<char_type, T> && !requires(T t) {
																			 { print_scatter_define(::fast_io::io_reserve_type<char_type, ::std::remove_cvref_t<T>>, t) } noexcept;
																		 }));

template <typename char_type, typename T>
concept has_any_print_define_operations_not_noexcept = ::fast_io::details::print_define_operations_nothrow<char_type, T>;

template <typename char_type, typename... Args>
concept has_any_print_define_operations_may_throw = (::fast_io::details::print_define_operations_nothrow<char_type, Args> || ...);


template <::std::integral char_type, typename T = char_type>
inline constexpr basic_io_scatter_t<T> line_scatter_common{
	__builtin_addressof(char_literal_v<u8'\n', char_type>),
	::std::same_as<T, void> ? sizeof(char_type) : 1};

template <::std::size_t first, ::std::size_t last>
inline constexpr auto create_index_array_range() noexcept
{
	::fast_io::containers::array<::std::size_t,
								 static_cast<::std::size_t>(last - first)>
		arr;
	::std::size_t i{first};
	for (auto &e : arr)
	{
		e = i;
		++i;
	}
	return arr;
}

template <::std::size_t n>
inline constexpr auto create_index_array() noexcept
{
	constexpr auto arr{::fast_io::details::create_index_array_range<0, n>()};
	return arr;
}

template <::std::size_t first, ::std::size_t last>
inline constexpr auto index_array_range{
	::fast_io::details::create_index_array_range<first, last>()};

struct first_print_define_index_range_result
{
	::std::size_t first, last;
};

/*
Returns the first print_index that has print_define only. returns
sizeof...(Args) if we do not find any print_index one
*/
template <::std::integral char_type, typename... Args>
inline consteval ::std::size_t first_print_define_index() noexcept
{
	template for (constexpr auto i :
				  ::fast_io::details::index_array_range<0, sizeof...(Args)>)
	{
		using ArgsIType = ::std::remove_cvref_t<Args...[i]>;
		if constexpr (!(::fast_io::reserve_printable<char_type, ArgsIType> ||
						::fast_io::dynamic_reserve_printable<char_type,
															 ArgsIType> ||
						::fast_io::scatter_printable<char_type, ArgsIType> ||
						::fast_io::context_printable<char_type, ArgsIType> ||
						::std::same_as<ArgsIType, ::fast_io::basic_io_scatter_t<char_type>>))
		{
			return i;
		}
	}
	return sizeof...(Args);
}

template <::std::integral char_type, typename... Args>
inline consteval ::fast_io::details::first_print_define_index_range_result
first_print_define_index_range() noexcept
{
	::std::size_t startpos{sizeof...(Args)};
	::std::size_t endpos{sizeof...(Args)};
	template for (constexpr auto i :
				  ::fast_io::details::index_array_range<0, sizeof...(Args)>)
	{
		using ArgsIType = ::std::remove_cvref_t<Args...[i]>;
		if constexpr (::fast_io::reserve_printable<char_type, ArgsIType> ||
					  ::fast_io::dynamic_reserve_printable<char_type,
														   ArgsIType> ||
					  ::fast_io::scatter_printable<char_type, ArgsIType> ||
					  ::fast_io::context_printable<char_type, ArgsIType> ||
					  ::std::same_as<ArgsIType, ::fast_io::basic_io_scatter_t<char_type>>)
		{
			if (startpos != sizeof...(Args))
			{
				endpos = i;
				break;
			}
		}
		else
		{
			startpos = i;
		}
	}
	return {startpos, endpos};
}

template <typename... Args>
consteval auto make_nonnull_index_sequence() noexcept
{
	constexpr ::std::size_t n = sizeof...(Args);

	// Compute count without mutation
	constexpr ::std::size_t count{
		((!::std::same_as<::std::remove_cvref_t<Args>, ::fast_io::io_null_t>
			  ? 1zu
			  : 0zu) +
		 ...)};

	// Build compact index array
	constexpr auto tmp{[&]<::std::size_t... pos>(::std::index_sequence<pos...>) {
		::fast_io::containers::array<::std::size_t, count> arr{};
		::std::size_t pos2{};
		(([&] {
			 using T = ::std::remove_cvref_t<Args...[pos]>;
			 if constexpr (!::std::same_as<T, ::fast_io::io_null_t>)
			 {
				 arr[pos2++] = pos;
			 }
		 }()),
		 ...);
		return arr;
	}(::std::make_index_sequence<n>{})};

	// Convert array to index_sequence
	return [&]<::std::size_t... idx>(::std::index_sequence<idx...>) {
		return ::std::index_sequence<tmp[idx]...>{};
	}(::std::make_index_sequence<count>{});
}

template <::std::integral char_type, typename... Args>
consteval auto compute_total_normal_reserved_size_or_scatters_cache_count(
	bool compute_scatters_count, bool line) noexcept
{
	constexpr ::std::size_t mx{::std::numeric_limits<::std::size_t>::max()};
	::std::size_t total_sz{};
	template for (constexpr auto pos :
				  ::fast_io::details::index_array_range<0, sizeof...(Args)>)
	{
		using arg_type = ::std::remove_cvref_t<Args...[pos]>;
		::std::size_t sz{};
		if (compute_scatters_count)
		{
			if constexpr (!::std::same_as<::fast_io::basic_io_scatter_t<char_type>,
										  arg_type> &&
						  ::fast_io::scatter_printable<char_type, arg_type>)
			{
				sz = 1zu;
			}
		}
		else
		{
			if constexpr (::fast_io::reserve_printable<char_type, arg_type>)
			{
				sz =
					print_reserve_size(::fast_io::io_reserve_type<char_type, arg_type>);
			}
		}
		if (static_cast<::std::size_t>(mx - sz) < total_sz)
		{
			::fast_io::fast_terminate();
		}
		total_sz += sz;
	}
	if (line)
	{
		if (total_sz == mx)
		{
			::fast_io::fast_terminate();
		}
		++total_sz;
	}
	return total_sz;
}

template <::std::integral char_type, bool line, typename... Args>
consteval auto compute_total_normal_reserved_size() noexcept
{
	return ::fast_io::details::compute_total_normal_reserved_size_or_scatters_cache_count<char_type,
																						  Args...>(
		false, line);
}

template <::std::integral char_type, bool line, typename... Args>
consteval auto compute_total_scatters_count() noexcept
{
	return ::fast_io::details::compute_total_normal_reserved_size_or_scatters_cache_count<char_type,
																						  Args...>(true, line);
}

template <::std::integral output_char_type, ::std::size_t idx, typename... Args>
consteval bool is_last_element_or_not_next_element_reserve_or_dynamic_reserve_printable() noexcept
{
	if constexpr (static_cast<::std::size_t>(idx + 1zu) ==
				  sizeof...(Args))
	{
		return true;
	}
	else if constexpr (!::fast_io::reserve_printable<
						   output_char_type,
						   ::std::remove_cvref_t<Args...[idx + 1zu]>> &&
					   !::fast_io::dynamic_reserve_printable<
						   output_char_type,
						   ::std::remove_cvref_t<Args...[idx + 1zu]>>)
	{
		return true;
	}
	else
	{
		return false;
	}
}

template <::std::integral output_char_type, ::std::size_t idx, typename... Args>
consteval bool has_element_after_of_reserve_or_dynamic_reserve_printable() noexcept
{
	template for (constexpr auto pos :
				  ::fast_io::details::index_array_range<idx + 1zu, sizeof...(Args)>)
	{
		using arg_type = ::std::remove_cvref_t<Args...[pos]>;
		if constexpr (::fast_io::reserve_printable<
						  output_char_type,
						  arg_type> ||
					  ::fast_io::dynamic_reserve_printable<
						  output_char_type,
						  arg_type>)
		{
			return true;
		}
	}
	return false;
}

template <::std::integral output_char_type, bool line, typename... Args>
consteval ::std::size_t compute_print_scatters_pos() noexcept
{
	constexpr ::std::size_t mx{::std::numeric_limits<::std::size_t>::max()};
	::std::size_t scatters{};
	template for (constexpr auto pos :
				  ::fast_io::details::index_array_range<0zu, sizeof...(Args)>)
	{
		using arg_type = ::std::remove_cvref_t<Args...[pos]>;
		bool skipaddone{};
		if constexpr (::fast_io::reserve_printable<output_char_type, arg_type> &&
					  ::fast_io::dynamic_reserve_printable<output_char_type, arg_type>)
		{
			if constexpr (!::fast_io::details::is_last_element_or_not_next_element_reserve_or_dynamic_reserve_printable<output_char_type, pos, Args...>)
			{
				continue;
			}
		}
		if (!skipaddone)
		{
			if (scatters == mx)
			{
				::fast_io::fast_terminate();
			}
			++scatters;
		}
		if constexpr (pos + 1zu == sizeof...(Args) && line)
		{
			if (scatters == mx)
			{
				::fast_io::fast_terminate();
			}
			++scatters;
		}
	}
	return scatters;
}

/*
Whether the print scratch buffer allocation may happen for these arguments.
Only the plain path reaches a heap allocation: streams with an obuffer may run
out of buffer space, unbuffered streams allocate when the reserved size exceeds
the small on-stack space or a dynamic_reserve_printable arg is present.
*/
template <bool line, typename outputstmtype, typename... Args>
inline constexpr bool print_freestanding_scratch_allocation_possible{[]() consteval -> bool {
	if constexpr (sizeof...(Args) == 0 ||
				  ::fast_io::operations::decay::defines::has_status_print_define<outputstmtype>)
	{
		return false;
	}
	else if constexpr ((::std::same_as<::std::remove_cvref_t<Args>, ::fast_io::io_null_t> || ...) ||
					   ::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outputstmtype>)
	{
		return true; // filters/unwraps, then re-enters this path
	}
	else
	{
		using output_char_type = typename outputstmtype::output_char_type;
		constexpr auto range{::fast_io::details::first_print_define_index_range<output_char_type, Args...>()};
		if constexpr (range.first != sizeof...(Args))
		{
			return false;
		}
		else
		{
			return !::fast_io::operations::decay::defines::has_obuffer_flush_reserve_define<outputstmtype> &&
				   (::fast_io::operations::decay::defines::has_obuffer_basic_operations<outputstmtype> ||
					(32zu / sizeof(output_char_type) <=
					 ::fast_io::details::compute_total_normal_reserved_size<output_char_type, line, Args...>()) ||
					(::fast_io::dynamic_reserve_printable<output_char_type, ::std::remove_cvref_t<Args>> || ...));
		}
	}
}()};

/*
Whether the size-accumulation overflow check is reachable: the else path runs
with an obuffer_flush_reserve (strlike) stream or a dynamic_reserve arg.
*/
template <bool line, typename outputstmtype, typename... Args>
inline constexpr bool print_freestanding_overflow_report_possible{[]() consteval -> bool {
	if constexpr (sizeof...(Args) == 0 ||
				  ::fast_io::operations::decay::defines::has_status_print_define<outputstmtype>)
	{
		return false;
	}
	else if constexpr ((::std::same_as<::std::remove_cvref_t<Args>, ::fast_io::io_null_t> || ...) ||
					   ::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outputstmtype>)
	{
		return true; // filters/unwraps, then re-enters this path
	}
	else
	{
		using output_char_type = typename outputstmtype::output_char_type;
		constexpr auto range{::fast_io::details::first_print_define_index_range<output_char_type, Args...>()};
		if constexpr (range.first != sizeof...(Args))
		{
			return false;
		}
		else
		{
			return ::fast_io::operations::decay::defines::has_obuffer_flush_reserve_define<outputstmtype> ||
				   (::fast_io::dynamic_reserve_printable<output_char_type, ::std::remove_cvref_t<Args>> || ...);
		}
	}
}()};

template <bool line, typename outputstmtype, typename... Args>
inline constexpr bool print_freestanding_allocation_may_throw{
	print_freestanding_scratch_allocation_possible<line, outputstmtype, Args...> &&
	::fast_io::typed_generic_allocator_adapter<
		::fast_io::operations::decay::output_stream_allocator_t<outputstmtype>,
		typename outputstmtype::output_char_type>::throws_on_allocation_failure};

template <bool line, typename outputstmtype, typename... Args>
inline constexpr bool print_freestanding_overflow_may_throw{
	print_freestanding_overflow_report_possible<line, outputstmtype, Args...> &&
	::fast_io::typed_generic_allocator_adapter<
		::fast_io::operations::decay::output_stream_allocator_t<outputstmtype>,
		typename outputstmtype::output_char_type>::throws_on_overflow};

template <typename outputstmtype, typename typed_allocator_type>
inline constexpr typename typed_allocator_type::handle_type
print_output_stream_allocator_handle(outputstmtype optstm) noexcept
{
	if constexpr (::fast_io::operations::decay::defines::has_output_or_io_stream_allocator_handle_define<outputstmtype>)
	{
		return {::fast_io::operations::decay::output_stream_allocator_handle_decay(optstm)};
	}
	else
	{
		static_assert(::fast_io::operations::decay::defines::has_output_or_io_stream_allocator_handle_define<outputstmtype>,
					  "a stream using a handle-based allocator must define "
					  "output_stream_allocator_handle_define(optstm) or io_stream_allocator_handle_define(optstm)");
		return {};
	}
}


template <bool line, typename outputstmtype, typename... Args>
concept decayed_output_stream_print_may_throw = (!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outputstmtype>) ||
												(::fast_io::details::has_any_print_define_operations_may_throw<typename outputstmtype::char_type, Args...>) ||
												::fast_io::details::print_freestanding_allocation_may_throw<line, outputstmtype, Args...> ||
												::fast_io::details::print_freestanding_overflow_may_throw<line, outputstmtype, Args...>;

} // namespace fast_io::details

namespace fast_io::operations::decay
{
template <bool line, typename outputstmtype, typename... Args>
inline constexpr decltype(auto)
print_freestanding_decay(outputstmtype optstm,
						 Args... args) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::details::decayed_output_stream_print_may_throw<line, outputstmtype, Args...>)
{
	using output_char_type = typename outputstmtype::output_char_type;
	if constexpr ((::std::same_as<::std::remove_cvref_t<Args>,
								  ::fast_io::io_null_t> ||
				   ...))
	{
		constexpr auto seq =
			::fast_io::details::make_nonnull_index_sequence<Args...>();
		return [&]<::std::size_t... pos>(::std::index_sequence<pos...>) {
			return ::fast_io::operations::decay::print_freestanding_decay<line>(
				optstm, args...[pos]...);
		}(seq);
	}
	else if constexpr (::fast_io::operations::decay::defines::
						   has_status_print_define<outputstmtype>)
	{
		return status_print_define<line>(optstm, args...);
	}
	else if constexpr (sizeof...(Args) == 0)
	{
		if constexpr (line)
		{
			return ::fast_io::operations::decay::char_put_decay(
				optstm, ::fast_io::char_literal_v<u8'\n', output_char_type>);
		}
		else
		{
			return;
		}
	}
	else if constexpr (::fast_io::operations::decay::defines::
						   has_output_or_io_stream_mutex_ref_define<
							   outputstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{
			::fast_io::operations::decay::output_stream_mutex_ref_decay(optstm)};
		return ::fast_io::operations::decay::print_freestanding_decay<line>(
			::fast_io::operations::decay::output_stream_unlocked_ref_decay(optstm),
			args...);
	}
	else
	{
		constexpr auto [printdefine_startpos, printdefine_endpos]{
			::fast_io::details::first_print_define_index_range<output_char_type, Args...>()};
		if constexpr (printdefine_startpos != sizeof...(Args))
		{
			// Left side: 0 .. startpos-1
			if constexpr (printdefine_startpos != 0)
			{
				[&]<::std::size_t... pos>(::std::index_sequence<pos...>) FAST_IO_HERBCEPTIONS_THROWS {
					::fast_io::operations::decay::print_freestanding_decay<line>(
						optstm, args...[pos]...);
				}(::std::make_index_sequence<printdefine_startpos>{});
			}
			// Middle: contiguous print_define-only range [startpos, endpos)
			template for (constexpr auto i :
						  ::fast_io::details::index_array_range<printdefine_startpos, printdefine_endpos>)
			{
				print_define(::fast_io::io_reserve_type<output_char_type, ::std::remove_cvref_t<Args...[i]>>,
							 optstm, args...[i]);
			}
			// Right side: split_pos+1 .. n-1
			if constexpr (printdefine_endpos == sizeof...(Args))
			{
				if constexpr (line)
				{
					return ::fast_io::operations::decay::char_put_decay(
						optstm, ::fast_io::char_literal_v<u8'\n', output_char_type>);
				}
				else
				{
					return;
				}
			}
			else
			{
				[&]<::std::size_t... pos>(::std::index_sequence<pos...>) FAST_IO_HERBCEPTIONS_THROWS {
					::fast_io::operations::decay::print_freestanding_decay<line>(
						optstm, args...[printdefine_endpos + pos]...);
				}(::std::make_index_sequence<sizeof...(Args) - printdefine_endpos>{});
			}
		}
		else
		{
#if !FAST_IO_HAS_BUILTIN(__builtin_add_overflow)
			constexpr ::std::size_t szmx{::std::numeric_limits<::std::size_t>::max()};
#endif
			constexpr ::std::size_t total_normal_reserved_size{
				::fast_io::details::compute_total_normal_reserved_size<
					output_char_type, line, Args...>()};

			constexpr bool is_buffer_strlike_output_stream{::fast_io::operations::decay::defines::has_obuffer_flush_reserve_define<outputstmtype>};
			constexpr bool is_buffer_output_stream{
				::fast_io::operations::decay::defines::has_obuffer_basic_operations<
					outputstmtype>};
			constexpr bool use_dynamic_storage{
				is_buffer_output_stream || (32zu / sizeof(output_char_type) <= total_normal_reserved_size ||
											(::fast_io::dynamic_reserve_printable<output_char_type,
																				  ::std::remove_cvref_t<Args>> ||
											 ...))};
			using print_allocator_type =
				::fast_io::operations::decay::output_stream_allocator_t<outputstmtype>;
			using print_typed_allocator_type =
				::fast_io::typed_generic_allocator_adapter<print_allocator_type, output_char_type>;
			constexpr bool report_overflow{
				print_typed_allocator_type::throws_on_overflow};
			::std::size_t total_to_allocate_size{total_normal_reserved_size};
			output_char_type *currptr FAST_IO_INDETERMINATE;
			::std::size_t buffer_remained_spaces FAST_IO_INDETERMINATE;
			bool buffer_enough_space FAST_IO_INDETERMINATE;
			{
				output_char_type *endptr FAST_IO_INDETERMINATE;
				if constexpr (is_buffer_output_stream)
				{
					currptr = obuffer_curr(optstm);
					endptr = obuffer_end(optstm);
					buffer_remained_spaces = static_cast<::std::size_t>(endptr - currptr);
					if constexpr (!is_buffer_strlike_output_stream)
					{
						buffer_enough_space = (total_normal_reserved_size <= buffer_remained_spaces);
						buffer_enough_space &= (currptr <= endptr);
					}
				}
				template for (constexpr auto i :
							  ::fast_io::details::index_array_range<0, sizeof...(
																		   Args)>)
				{
					using argtype = ::std::remove_cvref_t<Args...[i]>;
					if constexpr (!::fast_io::reserve_printable<output_char_type,
																argtype>)
					{
						if constexpr (is_buffer_strlike_output_stream)
						{
							::std::size_t argsz FAST_IO_INDETERMINATE;
							if constexpr (::std::same_as<argtype, ::fast_io::basic_io_scatter_t<output_char_type>>)
							{
								argsz = args...[i].len;
							}
							else
							{
								argsz = print_reserve_size(::fast_io::io_reserve_type<output_char_type, argtype>, args...[i]);
							}
#if FAST_IO_HAS_BUILTIN(__builtin_add_overflow)
							if (__builtin_add_overflow(total_to_allocate_size, argsz, __builtin_addressof(total_to_allocate_size)))
							{
								::fast_io::containers::details::contract_violation_report<report_overflow>(::std::errc::value_too_large);
							}
#else
							if (static_cast<::std::size_t>(szmx - argsz) <
								total_to_allocate_size)
							{
								::fast_io::containers::details::contract_violation_report<report_overflow>(::std::errc::value_too_large);
							}
							total_to_allocate_size += argsz;
#endif
						}
						else if constexpr (::fast_io::dynamic_reserve_printable<output_char_type, argtype>)
						{
							::std::size_t argsz{print_reserve_size(::fast_io::io_reserve_type<output_char_type, argtype>, args...[i])};
#if FAST_IO_HAS_BUILTIN(__builtin_add_overflow)
							if (__builtin_add_overflow(total_to_allocate_size, argsz, __builtin_addressof(total_to_allocate_size)))
							{
								::fast_io::containers::details::contract_violation_report<report_overflow>(::std::errc::value_too_large);
							}
#else
							if (static_cast<::std::size_t>(szmx - argsz) <
								total_to_allocate_size)
							{
								::fast_io::containers::details::contract_violation_report<report_overflow>(::std::errc::value_too_large);
							}
							total_to_allocate_size += argsz;
#endif
							if constexpr (is_buffer_output_stream)
							{
								buffer_enough_space &= argsz < buffer_remained_spaces;
								buffer_remained_spaces -= argsz;
							}
						}
						else if constexpr (is_buffer_output_stream && ::std::same_as<argtype, ::fast_io::basic_io_scatter_t<output_char_type>>)
						{
							::std::size_t const argsz{args...[i].len};
							buffer_enough_space &= argsz < buffer_remained_spaces;
							buffer_remained_spaces -= argsz;
						}
						else if constexpr (is_buffer_output_stream && ::fast_io::scatter_printable<output_char_type, argtype>)
						{
							::std::size_t const argsz{print_reserve_size(::fast_io::io_reserve_type<output_char_type, argtype>, args...[i]).len};
							buffer_enough_space &= argsz < buffer_remained_spaces;
							buffer_remained_spaces -= argsz;
						}
					}
				}
			}

			if constexpr (is_buffer_strlike_output_stream)
			{
				if (buffer_remained_spaces < total_to_allocate_size)
				{
					obuffer_flush_reserve_define(optstm, total_to_allocate_size);
					currptr = obuffer_curr(optstm);
				}

				template for (constexpr auto i :
							  ::fast_io::details::index_array_range<0zu, sizeof...(
																			 Args)>)
				{
					using argtype = ::std::remove_cvref_t<Args...[i]>;
					if constexpr (::std::same_as<argtype, ::fast_io::basic_io_scatter_t<output_char_type>>)
					{
						currptr = ::fast_io::details::copy_scatter(args...[i], currptr);
					}
					else if constexpr (::fast_io::scatter_printable<output_char_type, argtype>)
					{
						currptr = ::fast_io::details::copy_scatter(print_scatter_define(::fast_io::io_reserve_type<output_char_type, argtype>, args...[i]), currptr);
					}
					else if constexpr (::fast_io::reserve_printable<output_char_type,
																	argtype> ||
									   ::fast_io::dynamic_reserve_printable<output_char_type,
																			argtype>)
					{
						currptr = print_reserve_define(::fast_io::io_reserve_type<output_char_type, argtype>, currptr, args...[i]);
					}
				}
				if constexpr (line)
				{
					*currptr = ::fast_io::char_literal_v<u8'\n', output_char_type>;
					++currptr;
				}
				obuffer_set_curr(optstm, currptr);
			}
			else
			{
				::fast_io::containers::array<
					output_char_type,
					use_dynamic_storage ? 0zu : total_normal_reserved_size>
					buffer FAST_IO_INDETERMINATE;
				::std::conditional_t<
					use_dynamic_storage,
					::fast_io::details::buffer_alloc_arr_ptr<output_char_type, false, print_allocator_type>,
					::fast_io::details::empty>
					dynamic_buffer;
				output_char_type *it FAST_IO_INDETERMINATE;
				if constexpr (is_buffer_output_stream)
				{
					it = currptr;
					if (!buffer_enough_space) [[unlikely]]
					{
						if constexpr (use_dynamic_storage)
						{
							if constexpr (print_typed_allocator_type::has_status)
							{
								it = dynamic_buffer.allocate_new(
									::fast_io::details::print_output_stream_allocator_handle<outputstmtype,
																							 print_typed_allocator_type>(
										optstm),
									total_to_allocate_size);
							}
							else
							{
								it = dynamic_buffer.allocate_new(total_to_allocate_size);
							}
						}
						else
						{
							it = buffer.data();
						}
					}
				}
				else
				{
					if constexpr (use_dynamic_storage)
					{
						if constexpr (print_typed_allocator_type::has_status)
						{
							it = dynamic_buffer.allocate_new(
								::fast_io::details::print_output_stream_allocator_handle<outputstmtype,
																						 print_typed_allocator_type>(
									optstm),
								total_to_allocate_size);
						}
						else
						{
							it = dynamic_buffer.allocate_new(total_to_allocate_size);
						}
					}
					else
					{
						it = buffer.data();
					}
				}
				constexpr ::std::size_t requested_scatters{
					::fast_io::details::compute_print_scatters_pos<output_char_type, line, Args...>()};
				constexpr bool only_one_scatter{requested_scatters < 2zu};
				using scattertype = ::std::conditional_t<
					!only_one_scatter && ::fast_io::operations::decay::defines::has_any_of_write_or_seek_pwrite_bytes_operations<outputstmtype>,
					::fast_io::io_scatter_t, ::fast_io::basic_io_scatter_t<output_char_type>>;
				::fast_io::containers::array<
					scattertype, (only_one_scatter ? 0zu : requested_scatters)>
					scatters FAST_IO_INDETERMINATE;
				scattertype *scatterbase FAST_IO_INDETERMINATE,
					*scatterptr FAST_IO_INDETERMINATE;
				if constexpr (!only_one_scatter)
				{
					scatterptr = scatterbase = scatters.data();
				}
				output_char_type const *bufferbase{it};
				output_char_type const *itconst FAST_IO_INDETERMINATE;
				output_char_type *last_pos FAST_IO_INDETERMINATE;
				if constexpr (!only_one_scatter)
				{
					last_pos = it;
				}
				if consteval
				{
					itconst = it;
				}
				template for (constexpr auto i :
							  ::fast_io::details::index_array_range<0zu, sizeof...(
																			 Args)>)
				{
					constexpr bool islastwithlf{line && (i + 1zu == sizeof...(Args))};
					using argtype = ::std::remove_cvref_t<Args...[i]>;
					if constexpr (::fast_io::reserve_printable<output_char_type,
															   argtype> ||
								  ::fast_io::dynamic_reserve_printable<output_char_type,
																	   argtype>)
					{
						it = print_reserve_define(
							::fast_io::io_reserve_type<output_char_type, argtype>, it,
							args...[i]);
						if constexpr (islastwithlf)
						{
							*it = ::fast_io::char_literal_v<u8'\n', output_char_type>;
							++it;
						}
						if constexpr (!only_one_scatter &&
									  ::fast_io::details::is_last_element_or_not_next_element_reserve_or_dynamic_reserve_printable<output_char_type, i, Args...>())
						{
							if constexpr (is_buffer_output_stream)
							{
								if (buffer_enough_space) [[likely]]
								{
									continue;
								}
							}
							::std::size_t const diff{static_cast<::std::size_t>(it - last_pos)};
							if constexpr (::std::same_as<scattertype, ::fast_io::io_scatter_t>)
							{
								*scatterptr = {last_pos, diff * sizeof(output_char_type)};
							}
							else
							{
								*scatterptr = {last_pos, diff};
							}
							++scatterptr;
							if constexpr (::fast_io::details::has_element_after_of_reserve_or_dynamic_reserve_printable<output_char_type, i, Args...>())
							{
								last_pos = it;
							}
						}
					}
					else if constexpr (::std::same_as<argtype,
													  ::fast_io::basic_io_scatter_t<
														  output_char_type>> ||
									   ::fast_io::scatter_printable<output_char_type, argtype>)
					{
						::fast_io::basic_io_scatter_t<output_char_type> scatteri FAST_IO_INDETERMINATE;
						if constexpr (::std::same_as<argtype,
													 ::fast_io::basic_io_scatter_t<
														 output_char_type>>)
						{
							scatteri = args...[i];
						}
						else
						{
							scatteri = print_scatter_define(::fast_io::io_reserve_type<output_char_type, argtype>,
															args...[i]);
						}
						if constexpr (is_buffer_output_stream)
						{
							if (buffer_enough_space) [[likely]]
							{
								if consteval
								{
									auto oldit{it};
									it = ::fast_io::details::copy_scatter(scatteri, it);
									if constexpr (islastwithlf)
									{
										*it = ::fast_io::char_literal_v<u8'\n', output_char_type>;
										++it;
									}
									itconst += it - oldit;
								}
								else
								{
									it = ::fast_io::details::copy_scatter(scatteri, it);
									if constexpr (islastwithlf)
									{
										*it = ::fast_io::char_literal_v<u8'\n', output_char_type>;
										++it;
									}
								}
								continue;
							}
						}
						if constexpr (only_one_scatter && !islastwithlf)
						{
							bufferbase = scatteri.base;
							if consteval
							{
								itconst = bufferbase + scatteri.len;
							}
							else
							{
								it = const_cast<output_char_type *>(bufferbase) + scatteri.len;
							}
						}
						else
						{
							if constexpr (::std::same_as<scattertype, ::fast_io::io_scatter_t>)
							{
								*scatterptr = {scatteri.base, scatteri.len * sizeof(output_char_type)};
							}
							else
							{
								*scatterptr = scatteri;
							}
							++scatterptr;
							if constexpr (islastwithlf)
							{
								*scatterptr = ::fast_io::details::line_scatter_common<output_char_type, typename scattertype::value_type>;
								++scatterptr;
							}
						}
					}
				}
				if constexpr (is_buffer_output_stream)
				{
					if (buffer_enough_space) [[likely]]
					{
						obuffer_set_curr(optstm, it);
						return;
					}
				}
				if constexpr (only_one_scatter)
				{
					output_char_type const *itend FAST_IO_INDETERMINATE;
					if consteval
					{
						itend = itconst;
					}
					else
					{
						itend = it;
					}
					::fast_io::operations::decay::write_all_decay(optstm, bufferbase, static_cast<::std::size_t>(itend - bufferbase));
				}
				else
				{
					if constexpr (::std::same_as<scattertype, ::fast_io::io_scatter_t>)
					{
						::fast_io::operations::decay::scatter_write_all_bytes_decay(
							optstm, scatterbase,
							static_cast<::std::size_t>(scatterptr - scatterbase));
					}
					else
					{
						::fast_io::operations::decay::scatter_write_all_decay(
							optstm, scatterbase,
							static_cast<::std::size_t>(scatterptr - scatterbase));
					}
				}
			}
		}
	}
}

} // namespace fast_io::operations::decay

namespace fast_io::operations
{

template <bool line, typename output, typename... Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void print_freestanding(output &&outstm, Args &&...args) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::operations::decay::print_freestanding_decay<line>(
		::fast_io::operations::output_stream_ref(outstm),
		io_print_forward<typename decltype(::fast_io::operations::output_stream_ref(outstm))::output_char_type>(
			io_print_alias(args))...);
}

} // namespace fast_io::operations
