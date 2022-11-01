#pragma once

namespace fast_io
{

template<std::int_least64_t off_to_epoch>
struct basic_timestamp
{
	static inline constexpr std::int_least64_t seconds_offset_to_epoch{off_to_epoch};
	std::int_least64_t seconds{};
	std::uint_least64_t subseconds{};
	template<std::int_least64_t new_off_to_epoch>
	explicit constexpr operator basic_timestamp<new_off_to_epoch>() noexcept requires(off_to_epoch!=new_off_to_epoch)
	{
		constexpr std::int_least64_t diff{off_to_epoch-new_off_to_epoch};
		return {seconds+diff,subseconds};
	}
	
	template<std::floating_point flt_type>
	explicit constexpr operator flt_type() noexcept
	{
		//I know this is not accurate. but it is better than nothing
		constexpr flt_type precision{static_cast<flt_type>(uint_least64_subseconds_per_second)};
		return static_cast<flt_type>(seconds)+static_cast<flt_type>(subseconds)/precision;
	}
};

template<std::int_least64_t off_to_epoch>
inline constexpr bool operator==(basic_timestamp<off_to_epoch> a,basic_timestamp<off_to_epoch> b) noexcept
{
	return (a.seconds==b.seconds)&(a.subseconds==b.subseconds);
}

template<std::int_least64_t off_to_epoch>
inline constexpr auto operator<=>(basic_timestamp<off_to_epoch> a,basic_timestamp<off_to_epoch> b) noexcept
{
	auto v{a.seconds<=>b.seconds};
	if(v==0)
	{
		if(a.seconds<0)
			return b.subseconds<=>a.subseconds;
		return a.subseconds<=>b.subseconds;
	}
	return v;
}

namespace details
{
struct timestamp_u
{
	std::uint_least64_t seconds;
	std::uint_least64_t subseconds;
};

inline constexpr timestamp_u add_impl(timestamp_u a,timestamp_u b) noexcept
{
	constexpr std::uint_least64_t max_digit{std::numeric_limits<std::uint_least64_t>::max()/uint_least64_subseconds_per_second};
	std::uint_least64_t res;
	bool cf;
	if constexpr(max_digit==1)
	{
		cf=intrinsics::add_carry(false,a.subseconds,b.subseconds,res);
		if(uint_least64_subseconds_per_second<=res)
		{
			res-=uint_least64_subseconds_per_second;
			cf=true;
		}
	}
	else
	{
		if((cf=(uint_least64_subseconds_per_second<=res)))
			res-=uint_least64_subseconds_per_second;
	}
	std::uint_least64_t seconds;
	intrinsics::add_carry(cf,a.seconds,b.seconds,seconds);
	return {seconds,res};
}

inline constexpr timestamp_u sub_impl(timestamp_u a,timestamp_u b) noexcept
{
	std::uint_least64_t res;
	std::uint_least64_t seconds;
	bool borrow{intrinsics::sub_borrow(false,a.subseconds,b.subseconds,res)};
	intrinsics::sub_borrow(borrow,a.seconds,b.seconds,seconds);
	if(borrow)
		res+=uint_least64_subseconds_per_second;
	return {seconds,res};
}

inline constexpr basic_timestamp<0> div_uint(std::int_least64_t rseconds,std::uint_least64_t subseconds,std::uint_least64_t d) noexcept
{
	if(d==0)[[unlikely]]
		fast_terminate();
	bool minus{rseconds<0};
	std::uint_least64_t seconds{static_cast<std::uint_least64_t>(rseconds)};
	constexpr std::uint_least64_t zero{};
	if(minus)
		seconds=zero-seconds;
#ifdef __SIZEOF_INT128__
	__uint128_t total_subseconds{static_cast<__uint128_t>(seconds)*uint_least64_subseconds_per_second+subseconds};
	std::uint_least64_t mid{d>>1};
	__uint128_t rr{total_subseconds%d};
	std::uint_least64_t r{static_cast<std::uint_least64_t>(rr)};
	__uint128_t q{total_subseconds/d};
	if(mid<r)
		++q;
	else if(mid==r)
	{
		if((q&1)==1)
			++q;
	}
	std::uint_least64_t result_seconds{static_cast<std::uint_least64_t>(q/uint_least64_subseconds_per_second)};
	std::uint_least64_t result_subseconds{static_cast<std::uint_least64_t>(q%uint_least64_subseconds_per_second)};
	if(minus)
		result_seconds=zero-result_seconds;
	return {static_cast<std::int_least64_t>(result_seconds),result_subseconds};
#else
	constexpr std::uint_least64_t one{1};
	std::uint_least64_t total_seconds_high;
	std::uint_least64_t total_seconds_low{intrinsics::umul(seconds,uint_least64_subseconds_per_second,total_seconds_high)};
	intrinsics::add_carry(intrinsics::add_carry(false,total_seconds_low,subseconds,total_seconds_low),total_seconds_high,zero,total_seconds_high);
	std::uint_least64_t mid{d>>1};
	auto [q_low,q_high,r,r_high]=intrinsics::udivmod(total_seconds_low,total_seconds_high,d,zero);
	if(mid<r||(mid==r&&(q_low&1)==1))
		intrinsics::add_carry(intrinsics::add_carry(false,q_low,one,q_low),q_high,zero,q_high);
	auto [result_seconds_low,result_seconds_high,result_subseconds_low,result_subseconds_high]
		=intrinsics::udivmod(q_low,q_high,uint_least64_subseconds_per_second,zero);
	if(minus)
		result_seconds_low=zero-result_seconds_low;
	return {static_cast<std::int_least64_t>(result_seconds_low),result_subseconds_low};
#endif	
}

}

template<std::int_least64_t off_to_epoch>
inline constexpr basic_timestamp<off_to_epoch> operator-(basic_timestamp<off_to_epoch> a) noexcept
{
	std::uint_least64_t high{static_cast<std::uint_least64_t>(a.seconds)};
	high=0u-high;
	return {static_cast<std::int_least64_t>(high),a.subseconds};
}

template<std::int_least64_t off_to_epoch>
inline constexpr basic_timestamp<off_to_epoch> operator+(basic_timestamp<off_to_epoch> a,basic_timestamp<off_to_epoch> b) noexcept
{
	if(a.seconds<0)
	{
		std::uint_least64_t a_abs{static_cast<std::uint_least64_t>(a.seconds)};
		a_abs=0u-a_abs;
		if(b.seconds<0)
		{
			std::uint_least64_t b_abs{static_cast<std::uint_least64_t>(b.seconds)};
			b_abs=0u-b_abs;
			auto res{details::add_impl({a_abs,a.subseconds},
				{b_abs,b.subseconds})};
			res.seconds=0u-res.seconds;
			return {static_cast<std::int_least64_t>(res.seconds),res.subseconds};
		}
		else
		{
			std::uint_least64_t b_abs{static_cast<std::uint_least64_t>(b.seconds)};
			if(a_abs<b_abs||(a_abs==b_abs&&a.subseconds<b.subseconds))
			{
				auto res{details::sub_impl({b_abs,b.subseconds},{a_abs,a.subseconds})};
				return {static_cast<std::int_least64_t>(res.seconds),res.subseconds};
			}
			else
			{
				auto res{details::sub_impl({a_abs,a.subseconds},{b_abs,b.subseconds})};
				res.seconds=0u-res.seconds;
				return {static_cast<std::int_least64_t>(res.seconds),res.subseconds};
			}
		}
	}
	else
	{
		std::uint_least64_t a_abs{static_cast<std::uint_least64_t>(a.seconds)};
		if(b.seconds<0)
		{
			std::uint_least64_t b_abs{static_cast<std::uint_least64_t>(b.seconds)};
			b_abs=0u-b_abs;
			if(a_abs<b_abs||(a_abs==b_abs&&a.subseconds<b.subseconds))
			{
				auto res{details::sub_impl({b_abs,b.subseconds},{a_abs,a.subseconds})};
				res.seconds=0u-res.seconds;
				return {static_cast<std::int_least64_t>(res.seconds),res.subseconds};
			}
			else
			{
				auto res{details::sub_impl({a_abs,a.subseconds},{b_abs,b.subseconds})};
				return {static_cast<std::int_least64_t>(res.seconds),res.subseconds};
			}
		}
		else
		{
			std::uint_least64_t b_abs{static_cast<std::uint_least64_t>(b.seconds)};
			auto res{details::add_impl({a_abs,a.subseconds},
				{b_abs,b.subseconds})};
			return {static_cast<std::int_least64_t>(res.seconds),res.subseconds};
		}
	}
}
template<std::int_least64_t off_to_epoch>
inline constexpr basic_timestamp<off_to_epoch>& operator+=(basic_timestamp<off_to_epoch>& a,basic_timestamp<off_to_epoch> b) noexcept
{
	return a=a+b;
}

template<std::int_least64_t off_to_epoch>
inline constexpr basic_timestamp<off_to_epoch> operator-(basic_timestamp<off_to_epoch> a,basic_timestamp<off_to_epoch> b) noexcept
{
	return a+(-b);
}

template<std::int_least64_t off_to_epoch>
inline constexpr basic_timestamp<off_to_epoch>& operator-=(basic_timestamp<off_to_epoch>& a,basic_timestamp<off_to_epoch> b) noexcept
{
	return a=a+(-b);
}
template<std::int_least64_t off_to_epoch>
inline constexpr basic_timestamp<off_to_epoch> operator/(basic_timestamp<off_to_epoch> a,std::uint_least64_t b) noexcept
{
	if constexpr(off_to_epoch==0)
	{
		return details::div_uint(a.seconds,a.subseconds,b);
	}
	else
	{
		auto [seconds,subseconds]=details::div_uint(a.seconds,a.subseconds,b);
		return {seconds,subseconds};
	}
}

template<std::int_least64_t off_to_epoch>
inline constexpr basic_timestamp<off_to_epoch>& operator/=(basic_timestamp<off_to_epoch>& a,std::uint_least64_t b) noexcept
{
	if constexpr(off_to_epoch==0)
	{
		return a=details::div_uint(a.seconds,a.subseconds,b);
	}
	else
	{
		auto [seconds,subseconds]=details::div_uint(a.seconds,a.subseconds,b);
		a.seconds=seconds;
		a.subseconds=subseconds;
		return a;
	}
}
/*
https://www.epochconverter.com/seconds-days-since-y0
Seconds since year 0 (MySQL compatible)
Seconds since January 1, 1 AD
Days since year 0 (MySQL compatible)
Days since January 1, 1 AD
Days since January 1, 1900, used for Excel DAY functions
Days since January 1, 1904, formerly used for Excel DAY (Macintosh)
Days since January 1, 1970, Unix epoch
*/

using unix_timestamp=basic_timestamp<0>;		//UNIX
using win32_timestamp=basic_timestamp<-11644473600LL>;	//Windows
using csharp_timestamp=basic_timestamp<-62135712000LL>;	//C#
using year0_timestamp=basic_timestamp<-62167219200LL>;	//0000-01-01
using universe_timestamp=basic_timestamp<-434602341429235200LL>;		//Pesudo timestamp since the big bang of universe
/*
Referenced from: https://81018.com/universeclock/
*/

struct iso8601_timestamp
{
	std::int_least64_t year{};
	std::uint_least8_t month{};
	std::uint_least8_t day{};
	std::uint_least8_t hours{};
	std::uint_least8_t minutes{};
	std::uint_least8_t seconds{};
	std::uint_least64_t subseconds{};
	std::int_least32_t timezone{};
};

template<std::integral char_type,std::int_least64_t off_to_epoch>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,basic_timestamp<off_to_epoch>>) noexcept
{
	return print_reserve_size(io_reserve_type<char_type,std::int_least64_t>)+std::numeric_limits<std::uint_least64_t>::digits10;
}

namespace details
{
/*
Referenced from musl libc
https://git.musl-libc.org/cgit/musl/tree/src/time/__secs_to_tm.c
*/

inline constexpr char8_t days_in_month[]{31,30,31,30,31,31,30,31,30,31,31,29};

inline constexpr std::uint_least32_t secs_through_month[]{
0, 31*86400, 59*86400, 90*86400,
120*86400, 151*86400, 181*86400, 212*86400,
243*86400, 273*86400, 304*86400, 334*86400 
};
/*
y2k : 2000-01-01T00:00:00Z
*/
inline constexpr std::int_least64_t y2k{946684800LL};
/*
leapoch: 2000-03-01T00:00:00Z
*/
inline constexpr std::int_least64_t leapoch{y2k + 86400LL*(31LL+29LL)};

inline constexpr std::uint_least32_t days_per_400_year{365LL*400LL+97LL};
inline constexpr std::uint_least32_t days_per_100_year{365LL*100LL+24LL};
inline constexpr std::uint_least32_t days_per_4_year{365LL*4LL+1LL};

template<std::signed_integral T>
inline constexpr T sub_overflow(T a,T b) noexcept
{
#if defined(__has_builtin)
#if __has_builtin(__builtin_sub_overflow)
	T c;
	if(__builtin_sub_overflow(a,b,__builtin_addressof(c)))[[unlikely]]
		fast_terminate();
	return c;
#else

	if(b<=0)[[unlikely]]
	{
		if(a>std::numeric_limits<T>::max()+b)[[unlikely]]
			fast_terminate();

	}
	else
	{
		if(a<std::numeric_limits<T>::min()+b)[[unlikely]]
			fast_terminate();
	}
	return a-b;
#endif
#else
	if(b<=0)[[unlikely]]
	{
		if(a>std::numeric_limits<T>::max()+b)[[unlikely]]
			fast_terminate();
	}
	else
	{
		if(a<std::numeric_limits<T>::min()+b)[[unlikely]]
			fast_terminate();
	}
	return a-b;
#endif
}

#if __has_cpp_attribute(__gnu__::__pure__)
[[__gnu__::__pure__]]
#endif
inline constexpr iso8601_timestamp unix_timestamp_to_iso8601_tsp_impl_internal(std::int_least64_t seconds,std::uint_least64_t subseconds,std::int_least32_t timezone) noexcept
{
	std::int_least64_t secs{sub_overflow(seconds,leapoch)};
	std::int_least64_t days{secs/86400};
	std::int_least64_t remsecs{secs%86400};
	if(remsecs<0)
	{
		remsecs+=86400;
		--days;
	}

	std::int_least64_t qc_cycles{days / days_per_400_year};
	std::int_least64_t remdays{days % days_per_400_year};
	if (remdays < 0)
	{
		remdays += days_per_400_year;
		--qc_cycles;
	}
	std::int_least64_t c_cycles{remdays / days_per_100_year};
	if (c_cycles == 4)
		--c_cycles;
	remdays -= c_cycles * days_per_100_year;

	std::int_least64_t q_cycles{remdays / days_per_4_year};
	if (q_cycles == 25)
		--q_cycles;
	remdays -= q_cycles * days_per_4_year;

	std::int_least64_t remyears{remdays / 365};
	if (remyears == 4)
		--remyears;
	remdays -= remyears * 365;
	std::int_least64_t years{remyears + 4*q_cycles + 100*c_cycles + 400*qc_cycles};
	std::uint_least8_t months{};
	for (; days_in_month[months] <= remdays; ++months)
		remdays -= days_in_month[months];
	if(months>=10)
	{
		++years;
		months-=12;
	}
	return {years+2000,
		static_cast<std::uint_least8_t>(months+3),
		static_cast<std::uint_least8_t>(remdays+1),
		static_cast<std::uint_least8_t>(remsecs/3600),
		static_cast<std::uint_least8_t>(remsecs/60%60),
		static_cast<std::uint_least8_t>(remsecs%60),subseconds,timezone};
}

#if __has_cpp_attribute(__gnu__::__pure__)
[[__gnu__::__pure__]]
#endif
inline constexpr iso8601_timestamp unix_timestamp_to_iso8601_tsp_impl(std::int_least64_t t,std::uint_least64_t subseconds) noexcept
{
	return unix_timestamp_to_iso8601_tsp_impl_internal(t,subseconds,0);
}

#if __has_cpp_attribute(__gnu__::__pure__)
[[__gnu__::__pure__]]
#endif
inline constexpr std::int_least64_t year_month_to_seconds(std::int_least64_t year,std::uint_least8_t month) noexcept
{
	constexpr std::int_least64_t year_min{std::numeric_limits<std::int_least64_t>::min()/(365LL*86400LL)};
	constexpr std::int_least64_t year_max{std::numeric_limits<std::int_least64_t>::max()/(365LL*86400LL)};
	if(year<=year_min||year>=year_max)
		fast_terminate();
	std::int_least64_t leaps{year/4};
	std::int_least64_t leaps_remainder{year%4};
	std::int_least64_t cycles_quotient{year / 400};
	std::int_least64_t cycles_reminder{year % 400};
	std::int_least64_t cycles100_quotient{year / 100};
	std::int_least64_t cycles100_reminder{year % 100};
	bool year_is_leap_year{(!cycles_reminder)||((!leaps_remainder)&&cycles100_reminder)};
	leaps+=cycles_quotient-cycles100_quotient;
	--month;
	if(11<month)
		fast_terminate();
	std::uint_least32_t t{secs_through_month[month]};
	if((month)|(year>=0&&!year_is_leap_year))
		t+=0x15180;
	return (year*365LL+leaps)*86400LL-62167219200LL+static_cast<std::int_least64_t>(t);
}

#if __has_cpp_attribute(__gnu__::__pure__)
[[__gnu__::__pure__]]
#endif
inline constexpr unix_timestamp iso8601_to_unix_timestamp_impl(iso8601_timestamp const& tsp) noexcept
{
	return {static_cast<std::int_least64_t>(static_cast<std::uint_least32_t>(tsp.day-1)*static_cast<std::uint_least32_t>(86400LL)+
		static_cast<std::uint_least32_t>(tsp.hours)*static_cast<std::uint_least32_t>(3600LL)+
		static_cast<std::uint_least32_t>(tsp.minutes)*static_cast<std::uint_least32_t>(60LL)+
		static_cast<std::uint_least32_t>(tsp.seconds)-static_cast<std::uint_least32_t>(tsp.timezone))+
		year_month_to_seconds(tsp.year,tsp.month)
		,tsp.subseconds};
}

}

template<std::int_least64_t off_to_epoch>
inline constexpr iso8601_timestamp utc(basic_timestamp<off_to_epoch> timestamp) noexcept
{
	if constexpr(std::same_as<basic_timestamp<off_to_epoch>,unix_timestamp>)
	{
		return details::unix_timestamp_to_iso8601_tsp_impl(timestamp.seconds,timestamp.subseconds);
	}
	else
	{
		unix_timestamp tsp{static_cast<unix_timestamp>(timestamp)};
		return details::unix_timestamp_to_iso8601_tsp_impl(tsp.seconds,tsp.subseconds);
	}
}

template<std::int_least64_t off_to_epoch=0>
inline constexpr basic_timestamp<off_to_epoch> to_timestamp(iso8601_timestamp const& timestamp) noexcept
{
	return details::iso8601_to_unix_timestamp_impl(timestamp);
}

namespace details
{
inline constexpr std::uint_least8_t c_weekday_tb[]{0,3,2,5,0,3,5,1,4,6,2,4};

inline constexpr std::uint_least8_t c_weekday_impl(std::int_least64_t year,std::uint_least8_t month_minus1,std::uint_least8_t day) noexcept
{
#if defined(__has_builtin)
#if __has_builtin(__builtin_unreachable)
	if(12u<=month_minus1)
		__builtin_unreachable();
#endif
#endif
	return static_cast<std::uint_least8_t>(static_cast<std::uint_least64_t>(
		static_cast<std::uint_least64_t>(year) + static_cast<std::uint_least64_t>(year/4) -
		static_cast<std::uint_least64_t>(year/100) + static_cast<std::uint_least64_t>(year/400) +
		c_weekday_tb[month_minus1]+day)%7u);
}

inline constexpr std::uint_least8_t weekday_impl(std::int_least64_t year,std::uint_least8_t month_minus1,std::uint_least8_t day) noexcept
{
#if defined(__has_builtin)
#if __has_builtin(__builtin_unreachable)
	if(12u<=month_minus1)
		__builtin_unreachable();
#endif
#endif
	return static_cast<std::uint_least8_t>(static_cast<std::uint_least64_t>(
		static_cast<std::uint_least64_t>(year) + static_cast<std::uint_least64_t>(year/4) -
		static_cast<std::uint_least64_t>(year/100) + static_cast<std::uint_least64_t>(year/400) +
		c_weekday_tb[month_minus1]+day+6u)%7u+1u);
}

}

inline constexpr std::uint_least8_t c_weekday(std::int_least64_t year, std::uint_least8_t month, std::uint_least8_t day) noexcept
{
	--month;
	if (month < 2u)
		year = static_cast<std::int_least64_t>(static_cast<std::uint_least64_t>(year) - 1u);
	else if (11u < month)
		month %= 12u;
	return ::fast_io::details::c_weekday_impl(year, month, day);
}

inline constexpr std::uint_least8_t c_weekday(iso8601_timestamp const& timestamp) noexcept
{
	return ::fast_io::c_weekday(timestamp.year, timestamp.month, timestamp.day);
}

inline constexpr std::uint_least8_t weekday(std::int_least64_t year, std::uint_least8_t month, std::uint_least8_t day) noexcept
{
	--month;
	if (month < 2u)
		year = static_cast<std::int_least64_t>(static_cast<std::uint_least64_t>(year) - 1u);
	else if (11u < month)
		month %= 12u;
	return ::fast_io::details::weekday_impl(year, month, day);
}

inline constexpr std::uint_least8_t weekday(iso8601_timestamp const& timestamp) noexcept
{
	return ::fast_io::weekday(timestamp.year, timestamp.month, timestamp.day);
}

namespace details
{

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size_timezone_impl_v{print_reserve_size(io_reserve_type<char_type,std::int_least32_t>)+static_cast<std::size_t>(4u)};

template<::std::integral char_type>
inline constexpr char_type* print_reserve_timezone_impl(char_type* iter,std::int_least32_t timezone) noexcept
{
	std::uint_least64_t unsigned_tz{static_cast<std::uint_least64_t>(timezone)};
	if(timezone<0)
	{
		*iter=char_literal_v<u8'-',char_type>;
		unsigned_tz=0UL-unsigned_tz;
	}
	else
	{
		*iter=char_literal_v<u8'+',char_type>;
	}
	++iter;
	std::uint_least8_t tz_ss{static_cast<std::uint_least8_t>(unsigned_tz%60)};
	unsigned_tz/=60;
	std::uint_least8_t tz_mm{static_cast<std::uint_least8_t>(unsigned_tz%60)};
	unsigned_tz/=60;
	iter=chrono_two_digits_impl(iter,unsigned_tz);
	*iter=char_literal_v<u8':',char_type>;
	++iter;
	iter=chrono_two_digits_impl<true>(iter,tz_mm);
	if(tz_ss)
	{
		*iter=char_literal_v<u8':',char_type>;
		++iter;
		iter=chrono_two_digits_impl<true>(iter,tz_ss);
	}
	return iter;
}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_iso8601_timestamp_impl(char_type* iter,iso8601_timestamp const& timestamp) noexcept
{
	iter=chrono_year_impl(iter,timestamp.year);
	*iter=char_literal_v<u8'-',char_type>;
	++iter;
	iter=chrono_two_digits_impl<true>(iter,timestamp.month);
	*iter=char_literal_v<u8'-',char_type>;
	++iter;
	iter=chrono_two_digits_impl<true>(iter,timestamp.day);
	*iter=char_literal_v<u8'T',char_type>;
	++iter;
	iter=chrono_two_digits_impl<true>(iter,timestamp.hours);
	*iter=char_literal_v<u8':',char_type>;
	++iter;
	iter=chrono_two_digits_impl<true>(iter,timestamp.minutes);
	*iter=char_literal_v<u8':',char_type>;
	++iter;
	iter=chrono_two_digits_impl<true>(iter,timestamp.seconds);
	if(timestamp.subseconds)
		iter=output_iso8601_subseconds(iter,timestamp.subseconds);
	auto const timezone{timestamp.timezone};
	if(timezone==0)
	{
		*iter=char_literal_v<u8'Z',char_type>;
		++iter;
	}
	else
		iter=print_reserve_timezone_impl(iter,timezone);
	return iter;
}

template<bool comma=false, ::std::integral char_type>
inline constexpr char_type* print_reserve_bsc_timestamp_impl(char_type* iter,unix_timestamp timestamp) noexcept
{
	iter=print_reserve_define(io_reserve_type<char_type,std::int_least64_t>,iter,timestamp.seconds);
	if(timestamp.subseconds)
		iter=output_iso8601_subseconds<comma>(iter,timestamp.subseconds);
	return iter;
}

}

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,iso8601_timestamp>) noexcept
{
//ISO 8601 timestamp example : 2021-01-03T10:29:56Z
//ISO 8601 timestamp with timestamp : 2021-01-03T10:29:56.999999+99:99
	return print_reserve_size(io_reserve_type<char_type,std::int_least64_t>)+16+print_reserve_size(io_reserve_type<char_type,std::uint_least64_t>)+::fast_io::details::print_reserve_size_timezone_impl_v<char_type>+3+2;
}

template<std::integral char_type,std::int_least64_t off_to_epoch>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,basic_timestamp<off_to_epoch>>,
		char_type* iter,basic_timestamp<off_to_epoch> timestamp) noexcept
{
	if constexpr(off_to_epoch==0)
		return details::print_reserve_bsc_timestamp_impl(iter,timestamp);
	else
		return details::print_reserve_bsc_timestamp_impl(iter,{timestamp.seconds,timestamp.subseconds});
}

template<std::integral char_type>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,iso8601_timestamp>,char_type* iter,iso8601_timestamp const& timestamp) noexcept
{
	return details::print_reserve_iso8601_timestamp_impl(iter,timestamp);
}


inline constexpr win32_timestamp to_win32_timestamp_ftu64(::std::uint_least64_t ftu64) noexcept
{
	::std::uint_least64_t seconds{ftu64/10000000ULL};
	::std::uint_least64_t subseconds{ftu64%10000000ULL};
	constexpr ::std::uint_least64_t mul_factor{uint_least64_subseconds_per_second/10000000u};
	return {static_cast<::std::int_least64_t>(seconds),static_cast<::std::uint_least64_t>(subseconds*mul_factor)};
}
#if 0
namespace details
{

template <::std::integral char_type, bool comma>
inline constexpr parse_result<char_type const*> scn_cnt_define_iso8601_impl(
	char_type const* begin, char_type const* end,
	iso8601_timestamp& t) noexcept
{
	using ::fast_io::char_literal_v;
	begin = ::fast_io::find_none_c_space(begin, end);
	if (end - begin < 20) [[unlikely]]
		return { begin, parse_code::invalid };
	[[maybe_unused]] bool sign{};
	if (*begin == char_literal_v<u8'-', char_type>) [[unlikely]]
	{
		sign = true;
		++begin;
	}
	::std::int_least64_t tmp_year{};
	if (char_is_digit(*(begin + 4))) [[unlikely]]
	{
		auto [itr, ec] = scan_int_contiguous_define_impl(begin, end, tmp_year);
		if (ec != parse_code::ok) [[unlikely]]
			return { itr, ec };
		else if (itr < begin + 4)
			return { itr, parse_code::invalid };
		else
			begin = itr;
	}
	else
	{
		tmp_year += static_cast<::std::int_least64_t>(*begin++ - char_literal_v<u8'0', char_type>) * 1000;
		tmp_year += static_cast<::std::int_least64_t>(*begin++ - char_literal_v<u8'0', char_type>) * 100;
		tmp_year += static_cast<::std::int_least64_t>(*begin++ - char_literal_v<u8'0', char_type>) * 10;
		tmp_year += static_cast<::std::int_least64_t>(*begin++ - char_literal_v<u8'0', char_type>);
	}
	t.year = sign ? -tmp_year : tmp_year;
	if (end - begin < 16) [[unlikely]]
		return { begin, parse_code::invalid };
	if (*begin++ != char_literal_v<u8'-', char_type>) [[unlikely]]
		return { begin, parse_code::invalid };
	::std::uint_least8_t tmp{};
	tmp += static_cast<::std::uint_least8_t>(*begin++ - char_literal_v<u8'0', char_type>) * 10;
	tmp += static_cast<::std::uint_least8_t>(*begin++ - char_literal_v<u8'0', char_type>);
	t.month = tmp;
	if (*begin++ != char_literal_v<u8'-', char_type>) [[unlikely]]
		return { begin, parse_code::invalid };
	tmp = static_cast<::std::uint_least8_t>(*begin++ - char_literal_v<u8'0', char_type>) * 10;
	tmp += static_cast<::std::uint_least8_t>(*begin++ - char_literal_v<u8'0', char_type>);
	t.day = tmp;
	if (*begin++ != char_literal_v<u8'T', char_type>) [[unlikely]]
		return { begin, parse_code::invalid };
	tmp = static_cast<::std::uint_least8_t>(*begin++ - char_literal_v<u8'0', char_type>) * 10;
	tmp += static_cast<::std::uint_least8_t>(*begin++ - char_literal_v<u8'0', char_type>);
	t.hours = tmp;
	if (*begin++ != char_literal_v<u8':', char_type>) [[unlikely]]
		return { begin, parse_code::invalid };
	tmp = static_cast<::std::uint_least8_t>(*begin++ - char_literal_v<u8'0', char_type>) * 10;
	tmp += static_cast<::std::uint_least8_t>(*begin++ - char_literal_v<u8'0', char_type>);
	t.minutes = tmp;
	if (*begin++ != char_literal_v<u8':', char_type>) [[unlikely]]
		return { begin, parse_code::invalid };
	tmp = static_cast<::std::uint_least8_t>(*begin++ - char_literal_v<u8'0', char_type>) * 10;
	tmp += static_cast<::std::uint_least8_t>(*begin++ - char_literal_v<u8'0', char_type>);
	t.seconds = tmp;
	++begin;
	if (*begin == char_literal_v<u8'Z', char_type>)
	{
		t.subseconds = 0;
		t.timezone = 0;
		return { begin, parse_code::ok };
	}
	else if (*begin == char_literal_v<u8'+', char_type>)
	{
		t.subseconds = 0;
		sign = false;
	}
	else if (*begin == char_literal_v<u8'-', char_type>)
	{
		t.subseconds = 0;
		sign = true;
	}
	else if ((!comma && *begin == char_literal_v<u8'.', char_type>) || (comma && *begin == char_literal_v<u8',', char_type>))
	{
		// parse subseconds
		std::int_least32_t subseconds;
		auto [itr, ec] = scan_int_contiguous_define_impl(begin, end, subseconds);
		if (begin == itr) [[unlikely]]
			return { itr, parse_code::invalid };
		if (ec != parse_code::ok) [[unlikely]]
			return { itr, ec };
		constexpr std::size_t digitsm1(std::numeric_limits<std::uint_least64_t>::digits10);
		std::size_t zero_cnt{ digitsm1 - (itr - begin) };
		for (std::size_t i{}; i < zero_cnt; ++i)
			subseconds *= 10;
		t.subseconds = subseconds;
		begin = itr + 1;
		if (*begin == char_literal_v<u8'Z', char_type>)
		{
			t.timezone = 0;
			return { begin, parse_code::ok };
		}
		else if (*begin == char_literal_v<u8'+', char_type>)
			sign = false;
		else if (*begin == char_literal_v<u8'-', char_type>)
			sign = true;
		else [[unlikely]]
			return { begin, parse_code::invalid };
	}
	else [[unlikely]]
		return { begin, parse_code::invalid };
	// when control flow reaches here, it's to parse time-zone, format HH:MM
	std::int_least32_t timezone;
	timezone = static_cast<::std::int_least32_t>(*begin++ - char_literal_v<u8'0', char_type>) * 10 * 3600;
	timezone += static_cast<::std::int_least32_t>(*begin++ - char_literal_v<u8'0', char_type>) * 3600;
	if (*begin++ != char_literal_v<u8':', char_type>) [[unlikely]]
		return { begin, parse_code::invalid };
	timezone += static_cast<::std::int_least32_t>(*begin++ - char_literal_v<u8'0', char_type>) * 10;
	timezone += static_cast<::std::int_least32_t>(*begin++ - char_literal_v<u8'0', char_type>);
	if (sign) timezone = -timezone;
	t.timezone = timezone;
	return { begin, parse_code::ok };
}

}

template <::std::integral char_type>
inline constexpr parse_result<char_type const*> scan_contiguous_define(io_reserve_type_t<char_type, iso8601_timestamp>, char_type const* begin, char_type const* end, iso8601_timestamp& t) noexcept
{
	return details::scn_cnt_define_iso8601_impl<char_type, false>(begin, end, t);
}

template <::std::integral char_type, ::std::int_least64_t off_to_epoch>
inline constexpr parse_result<char_type const*> scan_contiguous_define(io_reserve_type_t<char_type, basic_timestamp<off_to_epoch>>, char_type const* begin, char_type const* end, basic_timestamp<off_to_epoch>& t) noexcept
{
	iso8601_timestamp tsp;
	auto result = details::scn_cnt_define_iso8601_impl<char_type, false>(begin, end, tsp);
	if constexpr (off_to_epoch == 0)
	{
		t = details::iso8601_to_unix_timestamp_impl(tsp);
	}
	else
	{
		auto [seconds, subseconds] = details::iso8601_to_unix_timestamp_impl(tsp);
		t.seconds = seconds - off_to_epoch;
		t.subseconds = subseconds;
	}
	return result;
}

template <::std::integral char_type>
inline constexpr io_type_t<int> scan_context_type(io_reserve_type_t<char_type, iso8601_timestamp>) noexcept { return{}; }
template <::std::integral char_type>
inline constexpr parse_result<char_type const*> scan_context_define(io_reserve_type_t<char_type, iso8601_timestamp>, int& state, char_type const* begin, char_type const* end, iso8601_timestamp& t) noexcept { return{}; }
template <::std::integral char_type>
inline constexpr parse_code scan_context_eof_define(io_reserve_type_t<char_type, iso8601_timestamp>, int& state, iso8601_timestamp& t) noexcept { return{}; }

#endif

}
