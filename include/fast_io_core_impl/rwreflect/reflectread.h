#pragma once

namespace fast_io
{

namespace details
{

namespace streamreflect
{

template<typename T>
concept has_ibuffer_basic_ops = requires(T instm,typename T::char_type *ptr)
{
	ibuffer_begin(instm);
	ibuffer_curr(instm);
	ibuffer_end(instm);
	ibuffer_set_curr(instm,ptr);
};

template<typename T>
concept has_ibuffer_all_ops = has_ibuffer_basic_ops<T>&&(requires(T instm,typename T::char_type *cptr)
{
	ibuffer_read_all_underflow_define(instm,cptr,cptr);
});

template<typename T>
concept has_ibuffer_some_ops = has_ibuffer_basic_ops<T>&&(requires(T instm,typename T::char_type *cptr)
{
	ibuffer_read_all_underflow_define(instm,cptr,cptr);
});

template<typename T>
concept has_ibuffer_ops = has_ibuffer_all_ops<T>||has_ibuffer_some_ops<T>;

template<typename T>
concept has_read_some_define = requires(T instm,typename T::char_type* ptr)
{
	read_some_define(instm,ptr,ptr);
};

template<typename T>
concept has_read_all_define = requires(T instm,typename T::char_type* ptr)
{
	read_all_define(instm,ptr,ptr);
};

template<typename T>
concept has_read_some_bytes_define = requires(T instm,::std::byte* ptr)
{
	read_some_bytes_define(instm,ptr,ptr);
};

template<typename T>
concept has_read_all_bytes_define = requires(T instm,::std::byte* ptr)
{
	read_all_bytes_define(instm,ptr,ptr);
};

template<typename T>
concept has_scatter_read_some_bytes_define = requires(T instm,::fast_io::io_scatter_t *scatter,::std::size_t len)
{
	scatter_read_some_bytes_define(instm,scatter,len);
};

template<typename T>
concept has_scatter_read_all_bytes_define = requires(T instm,::fast_io::io_scatter_t *scatter,::std::size_t len)
{
	scatter_read_all_bytes_define(instm,scatter,len);
};


template<typename T>
concept has_scatter_read_some_define = requires(T instm,::fast_io::basic_io_scatter_t<typename T::char_type> *pscatter,::std::size_t len)
{
	scatter_read_some_define(instm,pscatter,len);
};

template<typename T>
concept has_scatter_read_all_define = requires(T instm,::fast_io::basic_io_scatter_t<typename T::char_type> *pscatter,::std::size_t len)
{
	scatter_read_all_define(instm,pscatter,len);
};

}

template<typename F>
inline constexpr ::std::byte* read_some_common_chtypeptr_impl(F instm,
	::std::byte* firstbyteptr,::std::byte* lastbyteptr,::std::size_t n)
{
	auto it{firstbyteptr};
	while((it=read_some_bytes_define(instm,
			it,lastbyteptr))!=lastbyteptr)
	{
		::std::size_t diff{static_cast<::std::size_t>(it-firstbyteptr)};
		if(!(diff%n))
		{
			break;
		}
	}
	return it;
}

template<typename F,typename value_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr value_type* read_ibf_some_underflow_impl(F instm,value_type* first,value_type* last)
{
	if constexpr(::fast_io::details::streamreflect::has_ibuffer_some_ops<F>)
	{
		return ibuffer_read_some_underflow_define(instm,first,last);
	}
	else
	{
		ibuffer_read_all_underflow_define(instm,first,last);
		return last;
	}
}

template<typename F,typename value_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void read_ibf_all_underflow_impl(F instm,value_type* first,value_type* last)
{
	if constexpr(::fast_io::details::streamreflect::has_ibuffer_all_ops<F>)
	{
		ibuffer_read_all_underflow_define(instm,first,last);
	}
	else
	{
		while((first=ibuffer_read_some_underflow_define(instm,first,last))!=last);
	}
}

template<typename F,typename value_type>
inline constexpr value_type* read_some_common_chtypeptr_impl(F instm,value_type *first,value_type *last)
{
	using char_type = typename F::char_type;
	constexpr bool smtp{::std::same_as<char_type,value_type>};
	if constexpr(smtp&&(::fast_io::details::streamreflect::has_ibuffer_ops<F>))
	{
		char_type *curr{ibuffer_curr(instm)};
		char_type *ed{ibuffer_end(instm)};
		auto bufferremain{ed-curr};
		auto itdf{last-first};
		if(itdf<bufferremain)
#if __has_cpp_attribute(likely)
			[[likely]]
#endif
		{
			non_overlapped_copy_n(curr,static_cast<::std::size_t>(itdf),first);
			ibuffer_set_curr(instm,curr+itdf);
			return last;
		}
		return ::fast_io::details::read_ibf_all_underflow_impl(instm,first,last);
	}
	else if constexpr(smtp&&(::fast_io::details::streamreflect::has_read_some_define<F>))
	{
		return read_some_define(instm,first,last);
	}
	else if constexpr(smtp&&(::fast_io::details::streamreflect::has_scatter_read_some_define<F>))
	{
		basic_io_scatter_t<value_type> scatter
		{
			first,
			static_cast<::std::size_t>(last-first)
		};
		auto ret{scatter_read_some_define(instm,__builtin_addressof(scatter),1)};
		return ret.total_size+first;
	}
	else if constexpr(smtp&&(::fast_io::details::streamreflect::has_read_all_define<F>))
	{
		read_all_define(instm,first,last);
		return last;
	}
	else if constexpr(smtp&&(::fast_io::details::streamreflect::has_scatter_read_all_define<F>))
	{
		basic_io_scatter_t<value_type> scatter
		{
			first,
			static_cast<::std::size_t>(last-first)
		};
		auto ret{scatter_read_all_define(instm,__builtin_addressof(scatter),1)};
		return last;
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_some_bytes_define<F>)
	{
		::std::byte *firstbyteptr{reinterpret_cast<::std::byte*>(first)};
		::std::byte *lastbyteptr{reinterpret_cast<::std::byte*>(last)};
		if constexpr(sizeof(char_type)==sizeof(::std::byte))
		{
			auto p{read_some_bytes_define(instm,
				firstbyteptr,
				lastbyteptr)};
			return p-firstbyteptr+first;
		}
		else
		{
			auto it{firstbyteptr};
			::std::size_t n{};
			do
			{
				it=::fast_io::details::read_some_common_chtypeptr_impl(
					instm,it,lastbyteptr,sizeof(value_type));
				n=static_cast<::std::size_t>(it-firstbyteptr);
				if(n%sizeof(value_type)==0)
				{
					break;
				}
			}
			while(it!=lastbyteptr);
			::std::size_t const diff{n/sizeof(value_type)};

			auto retv{first+diff};
#if __has_cpp_attribute(assume)
			[[assume(reinterpret_cast<::std::byte*>(retv)==it)]];
#endif
			return retv;
		}
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_all_bytes_define<F>)
	{
		::std::byte *firstbyteptr{reinterpret_cast<::std::byte*>(first)};
		::std::byte *lastbyteptr{reinterpret_cast<::std::byte*>(last)};
		read_all_bytes_define(instm,firstbyteptr,lastbyteptr);
		return last;
	}
	else
	{
		return first;
	}
}

template<typename outputtype,::std::forward_iterator Iter>
inline constexpr Iter read_some_common_iter_impl(outputtype instm,Iter first,Iter last)
{
	using char_type = typename outputtype::char_type;
	using value_type = ::std::iter_value_t<Iter>;
	if constexpr(::std::contiguous_iterator<Iter>)
	{
		return ::fast_io::details::read_some_common_chtypeptr_impl(
			instm,
			::std::to_address(first),
			::std::to_address(last))-::std::to_address(first)+first;
	}
	else
	{
		for(;first!=last;++first)
		{
			auto fptr{__builtin_addressof(*first)};
			auto fptrp1{fptr+1};
			if(::fast_io::details::read_some_common_chtypeptr_impl(
				instm,fptr,fptrp1)!=fptrp1)
			{
				break;
			}
		}
		return first;
	}
}


template<typename F>
inline constexpr void read_all_bytes_common_chtypeptr_impl(F instm,::std::byte* firstbyteptr,::std::byte* lastbyteptr)
{
	while((firstbyteptr=read_some_bytes_define(instm,
		firstbyteptr,
		lastbyteptr))!=lastbyteptr);
}

template<typename F,typename value_type>
inline constexpr void read_all_common_chtypeptr_impl(F instm,value_type* first,value_type* last)
{
	using char_type = typename F::char_type;
	constexpr bool smtp{::std::same_as<char_type,value_type>};
	if constexpr(smtp&&(::fast_io::details::streamreflect::has_ibuffer_ops<F>))
	{
		char_type *curr{ibuffer_curr(instm)};
		char_type *ed{ibuffer_end(instm)};
		auto bufferremain{ed-curr};
		auto itdf{last-first};
		if(itdf<bufferremain)
#if __has_cpp_attribute(likely)
			[[likely]]
#endif
		{
			non_overlapped_copy_n(curr,static_cast<::std::size_t>(itdf),first);
			ibuffer_set_curr(instm,curr+itdf);
		}
		::fast_io::details::read_ibf_all_underflow_impl(instm,first,last);
	}
	else if constexpr(smtp&&(::fast_io::details::streamreflect::has_read_all_define<F>))
	{
		read_all_define(instm,first,last);
	}
	else if constexpr(smtp&&(::fast_io::details::streamreflect::has_scatter_read_all_define<F>))
	{
		basic_io_scatter_t<value_type> scatter
		{
			first,
			static_cast<::std::size_t>(last-first)
		};
		scatter_read_all_define(instm,__builtin_addressof(scatter),1);
	}
	else if constexpr(smtp&&(::fast_io::details::streamreflect::has_read_some_define<F>))
	{
		while((first=read_some_define(instm,first,last))!=last);
	}
	else if constexpr(smtp&&(::fast_io::details::streamreflect::has_scatter_read_some_define<F>))
	{
		do
		{
			basic_io_scatter_t<value_type> scatter
			{
				first,
				static_cast<::std::size_t>(last-first)
			};
			auto ret{scatter_read_some_define(instm,__builtin_addressof(scatter),1)};
			first+=ret.total_size;
		}
		while(first!=last);
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_all_bytes_define<F>)
	{
		::std::byte *firstbyteptr{reinterpret_cast<::std::byte*>(first)};
		::std::byte *lastbyteptr{reinterpret_cast<::std::byte*>(last)};
		read_all_bytes_define(instm,firstbyteptr,lastbyteptr);
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_some_bytes_define<F>)
	{
		::std::byte *firstbyteptr{reinterpret_cast<::std::byte*>(first)};
		::std::byte *lastbyteptr{reinterpret_cast<::std::byte*>(last)};
		read_all_bytes_common_chtypeptr_impl(instm,firstbyteptr,lastbyteptr);
	}
}

template<typename outputtype,::std::forward_iterator Iter>
inline constexpr void read_all_common_iter_impl(outputtype instm,Iter first,Iter last)
{
	using char_type = typename outputtype::char_type;
	using value_type = ::std::iter_value_t<Iter>;
	if constexpr(::std::contiguous_iterator<Iter>)
	{
		::fast_io::details::read_all_common_chtypeptr_impl(
			instm,::std::to_address(first),::std::to_address(last));
	}
	else
	{
		for(;first!=last;++first)
		{
			auto fptr{__builtin_addressof(*first)};
			auto fptrp1{fptr+1};
			::fast_io::details::read_all_common_chtypeptr_impl(
				instm,fptr,fptrp1);
		}
		return first;
	}
}

template<typename F>
inline constexpr io_scatter_status_t scatter_read_some_bytes_impl(F instm,io_scatter_t *base,::std::size_t len)
{
	using char_type = typename F::char_type;
	if constexpr(::fast_io::details::streamreflect::has_scatter_read_some_bytes_define<F>)
	{
		return scatter_read_some_bytes_define(instm,base,len);
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_read_all_bytes_define<F>)
	{
		scatter_read_all_bytes_define(instm,base,len);
		return {scatter_total_size(base,len),len,0};
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_some_bytes_define<F>)
	{
		::std::size_t readedn{};
		auto i{base};
		::std::size_t posinsp{};
		for(auto ed{base+len};i!=ed;++i)
		{
			auto ele{*i};
			auto basebg{reinterpret_cast<::std::byte*>(const_cast<void*>(ele.base))};
			auto baseed{basebg+ele.len};
			auto ret{read_some_bytes_define(instm,
			basebg,
			baseed)};
			::std::size_t diff{static_cast<::std::size_t>(ret-basebg)};
			readedn+=diff;
			if(ret!=baseed)
			{
				posinsp=diff;
				break;
			}
		}
		return {readedn,static_cast<::std::size_t>(i-base),posinsp};
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_all_bytes_define<F>)
	{
		::std::size_t readedn{};
		auto i{base};
		for(auto ed{base+len};i!=ed;++i)
		{
			auto ele{*i};
			auto basebg{reinterpret_cast<::std::byte*>(const_cast<void*>(ele.base))};
			auto baseed{basebg+ele.len};
			read_all_bytes_define(instm,basebg,baseed);
			::std::size_t diff{static_cast<::std::size_t>(baseed-basebg)};
			readedn+=diff;
		}
		return {readedn,len,0};
	}
}

template<typename F>
inline constexpr void scatter_read_all_bytes_impl(F instm,io_scatter_t const *base,::std::size_t len)
{
	using char_type = typename F::char_type;
	if constexpr(::fast_io::details::streamreflect::has_scatter_read_all_bytes_define<F>)
	{
		scatter_read_all_bytes_define(instm,base,len);
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_read_some_bytes_define<F>)
	{
		while(len)
		{
			auto ret{scatter_read_some_bytes_define(instm,base,len)};
			::std::size_t position{ret.position};
			::std::size_t position_in_scatter{ret.position_in_scatter};
			if(position==len)
#if __has_cpp_attribute(likely)
			[[likely]]
#endif
			{
				return;
			}
			base+=position;
			len-=position;
			if(!position_in_scatter)
			{
				continue;
			}
			auto b{reinterpret_cast<::std::byte*>(const_cast<void*>(base->base))};
			::fast_io::details::read_all_common_chtypeptr_impl(instm,
				b+position_in_scatter,b+base->len);
			++base;
			--len;
		}
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_some_bytes_define<F>
		||::fast_io::details::streamreflect::has_read_all_bytes_define<F>)
	{
		for(auto i{base},ed{base+len};i!=ed;++i)
		{
			auto ele{*i};
			auto basebg{reinterpret_cast<::std::byte*>(const_cast<void*>(ele.base))};
			auto baseed{basebg+ele.len};
			::fast_io::details::read_all_common_chtypeptr_impl(instm,basebg,baseed);
		}
	}
}

template<typename F>
inline constexpr io_scatter_status_t scatter_read_some_impl(F instm,basic_io_scatter_t<typename F::char_type> const *base,::std::size_t len)
{
	using char_type = typename F::char_type;
	if constexpr(::fast_io::details::streamreflect::has_scatter_read_some_define<F>)
	{
		return scatter_read_some_define(instm,base,len);
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_read_all_define<F>)
	{
		scatter_read_all_define(instm,base,len);
		return {scatter_total_size(base,len),len,0};
	}
	else if constexpr(sizeof(char_type)==sizeof(::std::byte))
	{
		using scatterbyteptr_constaliasptr
#if __has_cpp_attribute(__gnu__::__may_alias__)

#endif
		= io_scatter_t *;
		return ::fast_io::details::scatter_read_some_bytes_impl(instm,
			reinterpret_cast<scatterbyteptr_constaliasptr>(base),len);
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_some_define<F>)
	{
		::std::size_t readedn{};
		auto i{base};
		::std::size_t posinsp{};
		for(auto ed{base+len};i!=ed;++i)
		{
			auto ele{*i};
			auto basebg{ele.base};
			auto baseed{basebg+ele.len};
			auto ret{read_some_define(instm,basebg,baseed)};
			::std::size_t diff{static_cast<::std::size_t>(ret-basebg)};
			readedn+=diff;
			if(ret!=baseed)
			{
				posinsp=diff;
				break;
			}
		}
		return {readedn,static_cast<::std::size_t>(i-base),posinsp};
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_all_define<F>)
	{
		::std::size_t readedn{};
		auto i{base};
		for(auto ed{base+len};i!=ed;++i)
		{
			auto ele{*i};
			auto basebg{ele.base};
			auto baseed{basebg+ele.len};
			read_all_define(instm,basebg,baseed);
			::std::size_t diff{static_cast<::std::size_t>(baseed-basebg)};
			readedn+=diff;
		}
		return {readedn,len,0};
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_some_bytes_define<F>)
	{
		::std::size_t readedn{};
		auto i{base};
		::std::size_t posinsp{};
		for(auto ed{base+len};i!=ed;++i)
		{
			auto ele{*i};
			auto basebg{ele.base};
			auto baseed{basebg+ele.len};
			auto ret{::fast_io::details::read_some_common_chtypeptr_impl(instm,basebg,baseed)};
			::std::size_t diff{static_cast<::std::size_t>(ret-basebg)};
			readedn+=diff;
			if(ret!=baseed)
			{
				posinsp=diff;
				break;
			}
		}
		return {readedn,static_cast<::std::size_t>(i-base),posinsp};
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_all_define<F>)
	{
		::std::size_t readedn{};
		auto i{base};
		for(auto ed{base+len};i!=ed;++i)
		{
			auto ele{*i};
			auto basebg{ele.base};
			auto baseed{basebg+ele.len};
			read_all_define(instm,basebg,baseed);
			::std::size_t diff{static_cast<::std::size_t>(baseed-basebg)};
			readedn+=diff;
		}
		return {readedn,len,0};
	}
}

template<typename F>
inline constexpr void scatter_read_all_impl(F instm,basic_io_scatter_t<typename F::char_type>  const *base,::std::size_t len)
{
	using char_type = typename F::char_type;
	if constexpr(::fast_io::details::streamreflect::has_scatter_read_all_define<F>)
	{
		scatter_read_all_define(instm,base,len);
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_read_some_define<F>)
	{
		while(len)
		{
			auto ret{scatter_read_define(instm,base,len)};
			::std::size_t position{ret.position};
			::std::size_t position_in_scatter{ret.position_in_scatter};
			if(position==len)
#if __has_cpp_attribute(likely)
			[[likely]]
#endif
			{
				return;
			}
			base+=position;
			len-=position;
			if(!position_in_scatter)
			{
				continue;
			}
			auto b{base->base};
			::fast_io::details::read_all_common_chtypeptr_impl(instm,
				b+position_in_scatter,b+base->len);
			++base;
			--len;
		}
	}
	else if constexpr(sizeof(char_type)==sizeof(::std::byte))
	{
		using scatterbyteptr_constaliasptr
#if __has_cpp_attribute(__gnu__::__may_alias__)

#endif
		= io_scatter_t *;
		::fast_io::details::scatter_read_all_bytes_impl(instm,
			reinterpret_cast<scatterbyteptr_constaliasptr>(base),
			len);
	}
	else if constexpr(::fast_io::details::streamreflect::has_read_all_define<F>||
		::fast_io::details::streamreflect::has_read_some_define<F>)
	{
		for(auto i{base},e{base+len};i!=e;++i)
		{
			auto [bibs,bilen]{*i};
			::fast_io::details::read_all_common_chtypeptr_impl(instm,
			bibs,bibs+bilen);
		}
	}
}

}

template<typename F,::std::forward_iterator Iter>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr Iter reads_some(F&& foo,Iter first, Iter last)
{
	return ::fast_io::details::read_some_common_iter_impl(io_ref(foo),first,last);
}

template<typename F,::std::forward_iterator Iter>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void reads_all(F&& foo,Iter first,Iter last)
{
	::fast_io::details::read_all_common_iter_impl(io_ref(foo),first,last);
}

template<typename F>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr io_scatter_status_t scatter_read_some_bytes(F&& foo,
	io_scatter_t const* pscatter,::std::size_t len)
{
	return ::fast_io::details::scatter_read_some_bytes_impl(io_ref(foo),pscatter,len);
}

template<typename F>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void scatter_read_all_bytes(F&& foo,
	io_scatter_t const* pscatter,::std::size_t len)
{
	::fast_io::details::scatter_read_all_bytes_impl(io_ref(foo),pscatter,len);
}


template<typename F>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr io_scatter_status_t scatter_read_some(F&& foo,
	io_scatter_t const* pscatter,::std::size_t len)
{
	return ::fast_io::details::scatter_read_some_impl(io_ref(foo),pscatter,len);
}

template<typename F,::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void scatter_read_all(F&& foo,
	basic_io_scatter_t<char_type> const* pscatter,::std::size_t len)
{
	return ::fast_io::details::scatter_read_all_impl(io_ref(foo),pscatter,len);
}

}
