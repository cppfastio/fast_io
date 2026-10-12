#pragma once

namespace fast_io::operations::decay::defines
{

template <typename T>
concept has_input_transmit_handle_define = requires(T instm) {
	{ input_transmit_handle_define(instm) };
};

template <typename T>
concept has_output_transmit_handle_define = requires(T outstm) {
	{ output_transmit_handle_define(outstm) };
};

template <typename outtype, typename intype>
concept has_transmit_some_bytes_overflow_underflow_define =
	requires(outtype outstm, ::fast_io::fpos_nullable_ptr off_out, intype instm,
			 ::fast_io::fpos_nullable_ptr off_in) {
		{
			transmit_some_bytes_overflow_underflow_define(outstm, off_out, instm, off_in, ::std::size_t{0})
		} -> ::std::same_as<::std::size_t>;
	};

template <typename outtype, typename intype>
concept has_transmit_all_bytes_overflow_underflow_define =
	requires(outtype outstm, ::fast_io::fpos_nullable_ptr off_out, intype instm,
			 ::fast_io::fpos_nullable_ptr off_in) {
		transmit_all_bytes_overflow_underflow_define(outstm, off_out, instm, off_in, ::fast_io::size_t_opt{});
	};

template <typename optstmtype, typename instmtype>
concept transmit_handle_some_bytes_able =
	::fast_io::operations::decay::defines::has_output_transmit_handle_define<optstmtype> &&
	::fast_io::operations::decay::defines::has_input_transmit_handle_define<instmtype> &&
	::fast_io::operations::decay::defines::has_transmit_some_bytes_overflow_underflow_define<
		decltype(output_transmit_handle_define(::std::declval<optstmtype>())),
		decltype(input_transmit_handle_define(::std::declval<instmtype>()))>;

template <typename optstmtype, typename instmtype>
concept transmit_handle_all_bytes_able =
	::fast_io::operations::decay::defines::has_output_transmit_handle_define<optstmtype> &&
	::fast_io::operations::decay::defines::has_input_transmit_handle_define<instmtype> &&
	(::fast_io::operations::decay::defines::has_transmit_all_bytes_overflow_underflow_define<
		 decltype(output_transmit_handle_define(::std::declval<optstmtype>())),
		 decltype(input_transmit_handle_define(::std::declval<instmtype>()))> ||
	 ::fast_io::operations::decay::defines::has_transmit_some_bytes_overflow_underflow_define<
		 decltype(output_transmit_handle_define(::std::declval<optstmtype>())),
		 decltype(input_transmit_handle_define(::std::declval<instmtype>()))>);

} // namespace fast_io::operations::decay::defines
