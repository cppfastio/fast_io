#pragma once

namespace fast_io
{

namespace details
{
template<std::integral char_type,typename T>
inline constexpr std::size_t pr_rsv_size_impl() noexcept
{
	using nocvref_t = std::remove_cvref_t<T>;
	if constexpr(alias_printable<nocvref_t>)
	{
		using alias_type = decltype(print_alias_define(::fast_io::io_alias,
			*static_cast<nocvref_t const*>(static_cast<void const*>(nullptr))));
		constexpr bool error{reserve_printable<char_type,alias_type>};
		if constexpr(error)
		{
			constexpr std::size_t sz{print_reserve_size(io_reserve_type<char_type,alias_type>)};
			return sz;
		}
		else
		{
static_assert(error,"type is not reserve_printable");
			return 0;
		}
	}
	else
	{
		constexpr bool error{reserve_printable<char_type,nocvref_t>};
		if constexpr(error)
		{
			constexpr std::size_t sz{print_reserve_size(io_reserve_type<char_type,nocvref_t>)};
			return sz;
		}
		else
		{
static_assert(error,"type is not reserve_printable");
			return 0;
		}
	}
}
}

template<std::integral char_type,typename T>
inline constexpr std::size_t pr_rsv_size{::fast_io::details::pr_rsv_size_impl<char_type,T>()};

template<::std::random_access_iterator Iter,typename T>
inline constexpr Iter pr_rsv_to_iterator_unchecked(Iter it,T&& t) noexcept
{
	using char_type = ::std::iter_value_t<Iter>;
	if constexpr(alias_printable<std::remove_cvref_t<T>>)
	{
		using alias_type = decltype(print_alias_define(::fast_io::io_alias,t));
		constexpr bool error{reserve_printable<char_type,alias_type>};
		if constexpr(error)
		{
			return print_reserve_define(io_reserve_type<char_type,alias_type>,it,
				print_alias_define(::fast_io::io_alias,t));
		}
		else
		{
static_assert(error,"type is not reserve_printable");
			return it;
		}
	}
	else
	{
		constexpr bool error{reserve_printable<char_type,std::remove_cvref_t<T>>};
		if constexpr(error)
		{
			return print_reserve_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,it,t);
		}
		else
		{
static_assert(error,"type is not reserve_printable");
			return it;
		}
	}
}
template<std::integral char_type,std::size_t n,typename T>
inline constexpr char_type* pr_rsv_to_c_array(char_type (&buffer)[n],T&& t) noexcept
{
	constexpr bool error{(pr_rsv_size<char_type,std::remove_cvref_t<T>>)<=n};
	if constexpr(error)
	{
		return pr_rsv_to_iterator_unchecked(buffer,::std::forward<T>(t));
	}
	else
	{
static_assert(error,"C array size is not enough");
		return buffer;
	}
}

#if defined(_GLIBCXX_ARRAY) || defined(_GLIBCXX_ARRAY) || defined(_ARRAY_)
template<std::integral char_type,std::size_t n,typename T>
inline constexpr typename ::std::array<char_type,n>::iterator pr_rsv_to_array(::std::array<char_type,n> &buffer,T&& t) noexcept
{
	constexpr bool error{(pr_rsv_size<char_type,std::remove_cvref_t<T>>)<=n};
	if constexpr(error)
	{
		return pr_rsv_to_iterator_unchecked(buffer.data(),::std::forward<T>(t))-buffer.data()+buffer.begin();
	}
	else
	{
static_assert(error,"array size is not enough");
		return buffer.begin();
	}
}
#endif

namespace details
{

template<::std::integral char_type,typename T>
inline constexpr ::fast_io::parse_result<char_type const*> parse_by_scan_impl(char_type const* first,char_type const* last,T t) noexcept
{
	if constexpr(::fast_io::precise_reserve_scannable<char_type,T>)
	{
		constexpr std::size_t n{scan_precise_reserve_size(io_reserve_type<char_type,T>)};
		std::size_t const diff{static_cast<std::size_t>(last-first)};
		if(diff<n)[[unlikely]]
		{
			return {first,::fast_io::parse_code::end_of_file};
		}
		if constexpr(precise_reserve_scannable_no_error<char_type,T>)
		{
			scan_precise_reserve_define(io_reserve_type<char_type,T>,first,t);
			return {first+n,::fast_io::parse_code::ok};
		}
		else
		{
			return scan_precise_reserve_define(io_reserve_type<char_type,T>,first,t);
		}
	}
	else if constexpr(::fast_io::contiguous_scannable<char_type,T>)
	{
		return scan_contiguous_define(io_reserve_type<char_type,T>,first,last,t);
	}
	else if constexpr(::fast_io::context_scannable<char_type,T>)
	{
		typename std::remove_cvref_t<decltype(scan_context_type(io_reserve_type<char_type,T>))>::type state;
		auto [it,ec]=scan_context_define(io_reserve_type<char_type,T>,state,first,last,t);
		if(ec!=parse_code::partial)
		{
			return {it,ec};
		}
		ec=scan_context_eof_define(io_reserve_type<char_type,T>,state,t);
		return {it,ec};
	}
	else
	{
		constexpr bool not_scannable{context_scannable<char_type,T>};
		static_assert(not_scannable,"type not scannable. need context_scannable");
		return false;
	}
}

}

template<::std::integral char_type,typename T>
#if __has_cpp_attribute(nodiscard)
[[nodiscard("NEVER discard return pointer and parse code from parse_by_scan")]]
#endif
inline constexpr ::fast_io::parse_result<char_type const*> parse_by_scan(char_type const* first,char_type const* last,T&& t) noexcept
{
	using mytype = decltype(io_scan_forward<char_type>(io_scan_alias(t)));
	constexpr bool allscannable{::fast_io::precise_reserve_scannable<char_type,mytype>||
		::fast_io::contiguous_scannable<char_type,mytype>||
		::fast_io::context_scannable<char_type,mytype>};
	if constexpr(allscannable)
	{
		return ::fast_io::details::parse_by_scan_impl(first,last,io_scan_forward<char_type>(io_scan_alias(t)));
	}
	else
	{
static_assert(allscannable,"type not scannable. need context_scannable");
	}
}

}
