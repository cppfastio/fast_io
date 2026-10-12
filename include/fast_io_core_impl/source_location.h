#pragma once

namespace fast_io
{

namespace details
{

inline constexpr ::std::size_t prrsv_reserve_size_source_location_impl(::std::source_location const &location) noexcept
{
	constexpr ::std::size_t known_size_at_compilation{3zu +
													  (print_reserve_size(::fast_io::io_reserve_type<char8_t, ::std::uint_least32_t>) * 2zu)};
	::std::size_t filenamesz{::fast_io::cstr_len(location.file_name())};
	::std::size_t functionnamesz{::fast_io::cstr_len(location.function_name())};
#if FAST_IO_HAS_BUILTIN(__builtin_add_overflow)
	::std::size_t total_sum FAST_IO_INDETERMINATE;
	if (__builtin_add_overflow(known_size_at_compilation, filenamesz, __builtin_addressof(total_sum)))
	{
		::fast_io::fast_terminate();
	}
	if (__builtin_add_overflow(total_sum, functionnamesz, __builtin_addressof(total_sum)))
	{
		::fast_io::fast_terminate();
	}
	return total_sum;
#else
	constexpr ::std::size_t szmx_noknown{::std::numeric_limits<::std::size_t>::max() - known_size_at_compilation};
	if (szmx_noknown < filenamesz)
	{
		::fast_io::fast_terminate();
	}
	if (static_cast<::std::size_t>(szmx_noknown - filenamesz) < functionnamesz)
	{
		::fast_io::fast_terminate();
	}
	return known_size_at_compilation + filenamesz + functionnamesz;
#endif
}

template <::std::integral char_type>
	requires(sizeof(char_type) == 1)
inline constexpr char_type *
prrsv_reserve_define_source_location_impl(char_type *iter, ::std::source_location const &location) noexcept
{
	char const *flnm{location.file_name()};
	::std::size_t flnmlen{::fast_io::cstr_len(flnm)};
	iter = ::fast_io::details::non_overlapped_copy_n(flnm, flnmlen, iter);
	*iter = ::fast_io::char_literal_v<u8':', char_type>;
	++iter;
	iter = print_reserve_define(::fast_io::io_reserve_type<char_type, ::std::uint_least32_t>, iter, location.line());
	*iter = ::fast_io::char_literal_v<u8':', char_type>;
	++iter;
	iter = print_reserve_define(::fast_io::io_reserve_type<char_type, ::std::uint_least32_t>, iter, location.column());
	*iter = ::fast_io::char_literal_v<u8':', char_type>;
	++iter;
	char const *fnnm{location.function_name()};
	::std::size_t fnnmlen{::fast_io::cstr_len(fnnm)};
	return ::fast_io::details::non_overlapped_copy_n(fnnm, fnnmlen, iter);
}

} // namespace details

template <::std::integral char_type>
	requires(sizeof(char_type) == 1)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::std::size_t
print_reserve_size(::fast_io::io_reserve_type_t<char_type, ::std::source_location>, ::std::source_location const &location) noexcept
{
	return ::fast_io::details::prrsv_reserve_size_source_location_impl(location);
}

template <::std::integral char_type>
	requires(sizeof(char_type) == 1)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr char_type *
print_reserve_define(::fast_io::io_reserve_type_t<char_type, ::std::source_location>, char_type *dest, ::std::source_location const &location) noexcept
{
	return ::fast_io::details::prrsv_reserve_define_source_location_impl(dest, location);
}

namespace manipulators
{

inline
#ifdef __cpp_consteval
	consteval
#else
	constexpr
#endif
	::std::source_location
	cur_src_loc(::std::source_location loc = ::std::source_location::current()) noexcept
{
	return loc;
}

} // namespace manipulators

} // namespace fast_io
