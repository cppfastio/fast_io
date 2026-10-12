#pragma once

namespace fast_io
{

namespace details
{

template <typename optstmtype, typename instmtype>
inline constexpr void transmit_all_bytes_emulation_impl(
	optstmtype optstm, instmtype instm,
	::fast_io::fpos_nullable_ptr off_out, ::fast_io::fpos_nullable_ptr off_in, ::fast_io::size_t_opt totransmit)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype> ||
								   !::fast_io::operations::decay::defines::output_stream_operations_nothrow<optstmtype>)
{
	for (;;)
	{
		::std::size_t want{::std::numeric_limits<::std::size_t>::max()};
		if (totransmit.has_opt)
		{
			want = totransmit.opt;
			if (want == 0)
			{
				return;
			}
		}
		::std::size_t got{
			::fast_io::details::transmit_some_bytes_emulation_impl(optstm, instm, off_out, off_in, want)};
		if (got == 0)
		{
			return;
		}
		if (totransmit.has_opt)
		{
			totransmit.opt -= got;
		}
	}
}

template <typename optstmtype, typename instmtype>
inline constexpr void transmit_all_bytes_main_impl(
	optstmtype optstm, instmtype instm,
	::fast_io::fpos_nullable_ptr off_out, ::fast_io::fpos_nullable_ptr off_in, ::fast_io::size_t_opt totransmit)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype> ||
								   !::fast_io::operations::decay::defines::output_stream_operations_nothrow<optstmtype>)
{
	constexpr bool transmit_handle_able{
		::fast_io::operations::decay::defines::transmit_handle_all_bytes_able<optstmtype, instmtype> &&
		(!::fast_io::operations::decay::defines::has_obuffer_basic_operations<optstmtype> ||
		 ::fast_io::operations::decay::defines::has_output_or_io_stream_buffer_flush_define<optstmtype>)};
	if constexpr (transmit_handle_able)
	{
		if (off_in.ptr == nullptr)
		{
			if constexpr (::fast_io::operations::decay::defines::has_ibuffer_basic_operations<instmtype>)
			{
				::std::size_t bound{totransmit.has_opt ? totransmit.opt
													   : ::std::numeric_limits<::std::size_t>::max()};
				::std::size_t moved{transmit_ibuffer_forward_impl(optstm, instm, bound)};
				if (totransmit.has_opt)
				{
					totransmit.opt -= moved;
					if (totransmit.opt == 0)
					{
						return;
					}
				}
			}
		}
		if constexpr (::fast_io::operations::decay::defines::has_obuffer_basic_operations<optstmtype>)
		{
			::fast_io::operations::decay::output_stream_buffer_flush_decay(optstm);
		}
		auto outhdl{output_transmit_handle_define(optstm)};
		auto inhdl{input_transmit_handle_define(instm)};
		using outhdltp = decltype(outhdl);
		using inhdltp = decltype(inhdl);
		if constexpr (::fast_io::operations::decay::defines::has_transmit_all_bytes_overflow_underflow_define<
						  outhdltp, inhdltp>)
		{
			transmit_all_bytes_overflow_underflow_define(outhdl, off_out, inhdl, off_in, totransmit);
		}
		else
		{
			for (;;)
			{
				::std::size_t want{totransmit.has_opt ? totransmit.opt
													  : ::std::numeric_limits<::std::size_t>::max()};
				if (want == 0)
				{
					return;
				}
				::std::size_t got{transmit_some_bytes_overflow_underflow_define(outhdl, off_out, inhdl, off_in, want)};
				if (got == 0)
				{
					return;
				}
				if (totransmit.has_opt)
				{
					totransmit.opt -= got;
				}
			}
		}
	}
	else
	{
		::fast_io::details::transmit_all_bytes_emulation_impl(optstm, instm, off_out, off_in, totransmit);
	}
}

} // namespace details

namespace operations
{

namespace decay
{

template <typename optstmtype, typename instmtype>
inline constexpr void transmit_all_bytes_decay(
	optstmtype optstm, ::fast_io::fpos_nullable_ptr off_out,
	instmtype instm, ::fast_io::fpos_nullable_ptr off_in, ::fast_io::size_t_opt totransmit)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype> ||
								   !::fast_io::operations::decay::defines::output_stream_operations_nothrow<optstmtype>)
{
	if constexpr (::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<optstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{
			::fast_io::operations::decay::output_stream_mutex_ref_decay(optstm)};
		::fast_io::operations::decay::transmit_all_bytes_decay(
			::fast_io::operations::decay::output_stream_unlocked_ref_decay(optstm), off_out, instm, off_in, totransmit);
	}
	else if constexpr (::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{
			::fast_io::operations::decay::input_stream_mutex_ref_decay(instm)};
		::fast_io::operations::decay::transmit_all_bytes_decay(
			optstm, off_out, ::fast_io::operations::decay::input_stream_unlocked_ref_decay(instm), off_in, totransmit);
	}
	else
	{
		::fast_io::details::transmit_all_bytes_main_impl(optstm, instm, off_out, off_in, totransmit);
	}
}

} // namespace decay

template <typename optstmtype, typename instmtype>
inline constexpr void transmit_all_bytes(optstmtype &&optstm, ::fast_io::fpos_nullable_ptr off_out,
										 instmtype &&instm, ::fast_io::fpos_nullable_ptr off_in,
										 ::fast_io::size_t_opt totransmit)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::defines::input_stream_operations_nothrow<instmtype> ||
								   !::fast_io::operations::defines::output_stream_operations_nothrow<optstmtype>)
{
	::fast_io::operations::decay::transmit_all_bytes_decay(
		::fast_io::operations::output_stream_ref(optstm), off_out,
		::fast_io::operations::input_stream_ref(instm), off_in, totransmit);
}

} // namespace operations

} // namespace fast_io
