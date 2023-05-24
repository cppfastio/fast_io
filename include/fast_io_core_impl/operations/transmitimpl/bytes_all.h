#pragma once

namespace fast_io
{

namespace details
{

template<::fast_io::output_stream optstmtype,
	::fast_io::input_stream instmtype>
inline constexpr void transmit_bytes_all_main_impl(optstmtype optstm,instmtype instm,::fast_io::uintfpos_t totransmit)
{
/*
A dummy placeholder implementation
*/
	constexpr
		::std::size_t bfsz{::fast_io::details::transmit_buffer_size_cache<1>};
	::fast_io::details::local_operator_new_array_ptr<::std::byte> newptr(bfsz);
	::std::byte *buffer_start{newptr.ptr};
	while(totransmit)
	{
		::std::size_t this_round{bfsz};
		if(totransmit<this_round)
		{
			this_round=static_cast<::std::size_t>(totransmit);
		}
		auto iter{buffer_start+this_round};
		::fast_io::operations::decay::read_all_bytes_decay(instm,buffer_start,iter);
		::fast_io::operations::decay::write_all_bytes_decay(optstm,buffer_start,iter);
		totransmit-=this_round;
	}
}

}

namespace operations
{

namespace decay
{

template<::fast_io::output_stream optstmtype,
	::fast_io::input_stream instmtype>
inline constexpr decltype(auto) transmit_bytes_all_decay(optstmtype optstm,instmtype instm,::fast_io::uintfpos_t totransmit)
{
	if constexpr(::fast_io::status_output_stream<optstmtype>)
	{
		return status_transmit_bytes_all_define(optstm,instm,totransmit);
	}
	else if constexpr(::fast_io::status_input_stream<instmtype>)
	{
		return status_transmit_bytes_all_define(optstm,instm,totransmit);
	}
	else if constexpr(::fast_io::details::mutex_unlocked_buffer_output_stream_impl<optstmtype>)
	{
		::fast_io::operations::stream_ref_lock_guard lg{output_stream_mutex_ref_impl(optstm)};
		return ::fast_io::operations::decay::transmit_bytes_all_decay(optstm,instm,totransmit);
	}
	else if constexpr(::fast_io::details::mutex_unlocked_buffer_input_stream_impl<instmtype>)
	{
		::fast_io::operations::stream_ref_lock_guard lg{input_stream_mutex_ref_impl(instm)};
		return ::fast_io::operations::decay::transmit_bytes_all_decay(optstm,instm,totransmit);
	}
	else
	{
		return ::fast_io::details::transmit_bytes_all_main_impl(optstm,instm,totransmit);
	}
}

}

template<typename optstmtype,typename instmtype>
inline constexpr decltype(auto) transmit_bytes_all(optstmtype &&optstm,instmtype &&instm,::fast_io::uintfpos_t totransmit)
{
	return ::fast_io::operations::decay::transmit_bytes_all_decay(::fast_io::manipulators::output_stream_ref(optstm),
		::fast_io::manipulators::input_stream_ref(instm),totransmit);
}

}

}
