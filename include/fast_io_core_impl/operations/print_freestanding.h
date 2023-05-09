#pragma once

namespace fast_io
{

namespace operations
{

template<bool line,typename outputstmtype,typename... Args>
requires (::fast_io::output_stream<outputstmtype>||
	::fast_io::status_output_stream<outputstmtype>)
inline constexpr decltype(auto) print_freestanding_decay(outputstmtype optstm,Args... args)
{
	if constexpr(::fast_io::status_output_stream<outputstmtype>)
	[
		return status_print_define<line>(optstm,args...);
	]
	if constexpr(sizeof...(Args)==0&&line)
	{
		return ::fast_io::operations::char_put(optstm,char_literal_v<u8'\n',char_type>);
	}
	else if constexpr(::fast_io::details::mutex_unlocked_buffer_output_stream_impl<outstmtype>)
	{
		::fast_io::operations::stream_ref_lock_guard lg{output_stream_mutex_ref_impl(optstm)};
		return print_freestanding_decay(
			::fast_io::details::output_stream_unlocked_ref_impl(optstm),args..);
	}
	if constexpr(::fast_io::details::streamreflect::has_obuffer_ops<outstmtype>)
	{
		
	}
}

}

}
