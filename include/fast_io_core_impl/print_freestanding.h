#pragma once

namespace fast_io
{

namespace details::decay
{

template<std::integral char_type,typename T>
inline constexpr void scatter_print_recursive(io_scatter_t* arr,T t)
{
	auto scatter{print_scatter_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,t)};
	*arr={scatter.base,scatter.len*sizeof(char_type)};
}

template<std::integral char_type,typename T,typename... Args>
inline constexpr void scatter_print_recursive(io_scatter_t* arr,T t, Args ...args)
{
	auto scatter{print_scatter_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,t)};
	*arr={scatter.base,scatter.len*sizeof(char_type)};
	scatter_print_recursive<char_type>(arr+1,args...);
}

template<std::integral char_type,typename T>
inline constexpr std::size_t calculate_scatter_reserve_size_unit()
{
	using real_type = std::remove_cvref_t<T>;
	if constexpr(reserve_printable<char_type,real_type>)
	{
		constexpr std::size_t sz{print_reserve_size(io_reserve_type<char_type,real_type>)};
		return sz;
	}
	else
		return 0;
}

template<std::integral char_type,typename T,typename... Args>
inline constexpr std::size_t calculate_scatter_reserve_size()
{
	if constexpr(sizeof...(Args)==0)
	{
		return calculate_scatter_reserve_size_unit<char_type,T>();
	}
	else
	{
		return ::fast_io::details::intrinsics::add_or_overflow_die(calculate_scatter_reserve_size_unit<char_type,T>(),
			calculate_scatter_reserve_size<char_type,Args...>());
	}
}

template<std::integral char_type,typename T,typename... Args>
inline constexpr std::size_t calculate_scatter_dynamic_reserve_size([[maybe_unused]] T t,Args... args)
{
	if constexpr(!reserve_printable<char_type,T>&&
		dynamic_reserve_printable<char_type,T>)
	{
		std::size_t res{print_reserve_size(io_reserve_type<char_type,T>,t)};
		if constexpr(sizeof...(Args)==0)
			return res;
		else
			return ::fast_io::details::intrinsics::add_or_overflow_die(res,calculate_scatter_dynamic_reserve_size<char_type>(args...));
	}
	else
	{
		if constexpr(sizeof...(Args)==0)
			return 0;
		else
			return calculate_scatter_dynamic_reserve_size<char_type>(args...);
	}
}


template<std::integral char_type,typename T>
inline constexpr void scatter_print_with_reserve_recursive_unit(char_type*& start_ptr,
		io_scatter_t* arr,T t)
{
	using real_type = std::remove_cvref_t<T>;
	if constexpr(reserve_printable<char_type,T>)
	{
		if constexpr(::fast_io::details::asan_state::current==::fast_io::details::asan_state::activate)
		{
			constexpr std::size_t sz{print_reserve_size(io_reserve_type<char_type,real_type>)};
			char_type sanitize_buffer[sz];
			auto dt{print_reserve_define(io_reserve_type<char_type,real_type>,sanitize_buffer,t)};
			auto end_ptr{non_overlapped_copy_n(sanitize_buffer,static_cast<std::size_t>(dt-sanitize_buffer),start_ptr)};
			*arr={start_ptr,static_cast<std::size_t>(end_ptr-start_ptr)*sizeof(*start_ptr)};
			start_ptr=end_ptr;
		}
		else
		{
			auto end_ptr = print_reserve_define(io_reserve_type<char_type,real_type>,start_ptr,t);
			*arr={start_ptr,static_cast<std::size_t>(end_ptr-start_ptr)*sizeof(*start_ptr)};
			start_ptr=end_ptr;
		}
	}
	else
	{
		auto scatter{print_scatter_define(io_reserve_type<char_type,real_type>,t)};
		*arr={scatter.base,scatter.len*sizeof(*scatter.base)};
	}
}

template<std::integral char_type,typename T>
inline constexpr void scatter_print_with_reserve_recursive(char_type* ptr,
		io_scatter_t* arr,T t)
{
	scatter_print_with_reserve_recursive_unit(ptr,arr,t);
}

template<std::integral char_type,typename T,typename... Args>
inline constexpr void scatter_print_with_reserve_recursive(char_type* ptr,
	io_scatter_t* arr,T t, Args ...args)
{
	scatter_print_with_reserve_recursive_unit(ptr,arr,t);
	scatter_print_with_reserve_recursive(ptr,arr+1,args...);
}

template<std::integral char_type,typename T,typename... Args>
inline constexpr void scatter_print_with_dynamic_reserve_recursive(io_scatter_t* __restrict arr,
	[[maybe_unused]] char_type* __restrict ptr,
	[[maybe_unused]] char_type* __restrict dynamic_buffer_ptr,[[maybe_unused]] T t, Args ...args)
{
	
	if constexpr(scatter_printable<char_type,T>)
	{
		auto sc{print_scatter_define(io_reserve_type<char_type,T>,t)};
		*arr={sc.base,sc.len*sizeof(char_type)};
	}
	else if constexpr(reserve_printable<char_type,T>)
	{
		auto end_ptr = print_reserve_define(io_reserve_type<char_type,T>,ptr,t);
		*arr={ptr,(end_ptr-ptr)*sizeof(*ptr)};
		if constexpr(sizeof...(Args)!=0)
			ptr=end_ptr;
	}
	else if constexpr(dynamic_reserve_printable<char_type,T>)
	{
		auto end_ptr = print_reserve_define(io_reserve_type<char_type,T>,dynamic_buffer_ptr,t);
		*arr={dynamic_buffer_ptr,(end_ptr-dynamic_buffer_ptr)*sizeof(*dynamic_buffer_ptr)};
		if constexpr(sizeof...(Args)!=0)
			dynamic_buffer_ptr = end_ptr;
	}
	if constexpr(sizeof...(Args)!=0)
		scatter_print_with_dynamic_reserve_recursive(arr+1,ptr,dynamic_buffer_ptr,args...);
}

template<std::integral char_type,typename T,typename... Args>
inline constexpr void scatter_print_with_dynamic_only_reserve_recursive(io_scatter_t* __restrict arr,
	[[maybe_unused]] char_type* __restrict dynamic_buffer_ptr,[[maybe_unused]] T t, Args ...args)
{
	static_assert(!reserve_printable<char_type,T>);
	if constexpr(scatter_printable<char_type,T>)
	{
		auto sc{print_scatter_define(io_reserve_type<char_type,T>,t)};
		*arr={sc.base,sc.len*sizeof(char_type)};
	}
	else if constexpr(dynamic_reserve_printable<char_type,T>)
	{
		auto end_ptr = print_reserve_define(io_reserve_type<char_type,T>,dynamic_buffer_ptr,t);
		*arr={dynamic_buffer_ptr,(end_ptr-dynamic_buffer_ptr)*sizeof(*dynamic_buffer_ptr)};
		if constexpr(sizeof...(Args)!=0)
			dynamic_buffer_ptr = end_ptr;
	}
	if constexpr(sizeof...(Args)!=0)
		scatter_print_with_dynamic_only_reserve_recursive(arr+1,dynamic_buffer_ptr,args...);
}

template<std::integral char_type,typename T>
requires scatter_printable<char_type,T>
inline constexpr auto extract_one_scatter(T t)
{
	return print_scatter_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,t);
}

template<typename output,std::size_t N>
inline constexpr bool constant_buffer_output_stream_require_size_constant_impl =
	(N<obuffer_constant_size(io_reserve_type<typename output::char_type,output>));

template<typename output,std::size_t N>
concept constant_buffer_output_stream_require_size_impl = constant_buffer_output_stream<output>
	&& constant_buffer_output_stream_require_size_constant_impl<output,N>;

template<bool line,typename value_type,output_stream output>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void print_control_dynamic_reserve_bad_path(output out,value_type t,std::size_t sizep1)
{
	using char_type = typename output::char_type;
	local_operator_new_array_ptr<char_type> ptr(sizep1);
	auto it{print_reserve_define(io_reserve_type<char_type,value_type>,ptr.ptr,t)};
	if constexpr(line)
	{
		*it=char_literal_v<u8'\n',char_type>;
		++it;
	}
	write(out,ptr.ptr,it);
}

template<bool line=false,output_stream output,typename T>
requires (std::is_trivially_copyable_v<output>&&std::is_trivially_copyable_v<T>)
inline constexpr void print_control(output out,T t)
{
	using char_type = typename output::char_type;
	using value_type = std::remove_cvref_t<T>;
	constexpr auto lfch{char_literal_v<u8'\n',char_type>};
	constexpr bool asan_activated{::fast_io::details::asan_state::current==::fast_io::details::asan_state::activate};
	if constexpr(scatter_printable<char_type,value_type>)
	{
		basic_io_scatter_t<char_type> scatter{print_scatter_define(io_reserve_type<char_type,value_type>,t)};
		if constexpr(line)
		{		
			if constexpr(output_stream_with_writeln<output>)
				writeln(out,scatter.base,scatter.base+scatter.len);
			else if constexpr(contiguous_output_stream<output>)
			{
				auto curr=obuffer_curr(out);
				auto end=obuffer_end(out);
				std::ptrdiff_t sz(end-curr-1);
				std::size_t const len{scatter.len};
				if(sz<static_cast<std::ptrdiff_t>(len))
					fast_terminate();
				curr=non_overlapped_copy_n(scatter.base,scatter.len,curr);
				*curr=lfch;
				++curr;
				obuffer_set_curr(out,curr);
			}
			else if constexpr(buffer_output_stream<output>)
			{
				auto curr=obuffer_curr(out);
				auto end=obuffer_end(out);
				std::size_t const len{scatter.len};
				std::ptrdiff_t sz(end-curr-1);
				if(static_cast<std::ptrdiff_t>(len)<sz)[[likely]]
				{
					curr=details::non_overlapped_copy_n(scatter.base,len,curr);
					*curr=lfch;
					++curr;
					obuffer_set_curr(out,curr);
				}
				else
				{
					write(out,scatter.base,scatter.base+scatter.len);
					put(out,lfch);
				}
			}
			else 
			{
				write(out,scatter.base,scatter.base+scatter.len);
				write(out,__builtin_addressof(lfch),
				__builtin_addressof(lfch)+1);
			}
		}
		else
		{
			write(out,scatter.base,scatter.base+scatter.len);
		}
	}
	else if constexpr(reserve_printable<char_type,value_type>)
	{
		constexpr std::size_t real_size{print_reserve_size(io_reserve_type<char_type,value_type>)};
		constexpr std::size_t size{real_size+static_cast<std::size_t>(line)};
		static_assert(real_size<PTRDIFF_MAX);
		if constexpr(contiguous_output_stream<output>)
		{
			auto bcurr{obuffer_curr(out)};
			auto bend{obuffer_end(out)};
			std::size_t diff{static_cast<std::size_t>(bend-bcurr)};
			if(diff<size)[[unlikely]]
				fast_terminate();
			auto it{print_reserve_define(io_reserve_type<char_type,value_type>,bcurr,t)};
			if constexpr(line)
			{
				*it=lfch;
				++it;
			}
			obuffer_set_curr(out,it);
		}
		else
		{
			if constexpr(buffer_output_stream<output>&&!asan_activated)
			{
				char_type* bcurr{obuffer_curr(out)};
				char_type* bend{obuffer_end(out)};
				std::ptrdiff_t const diff(bend-bcurr);
				bool smaller{static_cast<std::ptrdiff_t>(size)<diff};
				if constexpr(constant_buffer_output_stream_require_size_impl<output,size>)
				{
					if(!smaller)[[unlikely]]
					{
						obuffer_constant_flush_prepare(out);
						bcurr=obuffer_curr(out);
					}
					bcurr=print_reserve_define(io_reserve_type<char_type,value_type>,bcurr,t);
					if constexpr(line)
					{
						*bcurr=lfch;
						++bcurr;
					}
					obuffer_set_curr(out,bcurr);
				}
				else
				{
					char_type buffer[size];
					if(!smaller)[[unlikely]]
						bcurr=buffer;
					bcurr=print_reserve_define(io_reserve_type<char_type,value_type>,bcurr,t);
					if constexpr(line)
					{
						*bcurr=lfch;
						++bcurr;
					}
					if(smaller)[[likely]]
						obuffer_set_curr(out,bcurr);
					else[[unlikely]]
						write(out,buffer,bcurr);
				}
			}
			else
			{
				char_type buffer[size];
				char_type* i{print_reserve_define(io_reserve_type<char_type,value_type>,buffer,t)};
				if constexpr(line)
				{
					*i=lfch;
					++i;
				}
				write(out,buffer,i);
			}
		}
	}
	else if constexpr(dynamic_reserve_printable<char_type,value_type>)
	{
		std::size_t size{print_reserve_size(io_reserve_type<char_type,value_type>,t)};
		if constexpr(line)
		{
			constexpr std::size_t mx{std::numeric_limits<std::ptrdiff_t>::max()-1};
			if(size>=mx)
				fast_terminate();
			++size;
		}
		else
		{
			constexpr std::size_t mx{std::numeric_limits<std::ptrdiff_t>::max()};
			if(mx<size)
				fast_terminate();
		}
		if constexpr(contiguous_output_stream<output>)
		{
			auto bcurr{obuffer_curr(out)};
			auto bend{obuffer_end(out)};
			auto it{print_reserve_define(io_reserve_type<char_type,value_type>,bcurr,t,size)};
			std::size_t diff{static_cast<std::size_t>(bend-bcurr)};
			if(diff<size)[[unlikely]]
				fast_terminate();
			if constexpr(line)
			{
				*it=lfch;
				++it;
			}
			obuffer_set_curr(out,it);
		}
		else
		{
			if constexpr(buffer_output_stream<output>&&!asan_activated)
			{
				auto curr{obuffer_curr(out)};
				auto ed{obuffer_end(out)};
				std::ptrdiff_t diff(ed-curr);
				if(static_cast<std::ptrdiff_t>(size)<diff)[[likely]]
				{
					auto it{print_reserve_define(io_reserve_type<char_type,value_type>,curr,t)};
					if constexpr(line)
					{
						*it=lfch;
						++it;
					}
					obuffer_set_curr(out,it);
					return;
				}
			}
			print_control_dynamic_reserve_bad_path<line,value_type>(out,t,size);
		}
	}
	else if constexpr(printable<char_type,value_type>)
	{
		print_define(io_reserve_type<char_type,value_type>,out,t);
		if constexpr(line)
			put(out,lfch);
	}
	else
	{
		constexpr bool no{printable<char_type,value_type>};
		static_assert(no,"type not printable");
	}
}


struct print_size_struct
{
	std::size_t n{};
	std::size_t total_size{};
};

inline constexpr std::size_t max_reserve_multiple_parameter_size{512u};

template<std::integral char_type,typename T,typename... Args>
inline constexpr print_size_struct print_reserve_total_size_para()
{
	if constexpr(reserve_printable<char_type,T>)
	{
		constexpr std::size_t rsvsize{print_reserve_size(io_reserve_type<char_type,T>)};
		if constexpr(rsvsize>=max_reserve_multiple_parameter_size)
			return {};
		else if constexpr(sizeof...(Args)==0)
		{
			return {1,rsvsize};
		}
		else
		{
			auto next{print_reserve_total_size_para<char_type,Args...>()};
			std::size_t mx_size{static_cast<std::size_t>(SIZE_MAX>>1u)-rsvsize};
			if(next.total_size>=mx_size)
				return next;
			return {next.n+1,next.total_size+rsvsize};
		}
	}
	else
	{
		return {};
	}
}

template<std::integral char_type,typename T,typename... Args>
inline constexpr print_size_struct print_reserve_multiple_parameters_total_size()
{
	constexpr std::size_t t_reserve_size{print_reserve_size(io_reserve_type<char_type,T>)};
	static_assert(t_reserve_size<(SIZE_MAX>>1u),"overflow");
	if constexpr((t_reserve_size>=max_reserve_multiple_parameter_size)||(sizeof...(Args)==0))
	{
		return {0,t_reserve_size};
	}
	else if constexpr(sizeof...(Args)!=0)
	{
		constexpr print_size_struct sizestrct{print_reserve_total_size_para<char_type,Args...>()};
		constexpr std::size_t overflow_max_size{max_reserve_multiple_parameter_size-t_reserve_size};
		if constexpr(sizestrct.total_size>=overflow_max_size)
			return {0,t_reserve_size};
		else
			return {sizestrct.n,t_reserve_size+sizestrct.total_size};
	}
}

template<std::size_t n,typename T,typename Iter,typename... Args>
requires(n!=0)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr Iter partition_reserve_impl(Iter iter,T t,[[maybe_unused]] Args ...args)
{
	using char_type = ::std::iter_value_t<Iter>;
	iter=print_reserve_define(io_reserve_type<char_type,T>,iter,t);
	if constexpr(n==1)
	{
		return iter;
	}
	else
	{
		return partition_reserve_impl<n-1>(iter,args...);
	}
}


template<bool line,
std::size_t n,
typename output,
typename T,typename... Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void print_controls_line_multi_impl(output out,T t,Args ...args)
{
	if constexpr(n==0)
	{
		using char_type = typename output::char_type;
		if constexpr(reserve_printable<char_type,T>)
		{
			constexpr auto reserve_paras_info{print_reserve_multiple_parameters_total_size<char_type,T,Args...>()};
			constexpr std::size_t reserve_paras_n{reserve_paras_info.n};
			if constexpr(reserve_paras_n!=0)
			{
				static_assert(reserve_paras_info.total_size<static_cast<std::size_t>(PTRDIFF_MAX),"reserve size cannot be zero");
				{
					constexpr bool need_output_lf{sizeof...(Args)==reserve_paras_n&&line};
					constexpr std::size_t buffer_size{reserve_paras_info.total_size+static_cast<std::size_t>(need_output_lf)};
					if constexpr(buffer_output_stream<output>)
					{
						auto curr_ptr{obuffer_curr(out)};
						auto end_ptr{obuffer_end(out)};
						std::ptrdiff_t diff{end_ptr-curr_ptr};
						bool const on_io_buffer{static_cast<std::ptrdiff_t>(buffer_size)<diff};
						if constexpr(constant_buffer_output_stream_require_size_impl<output,buffer_size>)
						{
							if(!on_io_buffer)[[unlikely]]
							{
								obuffer_constant_flush_prepare(out);
								curr_ptr=obuffer_curr(out);
							}
							curr_ptr=print_reserve_define(io_reserve_type<char_type,T>,curr_ptr,t);
							curr_ptr=partition_reserve_impl<reserve_paras_n>(curr_ptr,args...);
							if constexpr(need_output_lf)
							{
								*curr_ptr=char_literal_v<u8'\n',char_type>;
								++curr_ptr;
							}
							obuffer_set_curr(out,curr_ptr);
						}
						else
						{
							char_type stack_buffer[buffer_size];
							auto start{stack_buffer};
							auto ptr{print_reserve_define(io_reserve_type<char_type,T>,start,t)};
							ptr=partition_reserve_impl<reserve_paras_n>(ptr,args...);
							if constexpr(need_output_lf)
							{
								*ptr=char_literal_v<u8'\n',char_type>;
								++ptr;
							}
							write(out,start,ptr);
						}
					}
				}
				if constexpr(reserve_paras_info.n!=sizeof...(Args))
					print_controls_line_multi_impl<line,reserve_paras_info.n>(out,args...);
			}
			else
			{
				if constexpr(sizeof...(Args)==0)
					print_control<line>(out,t);
				else
				{
					print_control<false>(out,t);
					print_controls_line_multi_impl<line,0>(out,args...);
				}
			}
		}
		else
		{
			if constexpr(sizeof...(Args)==0)
				print_control<line>(out,t);
			else
			{
				print_control<false>(out,t);
				print_controls_line_multi_impl<line,0>(out,args...);
			}
		}
	}
	else
	{
		print_controls_line_multi_impl<line,n-1>(out,args...);
	}
}

template<bool ln,output_stream output,typename T,typename... Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void print_controls_line(output out,T t,Args... args)
{
	if constexpr(sizeof...(Args)==0)
		print_control<ln>(out,t);
	else
	{
#if defined(__OPTIMIZE__) || defined(__OPTIMIZE_SIZE__)
		print_controls_line_multi_impl<ln,0>(out,t,args...);
#else
		if constexpr(ln)
		{
			print_control<false>(out,t);
			print_controls_line<ln>(out,args...);
		}
		else
		{
			print_control<false>(out,t);
			(print_control<false>(out,args),...);
		}
#endif
	}
}

template<bool line,output_stream output,typename ...Args>
inline constexpr void print_fallback(output out,Args ...args)
{
	using char_type = typename output::char_type;
	if constexpr(line&&sizeof...(Args)==0)
	{
		write(out,__builtin_addressof(char_literal_v<u8'\n',char_type>),__builtin_addressof(char_literal_v<u8'\n',char_type>)+1);
	}
	else
	{
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if(__builtin_is_constant_evaluated())
#endif
		{
			temporary_buffer<output> buffer;
			buffer.out=out;
			auto ref{io_ref(buffer)};
			print_controls_line_multi_impl<line,0>(ref,args...);
			flush(buffer);
		}
		else
		{
			if constexpr(
				(scatter_output_stream<output>||scatter_constant_output_stream<output>)&&
				((scatter_printable<typename output::char_type,Args>||reserve_printable<typename output::char_type,Args>||dynamic_reserve_printable<typename output::char_type,Args>)&&...))
			{
				constexpr std::size_t args_num{sizeof...(Args)};
				constexpr std::size_t n{args_num+static_cast<std::size_t>(line)};
				io_scatter_t scatters[n];
				if constexpr((scatter_printable<typename output::char_type,Args>&&...))
				{
					scatter_print_recursive<typename output::char_type>(scatters,args...);
					if constexpr(line)
					{
						scatters[args_num]={__builtin_addressof(char_literal_v<u8'\n',char_type>),sizeof(char_type)};
					}
					if constexpr(scatter_constant_output_stream<output>)
					{
						scatter_constant_write<n>(out,scatters);
					}
					else
					{
						scatter_write(out,{scatters,n});
					}
				}
				else if constexpr(((scatter_printable<char_type,Args>||
					reserve_printable<char_type,Args>)&&...))
				{
					constexpr std::size_t total_size{calculate_scatter_reserve_size<char_type,Args...>()};
					char_type buffer[total_size];
					scatter_print_with_reserve_recursive(buffer,scatters,args...);
					if constexpr(line)
					{
						scatters[args_num]={__builtin_addressof(char_literal_v<u8'\n',char_type>),sizeof(char_type)};
					}
					if constexpr(scatter_constant_output_stream<output>)
					{
						scatter_constant_write<n>(out,scatters);
					}
					else
					{
						scatter_write(out,{scatters,n});
					}
				}
				else
				{
					constexpr std::size_t total_size{calculate_scatter_reserve_size<char_type,Args...>()};
					if constexpr(total_size!=0)
					{
						char_type buffer[total_size];
						local_operator_new_array_ptr<char_type> new_ptr(calculate_scatter_dynamic_reserve_size<char_type>(args...));
						scatter_print_with_dynamic_reserve_recursive(scatters,buffer,new_ptr.ptr,args...);
						if constexpr(line)
						{
							scatters[args_num]={__builtin_addressof(char_literal_v<u8'\n',char_type>),sizeof(char_type)};
						}
						if constexpr(scatter_constant_output_stream<output>)
						{
							scatter_constant_write<n>(out,scatters);
						}
						else
						{
							scatter_write(out,{scatters,n});
						}
					}
					else
					{
						local_operator_new_array_ptr<char_type> new_ptr(calculate_scatter_dynamic_reserve_size<char_type>(args...));
						scatter_print_with_dynamic_only_reserve_recursive(scatters,new_ptr.ptr,args...);
						if constexpr(line)
						{
							scatters[args_num]={__builtin_addressof(char_literal_v<u8'\n',char_type>),sizeof(char_type)};
						}
						if constexpr(scatter_constant_output_stream<output>)
						{
							scatter_constant_write<n>(out,scatters);
						}
						else
						{
							scatter_write(out,{scatters,n});
						}
					}
				}
			}
			else
			{
				temporary_buffer<output> buffer;
				buffer.out=out;
				auto ref{io_ref(buffer)};
				print_controls_line_multi_impl<line,0>(ref,args...);
				flush(buffer);
			}
		}
	}
}

}

template<typename char_type,typename ...Args>
concept print_freestanding_decay_okay_character_type_no_status = std::integral<char_type>&&((std::is_trivially_copyable_v<Args>&&(printable<char_type,Args>||scatter_printable<char_type,Args>||reserve_printable<char_type,Args>||dynamic_reserve_printable<char_type,Args>))&&...);

template<typename output,typename ...Args>
concept print_freestanding_decay_okay_no_status = std::is_trivially_copyable_v<output>&&print_freestanding_decay_okay_character_type_no_status<typename output::char_type,Args...>;

template<typename output,typename ...Args>
concept print_freestanding_decay_okay = std::is_trivially_copyable_v<output>&&((status_output_stream<output>&&requires(output out,Args ...args)
{
	print_status_define<false>(out,args...);
	print_status_define<true>(out,args...);
})||(!status_output_stream<output>&&output_stream<output>&&print_freestanding_decay_okay_no_status<output,Args...>));

namespace details
{

template<typename output,typename... Args>
requires print_freestanding_decay_okay<output,Args...>
inline constexpr void print_freestanding_deacy_okay_dummy(output,Args...)
{

}

}

template<typename output,typename ...Args>
concept print_freestanding_okay = requires(output&& out,Args&& ...args)
{
	::fast_io::details::print_freestanding_deacy_okay_dummy(io_ref(out),io_print_forward<typename std::remove_cvref_t<output>::char_type>(io_print_alias(args))...);
};

template<bool line,output_stream output,typename ...Args>
requires print_freestanding_decay_okay_no_status<output,Args...>
inline constexpr void print_freestanding_decay_no_status(output out,Args ...args)
{
	using char_type = typename output::char_type;
	if constexpr(sizeof...(Args)==0&&!line)
		return;
	else if constexpr(mutex_stream<output>)
	{
		io_lock_guard lg{out};
		decltype(auto) dout{out.unlocked_handle()};
		print_freestanding_decay_no_status<line>(io_ref(dout),args...);
	}
	else if constexpr(buffer_output_stream<output>)
	{
		if constexpr(sizeof...(Args)==0&&line)
			put(out,char_literal_v<u8'\n',char_type>);
		else
		{
#if defined(__OPTIMIZE__) || defined(__OPTIMIZE_SIZE__)
			::fast_io::details::decay::print_controls_line_multi_impl<line,0>(out,args...);
#else
			if constexpr(line)
			{
				::fast_io::details::decay::print_controls_line<line>(out,args...);
			}
			else
			{
				(::fast_io::details::decay::print_control<false>(out,args),...);
			}
#endif
		}
	}
	else if constexpr(sizeof...(Args)==1&&
		((((!line||output_stream_with_writeln<output>)&&((printable<char_type,Args>||scatter_printable<char_type,Args>)&&...))||
		((reserve_printable<char_type,Args>||dynamic_reserve_printable<char_type,Args>)&&...))))
	{
		if constexpr(line)
		{
			::fast_io::details::decay::print_controls_line<line>(out,args...);
		}
		else
		{
			(::fast_io::details::decay::print_control<false>(out,args),...);
		}
	}
	else
	{
		::fast_io::details::decay::print_fallback<line>(out,args...);
	}
}

template<bool line,typename output,typename ...Args>
requires print_freestanding_decay_okay<output,Args...>
inline constexpr void print_freestanding_decay(output out,Args ...args)
{
	if constexpr(status_output_stream<output>)
		print_status_define<line>(out,args...);
	else
		print_freestanding_decay_no_status<line>(out,args...);
}

template<bool line,typename output,typename ...Args>
requires print_freestanding_decay_okay<output,Args...>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void print_freestanding_decay_cold(output out,Args ...args)
{
	if constexpr(status_output_stream<output>)
		print_status_define<line>(out,args...);
	else
		print_freestanding_decay_no_status<line>(out,args...);
}

template<typename output,typename ...Args>
requires print_freestanding_okay<output,Args...>
inline constexpr void print_freestanding(output&& out,Args&& ...args)
{
	print_freestanding_decay<false>(io_ref(out),io_print_forward<typename std::remove_cvref_t<output>::char_type>(io_print_alias(args))...);
}

template<output_stream output,typename ...Args>
requires print_freestanding_okay<output,Args...>
inline constexpr void println_freestanding(output&& out,Args&& ...args)
{
	print_freestanding_decay<true>(io_ref(out),io_print_forward<typename std::remove_cvref_t<output>::char_type>(io_print_alias(args))...);
}

}
