#pragma once

namespace fast_io
{

namespace details
{

template<::fast_io::output_stream optstmtype,
	::fast_io::input_stream instmtype>
requires (sizeof(typename optstmtype::output_char_type)==sizeof(typename instmtype::input_char_type))
inline constexpr ::fast_io::uintfpos_t transmit_some_main_impl(optstmtype optstm,instmtype instm,::fast_io::uintfpos_t needtransmit)
{
/*
A dummy placeholder implementation
*/
	using input_char_type = typename instmtype::input_char_type;
	using output_char_type = typename optstmtype::output_char_type;
	constexpr
		::std::size_t bfsz{::fast_io::details::transmit_buffer_size_cache<sizeof(input_char_type)>};
	::fast_io::details::local_operator_new_array_ptr<input_char_type> newptr(bfsz);
	input_char_type *buffer_start{newptr.ptr};
	input_char_type *buffer_end{newptr.ptr+newptr.size};
	::fast_io::uintfpos_t totransmit{needtransmit};
	while(totransmit)
	{
		::std::size_t this_round{bfsz};
		if(totransmit<this_round)
		{
			this_round=static_cast<::std::size_t>(totransmit);
		}
		input_char_type *iter{::fast_io::operations::decay::read_some_decay(instm,buffer_start,buffer_start+this_round)};
		if(iter==buffer_start)
			break;
		if constexpr(::std::same_as<output_char_type,input_char_type>)
		{
			::fast_io::operations::decay::write_all_decay(optstm,buffer_start,iter);
		}
		else
		{
			using output_char_type_may_alias_const_ptrtp
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= output_char_type const*;
			::fast_io::operations::decay::write_all_decay(optstm,
				reinterpret_cast<output_char_type_may_alias_const_ptrtp>(buffer_start),
				reinterpret_cast<output_char_type_may_alias_const_ptrtp>(iter));
		}
		totransmit-=static_cast<::std::size_t>(iter-buffer_start);
	}
	return needtransmit-totransmit;
}

}

namespace operations
{

namespace decay
{

template<::fast_io::output_stream optstmtype,
	::fast_io::input_stream instmtype>
inline constexpr decltype(auto) transmit_some_decay(optstmtype optstm,instmtype instm,::fast_io::uintfpos_t totransmit)
{
	if constexpr(::fast_io::status_output_stream<optstmtype>)
	{
		return status_transmit_some_define(optstm,instm,totransmit);
	}
	else if constexpr(::fast_io::status_input_stream<instmtype>)
	{
		return status_transmit_some_define(optstm,instm,totransmit);
	}
	else if constexpr(::fast_io::details::has_output_or_io_stream_mutex_ref_define<optstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::output_stream_mutex_ref_decay(optstm)};
		return ::fast_io::operations::decay::transmit_some_decay(::fast_io::operations::decay::output_stream_unlocked_ref_decay(optstm),instm,totransmit);
	}
	else if constexpr(::fast_io::details::has_input_or_io_stream_mutex_ref_define<instmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::input_stream_mutex_ref_decay(instm)};
		return ::fast_io::operations::decay::transmit_some_decay(optstm,
			::fast_io::operations::decay::input_stream_unlocked_ref_decay(instm),totransmit);
	}
	else
	{
		return ::fast_io::details::transmit_some_main_impl(optstm,instm,totransmit);
	}
}

}

template<typename optstmtype,typename instmtype>
inline constexpr decltype(auto) transmit_some(optstmtype &&optstm,instmtype &&instm,::fast_io::uintfpos_t totransmit)
{
	return ::fast_io::operations::decay::transmit_some_decay(::fast_io::manipulators::output_stream_ref(optstm),
		::fast_io::manipulators::input_stream_ref(instm),totransmit);
}

}

}
