#pragma once

namespace fast_io
{

namespace details
{

namespace streamreflect
{

template<typename T>
concept has_obuffer_all_ops = requires(T outstm,typename T::char_type *ptr,typename T::char_type const *cptr)
{
	obuffer_begin(outstm);
	obuffer_curr(outstm);
	obuffer_end(outstm);
	obuffer_set_curr(outstm,ptr);
	obuffer_write_all_overflow_define(outstm,cptr,cptr);
};

template<typename T>
concept has_obuffer_some_ops = requires(T outstm,typename T::char_type *ptr,typename T::char_type const *cptr)
{
	obuffer_begin(outstm);
	obuffer_curr(outstm);
	obuffer_end(outstm);
	obuffer_set_curr(outstm,ptr);
	obuffer_write_some_overflow_define(outstm,cptr,cptr);
};

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
concept has_scatter_write_some_bytes_define = requires(T outstm,::fast_io::io_scatter_t scatter)
{
	scatter_write_some_bytes_define(outstm,scatter);
};

template<typename T>
concept has_scatter_write_all_bytes_define = requires(T outstm,::fast_io::io_scatter_t scatter)
{
	scatter_write_all_bytes_define(outstm,scatter);
};


template<typename T>
concept has_scatter_write_some_define = requires(T outstm,::fast_io::basic_io_scatter_t<typename T::char_type> scatter)
{
	scatter_write_some_define(outstm,scatter);
};

template<typename T>
concept has_scatter_write_all_define = requires(T outstm,::fast_io::basic_io_scatter_t<typename T::char_type> scatter)
{
	scatter_write_all_define(outstm,scatter);
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
		auto ret{scatter_write_some_define(outstm,__builtin_addressof(scatter),__builtin_addressof(scatter)+1)};
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
		auto ret{scatter_write_all_define(outstm,__builtin_addressof(scatter),__builtin_addressof(scatter)+1)};
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
			::std::size_t n{static_cast<::std::size_t>(::fast_io::details::write_some_common_chtypeptr_impl(
			outstm,firstbyteptr,lastbyteptr,sizeof(value_type))-firstbyteptr)};
			return first+(n/sizeof(value_type));
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
		scatter_write_all_define(outstm,__builtin_addressof(scatter),__builtin_addressof(scatter)+1);
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
			auto ret{scatter_write_some_define(outstm,__builtin_addressof(scatter),__builtin_addressof(scatter)+1)};
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
			outstm,
			::std::to_address(first),
			::std::to_address(last));
	}
	else
	{
		for(;first!=last;++first)
		{
			auto fptr{__builtin_addressof(*first)};
			auto fptrp1{fptr+1};
			::fast_io::details::write_all_common_chtypeptr_impl(
				outstm,
				fptr,
				fptrp1);
		}
		return first;
	}
}

}

template<typename F,::std::forward_iterator Iter>
inline constexpr Iter write_some(F&& foo,Iter first, Iter last)
{
	return ::fast_io::details::write_some_common_iter_impl(io_ref(foo),first,last);
}

template<typename F,::std::forward_iterator Iter>
inline constexpr void write_all(F&& foo,Iter first,Iter last)
{
	::fast_io::details::write_all_common_iter_impl(io_ref(foo),first,last);
}

}
