#pragma once

namespace fast_io
{

template<input_stream T>
requires std::is_trivially_copyable_v<T>
struct unget_temp_buffer
{
	using char_type = typename T::char_type;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	T input;
	typename T::char_type buffer;
	char8_t pos{};
	char8_t pos_end{};
	constexpr unget_temp_buffer(T in):input(in){}
	unget_temp_buffer(unget_temp_buffer const&)=delete;
	unget_temp_buffer& operator=(unget_temp_buffer const&)=delete;
#ifdef __clang__
	constexpr ~unget_temp_buffer()
	{
		if constexpr(requires()
		{
			try_unget(input,buffer);
		})
		{
#ifdef __cpp_exceptions
		if constexpr(noexcept(try_unget(input,buffer)))
		{
			if(pos!=pos_end)
				try_unget(input,buffer);
		}
		else
		{
			try
			{
				if(pos!=pos_end)
					try_unget(input,buffer);
			}
			catch(...)
			{
			}
		}
#else
		if(pos!=pos_end)
			try_unget(input,buffer);
#endif
		}
	}
#else

#if __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	~unget_temp_buffer() requires requires()
	{
		try_unget(input,buffer);
	}
	{
#ifdef __cpp_exceptions
		if constexpr(noexcept(try_unget(input,buffer)))
		{
			if(pos!=pos_end)
				try_unget(input,buffer);
		}
		else
		{
			try
			{
				if(pos!=pos_end)
					try_unget(input,buffer);
			}
			catch(...)
			{
			}
		}
#else
		if(pos!=pos_end)
			try_unget(input,buffer);
#endif
	}
#if __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	~unget_temp_buffer() = default;
#endif
};

template<input_stream T,::std::input_or_output_iterator Iter>
inline constexpr Iter read(unget_temp_buffer<T>& in,Iter begin, Iter end)
{
	if(begin==end)
		return begin;
	if(in.pos!=in.pos_end)
	{
		*begin=in.buffer;
		++begin;
	}
	if(begin!=end)
		begin=read(in.input,begin,end);	
	in.pos=in.pos_end={};
	return begin;
}

template<typename T>
inline constexpr auto ibuffer_begin(unget_temp_buffer<T>& in) noexcept
{
	return __builtin_addressof(in.buffer);
}

template<typename T>
inline constexpr auto ibuffer_curr(unget_temp_buffer<T>& in) noexcept
{
	return __builtin_addressof(in.buffer)+in.pos;
}

template<typename T>
inline constexpr auto ibuffer_end(unget_temp_buffer<T>& in) noexcept
{
	return __builtin_addressof(in.buffer)+in.pos_end;
}

template<typename T>
inline constexpr auto ibuffer_set_curr(unget_temp_buffer<T>& in,typename T::char_type* ptr) noexcept
{
	in.pos=ptr-__builtin_addressof(in.buffer);
}

template<typename T>
inline constexpr bool ibuffer_underflow(unget_temp_buffer<T>& in)
{
	if constexpr(try_get_input_stream<T>)
	{
		auto ret{try_get(in.input)};
		in.buffer=ret.first;
		in.pos={};
		in.pos_end=ret.second;
		return ret.second;
	}
	else
	{
		bool not_eof{read(in.input,__builtin_addressof(in.buffer),__builtin_addressof(in.buffer)+1)!=__builtin_addressof(in.buffer)};
		in.pos={};
		in.pos_end=not_eof;
		return not_eof;
	}
}
namespace details
{
#if 0
template<buffer_input_stream input,typename T,typename P>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr bool scan_single_status_impl(input in,T& state_machine,P arg)
{
	for(;state_machine.code==parse_code::partial;)
	{
		if(!ibuffer_underflow(in))
		{
			if(!state_machine.test_eof(arg))
				return false;
			if(state_machine.code==parse_code{})[[likely]]
				return true;
			break;
		}
		auto curr{ibuffer_curr(in)};
		auto end{ibuffer_end(in)};
		state_machine(curr,end,arg);
		ibuffer_set_curr(in, state_machine.iter);
		if(state_machine.code==parse_code::ok)[[likely]]
			return true;
	}
	throw_parse_code(state_machine.code);
}
#endif

template<buffer_input_stream input,typename P>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr bool scan_context_status_impl(input in,P arg)
{
	using char_type = typename input::char_type;
	for(typename std::remove_cvref_t<decltype(scan_context_type(io_reserve_type<char_type,P>))>::type state;;)
	{
		auto curr{ibuffer_curr(in)};
		auto end{ibuffer_end(in)};
		auto [it,ec]=scan_context_define(io_reserve_type<char_type,P>,state,curr,end,arg);
		if constexpr(std::same_as<decltype(curr),decltype(it)>)
		{
			ibuffer_set_curr(in,it);
		}
		else
		{
			ibuffer_set_curr(in,it-curr+curr);
		}
		if(ec==parse_code::ok)
		{
			return true;
		}
		else if(ec!=parse_code::partial)
			throw_parse_code(ec);
		if(!ibuffer_underflow(in))[[unlikely]]
		{
			ec=scan_context_eof_define(io_reserve_type<char_type,P>,state,arg);
			if(ec==parse_code::ok)
				return true;
			else if(ec==parse_code::end_of_file)
				break;
			throw_parse_code(ec);
		}
	}
	return false;
}

template<bool>
inline constexpr bool type_not_scannable=false;

template<buffer_input_stream input,typename T>
[[nodiscard]] inline constexpr bool scan_single_impl(input in,T arg)
{
	using char_type = typename input::char_type;
	if constexpr(contiguous_input_stream<input>)
	{
		if constexpr(precise_reserve_scannable<char_type,T>)
		{
			constexpr std::size_t n{scan_precise_reserve_size(io_reserve_type<char_type,T>)};
			auto curr_ptr{ibuffer_curr(in)};
			char_type const* curr{curr_ptr};
			char_type const* end{ibuffer_end(in)};
			std::size_t const diff{static_cast<std::size_t>(end-curr)};
			if(diff<n)[[unlikely]]
			{
				return false;
			}
			if constexpr(precise_reserve_scannable_no_error<char_type,T>)
			{
				scan_precise_reserve_define(io_reserve_type<char_type,T>,curr,arg);
			}
			else
			{
				auto ret{scan_precise_reserve_define(io_reserve_type<char_type,T>,curr,arg)};
				if(ret!=parse_code::ok)
				{
					if(ret==parse_code::end_of_file)
					{
						return false;
					}
					throw_parse_code(ret);
				}
			}
			ibuffer_set_curr(in,curr_ptr+n);
			return true;
		}
		else if constexpr(contiguous_scannable<char_type,T>)
		{
			auto curr{ibuffer_curr(in)};
			auto end{ibuffer_end(in)};
			auto [it,ec] = scan_contiguous_define(io_reserve_type<char_type,T>,curr,end,arg);
			if constexpr(std::same_as<decltype(curr),decltype(it)>)
			{
				ibuffer_set_curr(in,it);
			}
			else
			{
				ibuffer_set_curr(in,it-curr+curr);
			}
			if(ec!=parse_code::ok)
			{
				if(ec==parse_code::end_of_file)
					return false;
				throw_parse_code(ec);
			}
			return true;
		}
		else if constexpr(context_scannable<char_type,T>)
		{
			typename std::remove_cvref_t<decltype(scan_context_type(io_reserve_type<char_type,T>))>::type state;
			auto curr{ibuffer_curr(in)};
			auto end{ibuffer_end(in)};
			auto [it,ec]=scan_context_define(io_reserve_type<char_type,T>,state,curr,end,arg);
			if constexpr(std::same_as<decltype(curr),decltype(it)>)
			{
				ibuffer_set_curr(in,it);
			}
			else
			{
				ibuffer_set_curr(in,it-curr+curr);
			}
			if(ec==parse_code::ok)
				return true;
			else if(ec!=parse_code::partial)
				throw_parse_code(ec);
			ec=scan_context_eof_define(io_reserve_type<char_type,T>,state,arg);
			if(ec==parse_code::ok)
				return true;
			else if(ec!=parse_code::end_of_file)
				throw_parse_code(ec);
			return false;
		}
		else
		{
			constexpr bool not_scannable{context_scannable<char_type,T>};
			static_assert(not_scannable,"type not scannable. need context_scannable");
			return false;
		}
	}
	else
	{
		if constexpr(precise_reserve_scannable<char_type,T>)
		{
			constexpr std::size_t n{scan_precise_reserve_size(io_reserve_type<char_type,T>)};
			char_type buffer[n];
			if constexpr(::fast_io::details::asan_state::current==::fast_io::details::asan_state::activate)
			{
				char_type const* p{buffer};
				bool ef{::fast_io::details::read_all_impl_decay_cold<false>(in,buffer,buffer+n)};
				if(!ef)
				{
					return false;
				}
				if constexpr(precise_reserve_scannable_no_error<char_type,T>)
				{
					scan_precise_reserve_define(io_reserve_type<char_type,T>,p,arg);
				}
				else
				{
					auto ret{scan_precise_reserve_define(io_reserve_type<char_type,T>,p,arg)};
					if(ret!=parse_code::ok)
					{
						if(ret==parse_code::end_of_file)
						{
							return false;
						}
						throw_parse_code(ret);
					}
				}
			}
			else
			{
				auto curr_ptr{ibuffer_curr(in)};
				char_type const* curr{curr_ptr};
				char_type const* end{ibuffer_end(in)};
				char_type const* p{curr};
				std::size_t const diff{static_cast<std::size_t>(end-curr)};
				bool inbuffer{diff<n};
				if(inbuffer)[[unlikely]]
				{
					bool ef{::fast_io::details::read_all_impl_decay_cold<false>(in,buffer,buffer+n)};
					if(!ef)
					{
						return false;
					}
					p=buffer;
				}
				if constexpr(precise_reserve_scannable_no_error<char_type,T>)
				{
					scan_precise_reserve_define(io_reserve_type<char_type,T>,p,arg);
				}
				else
				{
					auto ret{scan_precise_reserve_define(io_reserve_type<char_type,T>,p,arg)};
					if(ret!=parse_code::ok)
					{
						if(ret==parse_code::end_of_file)
						{
							return false;
						}
						throw_parse_code(ret);
					}
				}
				if(!inbuffer)[[likely]]
				{
					ibuffer_set_curr(in,curr_ptr+n);
				}
			}
			return true;
		}
		else if constexpr(contiguous_scannable<char_type,T>&&context_scannable<char_type,T>)
		{
			auto curr{ibuffer_curr(in)};
			auto end{ibuffer_end(in)};
			auto [it,ec] = scan_contiguous_define(io_reserve_type<char_type,T>,curr,end,arg);
			if(it==end)
				return scan_context_status_impl(in,arg);
			else
			{
				if constexpr(std::same_as<decltype(curr),decltype(it)>)
				{
					ibuffer_set_curr(in,it);
				}
				else
				{
					ibuffer_set_curr(in,it-curr+curr);
				}
				if(ec!=parse_code::ok)
					throw_parse_code(ec);
			}
			return true;
		}
		else if constexpr(context_scannable<char_type,T>)
		{
			for(typename std::remove_cvref_t<decltype(scan_context_type(io_reserve_type<char_type,T>))>::type state;;)
			{
				auto curr{ibuffer_curr(in)};
				auto end{ibuffer_end(in)};
				auto [it,ec]=scan_context_define(io_reserve_type<char_type,T>,state,curr,end,arg);
				if constexpr(std::same_as<decltype(curr),decltype(it)>)
				{
					ibuffer_set_curr(in,it);
				}
				else
				{
					ibuffer_set_curr(in,it-curr+curr);
				}
				if(ec==parse_code::ok)
					return true;
				else if(ec!=parse_code::partial)
					throw_parse_code(ec);
				if(!ibuffer_underflow(in))[[unlikely]]
				{
					ec=scan_context_eof_define(io_reserve_type<char_type,T>,state,arg);
					if(ec==parse_code::ok)
						return true;
					else if(ec==parse_code::end_of_file)
						break;
					throw_parse_code(ec);
				}
			}
			return false;
		}
		else
		{
			constexpr bool not_scannable{context_scannable<char_type,T>};
			static_assert(not_scannable,"type not scannable. need context_scannable");
			return false;
		}
	}
}
}

template<typename input,typename... Args>
requires (status_input_stream<input>||input_stream<input>)
[[nodiscard]] inline constexpr bool scan_freestanding_decay(input in,Args... args)
{
	if constexpr(status_input_stream<input>)
		return scan_status_define(in,args...);
	else if constexpr(mutex_stream<input>)
	{
		io_lock_guard lg{in};
		decltype(auto) unlocked_in{in.unlocked_handle()};
		return scan_freestanding_decay(io_ref(unlocked_in),args...);
	}
	else if constexpr(buffer_input_stream<input>)
	{
		return (details::scan_single_impl(in,args)&&...);
	}
	else
	{
		unget_temp_buffer in_buffer(io_ref(in));
		return scan_freestanding_decay(io_ref(in_buffer),args...);
	}
}

template<typename input,typename ...Args>
requires (status_input_stream<input>||input_stream<input>)
[[nodiscard("scan does not require checking return value but scan_freestanding requires")]] inline constexpr bool scan_freestanding(input&& in,Args&& ...args)
{
	return scan_freestanding_decay(io_ref(in),io_scan_forward<typename std::remove_cvref_t<input>::char_type>(io_scan_alias(args))...);
}

}
