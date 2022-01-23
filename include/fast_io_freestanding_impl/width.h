#pragma once

namespace fast_io
{

namespace manipulators
{

#if 0
template<scalar_placement replacement,typename T>
requires (0u<static_cast<std::size_t>(replacement)&&static_cast<std::size_t>(replacement)<5u)
inline constexpr auto width(T&& t,std::size_t n) noexcept
{
	using noref = std::remove_cvref_t<T>;
	if constexpr((manipulator<noref>||std::is_trivially_copyable_v<noref>)&&
#if defined(_MSC_VER) || (defined(_WIN32)&&!defined(__WINE__)) || defined(__CYGWIN__)
		sizeof(noref)<=8u
#else
		sizeof(noref)<=(sizeof(std::uintptr_t)*2)
#endif	
		)

		return width_t<replacement,noref>{t,n};
	else
	{
		return width_t<replacement,noref const&>{t,n};
	}
}

#endif

template<typename T>
inline constexpr auto left(T&& t,std::size_t n) noexcept
{
	using noref = std::remove_cvref_t<T>;
	if constexpr(alias_printable<std::remove_cvref_t<T>>)
	{
		return width_t<scalar_placement::left,decltype(print_alias_define(io_alias,::fast_io::freestanding::forward<T>(t)))>{print_alias_define(io_alias,::fast_io::freestanding::forward<T>(t)),n};
	}
	else if constexpr((manipulator<noref>||std::is_trivially_copyable_v<noref>)&&
#if defined(_MSC_VER) || (defined(_WIN32)&&!defined(__WINE__)) || defined(__CYGWIN__)
		sizeof(noref)<=8u
#else
		sizeof(noref)<=(sizeof(std::uintptr_t)*2)
#endif	
		)

		return width_t<scalar_placement::left,noref>{t,n};
	else
	{
		return width_t<scalar_placement::left,noref const&>{t,n};
	}
}

template<typename T>
inline constexpr auto middle(T&& t,std::size_t n) noexcept
{
	using noref = std::remove_cvref_t<T>;
	if constexpr(alias_printable<std::remove_cvref_t<T>>)
	{
		return width_t<scalar_placement::middle,decltype(print_alias_define(io_alias,::fast_io::freestanding::forward<T>(t)))>{print_alias_define(io_alias,::fast_io::freestanding::forward<T>(t)),n};
	}
	else if constexpr((manipulator<noref>||std::is_trivially_copyable_v<noref>)&&
#if defined(_MSC_VER) || (defined(_WIN32)&&!defined(__WINE__)) || defined(__CYGWIN__)
		sizeof(noref)<=8u
#else
		sizeof(noref)<=(sizeof(std::uintptr_t)*2)
#endif	
		)

		return width_t<scalar_placement::middle,noref>{t,n};
	else
	{
		return width_t<scalar_placement::middle,noref const&>{t,n};
	}
}

template<typename T>
inline constexpr auto right(T&& t,std::size_t n) noexcept
{
	using noref = std::remove_cvref_t<T>;
	if constexpr(alias_printable<std::remove_cvref_t<T>>)
	{
		return width_t<scalar_placement::right,decltype(print_alias_define(io_alias,::fast_io::freestanding::forward<T>(t)))>{print_alias_define(io_alias,::fast_io::freestanding::forward<T>(t)),n};
	}
	else if constexpr((manipulator<noref>||std::is_trivially_copyable_v<noref>)&&
#if defined(_MSC_VER) || (defined(_WIN32)&&!defined(__WINE__)) || defined(__CYGWIN__)
		sizeof(noref)<=8u
#else
		sizeof(noref)<=(sizeof(std::uintptr_t)*2)
#endif	
		)

		return width_t<scalar_placement::right,noref>{t,n};
	else
	{
		return width_t<scalar_placement::right,noref const&>{t,n};
	}
}


template<typename T>
inline constexpr auto internal(T&& t,std::size_t n) noexcept
{
	using noref = std::remove_cvref_t<T>;
	if constexpr(alias_printable<std::remove_cvref_t<T>>)
	{
		return width_t<scalar_placement::internal,decltype(print_alias_define(io_alias,::fast_io::freestanding::forward<T>(t)))>{print_alias_define(io_alias,::fast_io::freestanding::forward<T>(t)),n};
	}
	else if constexpr((manipulator<noref>||std::is_trivially_copyable_v<noref>)&&
#if defined(_MSC_VER) || (defined(_WIN32)&&!defined(__WINE__)) || defined(__CYGWIN__)
		sizeof(noref)<=8u
#else
		sizeof(noref)<=(sizeof(std::uintptr_t)*2)
#endif	
		)

		return width_t<scalar_placement::internal,noref>{t,n};
	else
	{
		return width_t<scalar_placement::internal,noref const&>{t,n};
	}
}


template<typename T,std::integral char_type>
inline constexpr auto left(T&& t,std::size_t n,char_type ch) noexcept
{
	using noref = std::remove_cvref_t<T>;
	if constexpr(alias_printable<std::remove_cvref_t<T>>)
	{
		return width_ch_t<scalar_placement::left,decltype(print_alias_define(io_alias,::fast_io::freestanding::forward<T>(t))),char_type>{print_alias_define(io_alias,::fast_io::freestanding::forward<T>(t)),n,ch};
	}
	else if constexpr((manipulator<noref>||std::is_trivially_copyable_v<noref>)&&
#if defined(_MSC_VER) || (defined(_WIN32)&&!defined(__WINE__)) || defined(__CYGWIN__)
		sizeof(noref)<=8u
#else
		sizeof(noref)<=(sizeof(std::uintptr_t)*2)
#endif	
		)

		return width_ch_t<scalar_placement::left,noref,char_type>{t,n,ch};
	else
	{
		return width_ch_t<scalar_placement::left,noref const&,char_type>{t,n};
	}
}

template<typename T,std::integral char_type>
inline constexpr auto middle(T&& t,std::size_t n,char_type ch) noexcept
{
	using noref = std::remove_cvref_t<T>;
	if constexpr(alias_printable<std::remove_cvref_t<T>>)
	{
		return width_ch_t<scalar_placement::middle,decltype(print_alias_define(io_alias,::fast_io::freestanding::forward<T>(t))),char_type>{print_alias_define(io_alias,::fast_io::freestanding::forward<T>(t)),n,ch};
	}
	else if constexpr((manipulator<noref>||std::is_trivially_copyable_v<noref>)&&
#if defined(_MSC_VER) || (defined(_WIN32)&&!defined(__WINE__)) || defined(__CYGWIN__)
		sizeof(noref)<=8u
#else
		sizeof(noref)<=(sizeof(std::uintptr_t)*2)
#endif	
		)
		return width_ch_t<scalar_placement::middle,noref,char_type>{t,n,ch};
	else
	{
		return width_ch_t<scalar_placement::middle,noref const&,char_type>{t,n,ch};
	}
}

template<typename T,std::integral char_type>
inline constexpr auto right(T&& t,std::size_t n,char_type ch) noexcept
{
	using noref = std::remove_cvref_t<T>;
	if constexpr(alias_printable<std::remove_cvref_t<T>>)
	{
		return width_ch_t<scalar_placement::right,decltype(print_alias_define(io_alias,::fast_io::freestanding::forward<T>(t))),char_type>{print_alias_define(io_alias,::fast_io::freestanding::forward<T>(t)),n,ch};
	}
	else if constexpr((manipulator<noref>||std::is_trivially_copyable_v<noref>)&&
#if defined(_MSC_VER) || (defined(_WIN32)&&!defined(__WINE__)) || defined(__CYGWIN__)
		sizeof(noref)<=8u
#else
		sizeof(noref)<=(sizeof(std::uintptr_t)*2)
#endif	
		)
		return width_ch_t<scalar_placement::right,noref,char_type>{t,n,ch};
	else
	{
		return width_ch_t<scalar_placement::right,noref const&,char_type>{t,n,ch};
	}
}

template<typename T,std::integral char_type>
inline constexpr auto internal(T&& t,std::size_t n,char_type ch) noexcept
{
	using noref = std::remove_cvref_t<T>;
	if constexpr(alias_printable<std::remove_cvref_t<T>>)
	{
		return width_ch_t<scalar_placement::internal,decltype(print_alias_define(io_alias,::fast_io::freestanding::forward<T>(t))),char_type>{print_alias_define(io_alias,::fast_io::freestanding::forward<T>(t)),n,ch};
	}
	else if constexpr((manipulator<noref>||std::is_trivially_copyable_v<noref>)&&
#if defined(_MSC_VER) || (defined(_WIN32)&&!defined(__WINE__)) || defined(__CYGWIN__)
		sizeof(noref)<=8u
#else
		sizeof(noref)<=(sizeof(std::uintptr_t)*2)
#endif	
		)
		return width_ch_t<scalar_placement::internal,noref,char_type>{t,n,ch};
	else
	{
		return width_ch_t<scalar_placement::internal,noref const&,char_type>{t,n,ch};
	}
}

}

namespace details
{

template<std::integral char_type,typename T>
inline constexpr std::size_t print_reserve_size_width_impl(T t,std::size_t wid)
{
	if constexpr(reserve_printable<char_type,std::remove_cvref_t<T>>)
	{
		constexpr std::size_t sz{print_reserve_size(io_reserve_type<char_type,std::remove_cvref_t<T>>)};
		if(wid<sz)
			return sz;
	}
	else if constexpr(dynamic_reserve_printable<char_type,std::remove_cvref_t<T>>)
	{
		std::size_t sz{print_reserve_size(io_reserve_type<char_type,std::remove_cvref_t<T>>,t)};
		if(wid<sz)
			return sz;
	}
	else if constexpr(scatter_printable<char_type,std::remove_cvref_t<T>>)
	{
		auto sz{print_scatter_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,t).len};
		if(wid<sz)
			return sz;
	}
	return wid;
}

template<::fast_io::manipulators::scalar_placement placement,::fast_io::freestanding::random_access_iterator Iter>
inline constexpr Iter handle_common_ch(Iter first,Iter last,std::size_t wd,::fast_io::freestanding::iter_value_t<Iter> fillch)
{
	std::size_t const diff{static_cast<std::size_t>(last-first)};
	if(wd<=diff)
		return last;
	std::size_t const to_fill_chs{wd-diff};
	if constexpr(placement==::fast_io::manipulators::scalar_placement::left)
	{
		my_fill_n(last,to_fill_chs,fillch);
	}
	else if constexpr(placement==::fast_io::manipulators::scalar_placement::middle)
	{
		constexpr std::size_t one{1};
		std::size_t const left_indent{static_cast<std::size_t>(to_fill_chs>>one)};
		std::size_t const right_indent{to_fill_chs-left_indent};
		my_copy(first,last,first+left_indent);
		my_fill_n(first,left_indent,fillch);
		my_fill_n(first+wd-right_indent,right_indent,fillch);
	}
	else
	{
		my_copy(first,last,first+to_fill_chs);
		my_fill_n(first,to_fill_chs,fillch);
	}
	return first+wd;
}

template<::fast_io::freestanding::random_access_iterator Iter>
inline constexpr Iter handle_common_internal_ch(Iter first,Iter last,std::size_t wd,::fast_io::freestanding::iter_value_t<Iter> fillch,std::size_t internal_len)
{
	std::size_t const diff1{static_cast<std::size_t>(last-first)};
	if(wd<=diff1||diff1<internal_len)
		return last;
	first+=internal_len;
	wd-=internal_len;
	std::size_t const diff{static_cast<std::size_t>(last-first)};
	std::size_t const to_fill_chs{wd-diff};
	my_copy(first,last,first+to_fill_chs);
	my_fill_n(first,to_fill_chs,fillch);
	return first+wd;
}

template<::fast_io::manipulators::scalar_placement placement,::fast_io::freestanding::random_access_iterator Iter,typename T>
inline constexpr Iter print_reserve_define_width_ch_impl(Iter iter,T t,std::size_t wdt,::fast_io::freestanding::iter_value_t<Iter> fillch)
{
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
	using value_type = std::remove_cvref_t<T>;
	if constexpr(placement==::fast_io::manipulators::scalar_placement::internal)
	{
		if constexpr(printable_internal_shift<char_type,value_type>)
		{
			if constexpr(scatter_printable<char_type,value_type>)
			{
				auto sc{print_scatter_define(io_reserve_type<char_type,value_type>,t)};
				auto it{copy_scatter(sc,iter)};
				return handle_common_internal_ch(iter,it,wdt,fillch,print_define_internal_shift(io_reserve_type<char_type,value_type>,t));
			}
			else
			{
				Iter it{print_reserve_define(io_reserve_type<char_type,value_type>,iter,t)};
				return handle_common_internal_ch(iter,it,wdt,fillch,print_define_internal_shift(io_reserve_type<char_type,value_type>,t));
			}
		}
		else
		{
			return print_reserve_define_width_ch_impl<::fast_io::manipulators::scalar_placement::right>(iter,t,wdt,fillch);
		}
	}	
	else
	{
		if constexpr(scatter_printable<char_type,value_type>)
		{
			auto sc{print_scatter_define(io_reserve_type<char_type,value_type>,t)};
			auto it{copy_scatter(sc,iter)};
			return handle_common_ch<placement>(iter,it,wdt,fillch);
		}
		else
		{
			Iter it{print_reserve_define(io_reserve_type<char_type,value_type>,iter,t)};
			return handle_common_ch<placement>(iter,it,wdt,fillch);
		}
	}
}


template<::fast_io::manipulators::scalar_placement placement,::fast_io::freestanding::random_access_iterator Iter,typename T>
requires std::is_trivially_copyable_v<T>
inline constexpr Iter print_reserve_define_width_impl(Iter iter,T t,std::size_t wdt)
{
	return print_reserve_define_width_ch_impl<placement>(iter,t,wdt,char_literal_v<u8' ',::fast_io::freestanding::iter_value_t<Iter>>);
}

}

template<std::integral char_type,::fast_io::manipulators::scalar_placement placement,typename T>
requires ((reserve_printable<char_type,std::remove_cvref_t<T>>||dynamic_reserve_printable<char_type,std::remove_cvref_t<T>>
	||scatter_printable<char_type,std::remove_cvref_t<T>>)&&
	(static_cast<std::size_t>(static_cast<std::size_t>(placement)-static_cast<std::size_t>(1u))<static_cast<std::size_t>(4u)))
constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,::fast_io::manipulators::width_t<placement,T>>,
	::fast_io::manipulators::width_t<placement,T> t) noexcept
{
	if constexpr(std::is_reference_v<T>)
		return ::fast_io::details::print_reserve_size_width_impl<char_type>(parameter<T>{t.reference},t.width);
	else
		return ::fast_io::details::print_reserve_size_width_impl<char_type>(t.reference,t.width);
}

template<::fast_io::freestanding::random_access_iterator Iter,::fast_io::manipulators::scalar_placement placement,typename T>
requires ((reserve_printable<::fast_io::freestanding::iter_value_t<Iter>,std::remove_cvref_t<T>>||dynamic_reserve_printable<::fast_io::freestanding::iter_value_t<Iter>,std::remove_cvref_t<T>>
	||scatter_printable<::fast_io::freestanding::iter_value_t<Iter>,std::remove_cvref_t<T>>)&&
	(static_cast<std::size_t>(static_cast<std::size_t>(placement)-static_cast<std::size_t>(1u))<static_cast<std::size_t>(4u)))
constexpr Iter print_reserve_define(io_reserve_type_t<::fast_io::freestanding::iter_value_t<Iter>,::fast_io::manipulators::width_t<placement,T>>,Iter iter,
	::fast_io::manipulators::width_t<placement,T> t) noexcept
{
	if constexpr(std::is_reference_v<T>)
		return ::fast_io::details::print_reserve_define_width_impl<placement>(iter,parameter<T>{t.reference},t.width);
	else
		return ::fast_io::details::print_reserve_define_width_impl<placement>(iter,t.reference,t.width);
}

template<std::integral char_type,::fast_io::manipulators::scalar_placement placement,typename T>
requires ((reserve_printable<char_type,std::remove_cvref_t<T>>||dynamic_reserve_printable<char_type,std::remove_cvref_t<T>>
	||scatter_printable<char_type,std::remove_cvref_t<T>>)&&
	(static_cast<std::size_t>(static_cast<std::size_t>(placement)-static_cast<std::size_t>(1u))<static_cast<std::size_t>(4u)))
constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,::fast_io::manipulators::width_ch_t<placement,T,char_type>>,::fast_io::manipulators::width_ch_t<placement,T,char_type> t) noexcept
{
	if constexpr(std::is_reference_v<T>)
		return ::fast_io::details::print_reserve_size_width_impl<char_type>(parameter<T>{t.reference},t.width);
	else
		return ::fast_io::details::print_reserve_size_width_impl<char_type>(t.reference,t.width);
}

template<::fast_io::freestanding::random_access_iterator Iter,::fast_io::manipulators::scalar_placement placement,typename T>
requires ((reserve_printable<::fast_io::freestanding::iter_value_t<Iter>,std::remove_cvref_t<T>>||dynamic_reserve_printable<::fast_io::freestanding::iter_value_t<Iter>,std::remove_cvref_t<T>>
	||scatter_printable<::fast_io::freestanding::iter_value_t<Iter>,std::remove_cvref_t<T>>)&&
	(static_cast<std::size_t>(static_cast<std::size_t>(placement)-static_cast<std::size_t>(1u))<static_cast<std::size_t>(4u)))
constexpr Iter print_reserve_define(io_reserve_type_t<::fast_io::freestanding::iter_value_t<Iter>,
		::fast_io::manipulators::width_ch_t<placement,T,::fast_io::freestanding::iter_value_t<Iter>>>,
		Iter iter,::fast_io::manipulators::width_ch_t<placement,T,::fast_io::freestanding::iter_value_t<Iter>> t) noexcept
{
	if constexpr(std::is_reference_v<T>)
		return ::fast_io::details::print_reserve_define_width_ch_impl<placement>(iter,parameter<T>{t.reference},t.width,t.ch);
	else
		return ::fast_io::details::print_reserve_define_width_ch_impl<placement>(iter,t.reference,t.width,t.ch);
}

}
