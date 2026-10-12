#pragma once

namespace fast_io
{

namespace details
{

/*
Single-round emulation for transmit_some_bytes: read/pread into a temporary
buffer, then write_all/pwrite_all what was read. Returns bytes transferred, 0 on
EOF. Matches the "bytes actually copied" semantics of copy_file_range(2) — a byte
is only counted once it has actually reached the output, so short writes never
lose input data.
*/
template <typename optstmtype, typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr ::std::size_t transmit_some_bytes_emulation_impl(
	optstmtype optstm, instmtype instm,
	::fast_io::fpos_nullable_ptr off_out, ::fast_io::fpos_nullable_ptr off_in, ::std::size_t size)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype> ||
								   !::fast_io::operations::decay::defines::output_stream_operations_nothrow<optstmtype>)
{
	constexpr ::std::size_t bfsz{::fast_io::details::transmit_buffer_size_cache<1>};
	::std::size_t this_round{size < bfsz ? size : bfsz};
	if (this_round == 0)
	{
		return 0;
	}
	::fast_io::details::local_operator_new_array_ptr<::std::byte> newptr(this_round);
	::std::byte *buffer_start{newptr.ptr};
	::std::byte *iter;
	if constexpr (::fast_io::operations::decay::defines::bytes_preadable<instmtype>)
	{
		if (off_in.ptr != nullptr)
		{
			iter = ::fast_io::operations::decay::pread_some_bytes_decay(instm, buffer_start, this_round, *off_in.ptr);
		}
		else
		{
			iter = ::fast_io::operations::decay::read_some_bytes_decay(instm, buffer_start, this_round);
		}
	}
	else
	{
		if (off_in.ptr != nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::invalid_seek);
		}
		iter = ::fast_io::operations::decay::read_some_bytes_decay(instm, buffer_start, this_round);
	}
	::std::size_t got{static_cast<::std::size_t>(iter - buffer_start)};
	if (got == 0)
	{
		return 0;
	}
	if constexpr (::fast_io::operations::decay::defines::bytes_pwritable<optstmtype>)
	{
		if (off_out.ptr != nullptr)
		{
			::fast_io::operations::decay::pwrite_all_bytes_decay(optstm, buffer_start, got, *off_out.ptr);
		}
		else
		{
			::fast_io::operations::decay::write_all_bytes_decay(optstm, buffer_start, got);
		}
	}
	else
	{
		if (off_out.ptr != nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::invalid_seek);
		}
		::fast_io::operations::decay::write_all_bytes_decay(optstm, buffer_start, got);
	}
	if (off_in.ptr != nullptr)
	{
		*off_in.ptr = ::fast_io::fposoffadd_nonegative(*off_in.ptr, got);
	}
	if (off_out.ptr != nullptr)
	{
		*off_out.ptr = ::fast_io::fposoffadd_nonegative(*off_out.ptr, got);
	}
	return got;
}

/*
Write out the pending input-buffer contents (the read-ahead window), bounded by
size. Must be done before any fd-level zero-copy transfer which would bypass and
overtake the buffered data. Returns bytes forwarded.
*/
template <typename optstmtype, typename instmtype>
inline constexpr ::std::size_t transmit_ibuffer_forward_impl(optstmtype optstm, instmtype instm, ::std::size_t size)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<optstmtype>)
	requires(::fast_io::operations::decay::defines::has_ibuffer_basic_operations<instmtype>)
{
	using input_char_type = typename instmtype::input_char_type;
	auto curr{ibuffer_curr(instm)};
	::std::size_t buffered_chars{static_cast<::std::size_t>(ibuffer_end(instm) - curr)};
	::std::size_t take_chars{size / sizeof(input_char_type)};
	if (buffered_chars < take_chars)
	{
		take_chars = buffered_chars;
	}
	if (take_chars == 0)
	{
		return 0;
	}
	::std::size_t bytes{take_chars * sizeof(input_char_type)};
	using ibyte_constptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= ::std::byte const *;
	::fast_io::operations::decay::write_all_bytes_decay(optstm, reinterpret_cast<ibyte_constptr>(curr), bytes);
	ibuffer_set_curr(instm, curr + take_chars);
	return bytes;
}

template <typename optstmtype, typename instmtype>
inline constexpr ::std::size_t transmit_some_bytes_main_impl(
	optstmtype optstm, instmtype instm,
	::fast_io::fpos_nullable_ptr off_out, ::fast_io::fpos_nullable_ptr off_in, ::std::size_t size)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype> ||
								   !::fast_io::operations::decay::defines::output_stream_operations_nothrow<optstmtype>)
{
	/*
	Buffer-state rules before forwarding to the fd-level define:
	- ibuffered input data is written through the output stream first (it
	  precedes whatever the fd position would still yield); with an explicit
	  off_in the buffer window is unrelated to the requested position, so it is
	  left alone;
	- pending obuffered output must be flushed before a fd-level transmit so it
	  is not reordered after the transmitted bytes; a stream we cannot flush is
	  not eligible for the zero-copy path at all.
	*/
	constexpr bool transmit_handle_able{
		::fast_io::operations::decay::defines::transmit_handle_some_bytes_able<optstmtype, instmtype> &&
		(!::fast_io::operations::decay::defines::has_obuffer_basic_operations<optstmtype> ||
		 ::fast_io::operations::decay::defines::has_output_or_io_stream_buffer_flush_define<optstmtype>)};
	if constexpr (transmit_handle_able)
	{
		::std::size_t transmitted{};
		if (size != 0 && off_in.ptr == nullptr)
		{
			if constexpr (::fast_io::operations::decay::defines::has_ibuffer_basic_operations<instmtype>)
			{
				::std::size_t moved{transmit_ibuffer_forward_impl(optstm, instm, size)};
				transmitted = moved;
				size -= moved;
			}
		}
		if (size == 0)
		{
			return transmitted;
		}
		if constexpr (::fast_io::operations::decay::defines::has_obuffer_basic_operations<optstmtype>)
		{
			::fast_io::operations::decay::output_stream_buffer_flush_decay(optstm);
		}
		return transmitted +
			   transmit_some_bytes_overflow_underflow_define(output_transmit_handle_define(optstm), off_out,
															 input_transmit_handle_define(instm), off_in, size);
	}
	else
	{
		return ::fast_io::details::transmit_some_bytes_emulation_impl(optstm, instm, off_out, off_in, size);
	}
}

} // namespace details

namespace operations
{

namespace decay
{

template <typename optstmtype, typename instmtype>
inline constexpr ::std::size_t transmit_some_bytes_decay(
	optstmtype optstm, ::fast_io::fpos_nullable_ptr off_out,
	instmtype instm, ::fast_io::fpos_nullable_ptr off_in, ::std::size_t size)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype> ||
								   !::fast_io::operations::decay::defines::output_stream_operations_nothrow<optstmtype>)
{
	if constexpr (::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<optstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{
			::fast_io::operations::decay::output_stream_mutex_ref_decay(optstm)};
		return ::fast_io::operations::decay::transmit_some_bytes_decay(
			::fast_io::operations::decay::output_stream_unlocked_ref_decay(optstm), off_out, instm, off_in, size);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{
			::fast_io::operations::decay::input_stream_mutex_ref_decay(instm)};
		return ::fast_io::operations::decay::transmit_some_bytes_decay(
			optstm, off_out, ::fast_io::operations::decay::input_stream_unlocked_ref_decay(instm), off_in, size);
	}
	else
	{
		return ::fast_io::details::transmit_some_bytes_main_impl(optstm, instm, off_out, off_in, size);
	}
}

} // namespace decay

template <typename optstmtype, typename instmtype>
inline constexpr ::std::size_t transmit_some_bytes(optstmtype &&optstm, ::fast_io::fpos_nullable_ptr off_out,
												   instmtype &&instm, ::fast_io::fpos_nullable_ptr off_in,
												   ::std::size_t size)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::defines::input_stream_operations_nothrow<instmtype> ||
								   !::fast_io::operations::defines::output_stream_operations_nothrow<optstmtype>)
{
	return ::fast_io::operations::decay::transmit_some_bytes_decay(
		::fast_io::operations::output_stream_ref(optstm), off_out,
		::fast_io::operations::input_stream_ref(instm), off_in, size);
}

} // namespace operations

} // namespace fast_io
