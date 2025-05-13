#pragma once

namespace fast_io
{

namespace details
{

template <typename T1>
concept cond_value_transferable = ::std::is_trivially_copyable_v<::std::remove_cvref_t<T1>> &&
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
								  sizeof(::std::remove_cvref_t<T1>) <= 8u
#else
								  sizeof(::std::remove_cvref_t<T1>) <= (sizeof(::std::size_t) * 2)
#endif
	;
} // namespace details

namespace manipulators
{

template <typename T1, typename T2>
struct condition
{
	using manip_tag = manip_tag_t;
	using alias_type1 = T1;
	using alias_type2 = T2;
	alias_type1 t1;
	alias_type2 t2;
	bool pred;
};

template <typename T1, typename T2>
inline constexpr auto cond(bool pred, T1 &&t1, T2 &&t2) noexcept
{
	using t1aliastype =
		::std::conditional_t<::fast_io::details::alias_return_lvalue_ref<T1>,
							 ::std::conditional_t<::fast_io::details::cond_value_transferable<T1>,
												  ::std::remove_cvref_t<T1>, ::std::remove_cvref_t<T1> const &>,
							 ::std::remove_cvref_t<decltype(fast_io::io_print_alias(::std::forward<T1>(t1)))>>;
	using t2aliastype =
		::std::conditional_t<::fast_io::details::alias_return_lvalue_ref<T2>,
							 ::std::conditional_t<::fast_io::details::cond_value_transferable<T2>,
												  ::std::remove_cvref_t<T2>, ::std::remove_cvref_t<T2> const &>,
							 ::std::remove_cvref_t<decltype(fast_io::io_print_alias(::std::forward<T2>(t2)))>>;

	constexpr bool type_match{::std::same_as<t1aliastype, t2aliastype>};
	if constexpr (type_match)
	{
		if (pred)
		{
			return ::fast_io::io_print_alias(::std::forward<T1>(t1));
		}
		else
		{
			return ::fast_io::io_print_alias(::std::forward<T2>(t2));
		}
	}
	else
	{
		if constexpr (sizeof(t1aliastype) < sizeof(t2aliastype))
		{
			return condition<t2aliastype, t1aliastype>{::fast_io::io_print_alias(::std::forward<T2>(t2)),
													   ::fast_io::io_print_alias(::std::forward<T1>(t1)), !pred};
		}
		else
		{
			return condition<t1aliastype, t2aliastype>{::fast_io::io_print_alias(::std::forward<T1>(t1)),
													   ::fast_io::io_print_alias(::std::forward<T2>(t2)), pred};
		}
	}
}

template <typename T1>
inline constexpr auto cond(bool pred, T1 &&t1) noexcept
{
	using t1aliastype =
		::std::conditional_t<::fast_io::details::alias_return_lvalue_ref<T1>,
							 ::std::conditional_t<::fast_io::details::cond_value_transferable<T1>,
												  ::std::remove_cvref_t<T1>, ::std::remove_cvref_t<T1> const &>,
							 ::std::remove_cvref_t<decltype(fast_io::io_print_alias(::std::forward<T1>(t1)))>>;

	constexpr bool type_match{::std::same_as<t1aliastype, ::fast_io::io_null_t>};
	if constexpr (type_match)
	{
		return ::fast_io::io_null;
	}
	else
	{
		return condition<t1aliastype, ::fast_io::io_null_t>{::fast_io::io_print_alias(::std::forward<T1>(t1)),
															::fast_io::io_null, pred};
	}
}

} // namespace manipulators

namespace details
{

template <typename T1, typename T2>
concept cond_transferable_value = ::std::is_trivially_copyable_v<::fast_io::manipulators::condition<T1, T2>> &&
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
								  sizeof(::fast_io::manipulators::condition<T1, T2>) <= 8u
#else
								  sizeof(::fast_io::manipulators::condition<T1, T2>) <= (sizeof(::std::size_t) * 2)
#endif
	;
} // namespace details

template <::std::integral char_type, typename T1, typename T2>
	requires(reserve_printable<char_type, T1> && reserve_printable<char_type, T2>)
inline constexpr ::std::size_t
print_reserve_size(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<T1, T2>>) noexcept
{
	constexpr ::std::size_t s1{print_reserve_size(io_reserve_type<char_type, T1>)};
	constexpr ::std::size_t s2{print_reserve_size(io_reserve_type<char_type, T2>)};
	if constexpr (s1 < s2)
	{
		return s2;
	}
	else
	{
		return s1;
	}
}

template <::std::integral char_type, typename T1, typename T2>
	requires(scatter_printable<char_type, T1> && scatter_printable<char_type, T2> &&
			 details::cond_transferable_value<T1, T2>)
inline constexpr basic_io_scatter_t<char_type>
print_scatter_define(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<T1, T2>>,
					 ::fast_io::manipulators::condition<T1, T2> c)
{
	if (c.pred)
	{
		return {print_scatter_define(io_reserve_type<char_type, T1>, c.t1)};
	}
	else
	{
		return {print_scatter_define(io_reserve_type<char_type, T2>, c.t2)};
	}
}

template <::std::integral char_type, typename T1, typename T2>
	requires(scatter_printable<char_type, T1> && scatter_printable<char_type, T2> &&
			 !details::cond_transferable_value<T1, T2>)
inline constexpr basic_io_scatter_t<char_type>
print_scatter_define(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<T1, T2>>,
					 ::fast_io::manipulators::condition<T1, T2> const &c)
{
	if (c.pred)
	{
		return {print_scatter_define(io_reserve_type<char_type, T1>, c.t1)};
	}
	else
	{
		return {print_scatter_define(io_reserve_type<char_type, T2>, c.t2)};
	}
}

template <::std::integral char_type, typename T1>
	requires(reserve_printable<char_type, T1>)
inline constexpr ::std::size_t
print_reserve_size(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<T1, ::fast_io::io_null_t>>) noexcept
{
	constexpr ::std::size_t s1{print_reserve_size(io_reserve_type<char_type, T1>)};
	return s1;
}

template <::std::integral char_type, typename T1>
	requires(scatter_printable<char_type, T1> && details::cond_value_transferable<T1>)
inline constexpr basic_io_scatter_t<char_type>
print_scatter_define(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<T1, ::fast_io::io_null_t>>,
					 ::fast_io::manipulators::condition<T1, ::fast_io::io_null_t> c)
{
	if (c.pred)
	{
		return {print_scatter_define(io_reserve_type<char_type, T1>, c.t1)};
	}
	else
	{
		return basic_io_scatter_t<char_type>{};
	}
}

template <::std::integral char_type, typename T1>
	requires(scatter_printable<char_type, T1> && !details::cond_value_transferable<T1>)
inline constexpr basic_io_scatter_t<char_type>
print_scatter_define(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<T1, ::fast_io::io_null_t>>,
					 ::fast_io::manipulators::condition<T1, ::fast_io::io_null_t> const &c)
{
	if (c.pred)
	{
		return {print_scatter_define(io_reserve_type<char_type, T1>, c.t1)};
	}
	else
	{
		return basic_io_scatter_t<char_type>{};
	}
}

template <::std::integral char_type, typename T2>
	requires(reserve_printable<char_type, T2>)
inline constexpr ::std::size_t
print_reserve_size(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<::fast_io::io_null_t, T2>>) noexcept
{
	constexpr ::std::size_t s2{print_reserve_size(io_reserve_type<char_type, T2>)};
	return s2;
}

template <::std::integral char_type, typename T2>
	requires(scatter_printable<char_type, T2> && details::cond_value_transferable<T2>)
inline constexpr basic_io_scatter_t<char_type>
print_scatter_define(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<::fast_io::io_null_t, T2>>,
					 ::fast_io::manipulators::condition<::fast_io::io_null_t, T2> c)
{
	if (!c.pred)
	{
		return {print_scatter_define(io_reserve_type<char_type, T2>, c.t2)};
	}
	else
	{
		return basic_io_scatter_t<char_type>{};
	}
}

template <::std::integral char_type, typename T2>
	requires(scatter_printable<char_type, T2> && !details::cond_value_transferable<T2>)
inline constexpr basic_io_scatter_t<char_type>
print_scatter_define(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<::fast_io::io_null_t, T2>>,
					 ::fast_io::manipulators::condition<::fast_io::io_null_t, T2> const &c)
{
	if (!c.pred)
	{
		return {print_scatter_define(io_reserve_type<char_type, T2>, c.t2)};
	}
	else
	{
		return basic_io_scatter_t<char_type>{};
	}
}

namespace details
{

template <typename char_type, typename T1>
concept cond_ok_dynamic_rsv_printable_impl =
	reserve_printable<char_type, T1> || dynamic_reserve_printable<char_type, T1> || scatter_printable<char_type, T1>;

template <typename char_type, typename T1>
concept cond_ok_printable_impl = cond_ok_dynamic_rsv_printable_impl<char_type, T1> || printable<char_type, T1>;

template <::std::integral char_type, typename T1>
	requires(cond_value_transferable<T1>)
inline constexpr ::std::size_t cond_print_reserve_size_impl(T1 t1)
{
	if constexpr (scatter_printable<char_type, T1>)
	{
		return print_scatter_define(io_reserve_type<char_type, T1>, t1).len;
	}
	else if constexpr (reserve_printable<char_type, T1>)
	{
		constexpr ::std::size_t sz{print_reserve_size(io_reserve_type<char_type, T1>)};
		return sz;
	}
	else
	{
		return print_reserve_size(io_reserve_type<char_type, T1>, t1);
	}
}

template <::std::integral char_type, typename T1>
	requires(!cond_value_transferable<T1>)
inline constexpr ::std::size_t cond_print_reserve_size_impl(T1 const &t1)
{
	if constexpr (scatter_printable<char_type, T1>)
	{
		return print_scatter_define(io_reserve_type<char_type, T1>, t1).len;
	}
	else if constexpr (reserve_printable<char_type, T1>)
	{
		constexpr ::std::size_t sz{print_reserve_size(io_reserve_type<char_type, T1>)};
		return sz;
	}
	else
	{
		return print_reserve_size(io_reserve_type<char_type, T1>, t1);
	}
}

template <::std::integral char_type, typename T1>
	requires(cond_value_transferable<T1>)
inline constexpr char_type *cond_print_reserve_define_impl(char_type *iter, T1 t1)
{
	if constexpr (scatter_printable<char_type, T1>)
	{
		return copy_scatter(print_scatter_define(io_reserve_type<char_type, T1>, t1), iter);
	}
	else
	{
		return print_reserve_define(io_reserve_type<char_type, T1>, iter, t1);
	}
}

template <::std::integral char_type, typename T1>
	requires(!cond_value_transferable<T1>)
inline constexpr char_type *cond_print_reserve_define_impl(char_type *iter, T1 const &t1)
{
	if constexpr (scatter_printable<char_type, T1>)
	{
		return copy_scatter(print_scatter_define(io_reserve_type<char_type, T1>, t1), iter);
	}
	else
	{
		return print_reserve_define(io_reserve_type<char_type, T1>, iter, t1);
	}
}

} // namespace details

template <::std::integral char_type, typename T1, typename T2>
	requires((details::cond_ok_dynamic_rsv_printable_impl<char_type, T1> &&
			  details::cond_ok_dynamic_rsv_printable_impl<char_type, T2>) &&
			 (!(scatter_printable<char_type, T1> && scatter_printable<char_type, T2>)) &&
			 details::cond_transferable_value<T1, T2>)
inline constexpr ::std::size_t
print_reserve_size(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<T1, T2>>,
				   ::fast_io::manipulators::condition<T1, T2> c)
{
	if (c.pred)
	{
		return ::fast_io::details::cond_print_reserve_size_impl<char_type, T1>(c.t1);
	}
	else
	{
		return ::fast_io::details::cond_print_reserve_size_impl<char_type, T2>(c.t2);
	}
}

template <::std::integral char_type, typename T1, typename T2>
	requires((details::cond_ok_dynamic_rsv_printable_impl<char_type, T1> &&
			  details::cond_ok_dynamic_rsv_printable_impl<char_type, T2>) &&
			 (!(scatter_printable<char_type, T1> && scatter_printable<char_type, T2>)) &&
			 !details::cond_transferable_value<T1, T2>)
inline constexpr ::std::size_t
print_reserve_size(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<T1, T2>>,
				   ::fast_io::manipulators::condition<T1, T2> const &c)
{
	if (c.pred)
	{
		return ::fast_io::details::cond_print_reserve_size_impl<char_type, T1>(c.t1);
	}
	else
	{
		return ::fast_io::details::cond_print_reserve_size_impl<char_type, T2>(c.t2);
	}
}

template <::std::integral char_type, typename T1, typename T2>
	requires((details::cond_ok_dynamic_rsv_printable_impl<char_type, T1> &&
			  details::cond_ok_dynamic_rsv_printable_impl<char_type, T2>) &&
			 (!(scatter_printable<char_type, T1> && scatter_printable<char_type, T2>)) &&
			 details::cond_transferable_value<T1, T2>)
inline constexpr char_type *
print_reserve_define(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<T1, T2>>, char_type *iter,
					 ::fast_io::manipulators::condition<T1, T2> c)
{
	if (c.pred)
	{
		return ::fast_io::details::cond_print_reserve_define_impl<char_type, T1>(iter, c.t1);
	}
	else
	{
		return ::fast_io::details::cond_print_reserve_define_impl<char_type, T2>(iter, c.t2);
	}
}

template <::std::integral char_type, typename T1, typename T2>
	requires((details::cond_ok_dynamic_rsv_printable_impl<char_type, T1> &&
			  details::cond_ok_dynamic_rsv_printable_impl<char_type, T2>) &&
			 (!(scatter_printable<char_type, T1> && scatter_printable<char_type, T2>)) &&
			 !details::cond_transferable_value<T1, T2>)
inline constexpr char_type *
print_reserve_define(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<T1, T2>>, char_type *iter,
					 ::fast_io::manipulators::condition<T1, T2> const &c)
{
	if (c.pred)
	{
		return ::fast_io::details::cond_print_reserve_define_impl<char_type, T1>(iter, c.t1);
	}
	else
	{
		return ::fast_io::details::cond_print_reserve_define_impl<char_type, T2>(iter, c.t2);
	}
}

template <::std::integral char_type, typename T1>
	requires(details::cond_ok_dynamic_rsv_printable_impl<char_type, T1> && !scatter_printable<char_type, T1> &&
			 details::cond_value_transferable<T1>)
inline constexpr ::std::size_t
print_reserve_size(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<T1, ::fast_io::io_null_t>>,
				   ::fast_io::manipulators::condition<T1, ::fast_io::io_null_t> c)
{
	if (c.pred)
	{
		return ::fast_io::details::cond_print_reserve_size_impl<char_type, T1>(c.t1);
	}
	else
	{
		return 0;
	}
}

template <::std::integral char_type, typename T1>
	requires(details::cond_ok_dynamic_rsv_printable_impl<char_type, T1> && !scatter_printable<char_type, T1> &&
			 !details::cond_value_transferable<T1>)
inline constexpr ::std::size_t
print_reserve_size(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<T1, ::fast_io::io_null_t>>,
				   ::fast_io::manipulators::condition<T1, ::fast_io::io_null_t> const &c)
{
	if (c.pred)
	{
		return ::fast_io::details::cond_print_reserve_size_impl<char_type, T1>(c.t1);
	}
	else
	{
		return 0;
	}
}

template <::std::integral char_type, typename T1>
	requires(details::cond_ok_dynamic_rsv_printable_impl<char_type, T1> && !scatter_printable<char_type, T1> &&
			 details::cond_value_transferable<T1>)
inline constexpr char_type *
print_reserve_define(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<T1, ::fast_io::io_null_t>>, char_type *iter,
					 ::fast_io::manipulators::condition<T1, ::fast_io::io_null_t> c)
{
	if (c.pred)
	{
		return ::fast_io::details::cond_print_reserve_define_impl<char_type, T1>(iter, c.t1);
	}
	else
	{
		return iter;
	}
}

template <::std::integral char_type, typename T1>
	requires(details::cond_ok_dynamic_rsv_printable_impl<char_type, T1> && !scatter_printable<char_type, T1> &&
			 !details::cond_value_transferable<T1>)
inline constexpr char_type *
print_reserve_define(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<T1, ::fast_io::io_null_t>>, char_type *iter,
					 ::fast_io::manipulators::condition<T1, ::fast_io::io_null_t> const &c)
{
	if (c.pred)
	{
		return ::fast_io::details::cond_print_reserve_define_impl<char_type, T1>(iter, c.t1);
	}
	else
	{
		return iter;
	}
}

template <::std::integral char_type, typename T2>
	requires(details::cond_ok_dynamic_rsv_printable_impl<char_type, T2> && !scatter_printable<char_type, T2> &&
			 details::cond_value_transferable<T2>)
inline constexpr ::std::size_t
print_reserve_size(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<::fast_io::io_null_t, T2>>,
				   ::fast_io::manipulators::condition<::fast_io::io_null_t, T2> c)
{
	if (!c.pred)
	{
		return ::fast_io::details::cond_print_reserve_size_impl<char_type, T2>(c.t2);
	}
	else
	{
		return 0;
	}
}

template <::std::integral char_type, typename T2>
	requires(details::cond_ok_dynamic_rsv_printable_impl<char_type, T2> && !scatter_printable<char_type, T2> &&
			 !details::cond_value_transferable<T2>)
inline constexpr ::std::size_t
print_reserve_size(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<::fast_io::io_null_t, T2>>,
				   ::fast_io::manipulators::condition<::fast_io::io_null_t, T2> const &c)
{
	if (!c.pred)
	{
		return ::fast_io::details::cond_print_reserve_size_impl<char_type, T2>(c.t2);
	}
	else
	{
		return 0;
	}
}

template <::std::integral char_type, typename T2>
	requires(details::cond_ok_dynamic_rsv_printable_impl<char_type, T2> && !scatter_printable<char_type, T2> &&
			 details::cond_value_transferable<T2>)
inline constexpr char_type *
print_reserve_define(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<::fast_io::io_null_t, T2>>, char_type *iter,
					 ::fast_io::manipulators::condition<::fast_io::io_null_t, T2> c)
{
	if (!c.pred)
	{
		return ::fast_io::details::cond_print_reserve_define_impl<char_type, T2>(iter, c.t2);
	}
	else
	{
		return iter;
	}
}

template <::std::integral char_type, typename T2>
	requires(details::cond_ok_dynamic_rsv_printable_impl<char_type, T2> && !scatter_printable<char_type, T2> &&
			 !details::cond_value_transferable<T2>)
inline constexpr char_type *
print_reserve_define(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<::fast_io::io_null_t, T2>>, char_type *iter,
					 ::fast_io::manipulators::condition<::fast_io::io_null_t, T2> const &c)
{
	if (!c.pred)
	{
		return ::fast_io::details::cond_print_reserve_define_impl<char_type, T2>(iter, c.t2);
	}
	else
	{
		return iter;
	}
}

template <::std::integral char_type, typename T1, typename T2, typename bop>
	requires((details::cond_ok_printable_impl<char_type, T1> && details::cond_ok_printable_impl<char_type, T2>) &&
			 (!(details::cond_ok_dynamic_rsv_printable_impl<char_type, T1> &&
				details::cond_ok_dynamic_rsv_printable_impl<char_type, T2>)) &&
			 details::cond_transferable_value<T1, T2>)
inline constexpr void print_define(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<T1, T2>>, bop b,
								   ::fast_io::manipulators::condition<T1, T2> c)
{
	if (c.pred)
	{
		::fast_io::operations::print_freestanding<false>(b, c.t1);
	}
	else
	{
		::fast_io::operations::print_freestanding<false>(b, c.t2);
	}
}

template <::std::integral char_type, typename T1, typename T2, typename bop>
	requires((details::cond_ok_printable_impl<char_type, T1> && details::cond_ok_printable_impl<char_type, T2>) &&
			 (!(details::cond_ok_dynamic_rsv_printable_impl<char_type, T1> &&
				details::cond_ok_dynamic_rsv_printable_impl<char_type, T2>)) &&
			 !details::cond_transferable_value<T1, T2>)
inline constexpr void print_define(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<T1, T2>>, bop b,
								   ::fast_io::manipulators::condition<T1, T2> const &c)
{
	if (c.pred)
	{
		::fast_io::operations::print_freestanding<false>(b, c.t1);
	}
	else
	{
		::fast_io::operations::print_freestanding<false>(b, c.t2);
	}
}

template <::std::integral char_type, typename T1, typename bop>
	requires(details::cond_ok_printable_impl<char_type, T1> && !details::cond_ok_dynamic_rsv_printable_impl<char_type, T1> &&
			 details::cond_value_transferable<T1>)
inline constexpr void print_define(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<T1, ::fast_io::io_null_t>>, bop b,
								   ::fast_io::manipulators::condition<T1, ::fast_io::io_null_t> c)
{
	if (c.pred)
	{
		::fast_io::operations::print_freestanding<false>(b, c.t1);
	}
}

template <::std::integral char_type, typename T1, typename bop>
	requires(details::cond_ok_printable_impl<char_type, T1> && !details::cond_ok_dynamic_rsv_printable_impl<char_type, T1> &&
			 !details::cond_value_transferable<T1>)
inline constexpr void print_define(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<T1, ::fast_io::io_null_t>>, bop b,
								   ::fast_io::manipulators::condition<T1, ::fast_io::io_null_t> const &c)
{
	if (c.pred)
	{
		::fast_io::operations::print_freestanding<false>(b, c.t1);
	}
}

template <::std::integral char_type, typename T2, typename bop>
	requires(details::cond_ok_printable_impl<char_type, T2> && !details::cond_ok_dynamic_rsv_printable_impl<char_type, T2> &&
			 details::cond_value_transferable<T2>)
inline constexpr void print_define(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<::fast_io::io_null_t, T2>>, bop b,
								   ::fast_io::manipulators::condition<::fast_io::io_null_t, T2> c)
{
	if (!c.pred)
	{
		::fast_io::operations::print_freestanding<false>(b, c.t2);
	}
}

template <::std::integral char_type, typename T2, typename bop>
	requires(details::cond_ok_printable_impl<char_type, T2> && !details::cond_ok_dynamic_rsv_printable_impl<char_type, T2> &&
			 !details::cond_value_transferable<T2>)
inline constexpr void print_define(io_reserve_type_t<char_type, ::fast_io::manipulators::condition<::fast_io::io_null_t, T2>>, bop b,
								   ::fast_io::manipulators::condition<::fast_io::io_null_t, T2> const &c)
{
	if (!c.pred)
	{
		::fast_io::operations::print_freestanding<false>(b, c.t2);
	}
}
} // namespace fast_io
