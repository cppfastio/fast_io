#pragma once

namespace fast_io
{

namespace details::decay
{

struct contiguous_scatter_result
{
::std::size_t position{};
::std::size_t neededspace{};
bool needextraspacefordynamic{};
bool lastisreserve{};
bool hasscatters{};
};

template<::std::integral char_type,typename Arg,typename ...Args>
inline constexpr contiguous_scatter_result find_continuous_scatters_n()
{
	contiguous_scatter_result ret{};
	if constexpr(::fast_io::scatter_printable<char_type,Arg>)
	{
		if constexpr(sizeof...(Args)!=0)
		{
			ret = find_continuous_scatters_n<char_type,Args...>();
		}
		return {ret.position+1,ret.neededspace,ret.needextraspacefordynamic,false,true};
	}
	else if constexpr(::fast_io::reserve_printable<char_type,Arg>)
	{
		bool rv{};
		if constexpr(sizeof...(Args)!=0)
		{
			ret = find_continuous_scatters_n<char_type,Args...>();
		}
		else
		{
			rv = true;
		}
		constexpr
			::std::size_t sz{print_reserve_size(::fast_io::io_reserve_type<char_type,Arg>)};
		return {ret.position+1,
			::fast_io::details::intrinsics::add_or_overflow_die_chain(ret.neededspace,sz),
			ret.needextraspacefordynamic,rv,false};
	}
	else if constexpr(::fast_io::dynamic_reserve_printable<char_type,Arg>)
	{
		bool rv{};
		if constexpr(sizeof...(Args)!=0)
		{
			ret = find_continuous_scatters_n<char_type,Args...>();
		}
		else
		{
			rv = true;
		}
		return {ret.position+1,ret.neededspace,true,rv,false};
	}
	return ret;
}

#if 0
template<::std::integral char_type,typename Arg,typename ...Args>
inline constexpr bool know_last_is_lf_n()
{
	if constexpr(sizeof...(Args)!=0)
	{
		return know_last_is_lf_n<char_type,Args...>();
	}
	else
	{
		if constexpr(::fast_io::reserve_printable<char_type,Arg>)
		{
			return true;
		}
		else if constexpr(::fast_io::dynamic_reserve_printable<char_type,Arg>)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}
#endif
template<typename output,std::size_t N>
inline constexpr bool constant_buffer_output_stream_require_size_constant_impl =
	(N<obuffer_constant_size(io_reserve_type<typename output::output_char_type,output>));

template<typename output,std::size_t N>
concept constant_buffer_output_stream_require_size_impl = constant_size_buffer_output_stream<output>
	&& constant_buffer_output_stream_require_size_constant_impl<output,N>;

template<bool line=false,output_stream output,typename T>
requires (std::is_trivially_copyable_v<output>&&std::is_trivially_copyable_v<T>)
inline constexpr void print_control_single(output outstm,T t)
{
	using char_type = typename output::output_char_type;
	using value_type = std::remove_cvref_t<T>;
	constexpr bool asan_activated{::fast_io::details::asan_state::current==::fast_io::details::asan_state::activate};
	constexpr auto lfch{char_literal_v<u8'\n',char_type>};
	if constexpr(scatter_printable<char_type,value_type>)
	{
#if 0
		basic_io_scatter_t<char_type> scatter;
		if constexpr(::std::same_as<T,basic_io_scatter_t<char_type>>)
		{
			scatter=t;
		}
		else
		{
			scatter=print_scatter_define(io_reserve_type<char_type,value_type>,t);
		}
		if constexpr(line)
		{
			if constexpr(output_stream_with_writeln<output>)
				writeln(outstm,scatter.base,scatter.base+scatter.len);
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
				obuffer_set_curr(outstm,curr);
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
					obuffer_set_curr(outstm,curr);
				}
				else
				{
					write(outstm,scatter.base,scatter.base+scatter.len);
					put(outstm,lfch);
				}
			}
			else 
			{
				write(outstm,scatter.base,scatter.base+scatter.len);
				write(outstm,__builtin_addressof(lfch),
				__builtin_addressof(lfch)+1);
			}
		}
		else
		{
			write(outstm,scatter.base,scatter.base+scatter.len);
		}
#endif
		if constexpr(line)
		{
			if constexpr(byte_output_stream<output>)
			{
				::fast_io::io_scatter_t scatters[2]
				{{t.base,t.len*sizeof(char_type)},
				{__builtin_addressof(char_literal_v<u8'\n',char_type>),
				sizeof(char_type)}};
				::fast_io::operations::scatter_write_all_bytes(outstm,scatters,2);
			}
			else
			{
				::fast_io::basic_io_scatter_t<T> scatters[2]
				{t,
				{__builtin_addressof(char_literal_v<u8'\n',char_type>),
				1}};
				::fast_io::operations::scatter_write_all(outstm,scatters,2);
			}
		}
		else
		{
			::fast_io::operations::write_all(outstm,t.base,t.base+t.len);
		}
	}
	else if constexpr(reserve_printable<char_type,value_type>)
	{
		constexpr std::size_t real_size{print_reserve_size(io_reserve_type<char_type,value_type>)};
		constexpr std::size_t size{real_size+static_cast<std::size_t>(line)};
		static_assert(real_size<PTRDIFF_MAX);
		if constexpr(contiguous_output_stream<output>)
		{
			auto bcurr{obuffer_curr(outstm)};
			auto bend{obuffer_end(outstm)};
			std::size_t diff{static_cast<std::size_t>(bend-bcurr)};
			if(diff<size)[[unlikely]]
				fast_terminate();
			auto it{print_reserve_define(io_reserve_type<char_type,value_type>,bcurr,t)};
			if constexpr(line)
			{
				*it=lfch;
				++it;
			}
			obuffer_set_curr(outstm,it);
		}
		else
		{
			if constexpr(buffer_output_stream<output>&&!asan_activated)
			{
				char_type* bcurr{obuffer_curr(outstm)};
				char_type* bend{obuffer_end(outstm)};
				std::ptrdiff_t const diff(bend-bcurr);
				bool smaller{static_cast<std::ptrdiff_t>(size)<diff};
				if constexpr(constant_buffer_output_stream_require_size_impl<output,size>)
				{
					if(!smaller)[[unlikely]]
					{
						obuffer_constant_flush_prepare(outstm);
						bcurr=obuffer_curr(outstm);
					}
					bcurr=print_reserve_define(io_reserve_type<char_type,value_type>,bcurr,t);
					if constexpr(line)
					{
						*bcurr=lfch;
						++bcurr;
					}
					obuffer_set_curr(outstm,bcurr);
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
						obuffer_set_curr(outstm,bcurr);
					else[[unlikely]]
						::fast_io::operations::write_all(outstm,buffer,bcurr);
				}
			}
			else
			{
				char_type buffer[size];
				char_type *i{print_reserve_define(io_reserve_type<char_type,value_type>,buffer,t)};
				if constexpr(line)
				{
					*i=lfch;
					++i;
				}
				::fast_io::operations::write_all(outstm,buffer,i);
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
			auto bcurr{obuffer_curr(outstm)};
			auto bend{obuffer_end(outstm)};
			auto it{print_reserve_define(io_reserve_type<char_type,value_type>,bcurr,t,size)};
			std::size_t diff{static_cast<std::size_t>(bend-bcurr)};
			if(diff<size)[[unlikely]]
				fast_terminate();
			if constexpr(line)
			{
				*it=lfch;
				++it;
			}
			obuffer_set_curr(outstm,it);
		}
		else
		{
			if constexpr(buffer_output_stream<output>&&!asan_activated)
			{
				auto curr{obuffer_curr(outstm)};
				auto ed{obuffer_end(outstm)};
				std::ptrdiff_t diff(ed-curr);
				auto toptr{curr};
				bool smaller{static_cast<std::ptrdiff_t>(size)<diff};
				::fast_io::details::local_operator_new_array_ptr<char_type> newptr;
				if(!smaller)
#if __has_cpp_attribute(unlikely)
				[[unlikely]]
#endif
				{
					newptr.ptr=toptr=
						::fast_io::details::allocate_iobuf_space<char_type,
						typename ::fast_io::details::local_operator_new_array_ptr<char_type>::allocator_type>(size);
					newptr.size=size;
				}

				auto it{print_reserve_define(io_reserve_type<char_type,value_type>,toptr,t)};
				if constexpr(line)
				{
					*it=lfch;
					++it;
				}
				if(smaller)
				{
					obuffer_set_curr(outstm,it);
				}
				else
#if __has_cpp_attribute(unlikely)
				[[unlikely]]
#endif
				{
					::fast_io::operations::write_all(outstm,toptr,it);
				}
			}
			else
			{
				::fast_io::details::local_operator_new_array_ptr<char_type> newptr(size);
				auto it{print_reserve_define(io_reserve_type<char_type,value_type>,newptr.ptr,t)};
				if constexpr(line)
				{
					*it=lfch;
					++it;
				}
				::fast_io::operations::write_all(outstm,newptr.ptr,it);
			}
		}
	}
	else if constexpr(printable<char_type,value_type>)
	{
		print_define(io_reserve_type<char_type,value_type>,outstm,t);
		if constexpr(line)
			::fast_io::operations::char_put(outstm,lfch);
	}
	else
	{
		constexpr bool no{printable<char_type,value_type>};
static_assert(no,"type not printable");
	}
}

#if 0
template<bool ln,::std::integral char_type,std::size_t n,typename Arg,typename ...Args>
inline constexpr char_type* printrsvcontiguousimpl(char_type* iter,Arg arg,Args... args)
{
	if constexpr(sizeof...(Args)!=0)
	{
		ret = find_continuous_scatters_n<char_type,Args...>();
	}
	if constexpr(::fast_io::scatter_printable<char_type,Arg>)
	{
		return {ret.position+1,ret.neededspace,ret.needextraspacefordynamic};
	}
	else if constexpr(::fast_io::reserve_printable<char_type,Arg>)
	{
		constexpr
			::std::size_t sz{print_reserve_size(::fast_io::io_reserve_type<char_type,Arg>)};
		return {ret.position+1,
			::fast_io::details::add_overflow(ret.neededspace,sz),
			ret.needextraspacefordynamic};
	}
	else if constexpr(::fast_io::dynamic_reserve_printable<char_type,Arg>)
	{
		return {ret.position+1,ret.neededspace,true};
	}
}
#endif

template<bool ln,output_stream output,typename T,typename... Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void print_controls_line(output outstm,T t,Args... args)
{
	if constexpr(sizeof...(Args)==0)
		print_control_single<ln>(outstm,t);
	else
	{
#if (defined(__OPTIMIZE__) || defined(__OPTIMIZE_SIZE__)) && 0
		print_controls_line_multi_impl<ln,0>(outstm,t,args...);
#else
		if constexpr(ln)
		{
			print_control_single<false>(outstm,t);
			print_controls_line<ln>(outstm,args...);
		}
		else
		{
			print_control_single<false>(outstm,t);
			(print_control<false>(outstm,args),...);
		}
#endif
	}
}

template<::std::size_t n,::std::integral char_type,typename T,typename ...Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr char_type* print_n_reserve(char_type *ptr,T t,Args ...args)
{
	if constexpr(n==0)
	{
		return ptr;
	}
	else
	{
		ptr=print_reserve_define(::fast_io::io_reserve_type<char_type,::std::remove_cvref_t<T>>,ptr,t);
		if constexpr(sizeof...(Args)==0||1<n)
		{
			return ptr;
		}
		else
		{
			return print_n_reserve<n-1>(ptr,args...);
		}
	}
}

template<::std::size_t n,::std::integral char_type,typename scattertype,typename T,typename ...Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void print_n_scatters(basic_io_scatter_t<scattertype> *pscatters,
	T t,Args ...args)
{
	if constexpr(n!=0)
	{
		if constexpr(::std::same_as<::std::remove_cvref_t<T>,basic_io_scatter_t<scattertype>>)
		{
			if constexpr(::std::same_as<scattertype,void>)
			{
				*pscatters=io_scatter_t{t.base,t.len*sizeof(char_type)};
			}
			else
			{
				*pscatters=t;
			}
		}
		else
		{
			basic_io_scatter_t<char_type> sct{print_scatter_define(::fast_io::io_reserve_type<char_type,T>,t)};
			if constexpr(::std::same_as<scattertype,void>)
			{
				*pscatters=io_scatter_t{sct.base,sct.len*sizeof(char_type)};
			}
			else
			{
				*pscatters=sct;
			}
		}
		if constexpr(1<n)
		{
			++pscatters;
			print_n_scatters<n-1,char_type>(pscatters,args...);
		}
	}
}

template<::std::size_t n,::std::integral char_type,typename T,typename ...Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::std::size_t ndynamic_print_reserve_size(T t,Args ...args)
{
	using nocvreft = ::std::remove_cvref_t<T>;
	if constexpr(n==0)
	{
		return 0;
	}
	else if constexpr(n==1)
	{
		if constexpr(::fast_io::dynamic_reserve_printable<char_type,nocvreft>)
		{
			return print_reserve_size(::fast_io::io_reserve_type<char_type,nocvreft>,t);
		}
		else
		{
			return 0;
		}
	}
	else
	{
		if constexpr(::fast_io::dynamic_reserve_printable<char_type,nocvreft>)
		{
			return ::fast_io::details::intrinsics::add_or_overflow_die(print_reserve_size(::fast_io::io_reserve_type<char_type,nocvreft>,t),
				::fast_io::details::decay::ndynamic_print_reserve_size<n-1,char_type>(args...));
		}
		else
		{
			return ::fast_io::details::decay::ndynamic_print_reserve_size<n-1,char_type>(args...);
		}
	}
}

template<bool needprintlf,::std::size_t n,::std::integral char_type,typename scattertype,typename T,typename ...Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr auto print_n_scatters_reserve(basic_io_scatter_t<scattertype> *pscatters,
	char_type *ptr,T t,Args ...args)
{
	if constexpr(n!=0)
	{
		using nocvreft = ::std::remove_cvref_t<T>;
		if constexpr(reserve_printable<char_type,nocvreft>||
		dynamic_reserve_printable<char_type,nocvreft>)
		{
			auto ptred{print_reserve_define(io_reserve_type<char_type,nocvreft>,ptr,t)};
#if 0
			if constexpr(sizeof...(Args)==0&&line)
			{
				*ptred=char_literal_v<u8'\n',char_type>;
				++ptred;
			}
#endif
			::std::size_t const sz{static_cast<::std::size_t>(ptred-ptr)};
			if constexpr(::std::same_as<scattertype,void>)
			{
				*pscatters=io_scatter_t{ptr,sz*sizeof(char_type)};
			}
			else
			{
				*pscatters=basic_io_scatter_t<char_type>{ptr,sz};
			}
			++pscatters;
			ptr=ptred;
		}
		else if constexpr(::std::same_as<nocvreft,basic_io_scatter_t<scattertype>>)
		{
			if constexpr(::std::same_as<scattertype,void>)
			{
				*pscatters=io_scatter_t{t.base,t.len*sizeof(char_type)};
			}
			else
			{
				*pscatters=t;
			}
			++pscatters;
		}
		else
		{
			basic_io_scatter_t<char_type> sct{print_scatter_define(::fast_io::io_reserve_type<char_type,T>,t)};
			if constexpr(::std::same_as<scattertype,void>)
			{
				*pscatters=io_scatter_t{sct.base,sct.len*sizeof(char_type)};
			}
			else
			{
				*pscatters=sct;
			}
			++pscatters;
		}
		if constexpr(1<n)
		{
			print_n_scatters_reserve<needprintlf,n-1,char_type>(pscatters,ptr,args...);
		}
	}
}

template<::std::integral char_type,typename T=char_type>
inline constexpr basic_io_scatter_t<T> line_scatter_common{__builtin_addressof(char_literal_v<u8'\n',char_type>),
	::std::same_as<T,void>?sizeof(char_type):1};

template<bool line,typename outputstmtype,::std::size_t skippings=0,typename T,typename ...Args>
inline constexpr void print_controls_impl(outputstmtype optstm,T t,Args ...args)
{
	using char_type = typename outputstmtype::output_char_type;
	using scatter_type = ::std::conditional_t<byte_output_stream<outputstmtype>,
		io_scatter_t,basic_io_scatter_t<char_type>>;
	constexpr contiguous_scatter_result res{::fast_io::details::decay::find_continuous_scatters_n<char_type,T,Args...>()};
	if constexpr(skippings!=0)
	{
		print_controls_impl<line,outputstmtype,skippings-1>(optstm,args...);
	}
	else if constexpr(sizeof...(Args)==0)
	{
		print_control_single<line>(optstm,t);
	}
	else if constexpr(res.position==0)
	{
		print_controls_impl<line>(optstm,args...);
	}
	else
	{
		if constexpr(line)
		{
			static_assert(res.position!=SIZE_MAX);
		}
		static_assert(SIZE_MAX!=sizeof...(Args));
		constexpr
			::std::size_t n{sizeof...(Args)+static_cast<::std::size_t>(1)};
		constexpr
			bool needprintlf{n==res.position&&line};
		if constexpr(res.neededspace==0&&res.hasscatters&&!res.needextraspacefordynamic)
		{
			constexpr
				::std::size_t scatterscount{res.position+static_cast<::std::size_t>(needprintlf)};
			{
				scatter_type scatters[scatterscount];
				::fast_io::details::decay::print_n_scatters<res.position,char_type>(scatters,t,args...);
				if constexpr(needprintlf)
				{
					scatters[n]=::fast_io::details::decay::line_scatter_common<char_type,
						::std::conditional_t<::std::same_as<scatter_type,io_scatter_t>,void,char_type>>;
				}
				if constexpr(::fast_io::byte_output_stream<outputstmtype>)
				{
					::fast_io::operations::scatter_write_all_bytes(optstm,scatters,scatterscount);
				}
				else
				{
					::fast_io::operations::scatter_write_all(optstm,scatters,scatterscount);
				}
			}
		}
		else
		{
			constexpr
				::std::size_t mxsize{static_cast<::std::size_t>(res.neededspace+static_cast<::std::size_t>(needprintlf))};
			if constexpr(!res.hasscatters)
			{
				static_assert(!needprintlf||res.neededspace!=SIZE_MAX);
				if constexpr(!res.needextraspacefordynamic)
				{
					if constexpr(res.neededspace==0)
					{
						if constexpr(needprintlf)
						{
							::fast_io::operations::char_put(optstm,::fast_io::char_literal_v<u8'\n',char_type>);
						}
					}
					else
					{
						constexpr
							::std::size_t buffersize{res.neededspace+
							static_cast<::std::size_t>(needprintlf)};
						char_type buffer[buffersize];
						char_type *ptred{::fast_io::details::decay::print_n_reserve<res.position,char_type>(buffer,t,args...)};
						if constexpr(needprintlf)
						{
							*ptred=::fast_io::char_literal_v<u8'\n',char_type>;
							++ptred;
						}
						::fast_io::operations::write_all(optstm,buffer,ptred);
					}
				}
				else if constexpr(res.needextraspacefordynamic)
				{
					::std::size_t dynsz{::fast_io::details::decay::ndynamic_print_reserve_size<res.position,char_type>(t,args...)};
					::std::size_t totalsz{::fast_io::details::intrinsics::add_or_overflow_die(mxsize,dynsz)};
					::fast_io::details::local_operator_new_array_ptr<char_type> newptr(totalsz);
					char_type *buffer{newptr.ptr};
					char_type *ptred{::fast_io::details::decay::print_n_reserve<res.position,char_type>(buffer,t,args...)};
					if constexpr(needprintlf)
					{
						*ptred=::fast_io::char_literal_v<u8'\n',char_type>;
						++ptred;
					}
					::fast_io::operations::write_all(optstm,buffer,ptred);
				}
			}
			else if constexpr(res.neededspace!=0&&!res.needextraspacefordynamic)
			{
				constexpr
					::std::size_t scatterscount{res.position+static_cast<::std::size_t>(line&&res.position==n)};
				scatter_type scatters[scatterscount];
				char_type buffer[res.neededspace];
				::fast_io::details::decay::print_n_scatters_reserve<needprintlf,res.position,char_type>(scatters,buffer,t,args...);
				if constexpr(n==res.position&&line)
				{
					scatters[n]=::fast_io::details::decay::line_scatter_common<char_type,
						::std::conditional_t<::std::same_as<scatter_type,io_scatter_t>,void,char_type>>;
				}
				if constexpr(::fast_io::byte_output_stream<outputstmtype>)
				{
					::fast_io::operations::scatter_write_all_bytes(optstm,scatters,scatterscount);
				}
				else
				{
					::fast_io::operations::scatter_write_all(optstm,scatters,scatterscount);
				}
			}
			else if constexpr(res.needextraspacefordynamic)
			{
				constexpr
					::std::size_t scatterscount{res.position+static_cast<::std::size_t>(line&&res.position==n)};
				::std::size_t dynsz{::fast_io::details::decay::ndynamic_print_reserve_size<res.position,char_type>(t,args...)};
				::std::size_t totalsz{::fast_io::details::intrinsics::add_or_overflow_die(mxsize,dynsz)};
				::fast_io::details::local_operator_new_array_ptr<char_type> newptr(totalsz);
				scatter_type scatters[scatterscount];
				char_type *buffer{newptr.ptr};
				::fast_io::details::decay::print_n_scatters_reserve<needprintlf,res.position,char_type>(scatters,buffer,t,args...);
				if constexpr(n==res.position&&line)
				{
					scatters[n]=::fast_io::details::decay::line_scatter_common<char_type,
						::std::conditional_t<::std::same_as<scatter_type,io_scatter_t>,void,char_type>>;
				}
				if constexpr(::fast_io::byte_output_stream<outputstmtype>)
				{
					::fast_io::operations::scatter_write_all_bytes(optstm,scatters,scatterscount);
				}
				else
				{
					::fast_io::operations::scatter_write_all(optstm,scatters,scatterscount);
				}
			}
			if constexpr(res.position!=n)
			{
				print_controls_impl<line,res.position-1>(optstm,args...);
			}
		}
	}
}

}

namespace operations
{

template<bool line,typename outputstmtype,typename... Args>
requires (::fast_io::output_stream<outputstmtype>||
	::fast_io::status_output_stream<outputstmtype>)
inline constexpr decltype(auto) print_freestanding_decay(outputstmtype optstm,Args... args)
{
	if constexpr(::fast_io::status_output_stream<outputstmtype>)
	{
		return status_print_define<line>(optstm,args...);
	}
	if constexpr(sizeof...(Args)==0)
	{
		if constexpr(line)
		{
			using char_type = typename outputstmtype::output_char_type;
			return ::fast_io::operations::char_put(optstm,char_literal_v<u8'\n',char_type>);
		}
		else
		{
			return;
		}
	}
	else if constexpr(::fast_io::details::mutex_unlocked_buffer_output_stream_impl<outputstmtype>)
	{
		::fast_io::operations::stream_ref_lock_guard lg{output_stream_mutex_ref_impl(optstm)};
		return print_freestanding_decay(
			::fast_io::details::output_stream_unlocked_ref_impl(optstm),args...);
	}
#if 0
	if constexpr(::fast_io::details::streamreflect::has_obuffer_ops<outputstmtype>)
	{
		::fast_io::details::decay::printrsvcontiguousimpl();
	}
#endif
	using output_char_type = typename outputstmtype::output_char_type;
	if constexpr(!::fast_io::buffer_output_stream<outputstmtype>)
	{
		return ::fast_io::details::decay::print_controls_impl<line>(optstm,args...);
	}
	else if constexpr(line)
	{
		::fast_io::details::decay::print_controls_line<line>(optstm,args...);
	}
	else
	{
		(::fast_io::details::decay::print_control_single<false>(optstm,args),...);
	}
}


template<typename output,typename ...Args>
#if 0
requires print_freestanding_okay<output,Args...>
#endif
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void print_freestanding(output&& outstm,Args&& ...args)
{
	print_freestanding_decay<false>(::fast_io::manipulators::output_stream_ref(outstm),
	io_print_forward<typename decltype(::fast_io::manipulators::output_stream_ref(outstm))::output_char_type>(io_print_alias(args))...);
}

template<output_stream output,typename ...Args>
#if 0
requires print_freestanding_okay<output,Args...>
#endif
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void println_freestanding(output&& outstm,Args&& ...args)
{
	print_freestanding_decay<true>(::fast_io::manipulators::output_stream_ref(outstm),
	io_print_forward<typename decltype(::fast_io::manipulators::output_stream_ref(outstm))::output_char_type>(io_print_alias(args))...);
}

}

}
