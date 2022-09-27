#pragma once

namespace fast_io
{

namespace details
{
template<typename char_type,typename T1,typename T2>
concept cond_ok_lc_scatter_printable_impl=
	lc_scatter_printable<char_type,T1>&&
	(lc_scatter_printable<char_type,T2>||scatter_printable<char_type,T2>);

template<typename char_type,typename T1,typename T2>
concept cond_ok_lc_dynamic_reserve_printable_impl=
	(lc_dynamic_reserve_printable<char_type,T1>||lc_scatter_printable<char_type,T1>)&&
	(lc_dynamic_reserve_printable<char_type,T2>||lc_scatter_printable<char_type,T2>||reserve_printable<char_type,T2>||dynamic_reserve_printable<char_type,T2>||scatter_printable<char_type,T2>);

template<typename char_type,typename T1>
concept cond_ok_lc_printable_impl=cond_ok_printable_impl<char_type,T1>&&lc_printable<char_type,T1>;

template<typename char_type,typename T1>
concept cond_ok_lc_dynamic_rsv_printable_impl=cond_ok_dynamic_rsv_printable_impl<char_type,T1>||
	lc_dynamic_reserve_printable<char_type,T1>||lc_scatter_printable<char_type,T1>;
}

template<std::integral char_type,typename T1,typename T2>
requires (::fast_io::details::cond_ok_lc_scatter_printable_impl<char_type,T1,T2>||
	::fast_io::details::cond_ok_lc_scatter_printable_impl<char_type,T2,T1>)
inline constexpr basic_io_scatter_t<char_type> print_scatter_define(basic_lc_all<char_type> const* __restrict all,::fast_io::manipulators::condition<T1,T2> c)
{
	if(c.pred)
	{
		if constexpr(lc_scatter_printable<char_type,T1>)
		{
			return {print_scatter_define(all,c.t1)};
		}
		else
		{
			return {print_scatter_define(io_reserve_type<char_type,T1>,c.t1)};
		}
	}
	else
	{
		if constexpr(lc_scatter_printable<char_type,T2>)
		{
			return {print_scatter_define(all,c.t2)};
		}
		else
		{
			return {print_scatter_define(io_reserve_type<char_type,T2>,c.t2)};
		}
	}
}

namespace details
{

template<std::integral char_type,typename T1>
inline constexpr ::std::size_t cond_lc_print_reserve_size_impl(basic_lc_all<char_type> const* __restrict all,T1 c)
{
	if constexpr(lc_scatter_printable<char_type,T1>)
	{
		return print_scatter_define(all,c).len;
	}
	else if constexpr(lc_dynamic_reserve_printable<char_type,T1>)
	{
		return print_reserve_size(all,c);
	}
	else if constexpr(scatter_printable<char_type,T1>)
	{
		return print_scatter_define(io_reserve_type<char_type,T1>,c).len;
	}
	else if constexpr(reserve_printable<char_type,T1>)
	{
		constexpr std::size_t sz{print_reserve_size(io_reserve_type<char_type,T1>)};
		return sz;
	}
	else
	{
		return print_reserve_size(io_reserve_type<char_type,T1>,c);	
	}
}

template<std::integral char_type,typename T1>
inline constexpr char_type* cond_lc_print_reserve_define_impl(basic_lc_all<char_type> const* __restrict all,char_type* iter,T1 c)
{
	if constexpr(lc_scatter_printable<char_type,T1>)
	{
		return copy_scatter(print_scatter_define(all,c),iter);
	}
	else if constexpr(lc_dynamic_reserve_printable<char_type,T1>)
	{
		return print_reserve_define(all,iter,c);
	}
	else if constexpr(scatter_printable<char_type,T1>)
	{
		return copy_scatter(print_scatter_define(io_reserve_type<char_type,T1>,c),iter);
	}
	else if constexpr(reserve_printable<char_type,T1>)
	{
		constexpr std::size_t sz{print_reserve_size(io_reserve_type<char_type,T1>)};
		return sz;
	}
	else
	{
		return print_reserve_size(io_reserve_type<char_type,T1>,c);	
	}
}

}

template<std::integral char_type,typename T1,typename T2>
requires (::fast_io::details::cond_ok_lc_dynamic_reserve_printable_impl<char_type,T1,T2>||
	::fast_io::details::cond_ok_lc_dynamic_reserve_printable_impl<char_type,T2,T1>)
inline constexpr ::std::size_t print_reserve_size(basic_lc_all<char_type> const* __restrict all,::fast_io::manipulators::condition<T1,T2> c)
{
	if(c.pred)
	{
		return ::fast_io::details::cond_lc_print_reserve_size_impl<char_type,T1>(all,c.t1);
	}
	else
	{
		return ::fast_io::details::cond_lc_print_reserve_size_impl<char_type,T2>(all,c.t2);
	}
}

template<::std::integral char_type,typename T1,typename T2>
requires (::fast_io::details::cond_ok_lc_dynamic_reserve_printable_impl<char_type,T1,T2>||
	::fast_io::details::cond_ok_lc_dynamic_reserve_printable_impl<char_type,T2,T1>)
inline constexpr char_type* print_reserve_define(basic_lc_all<char_type> const* __restrict all,char_type* iter,::fast_io::manipulators::condition<T1,T2> c)
{
	if(c.pred)
	{
		return ::fast_io::details::cond_lc_print_reserve_define_impl<char_type,T1>(all,iter,c.t1);
	}
	else
	{
		return ::fast_io::details::cond_lc_print_reserve_define_impl<char_type,T2>(all,iter,c.t2);
	}
}

template<::std::integral char_type,typename T1,typename T2,::fast_io::buffer_output_stream bop>
requires ((::fast_io::details::cond_ok_lc_printable_impl<char_type,T1>
	||::fast_io::details::cond_ok_lc_printable_impl<char_type,T2>)
	&&(!(::fast_io::details::cond_ok_lc_dynamic_rsv_printable_impl<char_type,T1>)||
		::fast_io::details::cond_ok_lc_dynamic_rsv_printable_impl<char_type,T2>))
inline constexpr void print_define(basic_lc_all<char_type> const* __restrict all,bop b,::fast_io::manipulators::condition<T1,T2> c)
{
	if(c.pred)
	{
		print_freestanding(imbue(all,b),c.t1);
	}
	else
	{
		print_freestanding(imbue(all,b),c.t2);
	}
}

}
