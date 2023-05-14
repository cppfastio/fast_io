#pragma once

namespace fast_io
{

inline constexpr io_scatter_status_t scatter_size_to_status(std::size_t sz,io_scatter_t const *base,::std::size_t len) noexcept
{
	std::size_t total{sz};
	for(std::size_t i{};i!=len;++i)
	{
		::std::size_t blen{base[i].len};
		if(total<blen)[[unlikely]]
			return {sz,i,total};
		total-=blen;
	}
	return {sz,len,0};
}

template<typename T>
inline constexpr ::std::size_t scatter_total_size(basic_io_scatter_t<T> const *base,::std::size_t len) noexcept
{
	std::size_t total{};
	for(auto i{base},e{base+len};i!=e;++i)
	{
		total+=i->len;
	}
	return total;
}

namespace details
{

namespace streamreflect
{

template<typename T>
concept has_obuffer_basic_ops = requires(T outstm,typename T::char_type *ptr)
{
	obuffer_begin(outstm);
	obuffer_curr(outstm);
	obuffer_end(outstm);
	obuffer_set_curr(outstm,ptr);
};

template<typename T>
concept has_obuffer_all_ops = has_obuffer_basic_ops<T>&&(requires(T outstm,typename T::char_type const *cptr)
{
	obuffer_write_all_overflow_define(outstm,cptr,cptr);
});

template<typename T>
concept has_obuffer_some_ops = has_obuffer_basic_ops<T>&&(requires(T outstm,typename T::char_type const *cptr)
{
	obuffer_write_some_overflow_define(outstm,cptr,cptr);
});

template<typename T>
concept has_obuffer_ops = has_obuffer_all_ops<T>||has_obuffer_some_ops<T>;

template<typename T>
concept has_write_some_define = requires(T outstm,typename T::char_type const* ptr)
{
	write_some_define(outstm,ptr,ptr);
};

template<typename T>
concept has_write_all_define = requires(T outstm,typename T::char_type const* ptr)
{
	write_all_define(outstm,ptr,ptr);
};

template<typename T>
concept has_write_some_bytes_define = requires(T outstm,::std::byte const* ptr)
{
	write_some_bytes_define(outstm,ptr,ptr);
};

template<typename T>
concept has_write_all_bytes_define = requires(T outstm,::std::byte const* ptr)
{
	write_all_bytes_define(outstm,ptr,ptr);
};

template<typename T>
concept has_scatter_write_some_bytes_define = requires(T outstm,::fast_io::io_scatter_t const *scatter,::std::size_t len)
{
	scatter_write_some_bytes_define(outstm,scatter,len);
};

template<typename T>
concept has_scatter_write_all_bytes_define = requires(T outstm,::fast_io::io_scatter_t const *scatter,::std::size_t len)
{
	scatter_write_all_bytes_define(outstm,scatter,len);
};


template<typename T>
concept has_scatter_write_some_define = requires(T outstm,::fast_io::basic_io_scatter_t<typename T::char_type> const *pscatter,::std::size_t len)
{
	scatter_write_some_define(outstm,pscatter,len);
};

template<typename T>
concept has_scatter_write_all_define = requires(T outstm,::fast_io::basic_io_scatter_t<typename T::char_type> const *pscatter,::std::size_t len)
{
	scatter_write_all_define(outstm,pscatter,len);
};

}

template<typename F>
inline constexpr ::std::byte const* write_some_common_chtypeptr_impl(F outstm,
	::std::byte const* firstbyteptr,::std::byte const* lastbyteptr,::std::size_t n)
{
	auto it{firstbyteptr};
	while((it=write_some_bytes_define(outstm,
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
inline constexpr value_type const* write_obf_some_overflow_impl(F outstm,value_type const* first,value_type const* last)
{
	if constexpr(::fast_io::details::streamreflect::has_obuffer_some_ops<F>)
	{
		return obuffer_write_some_overflow_define(outstm,first,last);
	}
	else
	{
		obuffer_write_all_overflow_define(outstm,first,last);
		return last;
	}
}

template<typename F,typename value_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void write_obf_all_overflow_impl(F outstm,value_type const* first,value_type const* last)
{
	if constexpr(::fast_io::details::streamreflect::has_obuffer_all_ops<F>)
	{
		obuffer_write_all_overflow_define(outstm,first,last);
	}
	else
	{
		while((first=obuffer_write_some_overflow_define(outstm,first,last))!=last);
	}
}

template<typename F,typename value_type>
inline constexpr value_type const* write_some_common_chtypeptr_impl(F outstm,value_type const* first,value_type const* last)
{
	using char_type = typename F::char_type;
	constexpr bool smtp{::std::same_as<char_type,value_type>};
	if constexpr(smtp&&(::fast_io::details::streamreflect::has_obuffer_ops<F>))
	{
		char_type *curr{obuffer_curr(outstm)};
		char_type *ed{obuffer_end(outstm)};
		auto bufferremain{ed-curr};
		auto itdf{last-first};
		if(itdf<bufferremain)
#if __has_cpp_attribute(likely)
			[[likely]]
#endif
		{
			obuffer_set_curr(outstm,non_overlapped_copy_n(first,static_cast<::std::size_t>(itdf),curr));
			return last;
		}
		return ::fast_io::details::write_obf_some_overflow_impl(outstm,first,last);
	}
	else if constexpr(smtp&&(::fast_io::details::streamreflect::has_write_some_define<F>))
	{
		return write_some_define(outstm,first,last);
	}
	else if constexpr(smtp&&(::fast_io::details::streamreflect::has_scatter_write_some_define<F>))
	{
		basic_io_scatter_t<value_type> scatter
		{
			first,
			static_cast<::std::size_t>(last-first)
		};
		auto ret{scatter_write_some_define(outstm,__builtin_addressof(scatter),1)};
		return ret.total_size+first;
	}
	else if constexpr(smtp&&(::fast_io::details::streamreflect::has_write_all_define<F>))
	{
		write_all_define(outstm,first,last);
		return last;
	}
	else if constexpr(smtp&&(::fast_io::details::streamreflect::has_scatter_write_all_define<F>))
	{
		basic_io_scatter_t<value_type> scatter
		{
			first,
			static_cast<::std::size_t>(last-first)
		};
		auto ret{scatter_write_all_define(outstm,__builtin_addressof(scatter),1)};
		return last;
	}
	else if constexpr(::fast_io::details::streamreflect::has_write_some_bytes_define<F>)
	{
		::std::byte const *firstbyteptr{reinterpret_cast<::std::byte const*>(first)};
		::std::byte const *lastbyteptr{reinterpret_cast<::std::byte const*>(last)};
		if constexpr(sizeof(char_type)==sizeof(::std::byte))
		{
			auto p{write_some_bytes_define(outstm,
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
				it=::fast_io::details::write_some_common_chtypeptr_impl(
					outstm,it,lastbyteptr,sizeof(value_type));
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
			[[assume(reinterpret_cast<::std::byte const*>(retv)==it)]];
#endif
			return retv;
		}
	}
	else if constexpr(::fast_io::details::streamreflect::has_write_all_bytes_define<F>)
	{
		::std::byte const *firstbyteptr{reinterpret_cast<::std::byte const*>(first)};
		::std::byte const *lastbyteptr{reinterpret_cast<::std::byte const*>(last)};
		write_all_bytes_define(outstm,firstbyteptr,lastbyteptr);
		return last;
	}
	else
	{
		return first;
	}
}

template<typename outputtype,::std::forward_iterator Iter>
inline constexpr Iter write_some_common_iter_impl(outputtype outstm,Iter first,Iter last)
{
	using char_type = typename outputtype::char_type;
	using value_type = ::std::iter_value_t<Iter>;
	if constexpr(::std::contiguous_iterator<Iter>)
	{
		return ::fast_io::details::write_some_common_chtypeptr_impl(
			outstm,
			::std::to_address(first),
			::std::to_address(last))-::std::to_address(first)+first;
	}
	else
	{
		for(;first!=last;++first)
		{
			auto fptr{__builtin_addressof(*first)};
			auto fptrp1{fptr+1};
			if(::fast_io::details::write_some_common_chtypeptr_impl(
				outstm,fptr,fptrp1)!=fptrp1)
			{
				break;
			}
		}
		return first;
	}
}


template<typename F>
inline constexpr void write_all_bytes_common_chtypeptr_impl(F outstm,::std::byte const* firstbyteptr,::std::byte const* lastbyteptr)
{
	while((firstbyteptr=write_some_bytes_define(outstm,
		firstbyteptr,
		lastbyteptr))!=lastbyteptr);
}

template<typename F,typename value_type>
inline constexpr void write_all_common_chtypeptr_impl(F outstm,value_type const* first,value_type const* last)
{
	using char_type = typename F::char_type;
	constexpr bool smtp{::std::same_as<char_type,value_type>};
	if constexpr(smtp&&(::fast_io::details::streamreflect::has_obuffer_ops<F>))
	{
		char_type *curr{obuffer_curr(outstm)};
		char_type *ed{obuffer_end(outstm)};
		auto bufferremain{ed-curr};
		auto itdf{last-first};
		if(itdf<bufferremain)
#if __has_cpp_attribute(likely)
			[[likely]]
#endif
		{
			obuffer_set_curr(outstm,non_overlapped_copy_n(first,static_cast<::std::size_t>(itdf),curr));
		}
		::fast_io::details::write_obf_all_overflow_impl(outstm,first,last);
	}
	else if constexpr(smtp&&(::fast_io::details::streamreflect::has_write_all_define<F>))
	{
		write_all_define(outstm,first,last);
	}
	else if constexpr(smtp&&(::fast_io::details::streamreflect::has_scatter_write_all_define<F>))
	{
		basic_io_scatter_t<value_type> scatter
		{
			first,
			static_cast<::std::size_t>(last-first)
		};
		scatter_write_all_define(outstm,__builtin_addressof(scatter),1);
	}
	else if constexpr(smtp&&(::fast_io::details::streamreflect::has_write_some_define<F>))
	{
		while((first=write_some_define(outstm,first,last))!=last);
	}
	else if constexpr(smtp&&(::fast_io::details::streamreflect::has_scatter_write_some_define<F>))
	{
		do
		{
			basic_io_scatter_t<value_type> scatter
			{
				first,
				static_cast<::std::size_t>(last-first)
			};
			auto ret{scatter_write_some_define(outstm,__builtin_addressof(scatter),1)};
			first+=ret.total_size;
		}
		while(first!=last);
	}
	else if constexpr(::fast_io::details::streamreflect::has_write_all_bytes_define<F>)
	{
		::std::byte const *firstbyteptr{reinterpret_cast<::std::byte const*>(first)};
		::std::byte const *lastbyteptr{reinterpret_cast<::std::byte const*>(last)};
		write_all_bytes_define(outstm,firstbyteptr,lastbyteptr);
	}
	else if constexpr(::fast_io::details::streamreflect::has_write_some_bytes_define<F>)
	{
		::std::byte const *firstbyteptr{reinterpret_cast<::std::byte const*>(first)};
		::std::byte const *lastbyteptr{reinterpret_cast<::std::byte const*>(last)};
		write_all_bytes_common_chtypeptr_impl(outstm,firstbyteptr,lastbyteptr);
	}
}

template<typename outputtype,::std::forward_iterator Iter>
inline constexpr void write_all_common_iter_impl(outputtype outstm,Iter first,Iter last)
{
	using char_type = typename outputtype::char_type;
	using value_type = ::std::iter_value_t<Iter>;
	if constexpr(::std::contiguous_iterator<Iter>)
	{
		::fast_io::details::write_all_common_chtypeptr_impl(
			outstm,::std::to_address(first),::std::to_address(last));
	}
	else
	{
		for(;first!=last;++first)
		{
			auto fptr{__builtin_addressof(*first)};
			auto fptrp1{fptr+1};
			::fast_io::details::write_all_common_chtypeptr_impl(
				outstm,fptr,fptrp1);
		}
		return first;
	}
}

template<typename F>
inline constexpr io_scatter_status_t scatter_write_some_bytes_impl(F outstm,io_scatter_t const *base,::std::size_t len)
{
	using char_type = typename F::char_type;
	if constexpr(::fast_io::details::streamreflect::has_scatter_write_some_bytes_define<F>)
	{
		return scatter_write_some_bytes_define(outstm,base,len);
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_write_all_bytes_define<F>)
	{
		scatter_write_all_bytes_define(outstm,base,len);
		return {scatter_total_size(base,len),len,0};
	}
	else if constexpr(::fast_io::details::streamreflect::has_write_some_bytes_define<F>)
	{
		::std::size_t written{};
		auto i{base};
		::std::size_t posinsp{};
		for(auto ed{base+len};i!=ed;++i)
		{
			auto ele{*i};
			auto basebg{reinterpret_cast<::std::byte const*>(ele.base)};
			auto baseed{basebg+ele.len};
			auto ret{write_some_bytes_define(outstm,
			reinterpret_cast<::std::byte const*>(basebg),
			reinterpret_cast<::std::byte const*>(baseed))};
			::std::size_t diff{static_cast<::std::size_t>(ret-basebg)};
			written+=diff;
			if(ret!=baseed)
			{
				posinsp=diff;
				break;
			}
		}
		return {written,static_cast<::std::size_t>(i-base),posinsp};
	}
	else if constexpr(::fast_io::details::streamreflect::has_write_all_bytes_define<F>)
	{
		::std::size_t written{};
		auto i{base};
		for(auto ed{base+len};i!=ed;++i)
		{
			auto ele{*i};
			auto basebg{reinterpret_cast<::std::byte const*>(ele.base)};
			auto baseed{basebg+ele.len};
			write_all_bytes_define(outstm,basebg,baseed);
			::std::size_t diff{static_cast<::std::size_t>(baseed-basebg)};
			written+=diff;
		}
		return {written,len,0};
	}
}

template<typename F>
inline constexpr void scatter_write_all_bytes_impl(F outstm,io_scatter_t const *base,::std::size_t len)
{
	using char_type = typename F::char_type;
	if constexpr(::fast_io::details::streamreflect::has_scatter_write_all_bytes_define<F>)
	{
		scatter_write_all_bytes_define(outstm,base,len);
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_write_some_bytes_define<F>)
	{
		while(len)
		{
			auto ret{scatter_write_some_bytes_define(outstm,base,len)};
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
			auto b{reinterpret_cast<::std::byte const*>(base->base)};
			::fast_io::details::write_all_common_chtypeptr_impl(outstm,
				b+position_in_scatter,b+base->len);
			++base;
			--len;
		}
	}
	else if constexpr(::fast_io::details::streamreflect::has_write_some_bytes_define<F>
		||::fast_io::details::streamreflect::has_write_all_bytes_define<F>)
	{
		for(auto i{base},ed{base+len};i!=ed;++i)
		{
			auto ele{*i};
			auto basebg{reinterpret_cast<::std::byte const*>(ele.base)};
			auto baseed{basebg+ele.len};
			::fast_io::details::write_all_common_chtypeptr_impl(outstm,basebg,baseed);
		}
	}
}

template<typename F>
inline constexpr io_scatter_status_t scatter_write_some_impl(F outstm,basic_io_scatter_t<typename F::char_type> const *base,::std::size_t len)
{
	using char_type = typename F::char_type;
	if constexpr(::fast_io::details::streamreflect::has_scatter_write_some_define<F>)
	{
		return scatter_write_some_define(outstm,base,len);
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_write_all_define<F>)
	{
		scatter_write_all_define(outstm,base,len);
		return {scatter_total_size(base,len),len,0};
	}
	else if constexpr(sizeof(char_type)==sizeof(::std::byte))
	{
		using scatterbyteptr_constaliasptr
#if __has_cpp_attribute(__gnu__::__may_alias__)

#endif
		= io_scatter_t const *;
		return ::fast_io::details::scatter_write_some_bytes_impl(outstm,
			reinterpret_cast<scatterbyteptr_constaliasptr>(base),len);
	}
	else if constexpr(::fast_io::details::streamreflect::has_write_some_define<F>)
	{
		::std::size_t written{};
		auto i{base};
		::std::size_t posinsp{};
		for(auto ed{base+len};i!=ed;++i)
		{
			auto ele{*i};
			auto basebg{ele.base};
			auto baseed{basebg+ele.len};
			auto ret{write_some_define(outstm,basebg,baseed)};
			::std::size_t diff{static_cast<::std::size_t>(ret-basebg)};
			written+=diff;
			if(ret!=baseed)
			{
				posinsp=diff;
				break;
			}
		}
		return {written,static_cast<::std::size_t>(i-base),posinsp};
	}
	else if constexpr(::fast_io::details::streamreflect::has_write_all_define<F>)
	{
		::std::size_t written{};
		auto i{base};
		for(auto ed{base+len};i!=ed;++i)
		{
			auto ele{*i};
			auto basebg{ele.base};
			auto baseed{basebg+ele.len};
			write_all_define(outstm,basebg,baseed);
			::std::size_t diff{static_cast<::std::size_t>(baseed-basebg)};
			written+=diff;
		}
		return {written,len,0};
	}
	else if constexpr(::fast_io::details::streamreflect::has_write_some_bytes_define<F>)
	{
		::std::size_t written{};
		auto i{base};
		::std::size_t posinsp{};
		for(auto ed{base+len};i!=ed;++i)
		{
			auto ele{*i};
			auto basebg{ele.base};
			auto baseed{basebg+ele.len};
			auto ret{::fast_io::details::write_some_common_chtypeptr_impl(outstm,basebg,baseed)};
			::std::size_t diff{static_cast<::std::size_t>(ret-basebg)};
			written+=diff;
			if(ret!=baseed)
			{
				posinsp=diff;
				break;
			}
		}
		return {written,static_cast<::std::size_t>(i-base),posinsp};
	}
	else if constexpr(::fast_io::details::streamreflect::has_write_all_define<F>)
	{
		::std::size_t written{};
		auto i{base};
		for(auto ed{base+len};i!=ed;++i)
		{
			auto ele{*i};
			auto basebg{ele.base};
			auto baseed{basebg+ele.len};
			write_all_define(outstm,basebg,baseed);
			::std::size_t diff{static_cast<::std::size_t>(baseed-basebg)};
			written+=diff;
		}
		return {written,len,0};
	}
}

template<typename F>
inline constexpr void scatter_write_all_impl(F outstm,basic_io_scatter_t<typename F::char_type> const *base,::std::size_t len)
{
	using char_type = typename F::char_type;
	if constexpr(::fast_io::details::streamreflect::has_scatter_write_all_define<F>)
	{
		scatter_write_all_define(outstm,base,len);
	}
	else if constexpr(::fast_io::details::streamreflect::has_scatter_write_some_define<F>)
	{
		while(len)
		{
			auto ret{scatter_write_define(outstm,base,len)};
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
			::fast_io::details::write_all_common_chtypeptr_impl(outstm,
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
		= io_scatter_t const *;
		::fast_io::details::scatter_write_all_bytes_impl(outstm,
			reinterpret_cast<scatterbyteptr_constaliasptr>(base),
			len);
	}
	else if constexpr(::fast_io::details::streamreflect::has_write_all_define<F>||
		::fast_io::details::streamreflect::has_write_some_define<F>)
	{
		for(auto i{base},e{base+len};i!=e;++i)
		{
			auto [bibs,bilen]{*i};
			::fast_io::details::write_all_common_chtypeptr_impl(outstm,
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
inline constexpr Iter write_some(F&& foo,Iter first, Iter last)
{
	return ::fast_io::details::write_some_common_iter_impl(io_ref(foo),first,last);
}

template<typename F,::std::forward_iterator Iter>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void write_all(F&& foo,Iter first,Iter last)
{
	::fast_io::details::write_all_common_iter_impl(io_ref(foo),first,last);
}

template<typename F>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr io_scatter_status_t scatter_write_some_bytes(F&& foo,
	io_scatter_t const* pscatter,::std::size_t len)
{
	return ::fast_io::details::scatter_write_some_bytes_impl(io_ref(foo),pscatter,len);
}

template<typename F>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void scatter_write_all_bytes(F&& foo,
	io_scatter_t const* pscatter,::std::size_t len)
{
	::fast_io::details::scatter_write_all_bytes_impl(io_ref(foo),pscatter,len);
}


template<typename F>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr io_scatter_status_t scatter_write_some(F&& foo,
	io_scatter_t const* pscatter,::std::size_t len)
{
	return ::fast_io::details::scatter_write_some_impl(io_ref(foo),pscatter,len);
}

template<typename F,::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void scatter_write_all(F&& foo,
	basic_io_scatter_t<char_type> const* pscatter,::std::size_t len)
{
	return ::fast_io::details::scatter_write_all_impl(io_ref(foo),pscatter,len);
}

}
