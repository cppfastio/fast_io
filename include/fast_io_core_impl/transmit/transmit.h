#pragma once


namespace fast_io
{

template<typename T,typename P>
concept status_transmitable = requires(T&& t,P&& p)
{
	status_transmit_define(io_alias,::std::forward<T>(t),::std::forward<P>(p));
};

template<typename T,typename P>
concept zero_copy_transmitable = zero_copy_output_stream<T>&&zero_copy_input_stream<P>&&requires(T t,P p)
{
	zero_copy_transmit_define(io_alias,zero_copy_out_handle(t),zero_copy_in_handle(t));
};

namespace details
{

template<std::integral char_type>
inline constexpr std::size_t calculate_transmit_buffer_size() noexcept
{
#ifdef FAST_IO_BUFFER_SIZE
	static_assert(sizeof(char_type)>=FAST_IO_BUFFER_SIZE);
	static_assert(FAST_IO_BUFFER_SIZE<SIZE_MAX);
	return FAST_IO_BUFFER_SIZE/sizeof(char_type);
#else
	if constexpr(sizeof(std::size_t)<=sizeof(std::uint_least16_t))
	{
		return 4096/sizeof(char_type);	
	}
	else
	{
		return 131072/sizeof(char_type);
	}
#endif
}

template<std::integral char_type>
inline constexpr std::size_t transmit_buffer_size_cache{calculate_transmit_buffer_size<char_type>()};

}

template<output_stream output,input_stream input>
inline constexpr std::uintmax_t raw_transmit_decay(output outs,input ins)
{
	if constexpr(contiguous_input_stream<input>)
	{
		auto curr{ibuffer_curr(ins)};
		auto ed{ibuffer_end(ins)};
		write(outs,curr,ed);
		auto diff{ed-curr};
		ibuffer_set_curr(ins,ed);
		return static_cast<std::uintmax_t>(static_cast<std::size_t>(diff));
	}
	else if constexpr(buffer_input_stream<input>)
	{
		std::uintmax_t chars{};
		do
		{
			auto curr{ibuffer_curr(ins)};
			auto ed{ibuffer_end(ins)};
			if(curr==ed)[[unlikely]]
				continue;
			write(outs,curr,ed);
			chars+=static_cast<std::size_t>(ed-curr);
			ibuffer_set_curr(ins,ed);
		}
		while(ibuffer_underflow(ins));
		return chars;
	}
	else
	{
		using input_char_type = typename input::char_type;
		std::uintmax_t chars{};
		constexpr std::size_t buffer_size{details::transmit_buffer_size_cache<input_char_type>};
		::fast_io::details::buffer_alloc_arr_ptr<input_char_type,secure_clear_requirement_stream<input>||secure_clear_requirement_stream<output>> array_ptr(buffer_size);
		auto array_start{array_ptr.ptr},array_end{array_ptr.ptr+buffer_size};
		for(;;)
		{
			auto ed{read(ins,array_start,array_end)};
			if(array_start==ed)[[unlikely]]
				break;
			write(outs,array_start,ed);
			chars+=static_cast<std::size_t>(ed-array_start);
		}
		return chars;
	}
}

template<output_stream output,input_stream input>
inline constexpr decltype(auto) transmit_decay(output outs,input ins)
{
	if constexpr(mutex_stream<input>)
	{
		io_lock_guard lg{ins};
		decltype(auto) uh{ins.unlocked_handle()};
		return transmit_decay(outs,io_ref(uh));
	}
	else if constexpr(mutex_stream<output>)
	{
		io_lock_guard lg{outs};
		decltype(auto) uh{outs.unlocked_handle()};
		return transmit_decay(io_ref(uh),ins);
	}
	else if constexpr(zero_copy_transmitable<output,input>)
	{
		if constexpr(buffer_output_stream<output>&&!flush_output_stream<output>)
			return raw_transmit_decay(outs,ins);
		else
		{
			std::uintmax_t chars{};
			if constexpr(buffer_input_stream<input>)
			{
				auto curr{ibuffer_curr(ins)};
				auto ed{ibuffer_end(ins)};
				write(outs,curr,ed);
				chars+=static_cast<std::size_t>(ed-curr);
				ibuffer_set_curr(ins,ed);
			}
			if constexpr(buffer_output_stream<output>)
				flush(outs);
			return chars+zero_copy_transmit_define(io_alias,zero_copy_out_handle(outs),zero_copy_in_handle(ins));
		}
	}
	else if constexpr(output_stream<output>&&input_stream<input>)
		return raw_transmit_decay(outs,ins);
	else
	{
		constexpr bool no{output_stream<output>&&input_stream<input>};
		static_assert(no,"transmit must happen to output stream from input stream");
	}
}

template<output_stream output,input_stream input>
requires (std::is_trivially_copyable_v<output>&&std::is_trivially_copyable_v<input>)
inline constexpr std::uint_least64_t raw_transmit64_decay(output outs,input ins,std::uint_least64_t characters)
{
	if constexpr(contiguous_input_stream<input>)
	{
		auto curr{ibuffer_curr(ins)};
		auto ed{ibuffer_end(ins)};
		std::uint_least64_t this_round{static_cast<std::uint_least64_t>(ed-curr)};
		if(characters<=this_round)
		{
			this_round=characters;
		}
		ed=curr+this_round;
		write(outs,curr,ed);
		auto diff{ed-curr};
		ibuffer_set_curr(ins,ed);
		return this_round;
	}
	else if constexpr(buffer_input_stream<input>)
	{
		std::uint_least64_t chars{};
		do
		{
			auto curr{ibuffer_curr(ins)};
			auto ed{ibuffer_end(ins)};
			if(curr==ed)[[unlikely]]
				continue;
			std::uint_least64_t this_round{static_cast<std::uint_least64_t>(ed-curr)};
			bool transmit_enough_characters{characters<=this_round};
			if(transmit_enough_characters)
				this_round=characters;
			ed=curr+this_round;
			write(outs,curr,ed);
			ibuffer_set_curr(ins,ed);
			chars+=this_round;
			characters-=this_round;
			if(characters==0)
				break;
		}
		while(ibuffer_underflow(ins));
		return chars;
	}
	else
	{
		using input_char_type = typename input::char_type;
		std::uint_least64_t chars{};
		constexpr std::size_t buffer_size{details::transmit_buffer_size_cache<input_char_type>};
		std::size_t to_allocate{buffer_size};
		if(characters<to_allocate)
			to_allocate=static_cast<std::size_t>(characters);
		::fast_io::details::buffer_alloc_arr_ptr<input_char_type,secure_clear_requirement_stream<input>||secure_clear_requirement_stream<output>> array_ptr(to_allocate);
		auto array_start{array_ptr.ptr};
		for(;;)
		{
			std::uint_least64_t this_round{to_allocate};
			bool transmit_enough_characters{characters<=this_round};
			if(transmit_enough_characters)
				this_round=characters;
			if(this_round==0)
				break;
			auto ed{read(ins,array_start,array_start+this_round)};
			if(array_start==ed)[[unlikely]]
				break;
			write(outs,array_start,ed);
			std::uint_least64_t real_transmit_this_round{static_cast<std::uint_least64_t>(static_cast<std::size_t>(ed-array_start))};
			chars+=real_transmit_this_round;
			characters-=real_transmit_this_round;
			if(characters==0)
				break;
		}
		return chars;
	}
}

template<output_stream output,input_stream input>
requires (std::is_trivially_copyable_v<output>&&std::is_trivially_copyable_v<input>)
inline constexpr decltype(auto) transmit64_decay(output outs,input ins,std::uint_least64_t characters)
{
	if constexpr(mutex_stream<input>)
	{
		io_lock_guard lg{ins};
		decltype(auto) uh{ins.unlocked_handle()};
		return transmit64_decay(outs,io_ref(uh),characters);
	}
	else if constexpr(mutex_stream<output>)
	{
		io_lock_guard lg{outs};
		decltype(auto) uh{outs.unlocked_handle()};
		return transmit64_decay(io_ref(uh),ins,characters);
	}
	else if constexpr(zero_copy_transmitable<output,input>)
	{
		if constexpr(buffer_output_stream<output>&&!flush_output_stream<output>)
			return raw_transmit64_decay(outs,ins,characters);
		else
		{
			std::uint_least64_t chars{};
			if constexpr(buffer_input_stream<input>)
			{
				auto curr{ibuffer_curr(ins)};
				auto ed{ibuffer_end(ins)};
				std::size_t diff{static_cast<std::size_t>(ed-curr)};
				bool const no_need_further_transmit{characters<=diff};
				if(no_need_further_transmit)
				{
					diff=static_cast<std::size_t>(characters);
				}
				auto ed_ptr{curr+diff};
				write(outs,curr,curr+diff);
				chars+=diff;
				characters-=diff;
				ibuffer_set_curr(ins,ed_ptr);
				if(no_need_further_transmit)
					return chars;
			}
			if constexpr(buffer_output_stream<output>)
				flush(outs);
			return chars+zero_copy_transmit64_define(io_alias,zero_copy_out_handle(outs),zero_copy_in_handle(ins),characters);
		}
	}
	else if constexpr(output_stream<output>&&input_stream<input>)
		return raw_transmit64_decay(outs,ins,characters);
	else
	{
		constexpr bool no{output_stream<output>&&input_stream<input>};
		static_assert(no,"transmit must happen to output stream from input stream");
	}
}


template<typename output,typename input>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) transmit(output&& outs,input&& ins)
{
	if constexpr(status_transmitable<output,input>)
		return status_transmit_define(io_alias,::std::forward<output>(outs),::std::forward<input>(ins));
	else
		return transmit_decay(fast_io::io_ref(outs),fast_io::io_ref(ins));
}

template<typename output,typename input>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) transmit64(output&& outs,input&& ins,std::uint_least64_t characters=UINT_LEAST64_MAX)
{
	if constexpr(status_transmitable<output,input>)
		return status_transmit64_define(io_alias,::std::forward<output>(outs),::std::forward<input>(ins),characters);
	else
		return transmit64_decay(fast_io::io_ref(outs),fast_io::io_ref(ins),characters);
}

}
