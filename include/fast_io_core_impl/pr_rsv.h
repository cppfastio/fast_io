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

template<::fast_io::freestanding::random_access_iterator Iter,typename T>
inline constexpr Iter pr_rsv_to_iterator_unchecked(Iter it,T&& t) noexcept
{
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
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
		return pr_rsv_to_iterator_unchecked(buffer,::fast_io::freestanding::forward<T>(t));
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
		return pr_rsv_to_iterator_unchecked(buffer.data(),::fast_io::freestanding::forward<T>(t))-buffer.data()+buffer.begin();
	}
	else
	{
static_assert(error,"array size is not enough");
		return buffer.begin();
	}
}
#endif
}
