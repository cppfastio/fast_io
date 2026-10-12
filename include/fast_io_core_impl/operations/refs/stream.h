#pragma once

namespace fast_io::operations::decay
{

template <typename instmtype>
concept input_stream_decay =
	::fast_io::operations::decay::defines::readable<instmtype> ||
	::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>;

template <typename outstmtype>
concept output_stream_decay =
	::fast_io::operations::decay::defines::writable<outstmtype> ||
	::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>;

template <typename stmtype>
concept io_stream_decay =
	::fast_io::operations::decay::input_stream_decay<stmtype> &&
	::fast_io::operations::decay::output_stream_decay<stmtype>;

template <typename instmtype>
concept buffer_input_stream_decay =
	::fast_io::operations::decay::input_stream_decay<instmtype> &&
	::fast_io::operations::decay::defines::has_ibuffer_basic_operations<instmtype>;

template <typename outstmtype>
concept buffer_output_stream_decay =
	::fast_io::operations::decay::output_stream_decay<outstmtype> &&
	::fast_io::operations::decay::defines::has_obuffer_basic_operations<outstmtype>;

template <typename stmtype>
concept buffer_io_stream_decay =
	::fast_io::operations::decay::io_stream_decay<stmtype> &&
	::fast_io::operations::decay::buffer_input_stream_decay<stmtype> &&
	::fast_io::operations::decay::buffer_output_stream_decay<stmtype>;

} // namespace fast_io::operations::decay

namespace fast_io
{

template <typename stmtype>
concept input_stream =
	::fast_io::operations::defines::has_input_or_io_stream_ref_define<stmtype> &&
	::fast_io::operations::decay::input_stream_decay<decltype(::fast_io::operations::input_stream_ref(
		::std::declval<stmtype>()))>;

template <typename stmtype>
concept output_stream =
	::fast_io::operations::defines::has_output_or_io_stream_ref_define<stmtype> &&
	::fast_io::operations::decay::output_stream_decay<decltype(::fast_io::operations::output_stream_ref(
		::std::declval<stmtype>()))>;

template <typename stmtype>
concept io_stream =
	::fast_io::input_stream<stmtype> && ::fast_io::output_stream<stmtype> &&
	::fast_io::operations::defines::has_io_stream_ref_define<stmtype>;

template <typename stmtype>
concept buffer_input_stream =
	::fast_io::input_stream<stmtype> &&
	::fast_io::operations::decay::buffer_input_stream_decay<decltype(::fast_io::operations::input_stream_ref(
		::std::declval<stmtype>()))>;

template <typename stmtype>
concept buffer_output_stream =
	::fast_io::output_stream<stmtype> &&
	::fast_io::operations::decay::buffer_output_stream_decay<decltype(::fast_io::operations::output_stream_ref(
		::std::declval<stmtype>()))>;

template <typename stmtype>
concept buffer_io_stream = ::fast_io::io_stream<stmtype> && ::fast_io::buffer_input_stream<stmtype> &&
						   ::fast_io::buffer_output_stream<stmtype>;

} // namespace fast_io
