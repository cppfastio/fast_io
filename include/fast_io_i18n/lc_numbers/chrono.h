#pragma once

namespace fast_io
{

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(basic_lc_all<char_type> const* __restrict all,std::chrono::weekday wkd) noexcept
{
	unsigned value(wkd.c_encoding());
	if(7<value)
	{
		constexpr std::size_t unsigned_size{print_reserve_size(io_reserve_type<char_type,unsigned>)};
		return unsigned_size;
	}
	else
	{
		if(value==7)
			value=0;
		return all->time.day[value].len;
	}
}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define(basic_lc_all<char_type> const* __restrict all,char_type* it,std::chrono::weekday wkd) noexcept
{
	unsigned value{wkd.c_encoding()};
	if(7u<value)
	{
		return print_reserve_define(io_reserve_type<char_type,unsigned>,it,value);
	}
	else
	{
		if(value==7u)
			value={};
		return details::non_overlapped_copy_n(all->time.day[value].base,all->time.day[value].len,it);
	}
}

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(basic_lc_all<char_type> const* __restrict all,std::chrono::month m) noexcept
{
	unsigned value(m);
	--value;
	if(value<12u)
		return all->time.mon[value].len;
	else
	{
		constexpr std::size_t unsigned_size{print_reserve_size(io_reserve_type<char_type,unsigned>)};
		return unsigned_size;
	}
}

template<::std::random_access_iterator Iter,std::integral char_type>
inline constexpr Iter print_reserve_define(basic_lc_all<char_type> const* __restrict all,Iter iter,std::chrono::month m) noexcept
{
	unsigned value(m);
	unsigned value1(value);
	--value;
	if(value<12u)
		return details::non_overlapped_copy_n(all->time.mon[value].base,all->time.mon[value].len,iter);
	else
		return print_reserve_define(io_reserve_type<char_type,unsigned>,iter,value1);
}

/*
Referenced from IBM
LC_TIME Category for the Locale Definition Source File Format
https://www.ibm.com/support/knowledgecenter/ssw_aix_71/filesreference/LC_TIME.html
*/

namespace manipulators
{

template<typename T>
struct abbr_t
{
	using manip_tag = manip_tag_t;
	T reference;
};

template<typename T>
struct alt_t
{
	using manip_tag = manip_tag_t;
	T reference;
};

template<typename T>
struct alt_num_t
{
	using manip_tag = manip_tag_t;
	T reference;
};

template<typename T>
struct am_pm_t
{
	using manip_tag = manip_tag_t;
	T reference;
};

inline constexpr am_pm_t<bool> am_pm(bool is_pm) noexcept
{
	return {is_pm};
}

inline constexpr abbr_t<std::chrono::weekday> abbr(std::chrono::weekday wkd) noexcept
{
	return {wkd};
}

inline constexpr abbr_t<std::chrono::month> abbr(std::chrono::month wkd) noexcept
{
	return {wkd};
}

inline constexpr abbr_t<alt_t<std::chrono::month>> abbr_alt(std::chrono::month wkd) noexcept
{
	return {{wkd}};
}

template<typename T>
requires (std::same_as<T,std::chrono::month>||
std::same_as<T,std::chrono::day>||
std::same_as<T,std::chrono::weekday>)
inline constexpr alt_num_t<T> alt_num(T m) noexcept
{
	return {m};
}


template<std::integral char_type>
inline constexpr basic_io_scatter_t<char_type> print_scatter_define(basic_lc_all<char_type> const* __restrict all,am_pm_t<bool> ampm) noexcept
{
	return all->time.am_pm[ampm.reference];
}

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(basic_lc_all<char_type> const* __restrict all,abbr_t<std::chrono::weekday> wkd) noexcept
{
	unsigned value(wkd.reference.c_encoding());
	if(7<value)
	{
		constexpr std::size_t unsigned_size{print_reserve_size(io_reserve_type<char_type,unsigned>)};
		return unsigned_size;
	}
	else
	{
		if(value==7)
			value=0;
		return all->time.abday[value].len;
	}
}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define(basic_lc_all<char_type> const* __restrict all,char_type* it,abbr_t<std::chrono::weekday> wkd) noexcept
{
	unsigned value(wkd.reference.c_encoding());
	if(7u<value)
	{
		return print_reserve_define(io_reserve_type<char_type,unsigned>,it,value);
	}
	else
	{
		if(value==7u)
			value={};
		return details::non_overlapped_copy_n(all->time.abday[value].base,all->time.abday[value].len,it);
	}
}

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(basic_lc_all<char_type> const* __restrict all,abbr_t<std::chrono::month> m) noexcept
{
	unsigned value(m.reference);
	--value;
	if(value<12u)
		return all->time.abmon[value].len;
	else
	{
		constexpr std::size_t unsigned_size{print_reserve_size(io_reserve_type<char_type,unsigned>)};
		return unsigned_size;
	}
}

template<::std::random_access_iterator Iter,std::integral char_type>
inline constexpr Iter print_reserve_define(basic_lc_all<char_type> const* __restrict all,Iter iter,abbr_t<std::chrono::month> m) noexcept
{
	unsigned value(m.reference);
	unsigned value1(value);
	--value;
	if(value<12u)
		return details::non_overlapped_copy_n(all->time.abmon[value].base,all->time.abmon[value].len,iter);
	else
		return print_reserve_define(io_reserve_type<char_type,unsigned>,iter,value1);
}

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(basic_lc_all<char_type> const* __restrict all,abbr_t<alt_t<std::chrono::month>> m) noexcept
{
	unsigned value(m.reference.reference);
	--value;
	if(value<12u)
	{
		if(all->time.ab_alt_mon[value].len==0)
			return all->time.abmon[value].len;
		else
			return all->time.ab_alt_mon[value].len;
	}
	else
	{
		constexpr std::size_t unsigned_size{print_reserve_size(io_reserve_type<char_type,unsigned>)};
		return unsigned_size;
	}
}

template<::std::random_access_iterator Iter,std::integral char_type>
inline constexpr Iter print_reserve_define(basic_lc_all<char_type> const* __restrict all,Iter iter,abbr_t<alt_t<std::chrono::month>> m) noexcept
{
	unsigned value(m.reference.reference);
	unsigned value1(value);
	--value;
	if(value<12u)
	{
		if(all->time.ab_alt_mon[value].len==0)
			return details::non_overlapped_copy_n(all->time.abmon[value].base,all->time.abmon[value].len,iter);
		else
			return details::non_overlapped_copy_n(all->time.ab_alt_mon[value].base,all->time.ab_alt_mon[value].len,iter);
	}
	else
		return print_reserve_define(io_reserve_type<char_type,unsigned>,iter,value1);
}

template<std::integral char_type,typename T>
requires (std::same_as<T,std::chrono::month>||
std::same_as<T,std::chrono::day>||
std::same_as<T,std::chrono::weekday>)
inline constexpr std::size_t print_reserve_size(basic_lc_all<char_type> const* __restrict all,
	alt_num_t<T> m) noexcept
{
	using namespace std::chrono;
	if constexpr(std::same_as<month,T>||std::same_as<day,T>)
	{
		unsigned value(m.reference);
		if(value<all->time.alt_digits.len)
			return all->time.alt_digits.base[value].len;
		else
		{
			constexpr std::size_t unsigned_size{print_reserve_size(io_reserve_type<char_type,T>)};
			return unsigned_size;
		}
	}
	else
	{
		unsigned value(m.reference.iso_encoding());
		if(value<all->time.alt_digits.len)
			return all->time.alt_digits.base[value].len;
		else
		{
			constexpr std::size_t unsigned_size{print_reserve_size(io_reserve_type<char_type,T>)};
			return unsigned_size;
		}
	}
}

template<std::integral char_type,::std::random_access_iterator Iter,typename T>
requires (std::same_as<T,std::chrono::month>||
std::same_as<T,std::chrono::day>||
std::same_as<T,std::chrono::weekday>)
inline constexpr Iter print_reserve_define(basic_lc_all<char_type> const* __restrict all,Iter iter,
	alt_num_t<T> m) noexcept
{
	using namespace std::chrono;
	if constexpr(std::same_as<month,T>||std::same_as<day,T>)
	{
		unsigned value(m.reference);
		if(value<all->time.alt_digits.len)
			return details::non_overlapped_copy_n(all->time.alt_digits.base[value].base,
				all->time.alt_digits.base[value].len,iter);
		else
			return print_reserve_define(io_reserve_type<char_type,T>,iter,m.reference);
	}
	else
	{
		unsigned value(m.reference.iso_encoding());
		if(value<all->time.alt_digits.len)
			return details::non_overlapped_copy_n(all->time.alt_digits.base[value].base,
				all->time.alt_digits.base[value].len,iter);
		else
			return print_reserve_define(io_reserve_type<char_type,T>,iter,m.reference);
	}
}

#if 0
template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(basic_lc_all<char_type> const* __restrict all,std::chrono::year_month_day m) noexcept
{
	constexpr std::size_t unitsize{::fast_io::freestanding::max(print_reserve_size(io_reserve_type<char_type,int>),print_reserve_size(io_reserve_type<char_type,unsigned>))};
	return unitsize*all->time.d_fmt.len;
}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define(basic_lc_all<char_type> const* __restrict all,char_type* iter,std::chrono::year_month_day m) noexcept
{

}
#endif
}
}