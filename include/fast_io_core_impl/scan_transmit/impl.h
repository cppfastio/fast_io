#pragma once

namespace fast_io
{

enum class scan_transmit_operation
{
single,until
};

namespace manip
{

template<typename Func,scan_transmit_operation operation>
requires (std::invocable<Func,char8_t>)
struct scan_transmitter
{
	using function_type = Func;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	function_type reference;
};

}

template<typename Func>
requires (std::invocable<Func,char8_t>)
inline constexpr manip::scan_transmitter<Func,scan_transmit_operation::single> st_single(Func func)
{
	return {func};
}

template<typename Func>
requires (std::invocable<Func,char8_t>)
inline constexpr manip::scan_transmitter<Func,scan_transmit_operation::until> st_until(Func func)
{
	return {func};
}

namespace details
{
template<buffer_output_stream output,buffer_input_stream input,typename Func,scan_transmit_operation operation>
requires (std::invocable<Func,char8_t>)
inline constexpr void impl_svt(output& out,input& in,manip::scan_transmitter<Func,operation> func)
{
	static_assert(operation==scan_transmit_operation::until||operation==scan_transmit_operation::single);
	if constexpr(operation==scan_transmit_operation::until)
	{
		do
		{
			auto curr{ibuffer_curr(in)};
			auto ed{ibuffer_end(in)};
			if(curr!=ed)[[likely]]
			{
				auto i{::fast_io::freestanding::find_if(curr,ed,func.reference)};
				write(out,curr,i);
				ibuffer_set_curr(in,i);
				if(i!=ed)[[likely]]
					return;
			}
		}
		while(ibuffer_underflow(in));
	}
	else
	{
		auto curr{ibuffer_curr(in)};
		if(curr==ibuffer_end(in))[[unlikely]]
		{
			if(!ibuffer_underflow(in))
				return;
			curr=ibuffer_curr(in);
		}
		auto ch(*curr);
		if(func.reference(ch))
		{
			put(out,ch);
			ibuffer_set_curr(in,curr+1);
		}
		return;
	}
}

template<buffer_output_stream output,typename T,typename U,std::invocable<char8_t> Func,scan_transmit_operation operation>
inline constexpr void impl_svt_none_buffer(output& out,T& bg,U& ed,manip::scan_transmitter<Func,operation> func)
{
	static_assert(operation==scan_transmit_operation::until||operation==scan_transmit_operation::single);
	if constexpr(operation==scan_transmit_operation::until)
	{
		for(;bg!=ed&&!func.reference(*bg);++bg)
			put(out,*bg);
	}
	else
	{
		if(bg!=ed&&func.reference(*bg))
		{
			put(out,*bg);
			++bg;
		}
	}
}
}

template<buffer_output_stream output,input_stream input,typename... Args>
inline constexpr bool scan_transmit(output&& out,input&& in,Args&& ...args)
{
	using real_input_type = std::remove_cvref_t<input>;
	if constexpr(buffer_input_stream<real_input_type>)
	{
		auto curr{ibuffer_curr(in)};
		auto ed{ibuffer_end(in)};
		if(curr==ed)
		{
			if(!ibuffer_underflow(in))
				return false;
		}
		((details::impl_svt(out,in,::fast_io::freestanding::forward<Args>(args))),...);
		return true;
	}
	else
	{
		auto igen{igenerator(in)};
		auto bg{begin(igen)};
		auto ed{end(igen)};
		if(bg==ed)
			return false;
		((details::impl_svt_none_buffer(out,bg,ed,::fast_io::freestanding::forward<Args>(args))),...);
		return true;
	}
}

}
