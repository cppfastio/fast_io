#pragma once

namespace fast_io
{

namespace details::decay
{

template<::std::integral char_type,typename T=char_type>
inline constexpr basic_io_scatter_t<T> line_scatter_common{__builtin_addressof(char_literal_v<u8'\n',char_type>),
	::std::same_as<T,void>?sizeof(char_type):1};

struct contiguous_scatter_result
{
::std::size_t position{};
::std::size_t neededscatters{};
::std::size_t neededspace{};
bool lastisreserve{};
bool hasscatters{};
bool hasreserve{};
bool hasdynamicreserve{};
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
		++ret.position;
		ret.hasscatters = true;
		ret.neededscatters=::fast_io::details::intrinsics::add_or_overflow_die_chain(ret.neededscatters,static_cast<::std::size_t>(1));
	}
	else if constexpr(::fast_io::reserve_printable<char_type,Arg>)
	{
		if constexpr(sizeof...(Args)!=0)
		{
			ret = find_continuous_scatters_n<char_type,Args...>();
		}
		else
		{
			ret.lastisreserve=true;
		}
		constexpr
			::std::size_t sz{print_reserve_size(::fast_io::io_reserve_type<char_type,Arg>)};
		static_assert(sz!=0);
		++ret.position;
		ret.neededscatters=
			::fast_io::details::intrinsics::add_or_overflow_die_chain(ret.neededscatters,static_cast<::std::size_t>(1));
		ret.neededspace=
			::fast_io::details::intrinsics::add_or_overflow_die_chain(ret.neededspace,sz);
		ret.hasreserve=true;
	}
	else if constexpr(::fast_io::dynamic_reserve_printable<char_type,Arg>)
	{
		if constexpr(sizeof...(Args)!=0)
		{
			ret = find_continuous_scatters_n<char_type,Args...>();
		}
		else
		{
			ret.lastisreserve=true;
		}
		++ret.position;
		ret.neededscatters=
			::fast_io::details::intrinsics::add_or_overflow_die_chain(ret.neededscatters,static_cast<::std::size_t>(1));
		ret.hasdynamicreserve=true;
	}
	else if constexpr(::fast_io::reserve_scatters_printable<char_type,Arg>)
	{
		if constexpr(sizeof...(Args)!=0)
		{
			ret = find_continuous_scatters_n<char_type,Args...>();
		}
		constexpr auto scatszres{print_reserve_scatters_size(::fast_io::io_reserve_type<char_type,Arg>)};
		static_assert(scatszres.scatters_size!=0);
		ret.hasscatters=true;
		ret.hasreserve=true;
		++ret.position;
		ret.neededspace=
			::fast_io::details::intrinsics::add_or_overflow_die_chain(ret.neededspace,scatszres.reserve_size);
		ret.neededscatters=
			::fast_io::details::intrinsics::add_or_overflow_die_chain(ret.neededscatters,scatszres.scatters_size);
	}
	return ret;
}

struct scatter_rsv_result
{
::std::size_t position{};
::std::size_t neededspace{};
};

template<bool findscatter,::std::integral char_type,typename Arg,typename ...Args>
inline constexpr scatter_rsv_result find_continuous_scatters_reserve_n()
{
	if constexpr(::fast_io::reserve_printable<char_type,Arg>&&!findscatter)
	{
		constexpr
			::std::size_t sz{print_reserve_size(::fast_io::io_reserve_type<char_type,Arg>)};
		if constexpr(sizeof...(Args)==0)
		{
			return {1,sz};
		}
		else
		{
			auto res{find_continuous_scatters_reserve_n<findscatter,char_type,Args...>()};
			return {res.position+1,
			::fast_io::details::intrinsics::add_or_overflow_die_chain(res.neededspace,sz)};
		}
	}
	else if constexpr(::fast_io::scatter_printable<char_type,Arg>&&findscatter)
	{
		if constexpr(sizeof...(Args)==0)
		{
			return {1,0};
		}
		else
		{
			auto res{find_continuous_scatters_reserve_n<findscatter,char_type,Args...>()};
			return {res.position+1,0};
		}
	}
	else
	{
		return {0,0};
	}
}

template<typename output,std::size_t N>
inline constexpr bool minimum_buffer_output_stream_require_size_constant_impl =
	(N<obuffer_minimum_size_define(::fast_io::io_reserve_type<typename output::output_char_type,output>));

template<typename output,std::size_t N>
concept minimum_buffer_output_stream_require_size_impl = ::fast_io::operations::decay::defines::has_obuffer_minimum_size_operations<output>
	&& minimum_buffer_output_stream_require_size_constant_impl<output,N>;

template<::std::size_t sz>
requires (sz!=0)
inline constexpr void scatter_rsv_update_times(::fast_io::io_scatter_t *first,::fast_io::io_scatter_t *last) noexcept
{
	if constexpr(sz!=1)
	{
		for(;first!=last;++first)
		{
			first->len*=sz;
		}
	}
}

template<::std::integral char_type>
struct basic_reserve_scatters_define_byte_result
{
	io_scatter_t *scatters_pos_ptr;
	char_type *reserve_pos_ptr;
};

template<::std::integral char_type,typename T>
inline ::fast_io::details::decay::basic_reserve_scatters_define_byte_result<char_type> prrsvsct_byte_common_rsvsc_impl(io_scatter_t* pscatters,char_type* buffer,T t)
{
	using basicioscattertypealiasptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= basic_io_scatter_t<char_type>*;
	using scatterioscattertypealiasptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= io_scatter_t*;
	auto result{print_reserve_scatters_define(::fast_io::io_reserve_type<char_type,::std::remove_cvref_t<T>>,
		reinterpret_cast<basicioscattertypealiasptr>(pscatters),buffer,t)};
	scatterioscattertypealiasptr ptr{reinterpret_cast<scatterioscattertypealiasptr>(result.scatters_pos_ptr)};
	if constexpr(sizeof(char_type)!=1)
	{
		scatter_rsv_update_times<sizeof(char_type)>(pscatters,ptr);
	}
	return {ptr,result.reserve_pos_ptr};
}

template<::std::integral char_type,typename T>
inline auto prrsvsct_byte_common_impl(io_scatter_t* pscatters,char_type const* buffer,T t)
{
	return ::fast_io::details::decay::prrsvsct_byte_common_rsvsc_impl(pscatters,buffer,t).scatters_pos_ptr;
}

template<bool line=false,typename output,typename T>
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
			scatter=print_scatter_define(::fast_io::io_reserve_type<char_type,value_type>,t);
		}
		if constexpr(line)
		{
			if constexpr(contiguous_output_stream<output>)
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
			else if constexpr(::fast_io::operations::decay::defines::has_obuffer_basic_operations<output>)
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
			if constexpr(::fast_io::operations::decay::defines::has_any_of_write_or_seek_pwrite_bytes_operations<output>)
			{
				::fast_io::io_scatter_t scatters[2]
				{{t.base,t.len*sizeof(char_type)},
				{__builtin_addressof(char_literal_v<u8'\n',char_type>),
				sizeof(char_type)}};
				::fast_io::operations::decay::scatter_write_all_bytes_decay(outstm,scatters,2);
			}
			else
			{
				::fast_io::basic_io_scatter_t<T> scatters[2]
				{t,
				{__builtin_addressof(char_literal_v<u8'\n',char_type>),
				1}};
				::fast_io::operations::decay::scatter_write_all_decay(outstm,scatters,2);
			}
		}
		else
		{
			::fast_io::operations::decay::write_all_decay(outstm,t.base,t.base+t.len);
		}
	}
	else if constexpr(reserve_printable<char_type,value_type>)
	{
		constexpr std::size_t real_size{print_reserve_size(::fast_io::io_reserve_type<char_type,value_type>)};
		constexpr std::size_t size{real_size+static_cast<std::size_t>(line)};
		static_assert(real_size<PTRDIFF_MAX);
#if 0
		if constexpr(contiguous_output_stream<output>)
		{
			auto bcurr{obuffer_curr(outstm)};
			auto bend{obuffer_end(outstm)};
			std::size_t diff{static_cast<std::size_t>(bend-bcurr)};
			if(diff<size)[[unlikely]]
				fast_terminate();
			auto it{print_reserve_define(::fast_io::io_reserve_type<char_type,value_type>,bcurr,t)};
			if constexpr(line)
			{
				*it=lfch;
				++it;
			}
			obuffer_set_curr(outstm,it);
		}
		else
#endif
		{
			if constexpr(::fast_io::operations::decay::defines::has_obuffer_basic_operations<output>&&!asan_activated)
			{
				char_type* bcurr{obuffer_curr(outstm)};
				char_type* bend{obuffer_end(outstm)};
				std::ptrdiff_t const diff(bend-bcurr);
				bool smaller{static_cast<std::ptrdiff_t>(size)<diff};
				if constexpr(minimum_buffer_output_stream_require_size_impl<output,size>)
				{
					if(!smaller)[[unlikely]]
					{
						obuffer_minimum_size_flush_prepare_define(outstm);
						bcurr=obuffer_curr(outstm);
					}
					bcurr=print_reserve_define(::fast_io::io_reserve_type<char_type,value_type>,bcurr,t);
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
					{
						bcurr=buffer;
					}
					bcurr=print_reserve_define(::fast_io::io_reserve_type<char_type,value_type>,bcurr,t);
					if constexpr(line)
					{
						*bcurr=lfch;
						++bcurr;
					}
					if(smaller)[[likely]]
						obuffer_set_curr(outstm,bcurr);
					else[[unlikely]]
						::fast_io::operations::decay::write_all_decay(outstm,buffer,bcurr);
				}
			}
			else
			{
				char_type buffer[size];
				char_type *i{print_reserve_define(::fast_io::io_reserve_type<char_type,value_type>,buffer,t)};
				if constexpr(line)
				{
					*i=lfch;
					++i;
				}
				::fast_io::operations::decay::write_all_decay(outstm,buffer,i);
			}
		}
	}
	else if constexpr(dynamic_reserve_printable<char_type,value_type>)
	{
		std::size_t size{print_reserve_size(::fast_io::io_reserve_type<char_type,value_type>,t)};
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
#if 0
		if constexpr(contiguous_output_stream<output>)
		{
			auto bcurr{obuffer_curr(outstm)};
			auto bend{obuffer_end(outstm)};
			auto it{print_reserve_define(::fast_io::io_reserve_type<char_type,value_type>,bcurr,t,size)};
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
#endif
		{
			if constexpr(::fast_io::operations::decay::defines::has_obuffer_basic_operations<output>&&!asan_activated)
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

				auto it{print_reserve_define(::fast_io::io_reserve_type<char_type,value_type>,toptr,t)};
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
					::fast_io::operations::decay::write_all_decay(outstm,toptr,it);
				}
			}
			else
			{
				::fast_io::details::local_operator_new_array_ptr<char_type> newptr(size);
				auto it{print_reserve_define(::fast_io::io_reserve_type<char_type,value_type>,newptr.ptr,t)};
				if constexpr(line)
				{
					*it=lfch;
					++it;
				}
				::fast_io::operations::decay::write_all_decay(outstm,newptr.ptr,it);
			}
		}
	}
	else if constexpr(reserve_scatters_printable<char_type,value_type>)
	{
		constexpr auto sz{print_reserve_scatters_size(::fast_io::io_reserve_type<char_type,value_type>)};
		static_assert(!line||sz.scatters_size!=SIZE_MAX);
		constexpr
			::std::size_t scattersnum{sz.scatters_size+static_cast<::std::size_t>(line)};
#if __cpp_if_consteval >= 202106L
		if !consteval
#else
		if(!__builtin_is_constant_evaluated())
#endif
		{
			if constexpr(::fast_io::operations::decay::defines::has_any_of_write_or_seek_pwrite_bytes_operations<output>)
			{
				::fast_io::io_scatter_t scattersbuffer[scattersnum];
				char_type buffer[sz.reserve_size];
				::fast_io::io_scatter_t *ptr{::fast_io::details::decay::prrsvsct_byte_common_impl(scattersbuffer,buffer,t)};
				if constexpr(line)
				{
					*ptr=::fast_io::details::decay::line_scatter_common<char_type,void>;
					++ptr;
				}
				::fast_io::operations::decay::scatter_write_all_bytes_decay(outstm,scattersbuffer,ptr);
				return;
			}
		}
		::fast_io::basic_io_scatter_t<char_type> scattersbuffer[scattersnum];
		char_type buffer[sz.reserve_size];
		auto ptr{print_reserve_scatters_define(::fast_io::io_reserve_type<char_type,::std::remove_cvref_t<T>>,scattersbuffer,buffer,t).scatters_pos_ptr};
		if constexpr(line)
		{
			*ptr=::fast_io::details::decay::line_scatter_common<char_type>;
			++ptr;
		}
		::fast_io::operations::decay::scatter_write_all_decay(outstm,scattersbuffer,ptr);
	}
	else if constexpr(printable<char_type,value_type>)
	{
		print_define(::fast_io::io_reserve_type<char_type,value_type>,outstm,t);
		if constexpr(line)
			::fast_io::operations::decay::char_put_decay(outstm,lfch);
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
		return {ret.position+1,ret.neededspace,ret.hasdynamicreserve};
	}
	else if constexpr(::fast_io::reserve_printable<char_type,Arg>)
	{
		constexpr
			::std::size_t sz{print_reserve_size(::fast_io::io_reserve_type<char_type,Arg>)};
		return {ret.position+1,
			::fast_io::details::add_overflow(ret.neededspace,sz),
			ret.hasdynamicreserve};
	}
	else if constexpr(::fast_io::dynamic_reserve_printable<char_type,Arg>)
	{
		return {ret.position+1,ret.neededspace,true};
	}
}
#endif

template<bool ln,typename output,typename T,typename... Args>
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
		if constexpr(sizeof...(Args)==0||n<2)
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
#if __has_cpp_attribute(maybe_unused)
	[[maybe_unused]]
#endif
	T t,
#if __has_cpp_attribute(maybe_unused)
	[[maybe_unused]]
#endif
	Args ...args)
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
			return print_n_scatters<n-1,char_type>(pscatters,args...);
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
	char_type *ptr,T t,Args ...args);

template<::std::integral char_type,typename T,typename ...Args>
inline constexpr bool print_next_is_reserve() noexcept
{
	using nocvreft = ::std::remove_cvref_t<T>;
	if constexpr(::fast_io::reserve_printable<char_type,nocvreft>||
		::fast_io::dynamic_reserve_printable<char_type,nocvreft>)
	{
		return true;
	}
	else
	{
		return false;
	}
}

template<::std::integral char_type>
inline constexpr bool print_next_is_reserve() noexcept
{
	return false;
}

template<bool needprintlf,::std::size_t n,::std::integral char_type,typename scattertype,typename T,typename ...Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr auto print_n_scatters_reserve_cont(basic_io_scatter_t<scattertype> *pscatters,char_type *base,char_type *ptr,T t,Args ...args)
{
	if constexpr(n!=0)
	{
		using nocvreft = ::std::remove_cvref_t<T>;
		if constexpr(reserve_printable<char_type,nocvreft>||
			dynamic_reserve_printable<char_type,nocvreft>)
		{
			ptr=print_reserve_define(::fast_io::io_reserve_type<char_type,nocvreft>,ptr,t);
			if constexpr(::fast_io::details::decay::print_next_is_reserve<char_type,Args...>())
			{
				return ::fast_io::details::decay::print_n_scatters_reserve_cont<needprintlf,n-1,char_type>(pscatters,base,ptr,args...);
			}
			else
			{
				if constexpr(n==1&&needprintlf)
				{
					*ptr=char_literal_v<u8'\n',char_type>;
					++ptr;
				}
				::std::size_t const sz{static_cast<::std::size_t>(ptr-base)};
				if constexpr(::std::same_as<scattertype,void>)
				{
					*pscatters=io_scatter_t{base,sz*sizeof(char_type)};
				}
				else
				{
					*pscatters=basic_io_scatter_t<char_type>{base,sz};
				}
				++pscatters;
				if constexpr(1<n)
				{
					return ::fast_io::details::decay::print_n_scatters_reserve<needprintlf,n-1,char_type>(pscatters,ptr,args...);
				}
			}

		}
		else
		{
			::std::size_t const sz{static_cast<::std::size_t>(ptr-base)};
			if constexpr(::std::same_as<scattertype,void>)
			{
				*pscatters=io_scatter_t{base,sz*sizeof(char_type)};
			}
			else
			{
				*pscatters=basic_io_scatter_t<char_type>{base,sz};
			}
			++pscatters;
			if constexpr(1<n)
			{
				return ::fast_io::details::decay::print_n_scatters_reserve<needprintlf,n-1,char_type>(pscatters,ptr,t,args...);
			}
		}
	}
	return pscatters;
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
		if constexpr(::fast_io::reserve_printable<char_type,nocvreft>||
			::fast_io::dynamic_reserve_printable<char_type,nocvreft>)
		{
			auto ptred{print_reserve_define(::fast_io::io_reserve_type<char_type,nocvreft>,ptr,t)};
			if constexpr(sizeof...(Args)!=0&&::fast_io::details::decay::print_next_is_reserve<char_type,Args...>())
			{
				return ::fast_io::details::decay::print_n_scatters_reserve_cont<needprintlf,n-1,char_type>(pscatters,ptr,ptred,args...);
			}
			else
			{
				if constexpr(n==1&&needprintlf)
				{
					*ptred=char_literal_v<u8'\n',char_type>;
					++ptred;
				}
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
				if constexpr(1<n)
				{
					return ::fast_io::details::decay::print_n_scatters_reserve<needprintlf,n-1,char_type>(pscatters,ptred,args...);
				}
			}
		}
		else if constexpr(::fast_io::scatter_printable<char_type,nocvreft>)
		{
			if constexpr(::std::same_as<nocvreft,basic_io_scatter_t<scattertype>>)
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
			++pscatters;
			if constexpr(n==1&&needprintlf)
			{
				*pscatters=::fast_io::details::decay::line_scatter_common<char_type,scattertype>;
				++pscatters;
			}
			if constexpr(1<n)
			{
				return ::fast_io::details::decay::print_n_scatters_reserve<needprintlf,n-1,char_type>(pscatters,ptr,args...);
			}
		}
		else if constexpr(::fast_io::reserve_scatters_printable<char_type,nocvreft>)
		{
			if constexpr(::std::same_as<scattertype,void>)
			{
				auto pit{::fast_io::details::decay::prrsvsct_byte_common_rsvsc_impl(pscatters,ptr,t)};
				if constexpr(1<n)
				{
					return ::fast_io::details::decay::print_n_scatters_reserve<needprintlf,n-1,char_type>(pit.scatters_pos_ptr,pit.reserve_pos_ptr,args...);
				}
				else
				{
					return pscatters;
				}
			}
			else
			{
				auto pit{print_reserve_scatters_define(::fast_io::io_reserve_type<char_type,nocvreft>,
					pscatters,ptr,t)};
				if constexpr(1<n)
				{
					return ::fast_io::details::decay::print_n_scatters_reserve<needprintlf,n-1,char_type>(pit.scatters_pos_ptr,pit.reserve_pos_ptr,args...);
				}
			}
		}
	}
	return pscatters;
}

template<bool line,typename outputstmtype,::std::size_t skippings=0,typename T,typename ...Args>
inline constexpr void print_controls_impl(outputstmtype optstm,T t,Args ...args)
{
	using char_type = typename outputstmtype::output_char_type;
	using scatter_type = ::std::conditional_t<::fast_io::operations::decay::defines::has_any_of_write_or_seek_pwrite_bytes_operations<outputstmtype>,
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
		print_control_single<false>(optstm,t);
		print_controls_impl<line>(optstm,args...);
	}
	else
	{
		if constexpr(line)
		{
			static_assert(res.neededscatters!=SIZE_MAX);
		}
		static_assert(SIZE_MAX!=sizeof...(Args));
		constexpr
			::std::size_t n{sizeof...(Args)+static_cast<::std::size_t>(1)};
		constexpr
			bool needprintlf{n==res.position&&line};
		if constexpr(res.hasscatters&&!res.hasreserve&&!res.hasdynamicreserve)
		{
			constexpr
				::std::size_t scatterscount{res.neededscatters+static_cast<::std::size_t>(needprintlf)};
			{
				scatter_type scatters[scatterscount];
				::fast_io::details::decay::print_n_scatters<res.position,char_type>(scatters,t,args...);
				if constexpr(needprintlf)
				{
					scatters[n]=::fast_io::details::decay::line_scatter_common<char_type,
						::std::conditional_t<::std::same_as<scatter_type,io_scatter_t>,void,char_type>>;
				}
				if constexpr(::fast_io::operations::decay::defines::has_any_of_write_or_seek_pwrite_bytes_operations<outputstmtype>)
				{
					::fast_io::operations::decay::scatter_write_all_bytes_decay(optstm,scatters,scatterscount);
				}
				else
				{
					::fast_io::operations::decay::scatter_write_all_decay(optstm,scatters,scatterscount);
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
				if constexpr(res.hasdynamicreserve)
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
					::fast_io::operations::decay::write_all_decay(optstm,buffer,ptred);
				}
				else if constexpr(res.hasreserve)
				{
					if constexpr(res.neededspace==0)
					{
						if constexpr(needprintlf)
						{
							::fast_io::operations::decay::char_put_decay(optstm,::fast_io::char_literal_v<u8'\n',char_type>);
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
						::fast_io::operations::decay::write_all_decay(optstm,buffer,ptred);
					}
				}
			}
			else if constexpr(res.hasreserve&&!res.hasdynamicreserve)
			{
				constexpr
					::std::size_t scatterscount{res.neededscatters+static_cast<::std::size_t>(line&&res.position==n)};
				scatter_type scatters[scatterscount];
				char_type buffer[res.neededspace];

				auto ptr{::fast_io::details::decay::print_n_scatters_reserve<needprintlf,res.position,char_type>(scatters,buffer,t,args...)};
				::std::size_t diff{static_cast<::std::size_t>(ptr-scatters)};

				if constexpr(::fast_io::operations::decay::defines::has_any_of_write_or_seek_pwrite_bytes_operations<outputstmtype>)
				{
					::fast_io::operations::decay::scatter_write_all_bytes_decay(optstm,scatters,diff);
				}
				else
				{
					::fast_io::operations::decay::scatter_write_all_decay(optstm,scatters,diff);
				}
			}
			else if constexpr(res.hasdynamicreserve)
			{
				constexpr
					::std::size_t scatterscount{res.neededscatters+static_cast<::std::size_t>(line&&res.position==n)};
				::std::size_t dynsz{::fast_io::details::decay::ndynamic_print_reserve_size<res.position,char_type>(t,args...)};
				::std::size_t totalsz{::fast_io::details::intrinsics::add_or_overflow_die(mxsize,dynsz)};
				::fast_io::details::local_operator_new_array_ptr<char_type> newptr(totalsz);
				scatter_type scatters[scatterscount];
				char_type *buffer{newptr.ptr};
				auto ptr{::fast_io::details::decay::print_n_scatters_reserve<needprintlf,res.position,char_type>(scatters,buffer,t,args...)};
				::std::size_t diff{static_cast<::std::size_t>(ptr-scatters)};
				if constexpr(::fast_io::operations::decay::defines::has_any_of_write_or_seek_pwrite_bytes_operations<outputstmtype>)
				{
					::fast_io::operations::decay::scatter_write_all_bytes_decay(optstm,scatters,diff);
				}
				else
				{
					::fast_io::operations::decay::scatter_write_all_decay(optstm,scatters,diff);
				}
			}
			if constexpr(res.position!=n)
			{
				print_controls_impl<line,outputstmtype,res.position-1>(optstm,args...);
			}
		}
	}
}

template<bool line,typename outputstmtype,::std::size_t skippings=0,typename T,typename ...Args>
inline constexpr void print_controls_buffer_impl(outputstmtype optstm,T t,Args ...args)
{
	if constexpr(skippings!=0)
	{
		::fast_io::details::decay::print_controls_buffer_impl<line,outputstmtype,skippings-1>(optstm,args...);
	}
	else if constexpr(sizeof...(Args)==0)
	{
		print_control_single<line>(optstm,t);
	}
	else
	{
		using char_type = typename outputstmtype::output_char_type;
		static_assert(SIZE_MAX!=sizeof...(Args));
		constexpr
			::std::size_t n{sizeof...(Args)+static_cast<::std::size_t>(1)};
		constexpr auto scatters_result{::fast_io::details::decay::find_continuous_scatters_reserve_n<true,char_type,T,Args...>()};
		using scatter_type = ::std::conditional_t<::fast_io::operations::decay::defines::has_any_of_write_or_seek_pwrite_bytes_operations<outputstmtype>,
			io_scatter_t,basic_io_scatter_t<char_type>>;
		if constexpr(scatters_result.position!=0)
		{
			if constexpr(line)
			{
				static_assert(scatters_result.position!=SIZE_MAX);
			}
			constexpr
				bool needprintlf{n==scatters_result.position&&line};
			constexpr
				::std::size_t scatterscount{scatters_result.position+static_cast<::std::size_t>(needprintlf)};
			scatter_type scatters[scatterscount];
			::fast_io::details::decay::print_n_scatters<scatters_result.position,char_type>(scatters,t,args...);
			if constexpr(::fast_io::operations::decay::defines::has_any_of_write_or_seek_pwrite_bytes_operations<outputstmtype>)
			{
				if constexpr(needprintlf)
				{
					scatters[scatterscount-1]=::fast_io::details::decay::line_scatter_common<char_type,void>;
				}
				::fast_io::operations::decay::scatter_write_all_bytes_decay(optstm,scatters,scatterscount);
			}
			else
			{
				if constexpr(needprintlf)
				{
					scatters[scatterscount-1]=::fast_io::details::decay::line_scatter_common<char_type>;
				}
				::fast_io::operations::decay::scatter_write_all_decay(optstm,scatters,scatterscount);
			}
			if constexpr(scatters_result.position!=n)
			{
				::fast_io::details::decay::print_controls_buffer_impl<line,outputstmtype,scatters_result.position-1>(optstm,args...);
			}
		}
		else
		{
			constexpr auto rsvresult{::fast_io::details::decay::find_continuous_scatters_reserve_n<false,char_type,T,Args...>()};
			if constexpr(1<rsvresult.position)
			{
				constexpr
					bool needprintlf{n==rsvresult.position&&line};
				constexpr
					::std::size_t buffersize{rsvresult.neededspace+
					static_cast<::std::size_t>(needprintlf)};
				char_type* bcurr{obuffer_curr(optstm)};
				char_type* bend{obuffer_end(optstm)};
				std::ptrdiff_t const diff(bend-bcurr);
				bool smaller{static_cast<std::ptrdiff_t>(buffersize)<diff};
				if constexpr(minimum_buffer_output_stream_require_size_impl<outputstmtype,buffersize>)
				{
					if(!smaller)[[unlikely]]
					{
						obuffer_minimum_size_flush_prepare_define(optstm);
						bcurr=obuffer_curr(optstm);
					}
					bcurr=::fast_io::details::decay::print_n_reserve<rsvresult.position,char_type>(bcurr,t,args...);
					if constexpr(needprintlf)
					{
						*bcurr=::fast_io::char_literal_v<u8'\n',char_type>;;
						++bcurr;
					}
					obuffer_set_curr(optstm,bcurr);
				}
				else
				{
					char_type buffer[buffersize];
					if(!smaller)[[unlikely]]
						bcurr=buffer;
					bcurr=::fast_io::details::decay::print_n_reserve<rsvresult.position,char_type>(bcurr,t,args...);
					if constexpr(needprintlf)
					{
						*bcurr=::fast_io::char_literal_v<u8'\n',char_type>;
						++bcurr;
					}
					if(smaller)[[likely]]
						obuffer_set_curr(optstm,bcurr);
					else[[unlikely]]
					{
						::fast_io::operations::decay::write_all_decay(optstm,buffer,bcurr);
					}
				}
				if constexpr(rsvresult.position!=n)
				{
					::fast_io::details::decay::print_controls_buffer_impl<line,outputstmtype,rsvresult.position-1>(optstm,args...);
				}
			}
			else
			{
				::fast_io::details::decay::print_control_single<line&&sizeof...(args)==0>(optstm,t);
				if constexpr(sizeof...(args)!=0)
				{
					::fast_io::details::decay::print_controls_buffer_impl<line>(optstm,args...);
				}
			}
		}
	}
}

}

namespace operations
{

namespace decay
{

template<bool line,typename outputstmtype,typename... Args>
inline constexpr decltype(auto) print_freestanding_decay(outputstmtype optstm,Args... args)
{
	if constexpr(::fast_io::operations::decay::defines::has_status_print_define<outputstmtype>)
	{
		return status_print_define<line>(optstm,args...);
	}
	else if constexpr(sizeof...(Args)==0)
	{
		if constexpr(line)
		{
			using char_type = typename outputstmtype::output_char_type;
			return ::fast_io::operations::decay::char_put_decay(optstm,char_literal_v<u8'\n',char_type>);
		}
		else
		{
			return;
		}
	}
	else if constexpr(::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outputstmtype>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::output_stream_mutex_ref_decay(optstm)};
		return ::fast_io::operations::decay::print_freestanding_decay<line>(
			::fast_io::operations::decay::output_stream_unlocked_ref_decay(optstm),args...);
	}
	else if constexpr(::fast_io::operations::decay::defines::has_obuffer_basic_operations<outputstmtype>)
	{
		return ::fast_io::details::decay::print_controls_buffer_impl<line>(optstm,args...);
	}
	else
	{
		return ::fast_io::details::decay::print_controls_impl<line>(optstm,args...);
	}
}

template<bool line,typename outputstmtype,typename... Args>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr decltype(auto) print_freestanding_decay_cold(outputstmtype optstm,Args... args)
{
#if !__has_cpp_attribute(__gnu__::__cold__)&&__has_cpp_attribute(unlikely)
	if(true)[[unlikely]]
#endif
	return ::fast_io::operations::decay::print_freestanding_decay<line>(optstm,args...);
}

namespace defines
{

template<typename char_type,typename ...Args>
concept print_freestanding_params_decay_okay =
::std::integral<char_type>&&((::fast_io::printable<char_type,Args>||
	::fast_io::reserve_printable<char_type,Args>||
	::fast_io::dynamic_reserve_printable<char_type,Args>||
	::fast_io::scatter_printable<char_type,Args>||
	::fast_io::reserve_scatters_printable<char_type,Args>)&&...);

}

}

namespace defines
{
template<typename output,typename ...Args>
concept print_freestanding_okay = ::fast_io::operations::defines::has_output_or_io_stream_ref_define<output>&&
	::fast_io::operations::decay::defines::print_freestanding_params_decay_okay<
	typename decltype(::fast_io::operations::output_stream_ref(*static_cast<output*>(nullptr)))::output_char_type,
	Args...>;
}

template<bool line,typename output,typename ...Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void print_freestanding(output&& outstm,Args&& ...args)
{
	::fast_io::operations::decay::print_freestanding_decay<line>(::fast_io::operations::output_stream_ref(outstm),
	io_print_forward<typename decltype(::fast_io::operations::output_stream_ref(outstm))::output_char_type>(io_print_alias(args))...);
}

}

}
