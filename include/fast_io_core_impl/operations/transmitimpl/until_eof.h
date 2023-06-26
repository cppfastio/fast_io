#pragma once

namespace fast_io
{

namespace details
{

template<typename optstmtype,
	typename instmtype,
	typename T>
requires (sizeof(typename optstmtype::output_char_type)==sizeof(typename instmtype::input_char_type))
inline constexpr void transmit_until_eof_generic_main_impl(optstmtype optstm,instmtype instm,T resultint)
{
/*
A dummy placeholder implementation
*/
	using input_char_type = typename instmtype::input_char_type;
	using output_char_type = typename optstmtype::output_char_type;
	::fast_io::details::local_operator_new_array_ptr<input_char_type> newptr(
		::fast_io::details::transmit_buffer_size_cache<sizeof(input_char_type)>
	);
	input_char_type *buffer_start{newptr.ptr};
	input_char_type *buffer_end{newptr.ptr+newptr.size};
	for(input_char_type *iter;(iter=::fast_io::operations::decay::read_some_decay(instm,buffer_start,buffer_end))!=buffer_start;)
	{
		::std::size_t off{static_cast<::std::size_t>(iter-buffer_start)};
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
		transmit_integer_add_define(resultint,off);
	}
}

template<typename optstmtype,
	typename instmtype>
inline constexpr ::fast_io::transmit_result transmit_until_eof_main_impl(optstmtype optstm,instmtype instm)
{
	::fast_io::uintfpos_t transmitted{};
	uintfpos_transmit_reference_wrapper wrapper{__builtin_addressof(transmitted)};
	::fast_io::details::transmit_until_eof_generic_main_impl(optstm,instm,wrapper);
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
inline constexpr decltype(auto) transmit_until_eof_generic_decay(optstmtype optstm,instmtype instm,T resultint)
{
#if 0
	if constexpr(::fast_io::status_output_stream<optstmtype>)
	{
		return status_transmit_until_eof_generic_define(
			optstm,instm,resultint);
	}
	else if constexpr(::fast_io::status_input_stream<instmtype>)
	{
		return status_transmit_until_eof_generic_define(
			optstm,instm,resultint);
	}
	else
#endif
	if constexpr(::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<optstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::output_stream_mutex_ref_decay(optstm)};
		return ::fast_io::operations::decay::transmit_until_eof_generic_decay(::fast_io::operations::decay::output_stream_unlocked_ref_decay(optstm),
			instm,resultint);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::input_stream_mutex_ref_decay(instm)};
		return ::fast_io::operations::decay::transmit_until_eof_generic_decay(optstm,
			::fast_io::operations::decay::input_stream_unlocked_ref_decay(instm),
			resultint);
	}
	else
	{
		return ::fast_io::details::transmit_until_eof_generic_main_impl(optstm,instm,resultint);
	}
}

template<typename optstmtype,
	typename instmtype>
inline constexpr decltype(auto) transmit_until_eof_decay(optstmtype optstm,instmtype instm)
{
#if 0
	if constexpr(::fast_io::status_output_stream<optstmtype>)
	{
		return status_transmit_until_eof_define(optstm,instm);
	}
	else if constexpr(::fast_io::status_input_stream<instmtype>)
	{
		return status_transmit_until_eof_define(optstm,instm);
	}
	else
#endif
	if constexpr(::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<optstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::output_stream_mutex_ref_decay(optstm)};
		return ::fast_io::operations::decay::transmit_until_eof_decay(
			::fast_io::operations::decay::output_stream_unlocked_ref_decay(optstm),
			instm);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::input_stream_mutex_ref_decay(instm)};
		return ::fast_io::operations::decay::transmit_until_eof_decay(optstm,
			::fast_io::operations::decay::input_stream_unlocked_ref_decay(optstm));
	}
	else
	{
		return ::fast_io::details::transmit_until_eof_main_impl(optstm,instm);
	}
}

}

template<typename optstmtype,typename instmtype,typename T>
inline constexpr decltype(auto) transmit_until_eof_generic(optstmtype &&optstm,instmtype &&instm,T resultint)
{
	return ::fast_io::operations::decay::transmit_until_eof_generic_decay(::fast_io::operations::output_stream_ref(optstm),
		::fast_io::operations::input_stream_ref(instm),resultint);
}

template<typename optstmtype,typename instmtype>
inline constexpr decltype(auto) transmit_until_eof(optstmtype &&optstm,instmtype &&instm)
{
	return ::fast_io::operations::decay::transmit_until_eof_decay(::fast_io::operations::output_stream_ref(optstm),
		::fast_io::operations::input_stream_ref(instm));
}

}

}
