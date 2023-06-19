#pragma once

namespace fast_io
{

namespace details
{

template<typename optstmtype,
	typename instmtype,
	typename T>
inline constexpr void transmit_bytes_until_eof_generic_main_impl(optstmtype optstm,instmtype instm,T resultint)
{
/*
A dummy placeholder implementation
*/
	using input_char_type = typename instmtype::input_char_type;
	::fast_io::details::local_operator_new_array_ptr<input_char_type> newptr(
		::fast_io::details::transmit_buffer_size_cache<sizeof(input_char_type)>
	);
	input_char_type *buffer_start{newptr.ptr};
	input_char_type *buffer_end{newptr.ptr+newptr.size};
	for(input_char_type *iter;(iter=::fast_io::operations::decay::read_some_decay(instm,buffer_start,buffer_end))!=buffer_start;)
	{
		auto bufferstartpbyte{reinterpret_cast<::std::byte const*>(buffer_start)};
		auto iterpbyte{reinterpret_cast<::std::byte const*>(iter)};
		::std::size_t off{static_cast<::std::size_t>(iterpbyte-bufferstartpbyte)};
		::fast_io::operations::decay::write_all_bytes_decay(optstm,bufferstartpbyte,iterpbyte);
		transmit_integer_add_define(resultint,off);
	}
}

template<typename optstmtype,
	typename instmtype>
inline constexpr ::fast_io::transmit_result transmit_bytes_until_eof_main_impl(optstmtype optstm,instmtype instm)
{
	::fast_io::uintfpos_t transmitted{};
	uintfpos_transmit_reference_wrapper wrapper{__builtin_addressof(transmitted)};
	::fast_io::details::transmit_bytes_until_eof_generic_main_impl(optstm,instm,wrapper);
	return {transmitted};
}

}

namespace operations
{

namespace decay
{

template<typename optstmtype,
	typename instmtype,
	typename T>
inline constexpr decltype(auto) transmit_bytes_until_eof_generic_decay(optstmtype optstm,instmtype instm,T resultint)
{
#if 0
	if constexpr(::fast_io::status_output_stream<optstmtype>)
	{
		return status_transmit_bytes_until_eof_generic_define(
			optstm,instm,resultint);
	}
	else if constexpr(::fast_io::status_input_stream<instmtype>)
	{
		return status_transmit_bytes_until_eof_generic_define(
			optstm,instm,resultint);
	}
	else 
#endif
	if constexpr(::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<optstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::output_stream_mutex_ref_decay(optstm)};
		return ::fast_io::operations::decay::transmit_bytes_until_eof_generic_decay(
			::fast_io::operations::decay::output_stream_unlocked_ref_decay(optstm),
			instm,resultint);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::input_stream_mutex_ref_decay(instm)};
		return ::fast_io::operations::decay::transmit_bytes_until_eof_generic_decay(optstm,
			::fast_io::operations::decay::input_stream_unlocked_ref_decay(instm),resultint);
	}
	else
	{
		return ::fast_io::details::transmit_bytes_until_eof_generic_main_impl(optstm,instm,resultint);
	}
}

template<typename optstmtype,
	typename instmtype>
inline constexpr decltype(auto) transmit_bytes_until_eof_decay(optstmtype optstm,instmtype instm)
{
#if 0
	if constexpr(::fast_io::status_output_stream<optstmtype>)
	{
		return status_transmit_bytes_until_eof_define(optstm,instm);
	}
	else if constexpr(::fast_io::status_input_stream<instmtype>)
	{
		return status_transmit_bytes_until_eof_define(optstm,instm);
	}
	else
#endif
	if constexpr(::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<optstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::output_stream_mutex_ref_decay(optstm)};
		return ::fast_io::operations::decay::transmit_bytes_until_eof_decay(
			::fast_io::operations::decay::output_stream_unlocked_ref_decay(optstm),instm);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::input_stream_mutex_ref_decay(instm)};
		return ::fast_io::operations::decay::transmit_bytes_until_eof_decay(optstm,
			::fast_io::operations::decay::input_stream_unlocked_ref_decay(instm));
	}
	else
	{
		return ::fast_io::details::transmit_bytes_until_eof_main_impl(optstm,instm);
	}
}

}

template<typename optstmtype,typename instmtype,typename T>
inline constexpr decltype(auto) transmit_bytes_until_eof_generic(optstmtype &&optstm,instmtype &&instm,T resultint)
{
	return ::fast_io::operations::decay::transmit_bytes_until_eof_generic_decay(::fast_io::operations::output_stream_ref(optstm),
		::fast_io::operations::input_stream_ref(instm),resultint);
}

template<typename optstmtype,typename instmtype>
inline constexpr decltype(auto) transmit_bytes_until_eof(optstmtype &&optstm,instmtype &&instm)
{
	return ::fast_io::operations::decay::transmit_bytes_until_eof_decay(::fast_io::operations::output_stream_ref(optstm),
		::fast_io::operations::input_stream_ref(instm));
}

}

}
