#pragma once

namespace fast_io::operations::decay
{

template <bool line, typename outputstmtype, typename... Args>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr decltype(auto) print_freestanding_decay_cold(outputstmtype optstm, Args... args) FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::operations::decay::print_freestanding_decay<line>(optstm, args...))
{
#if !__has_cpp_attribute(__gnu__::__cold__)
	if (true) [[unlikely]]
#endif
		return ::fast_io::operations::decay::print_freestanding_decay<line>(optstm, args...);
}

} // namespace fast_io::operations::decay

namespace fast_io::operations::decay::defines
{

template <typename char_type, typename... Args>
concept print_freestanding_params_okay =
	::std::integral<char_type> &&
	((::fast_io::printable<char_type, Args> || ::fast_io::reserve_printable<char_type, Args> ||
	  ::fast_io::dynamic_reserve_printable<char_type, Args> || ::fast_io::scatter_printable<char_type, Args> ||
	  ::fast_io::context_printable<char_type, Args> ||
	  ::fast_io::transcode_imaginary_printable<char_type, Args> ||
	  ::std::same_as<::std::remove_cvref_t<Args>, ::fast_io::basic_io_scatter_t<char_type>> ||
	  ::std::same_as<::std::remove_cvref_t<Args>, ::fast_io::io_null_t>) &&
	 ...);

template <typename output, typename... Args>
concept print_freestanding_okay =
	::fast_io::operations::decay::defines::print_freestanding_params_okay<typename output::output_char_type, Args...>;

} // namespace fast_io::operations::decay::defines

namespace fast_io::operations::defines
{

template <typename char_type, typename... Args>
concept print_freestanding_params_okay = ::fast_io::operations::decay::defines::print_freestanding_params_okay<char_type,
																											   decltype(::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(::std::declval<Args>())))...>;

template <typename output, typename... Args>
concept print_freestanding_okay = ::fast_io::operations::decay::defines::print_freestanding_okay<
	decltype(::fast_io::operations::output_stream_ref(::std::declval<output>())),
	decltype(::fast_io::io_print_forward<typename decltype(::fast_io::operations::output_stream_ref(
				 ::std::declval<output>()))::output_char_type>(::fast_io::io_print_alias(::std::declval<Args>())))...>;

} // namespace fast_io::operations::defines
