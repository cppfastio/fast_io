#pragma once

namespace fast_io
{

/// @brief    contiguous_scannable
/// @details  That a type is contiguous scannable
///           is that it can be scanned from a contiguous memory region
///           passed in as a pointer to the beginning and end of the region
///           as the 2nd and 3rd arguments in `scan_contiguous_define`.
/// @fn       scan_contiguous_define
/// @brief    Scans an object from a contiguous memory region.
/// @tparam   <auto-inferred>
/// @param    ::fast_io::io_reserve_type_t<char_type, T>    tag-invoke
/// @param    char_type const*                              a pointer to the beginning of the buffer
/// @param    char_type const*                              a pointer to the end of the buffer
/// @param    T                                             the object to be scanned, can be any passing style
/// @return   ::fast_io::parse_result<char_type const*>     a pointer to the next after scanning,
///                                                         and a parse code indicating parsing state
template <typename char_type, typename T>
concept contiguous_scannable = requires(char_type const *begin, char_type const *end, T t) {
	{
		scan_contiguous_define(io_reserve_type<char_type, T>, begin, end, t)
	} -> ::std::same_as<parse_result<char_type const *>>;
};

/// @brief    context_scannable
/// @details  That a type is context scannable
///           is that it can be scanned multiple times until it's fully scanned
///           with the scanning state stored in a context object,
///           each time from a contiguous memory region.
/// @note     A type MUST be context scannable if it is to be scanned
/// @fn       scan_context_type
/// @brief    Indicates the type of your context object for context scannable types.
/// @tparam   <auto-inferred>
/// @param    ::fast_io::io_reserve_type_t<char_type, T>    tag-invoke
/// @return   ::fast_io::io_type_t<your_context_type>       the type of the context object
/// @struct   your_context_type
/// @brief    The context object, usually stores partial results.
/// @fn       scan_context_define
/// @brief    Partially scans an object from a contiguous memory region with a context object.
/// @tparam   <auto-inferred>
/// @param    ::fast_io::io_reserve_type_t<char_type, T>    tag-invoke
/// @param    your_context_type&                            the context object
/// @param    char_type const*                              a pointer to the beginning of the buffer
/// @param    char_type const*                              a pointer to the end of the buffer
/// @param    T                                             the object to be scanned, can be any passing style
/// @return   ::fast_io::parse_result<char_type const*>     a pointer to the next after scanning,
///                                                         and a parse code indicating parsing state
/// @note     It's not recommended to write to T if the scanning is still partial, so as to hold strong exception guarantee.
/// @fn       scan_context_eof_define
/// @brief    Indicates that the scanning meets an EOF.
/// @tparam   <auto-inferred>
/// @param    ::fast_io::io_reserve_type_t<char_type, T>    tag-invoke
/// @param    your_context_type&                            the context object
/// @param    T                                             the object to be scanned, can be any passing style
/// @return   ::fast_io::parse_code                         a parse code indicating parsing state
template <typename char_type, typename T>
concept context_scannable = requires(char_type const *begin, char_type const *end, T t) {
	requires requires(
		typename ::std::remove_cvref_t<decltype(scan_context_type(io_reserve_type<char_type, T>))>::type st) {
		{
			scan_context_define(io_reserve_type<char_type, T>, st, begin, end, t)
		} -> ::std::same_as<parse_result<char_type const *>>;
		{ scan_context_eof_define(io_reserve_type<char_type, T>, st, t) } -> ::std::same_as<parse_code>;
	};
};

/// @brief    reserve_printable
/// @details  That a type is reserve printable
///           is that it can be printed to a buffer with a compile-time-known size.
/// @fn       print_reserve_size
/// @brief    Returns the size of the buffer needed to print an object at most.
/// @tparam   <auto-inferred>
/// @param    ::fast_io::io_reserve_type_t<char_type, ::std::remove_cvref_t<T>> tag-invoke
/// @return   ::std::size_t                                 the size of the buffer to be reserved
/// @fn       print_reserve_define
/// @brief    Prints the object to a buffer.
/// @tparam   <auto-inferred>
/// @param    ::fast_io::io_reserve_type_t<char_type, ::std::remove_cvref_t<T>> tag-invoke
/// @param    char_type*                                    a pointer to the beginning of the buffer
/// @param    T                                             the object to be printed
/// @return   char_type*                                    a pointer to the next after printing
template <typename char_type, typename T>
concept reserve_printable = ::std::integral<char_type> && requires(T t, char_type *ptr) {
	{
		print_reserve_size(io_reserve_type<char_type, ::std::remove_cvref_t<T>>)
	} -> ::std::convertible_to<::std::size_t>;
	{
		print_reserve_define(io_reserve_type<char_type, ::std::remove_cvref_t<T>>, ptr, t)
	} -> ::std::convertible_to<char_type *>;
};

/// @brief    dynamic_reserve_printable
/// @details  That a type is dynamic reserve printable
///           is that it can be printed to a buffer with a run-time-known size,
///           which can be different between objects of the same type.
/// @fn       print_reserve_size
/// @brief    Returns the size of the buffer needed to print an object at most.
/// @tparam   <auto-inferred>
/// @param    ::fast_io::io_reserve_type_t<char_type, ::std::remove_cvref_t<T>> tag-invoke
/// @param    T                                             the object to be printed
/// @return   ::std::size_t                                 the size of the buffer to be reserved
/// @fn       print_reserve_define
/// @brief    Prints the object to a buffer.
/// @tparam   <auto-inferred>
/// @param    ::fast_io::io_reserve_type_t<char_type, ::std::remove_cvref_t<T>> tag-invoke
/// @param    char_type*                                    a pointer to the beginning of the buffer
/// @param    T                                             the object to be printed
/// @return   char_type*                                    a pointer to the next after printing
template <typename char_type, typename T>
concept dynamic_reserve_printable = ::std::integral<char_type> && requires(T t, char_type *ptr) {
	{
		print_reserve_size(io_reserve_type<char_type, ::std::remove_cvref_t<T>>, t)
	} -> ::std::convertible_to<::std::size_t>;
	{
		print_reserve_define(io_reserve_type<char_type, ::std::remove_cvref_t<T>>, ptr, t)
	} -> ::std::convertible_to<char_type *>;
};

/// @warning    UNSTABLE
/// @brief      context_printable
/// @details    That a type is context printable
///             is that it can be partially printed to a buffer multiple times.
/// @fn         print_context_type
/// @brief      Indicates the type of your context object for context printable types.
/// @tparam     <auto-inferred>
/// @param      ::fast_io::io_reserve_type_t<char_type, T>  tag-invoke
/// @return     ::fast_io::io_type_t<your_context_type>     the type of the context object
/// @struct     your_context_type
/// @brief      The context object, usually stores the progress of printing.
/// @fn         print_context_define
/// @brief      Partially prints an object to a buffer with a context object.
/// @tparam     <auto-inferred>
/// @param      this                                        the context object
/// @param      T                                           the object to be printed
/// @param      char_type*                                  a pointer to the beginning of the buffer
/// @param      char_type*                                  a pointer to the end of the buffer
/// @return     ::fast_io::context_print_result<char_type*> a pointer to the next after printing
///                                                         and a boolean indicating whether the printing is done
template <typename char_type, typename T>
concept context_printable = ::std::integral<char_type> && requires(T t, char_type *ptr) {
	requires requires(
		typename ::std::remove_cvref_t<decltype(print_context_type(io_reserve_type<char_type, T>))>::type st) {
		{ st.print_context_define(t, ptr, ptr) } -> ::std::same_as<context_print_result<char_type *>>;
	};
};

/// @brief      printable_internal_shift
/// @details    Defines the behaviour when printed with ::fast_io::mnp::width<::fast_io::mnp::scalar_placement::internal>
/// @fn         print_define_internal_shift
/// @brief      Returns the number of the characters printed on the left of the filling spaces.
/// @tparam     <auto-inferred>
/// @param      ::fast_io::io_reserve_type_t<char_type, ::std::remove_cvref_t<T>> tag-invoke
/// @param      T                                           the object to be printed
/// @return     ::std::size_t                               the number of characters appaired on the left
template <typename char_type, typename T>
concept printable_internal_shift = requires(T t) {
	{
		print_define_internal_shift(io_reserve_type<char_type, ::std::remove_cvref_t<T>>, t)
	} -> ::std::same_as<::std::size_t>;
};

/// @brief      precise_reserve_printable
/// @details    That a type is precise reserve printable
///             is that it's `reserve_printable` or `dynamic_reserve_printable`
///             and further has a precise printing size.
/// @fn         print_reserve_precise_size
/// @brief      Returns the precise size of the buffer needed to print an object.
/// @tparam     <auto-inferred>
/// @param      ::fast_io::io_reserve_type_t<char_type, ::std::remove_cvref_t<T>> tag-invoke
/// @param      T                                           the object to be printed
/// @return     ::std::size_t                               the precise size of the buffer to be reserved
/// @fn         print_reserve_precise_define
/// @brief      Prints the object to a buffer with a precise size.
/// @tparam     <auto-inferred>
/// @param      ::fast_io::io_reserve_type_t<char_type, ::std::remove_cvref_t<T>> tag-invoke
/// @param      char_type*                                  a pointer to the beginning of the buffer
/// @param      ::std::size_t                               the size of the buffer
/// @param      T                                           the object to be printed
/// @return     void
template <typename char_type, typename T>
concept precise_reserve_printable =
	::std::integral<char_type> &&
	(reserve_printable<char_type, T> || dynamic_reserve_printable<char_type, T>) && requires(T t, char_type *ptr, ::std::size_t n) {
		{
			print_reserve_precise_size(io_reserve_type<char_type, ::std::remove_cvref_t<T>>, t)
		} -> ::std::convertible_to<::std::size_t>;
		print_reserve_precise_define(io_reserve_type<char_type, ::std::remove_cvref_t<T>>, ptr, n, t);
	};

/// @brief      reserve_scatters_printable
/// @details    That a type is reserve scatters printable
///             is that it's composed of seperate parts
///             that can be printed to several scatters with a compile-time-known size.
/// @fn         print_reserve_scatters_size
/// @brief      Returns the count of the scatters and some extra space required.
/// @tparam     <auto-inferred>
/// @param      ::fast_io::io_reserve_type_t<char_type, ::std::remove_cvref_t<T>> tag-invoke
/// @return     ::fast_io::reserve_scatters_size_result     the count of the scatters and the size of an extra buffer
///                                                         which can later be pointed in the scatters.
/// @fn         print_reserve_scatters_define
/// @brief      Prints the object to several scatters.
/// @tparam     <auto-inferred>
/// @param      ::fast_io::io_reserve_type_t<char_type, ::std::remove_cvref_t<T>> tag-invoke
/// @param      ::fast_io::basic_io_scatter_t<char_type>*   a pointer to the beginning of the scatters
/// @param      char_type*                                  a pointer to the beginning of the buffer
/// @param      T                                           the object to be printed
/// @return     ::fast_io::basic_reserve_scatters_define_result<char_type>
///                                                         a pointer to the next scatter after printing
///                                                         and a pointer to the next character after printing
template <typename char_type, typename T>
concept reserve_scatters_printable =
	::std::integral<char_type> && requires(T t, ::fast_io::basic_io_scatter_t<char_type> *scatters, char_type *ptr) {
		{
			print_reserve_scatters_size(io_reserve_type<char_type, ::std::remove_cvref_t<T>>)
		} -> ::std::same_as<reserve_scatters_size_result>;
		{
			print_reserve_scatters_define(io_reserve_type<char_type, ::std::remove_cvref_t<T>>, scatters, ptr, t)
		} -> ::std::same_as<::fast_io::basic_reserve_scatters_define_result<char_type>>;
	};

/// @brief      printable
/// @details    Makes a type printable
/// @warning    This concept will be soon deprecated.
/// @fn         print_define
/// @brief      Prints the object to a device directly.
/// @tparam     <auto-inferred>
/// @param      ::fast_io::io_reserve_type_t<char_type, ::std::remove_cvref_t<T>> tag-invoke
/// @param      output                                      the output device
/// @param      T                                           the object to be printed
/// @return     void
template <typename char_type, typename T>
concept printable = requires(::fast_io::details::dummy_buffer_output_stream<char_type> out, T t) {
	print_define(io_reserve_type<char_type, ::std::remove_cvref_t<T>>, out, t);
};

/// @brief      scatter_printable
/// @details    That a type is scatter printable
///             is that it can be printed to a scatter.
/// @fn         print_scatter_define
/// @brief      Generates a printed scatter.
/// @tparam     <auto-inferred>
/// @param      ::fast_io::io_reserve_type_t<char_type, ::std::remove_cvref_t<T>> tag-invoke
/// @param      T                                           the object to be printed
/// @return     ::fast_io::basic_io_scatter_t<char_type>    a scatter of the printed object
template <typename char_type, typename T>
concept scatter_printable = requires(char_type ch, T &&t) {
	{
		print_scatter_define(io_reserve_type<char_type, ::std::remove_cvref_t<T>>,
							 ::fast_io::freestanding::forward<T>(t))
	} -> ::std::same_as<basic_io_scatter_t<char_type>>;
};

/// @brief      alias_scannable
/// @details    Alias scannable enables a type to be scanned using an alias
///             where the scanning function is defined on the alias type.
/// @fn         scan_alias_define
/// @brief      Indicates the alias type.
/// @tparam     <auto-inferred>
/// @param      ::fast_io::io_alias_t                       tag-invoke
/// @param      T                                           the object to be scanned
/// @return     your_alias_type                             your alias type, commonly owns a reference to T
template <typename T>
concept alias_scannable = requires(T &&t) { scan_alias_define(io_alias, t); };

/// @brief      alias_printable
/// @details    Alias printable enables a type to be printed using an alias
///             where the printing function is defined on the alias type.
/// @fn         print_alias_define
/// @brief      Indicates the alias type.
/// @tparam     <auto-inferred>
/// @param      ::fast_io::io_alias_t                       tag-invoke
/// @param      T                                           the object to be printed
/// @return     your_alias_type                             your alias type, commonly owns T
template <typename T>
concept alias_printable = requires(T &&t) { print_alias_define(io_alias, ::fast_io::freestanding::forward<T>(t)); };

template <typename char_type, typename T>
concept status_io_scan_forwardable =
	::std::integral<char_type> && requires(T t) { status_io_scan_forward(io_alias_type<char_type>, t); };

template <typename char_type, typename T>
concept status_io_print_forwardable = ::std::integral<char_type> && requires(T &&t) {
	status_io_print_forward(io_alias_type<char_type>, ::fast_io::freestanding::forward<T>(t));
};

struct manip_tag_t
{
};

template <typename T>
concept manipulator = requires(T t) { typename T::manip_tag; };

template <typename T>
struct parameter
{
	using manip_tag = manip_tag_t;
	T reference;
};

template <::std::integral char_type, typename output, typename value_type>
	requires(printable<char_type, ::std::remove_cvref_t<value_type>> && ::std::is_trivially_copyable_v<output>)
inline constexpr void print_define(io_reserve_type_t<char_type, parameter<value_type>>, output out,
							parameter<value_type> wrapper)
{
	print_define(io_reserve_type<char_type, ::std::remove_cvref_t<value_type>>, out, wrapper.reference);
}

template <::std::integral char_type, typename value_type>
	requires reserve_printable<char_type, ::std::remove_cvref_t<value_type>>
inline constexpr ::std::size_t print_reserve_size(io_reserve_type_t<char_type, parameter<value_type>>)
{
	return print_reserve_size(io_reserve_type<char_type, ::std::remove_cvref_t<value_type>>);
}

template <::std::integral char_type, typename value_type>
	requires dynamic_reserve_printable<char_type, ::std::remove_cvref_t<value_type>>
inline constexpr ::std::size_t print_reserve_size(io_reserve_type_t<char_type, parameter<value_type>>,
										   parameter<value_type> para)
{
	return print_reserve_size(io_reserve_type<char_type, ::std::remove_cvref_t<value_type>>, para.reference);
}

template <::std::integral char_type, typename value_type>
	requires(reserve_printable<char_type, ::std::remove_cvref_t<value_type>> ||
			 dynamic_reserve_printable<char_type, ::std::remove_cvref_t<value_type>>)
inline constexpr auto print_reserve_define(io_reserve_type_t<char_type, parameter<value_type>>, char_type *begin,
									parameter<value_type> para)
{
	return print_reserve_define(io_reserve_type<char_type, ::std::remove_cvref_t<value_type>>, begin, para.reference);
}

template <::std::integral char_type, typename value_type, typename Iter>
	requires(printable_internal_shift<char_type, ::std::remove_cvref_t<value_type>>)
inline constexpr auto print_define_internal_shift(io_reserve_type_t<char_type, parameter<value_type>>, Iter begin,
										   parameter<value_type> para)
{
	return print_define_internal_shift(io_reserve_type<char_type, ::std::remove_cvref_t<value_type>>, begin,
									   para.reference);
}

template <::std::integral char_type, typename value_type>
	requires precise_reserve_printable<char_type, ::std::remove_cvref_t<value_type>>
inline constexpr ::std::size_t print_reserve_precise_size(io_reserve_type_t<char_type, parameter<value_type>>,
												   parameter<value_type> para)
{
	return print_reserve_precise_size(io_reserve_type<char_type, ::std::remove_cvref_t<value_type>>, para.reference);
}

template <::std::integral char_type, typename value_type, typename Iter>
	requires precise_reserve_printable<char_type, ::std::remove_cvref_t<value_type>>
inline constexpr void print_reserve_precise_define(io_reserve_type_t<char_type, parameter<value_type>>, Iter begin,
											::std::size_t n, parameter<value_type> para)
{
	print_reserve_precise_define(io_reserve_type<char_type, ::std::remove_cvref_t<value_type>>, begin, n,
								 para.reference);
}

template <::std::integral char_type, typename value_type>
	requires scatter_printable<char_type, ::std::remove_cvref_t<value_type>>
inline constexpr auto print_scatter_define(io_reserve_type_t<char_type, parameter<value_type>>, parameter<value_type> para)
{
	return print_scatter_define(io_reserve_type<char_type, ::std::remove_cvref_t<value_type>>, para.reference);
}

template <typename char_type, typename T>
concept iterative_scannable =
	::std::integral<char_type> && requires(T &t, char_type const *buffer_curr, char_type const *buffer_end) {
		{ scan_iterative_init_define(io_reserve_type<char_type, ::std::remove_cvref_t<T>>, t) };
		{
			scan_iterative_next_define(io_reserve_type<char_type, ::std::remove_cvref_t<T>>, t, buffer_curr, buffer_end)
		} -> ::std::same_as<parse_result<char_type const *>>;
		{
			scan_iterative_eof_define(io_reserve_type<char_type, ::std::remove_cvref_t<T>>, t)
		} -> ::std::same_as<fast_io::parse_code>;
	};

template <typename char_type, typename T>
concept iterative_contiguous_scannable =
	::std::integral<char_type> && requires(T t, char_type const *buffer_curr, char_type const *buffer_end) {
		{
			scan_iterative_contiguous_define(io_reserve_type<char_type, ::std::remove_cvref_t<T>>, t, buffer_curr,
											 buffer_end)
		} -> ::std::same_as<parse_result<char_type const *>>;
	};

template <typename char_type, typename T>
concept precise_reserve_scannable = ::std::integral<char_type> && requires(char_type const *buffer_curr, T t) {
	{
		scan_precise_reserve_size(io_reserve_type<char_type, ::std::remove_cvref_t<T>>)
	} -> ::std::same_as<::std::size_t>;
	scan_precise_reserve_define(io_reserve_type<char_type, ::std::remove_cvref_t<T>>, buffer_curr, t);
};

template <typename char_type, typename T>
concept precise_reserve_scannable_no_error =
	precise_reserve_scannable<char_type, T> && requires(char_type const *buffer_curr, T t) {
		{
			scan_precise_reserve_define(io_reserve_type<char_type, ::std::remove_cvref_t<T>>, buffer_curr, t)
		} -> ::std::same_as<void>;
	};

namespace manipulators
{
}

namespace mnp = manipulators;

} // namespace fast_io
