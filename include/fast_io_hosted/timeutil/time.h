#pragma once

namespace fast_io
{
#if !defined(_WIN32) && !defined(__AVR__) && !defined(__MSDOS__)
namespace posix
{
extern int libc_clock_getres(clockid_t clk_id, struct timespec *tp) noexcept __asm__("clock_getres");
extern int libc_clock_settime(clockid_t clk_id, struct timespec const* tp) noexcept __asm__("clock_settime");
extern int libc_clock_gettime(clockid_t clk_id, struct timespec* tp) noexcept __asm__("clock_gettime");
}
#endif

namespace details
{
#if !defined(__AVR__)
#if __has_cpp_attribute(__gnu__::__pure__)
[[__gnu__::__pure__]]
#endif
inline constexpr auto posix_clock_id_to_native_value(posix_clock_id pcid)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
	switch(pcid)
	{
	case posix_clock_id::realtime:
	case posix_clock_id::tai:
	return posix_clock_id::tai;
	break;
	case posix_clock_id::monotonic:
	return posix_clock_id::monotonic;
	break;
	default:
		throw_win32_error(0x00000057);
	};
#else
	switch(pcid)
	{
#ifdef CLOCK_REALTIME
	case posix_clock_id::realtime:
	return CLOCK_REALTIME;
	break;
#endif
#ifdef CLOCK_REALTIME_ALARM
	case posix_clock_id::realtime_alarm:
	return CLOCK_REALTIME_ALARM;
	break;
#elif defined(CLOCK_REALTIME)
	case posix_clock_id::realtime_alarm:
	return CLOCK_REALTIME;
	break;
#endif
#ifdef CLOCK_REALTIME_COARSE
	case posix_clock_id::realtime_coarse:
	return CLOCK_REALTIME_COARSE;
	break;
#elif defined(CLOCK_REALTIME)
	case posix_clock_id::realtime_coarse:
	return CLOCK_REALTIME;
	break;
#endif
#ifdef CLOCK_TAI
	case posix_clock_id::tai:
	return CLOCK_TAI;
	break;
#elif defined(CLOCK_REALTIME)
	case posix_clock_id::tai:
	return CLOCK_REALTIME;
	break;
#endif
#ifdef CLOCK_MONOTONIC
	case posix_clock_id::monotonic:
	return CLOCK_MONOTONIC;
	break;
#endif
#ifdef CLOCK_MONOTONIC_COARSE
	case posix_clock_id::monotonic_coarse:
	return CLOCK_MONOTONIC_COARSE;
	break;
#endif
#ifdef CLOCK_MONOTONIC_RAW
	case posix_clock_id::monotonic_raw:
	return CLOCK_MONOTONIC_RAW;
	break;
#elif defined(CLOCK_MONOTONIC)
	case posix_clock_id::monotonic_raw:
	return CLOCK_MONOTONIC;
	break;
#endif
#ifdef CLOCK_MONOTONIC_RAW_APPROX
	case posix_clock_id::monotonic_raw_approx:
	return CLOCK_MONOTONIC_RAW_APPROX;
	break;
#endif
#ifdef CLOCK_BOOTTIME
	case posix_clock_id::boottime:
	return CLOCK_BOOTTIME;
	break;
#else
#ifdef CLOCK_MONOTONIC
	case posix_clock_id::boottime:
	return CLOCK_MONOTONIC;
	break;
#endif
#endif
#ifdef CLOCK_BOOTTIME_ALARM
	case posix_clock_id::boottime_alarm:
	return CLOCK_BOOTTIME_ALARM;
	break;
#endif
#ifdef CLOCK_UPTIME_RAW
	case posix_clock_id::uptime_raw:
	return CLOCK_UPTIME_RAW;
	break;
#endif
#ifdef CLOCK_UPTIME_RAW_APPROX
	case posix_clock_id::uptime_raw_approx:
	return CLOCK_UPTIME_RAW_APPROX;
	break;
#endif
#ifdef CLOCK_PROCESS_CPUTIME_ID
	case posix_clock_id::process_cputime_id:
	return CLOCK_PROCESS_CPUTIME_ID;
	break;
#endif
#ifdef CLOCK_THREAD_CPUTIME_ID
	case posix_clock_id::thread_cputime_id:
	return CLOCK_THREAD_CPUTIME_ID;
	break;
#endif
	default:
		throw_posix_error(EINVAL);
	};
#endif
}

#endif
#ifdef _WIN32
#if __has_cpp_attribute(__gnu__::__pure__)
[[__gnu__::__pure__]]
#endif
inline std::int_least64_t win32_query_performance_frequency()
{
	std::int_least64_t val{};
	if(!::fast_io::win32::QueryPerformanceFrequency(__builtin_addressof(val)))
		throw_win32_error();
	if(val<=0)
		throw_win32_error(0x00000057);
	return val;
}

#if __has_cpp_attribute(__gnu__::__pure__)
[[__gnu__::__pure__]]
#endif
inline unix_timestamp win32_query_performance_frequency_to_unix_timestamp()
{
	return {0,uint_least64_subseconds_per_second/static_cast<std::uint_least64_t>(win32_query_performance_frequency())};
}
#endif
}

inline
#if (defined(_WIN32) && !defined(__CYGWIN__)) || defined(__MSDOS__)
constexpr
#endif
unix_timestamp posix_clock_getres([[maybe_unused]] posix_clock_id pclk_id)
{
#if (defined(_WIN32) && !defined(__CYGWIN__))
	switch(pclk_id)
	{
	case posix_clock_id::realtime:
	case posix_clock_id::realtime_alarm:
	case posix_clock_id::realtime_coarse:
	case posix_clock_id::tai:
	case posix_clock_id::process_cputime_id:
	case posix_clock_id::thread_cputime_id:
	{
		constexpr std::uint_least64_t mul_factor{uint_least64_subseconds_per_second/10000000u};
		constexpr unix_timestamp constexpr_stamp{0,mul_factor};
		return constexpr_stamp;
	}
	case posix_clock_id::monotonic:
	case posix_clock_id::monotonic_coarse:
	case posix_clock_id::monotonic_raw:
	case posix_clock_id::boottime:
		return ::fast_io::details::win32_query_performance_frequency_to_unix_timestamp();
	break;
	default:
		throw_win32_error(0x00000057);
	}
#elif defined(__MSDOS__)
	switch(pclk_id)
	{
	case posix_clock_id::realtime:
	case posix_clock_id::realtime_alarm:
	case posix_clock_id::realtime_coarse:
	case posix_clock_id::tai:
	case posix_clock_id::monotonic:
	case posix_clock_id::monotonic_coarse:
	case posix_clock_id::monotonic_raw:
	case posix_clock_id::boottime:
	{
		constexpr std::uint_least64_t mul_factor{uint_least64_subseconds_per_second/100u};
		return {0,mul_factor};
	}
	case posix_clock_id::process_cputime_id:
	case posix_clock_id::thread_cputime_id:
	{
		constexpr std::uint_least64_t mul_factor{uint_least64_subseconds_per_second/UCLOCKS_PER_SEC};
		return {0,mul_factor};
	}
	default:
		throw_posix_error(EINVAL);
	};
#elif defined(__AVR__)
	return {1,0};
#elif !defined(__NEWLIB__) || defined(_POSIX_TIMERS)
	auto clk{details::posix_clock_id_to_native_value(pclk_id)};
	struct timespec res;
//vdso
	if(::fast_io::posix::libc_clock_getres(clk,__builtin_addressof(res))<0)
		throw_posix_error();
	constexpr std::uint_least64_t mul_factor{uint_least64_subseconds_per_second/1000000000u};
	return {static_cast<std::int_least64_t>(res.tv_sec),static_cast<std::uint_least64_t>(res.tv_nsec)*mul_factor};
#else
	throw_posix_error(EINVAL);
#endif
}
#if (defined(_WIN32) && !defined(__CYGWIN__))
namespace win32::details
{

inline unix_timestamp win32_posix_clock_gettime_tai_impl() noexcept
{
	::fast_io::win32::filetime ftm;
#if (defined(_WIN32_WINNT) && _WIN32_WINNT >= 0x0602) && !defined(_WIN32_WINDOWS)
	::fast_io::win32::GetSystemTimePreciseAsFileTime(__builtin_addressof(ftm));
#else
	::fast_io::win32::GetSystemTimeAsFileTime(__builtin_addressof(ftm));
#endif
	return static_cast<unix_timestamp>(to_win32_timestamp(ftm));
}

inline unix_timestamp win32_posix_clock_gettime_boottime_impl()
{
	std::uint_least64_t ftm{};
	if(!::fast_io::win32::QueryUnbiasedInterruptTime(__builtin_addressof(ftm)))
		throw_win32_error();
	constexpr std::uint_least64_t mul_factor{uint_least64_subseconds_per_second/10000000u};
	std::uint_least64_t seconds{ftm/10000000ULL};
	std::uint_least64_t subseconds{ftm%10000000ULL};
	return {static_cast<std::int_least64_t>(seconds),static_cast<std::uint_least64_t>(subseconds*mul_factor)};
}

template<bool is_thread>
inline unix_timestamp win32_posix_clock_gettime_process_or_thread_time_impl()
{
	::fast_io::win32::filetime creation_time,exit_time,kernel_time,user_time;
	if constexpr(is_thread)
	{
		auto hthread{bit_cast<void*>(std::intptr_t(-2))};
		if(!::fast_io::win32::GetThreadTimes(hthread,__builtin_addressof(creation_time),
		__builtin_addressof(exit_time),
		__builtin_addressof(kernel_time),
		__builtin_addressof(user_time)))
			throw_win32_error();
	}
	else
	{
		auto hprocess{bit_cast<void*>(std::intptr_t(-1))};
		if(!::fast_io::win32::GetProcessTimes(hprocess,__builtin_addressof(creation_time),
		__builtin_addressof(exit_time),
		__builtin_addressof(kernel_time),
		__builtin_addressof(user_time)))
			throw_win32_error();
	}
	std::uint_least64_t ftm{::fast_io::win32::filetime_to_uint_least64_t(kernel_time)+
		::fast_io::win32::filetime_to_uint_least64_t(user_time)};
	std::uint_least64_t seconds{ftm/10000000ULL};
	std::uint_least64_t subseconds{ftm%10000000ULL};
	constexpr std::uint_least64_t mul_factor{uint_least64_subseconds_per_second/10000000u};
	return {static_cast<std::int_least64_t>(seconds),static_cast<std::uint_least64_t>(subseconds*mul_factor)};
}

inline unix_timestamp win32_posix_clock_gettime_boottime_xp_impl()
{
	std::uint_least64_t freq{static_cast<std::uint_least64_t>(::fast_io::details::win32_query_performance_frequency())};
	std::int_least64_t counter{};
	if(!::fast_io::win32::QueryPerformanceCounter(__builtin_addressof(counter)))
		throw_win32_error();
	if(counter<0)
		throw_win32_error(0x00000057);
	std::uint_least64_t ucounter{static_cast<std::uint_least64_t>(counter)};
	std::uint_least64_t val{uint_least64_subseconds_per_second/freq};
	std::uint_least64_t dv{ucounter/freq};
	std::uint_least64_t md{ucounter%freq};
	return unix_timestamp{static_cast<std::int_least64_t>(dv),static_cast<std::uint_least64_t>(md*static_cast<std::uint_least64_t>(val))};
}

}
#endif

#ifdef __MSDOS__
namespace details
{

struct my_dos_date_t
{
	std::uint_least8_t day;
	std::uint_least8_t month;
	std::uint_least16_t year;
	std::uint_least8_t dayofweek;
};

struct my_dos_time_t
{
	std::uint_least8_t hour;     /* 0-23 */
	std::uint_least8_t minute;   /* 0-59 */
	std::uint_least8_t second;   /* 0-59 */
	std::uint_least8_t hsecond;  /* 0-99 */
};

extern void my_dos_getdate(my_dos_date_t*) noexcept __asm__("__dos_getdate");
extern void my_dos_gettime(my_dos_time_t*) noexcept __asm__("__dos_gettime");

extern unsigned int my_dos_settime(my_dos_time_t*) noexcept __asm__("__dos_settime");
extern unsigned int my_dos_setdate(my_dos_date_t*) noexcept __asm__("__dos_setdate");

inline iso8601_timestamp get_dos_iso8601_timestamp()
{
	my_dos_date_t dos_date;
	my_dos_time_t dos_time;
	my_dos_date_t dos_date_temp;
	for(std::size_t i{};i!=100;++i)
	{
		my_dos_getdate(&dos_date);
		my_dos_gettime(&dos_time);
		my_dos_getdate(&dos_date_temp);
		if(dos_date.day==dos_date_temp.day&&
		dos_date.month==dos_date_temp.month&&
		dos_date.year==dos_date_temp.year&&
		dos_date.dayofweek==dos_date_temp.dayofweek)
		{
			constexpr std::uint_least64_t factor{uint_least64_subseconds_per_second/100u};
			return {static_cast<std::int_least64_t>(dos_date.year),dos_date.month,dos_date.day,
				dos_time.hour,dos_time.minute,dos_time.second,
				static_cast<std::uint_least64_t>(dos_time.hsecond)*factor,0};
		}
	}
	throw_posix_error(EINVAL);
}

inline unix_timestamp get_dos_unix_timestamp()
{
	return to_timestamp(get_dos_iso8601_timestamp());
}

inline void set_dos_unix_timestamp(unix_timestamp tsp)
{
	iso8601_timestamp iso8601{utc(tsp)};
	if(iso8601.year>static_cast<std::int_least64_t>(UINT_LEAST16_MAX)||iso8601.year<0)
		throw_posix_error(EINVAL);
	std::uint_least16_t year{static_cast<std::uint_least16_t>(iso8601.year)};
	my_dos_date_t dos_date{static_cast<std::uint_least8_t>(iso8601.day),static_cast<std::uint_least8_t>(iso8601.month),year,0};
	constexpr std::uint_least64_t precision{uint_least64_subseconds_per_second/100ULL};
	my_dos_time_t dos_time{static_cast<std::uint_least8_t>(iso8601.hours),
		static_cast<std::uint_least8_t>(iso8601.minutes),
		static_cast<std::uint_least8_t>(iso8601.seconds),
		static_cast<std::uint_least8_t>(iso8601.subseconds/precision)};
	if(!my_dos_setdate(__builtin_addressof(dos_date)))
		throw_posix_error();
	if(!my_dos_settime(__builtin_addressof(dos_time)))
		throw_posix_error();
	my_dos_date_t dos_date_temp;
	for(std::size_t i{};i!=100;++i)
	{
		my_dos_getdate(__builtin_addressof(dos_date_temp));
		if(dos_date.day==dos_date_temp.day&&
		dos_date.month==dos_date_temp.month&&
		dos_date.year==dos_date_temp.year&&
		dos_date.dayofweek==dos_date_temp.dayofweek)
			return;
		if(!my_dos_setdate(__builtin_addressof(dos_date)))
			throw_posix_error();
		if(!my_dos_settime(__builtin_addressof(dos_time)))
			throw_posix_error();
	}
	throw_posix_error(EINVAL);
}

}
#endif

inline unix_timestamp posix_clock_gettime([[maybe_unused]] posix_clock_id pclk_id)
{
#if (defined(_WIN32) && !defined(__CYGWIN__))
	switch(pclk_id)
	{
	case posix_clock_id::realtime:
	case posix_clock_id::realtime_alarm:
	case posix_clock_id::realtime_coarse:
	case posix_clock_id::tai:
		return win32::details::win32_posix_clock_gettime_tai_impl();
	case posix_clock_id::monotonic:
	case posix_clock_id::monotonic_coarse:
	case posix_clock_id::monotonic_raw:
	case posix_clock_id::boottime:
//		return win32::details::win32_posix_clock_gettime_boottime_impl();
		return win32::details::win32_posix_clock_gettime_boottime_xp_impl();
	case posix_clock_id::process_cputime_id:
		return win32::details::win32_posix_clock_gettime_process_or_thread_time_impl<false>();
	case posix_clock_id::thread_cputime_id:
		return win32::details::win32_posix_clock_gettime_process_or_thread_time_impl<true>();
	default:
		throw_win32_error(0x00000057);
	}
#elif defined(__MSDOS__)
	switch(pclk_id)
	{
	case posix_clock_id::realtime:
	case posix_clock_id::realtime_alarm:
	case posix_clock_id::realtime_coarse:
	case posix_clock_id::tai:
	case posix_clock_id::monotonic:
	case posix_clock_id::monotonic_coarse:
	case posix_clock_id::monotonic_raw:
	case posix_clock_id::boottime:
	{
		return details::get_dos_unix_timestamp();
	}
	case posix_clock_id::process_cputime_id:
	case posix_clock_id::thread_cputime_id:
	{
		std::make_unsigned_t<decltype(uclock())> u(noexcept_call(::uclock));
		std::uint_least64_t seconds(u/UCLOCKS_PER_SEC);
		std::uint_least64_t subseconds(u%UCLOCKS_PER_SEC);
		constexpr std::uint_least64_t mul_factor{uint_least64_subseconds_per_second/UCLOCKS_PER_SEC};
		return {static_cast<std::int_least64_t>(seconds),static_cast<std::uint_least64_t>(subseconds)*mul_factor};
	}
	default:
		throw_posix_error(EINVAL);
	}
#elif defined(__AVR__)
	time_t t{};
	constexpr std::int_least64_t y2k_offset{static_cast<std::int_least64_t>(UNIX_OFFSET)};
	return {static_cast<std::int_least64_t>(static_cast<std::uint_least64_t>(static_cast<std::make_unsigned_t<time_t>>(noexcept_call(::time,__builtin_addressof(t)))))+y2k_offset,0};
#elif defined(_POSIX_TIMERS)
	struct timespec res;
	auto clk{details::posix_clock_id_to_native_value(pclk_id)};
//vdso
	if(::fast_io::posix::libc_clock_gettime(clk,__builtin_addressof(res))<0)
		throw_posix_error();
	constexpr std::uint_least64_t mul_factor{uint_least64_subseconds_per_second/1000000000u};
	return {static_cast<std::int_least64_t>(res.tv_sec),static_cast<std::uint_least64_t>(res.tv_nsec)*mul_factor};
#else
	throw_posix_error(EINVAL);
#endif
}

#if defined(_WIN32) || defined(__CYGWIN__)
template<nt_family family,std::int_least64_t off_to_epoch>
requires (family==nt_family::nt||family==nt_family::zw)
inline basic_timestamp<off_to_epoch> nt_family_clock_settime(posix_clock_id pclk_id,basic_timestamp<off_to_epoch> timestamp)
{
	if constexpr(std::same_as<win32_timestamp,basic_timestamp<off_to_epoch>>)
	{
		switch(pclk_id)
		{
			case posix_clock_id::realtime:
			case posix_clock_id::realtime_alarm:
			case posix_clock_id::realtime_coarse:
			case posix_clock_id::tai:
			{
				constexpr std::uint_least64_t mul_factor{uint_least64_subseconds_per_second/10000000u};
				std::uint_least64_t tms(static_cast<std::uint_least64_t>(timestamp.seconds)*10000000ULL+timestamp.subseconds/mul_factor);
				std::uint_least64_t old_tms{};
				auto ntstatus{win32::nt::nt_set_system_time<family==nt_family::zw>(__builtin_addressof(tms),__builtin_addressof(old_tms))};
				if(ntstatus)
					throw_nt_error(ntstatus);
				return to_win32_timestamp_ftu64(old_tms);
			}
			default:
				throw_nt_error(0xC00000EF);
		};
	}
	else
	{
		return static_cast<basic_timestamp<off_to_epoch>>(nt_family_clock_settime<family>(pclk_id,static_cast<win32_timestamp>(timestamp)));
	}
}
template<std::int_least64_t off_to_epoch>
inline basic_timestamp<off_to_epoch> nt_clock_settime(posix_clock_id pclk_id,basic_timestamp<off_to_epoch> timestamp)
{
	return nt_family_clock_settime<nt_family::nt>(pclk_id,timestamp);
}

template<std::int_least64_t off_to_epoch>
inline basic_timestamp<off_to_epoch> zw_clock_settime(posix_clock_id pclk_id,basic_timestamp<off_to_epoch> timestamp)
{
	return nt_family_clock_settime<nt_family::zw>(pclk_id,timestamp);
}

#endif

namespace details
{
#if !defined(_WIN32) || defined(__CYGWIN__)
template<bool local_tm>
inline struct tm unix_timestamp_to_tm_impl(std::int_least64_t seconds)
{
#if defined(_TIME64_T)
	time64_t val{static_cast<time64_t>(seconds)};
	struct tm t;
	if constexpr(local_tm)
	{
	if(::fast_io::noexcept_call(localtime64_r,__builtin_addressof(val),__builtin_addressof(t))==0)
		throw_posix_error();
	}
	else
	{
	if(::fast_io::noexcept_call(gmtime64_r,__builtin_addressof(val),__builtin_addressof(t))==0)
		throw_posix_error();
	}
	return t;
#else
	time_t val{static_cast<time_t>(seconds)};
	struct tm t;
#if defined(__AVR__)
	if constexpr(local_tm)
		noexcept_call(localtime_r,__builtin_addressof(val),__builtin_addressof(t));
	else
		noexcept_call(gmtime_r,__builtin_addressof(val),__builtin_addressof(t));
#else
	if constexpr(local_tm)
	{
	if(localtime_r(__builtin_addressof(val),__builtin_addressof(t))==0)
		throw_posix_error();
	}
	else
	{
	if(gmtime_r(__builtin_addressof(val),__builtin_addressof(t))==0)
		throw_posix_error();
	}
#endif
	return t;

#endif
}
#endif


inline iso8601_timestamp to_iso8601_local_impl(std::int_least64_t seconds,std::uint_least64_t subseconds,[[maybe_unused]] bool dstadj)
{
#if defined(__MSDOS__) || (defined(__NEWLIB__)&&!defined(__CYGWIN__)) || defined(__AVR__) || defined(_PICOLIBC__) || defined(__serenity__)
	return unix_timestamp_to_iso8601_tsp_impl_internal(seconds,subseconds,0);
#elif (defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__)) || defined(__linux__)
#if (defined(__MINGW32__) && !__has_include(<_mingw_stat64.h>))
	return unix_timestamp_to_iso8601_tsp_impl_internal(seconds,subseconds,0);
#else
	long tm_gmtoff{};
	#if (defined(_MSC_VER) || defined(_UCRT)) && !defined(__BIONIC__)
	{
		auto errn{noexcept_call(_get_timezone,__builtin_addressof(tm_gmtoff))};
		if(errn)
			throw_posix_error(static_cast<int>(errn));
	}
	#elif defined(_WIN32) && !defined(__BIONIC__) && !defined(__WINE__) && !defined(__CYGWIN__)
	tm_gmtoff=_timezone;
	#else
	tm_gmtoff=timezone;
	#endif
	long bias{};
	if(dstadj)
	{
	#if (defined(_MSC_VER) || defined(_UCRT)) && !defined(__BIONIC__)
	{
		auto errn{noexcept_call(_get_dstbias,__builtin_addressof(bias))};
		if(errn)
			throw_posix_error(static_cast<int>(errn));
	}
	#elif defined(_WIN32) && !defined(__BIONIC__) && !defined(__WINE__) && !defined(__CYGWIN__)&& (defined(_M_IX86) || defined(_M_X64) ||defined(__i386__) || defined(__x86_64__))
		bias=_dstbias;
	#else
		bias=daylight?-3600L:0;
	#endif
	}
	std::uint_least32_t const ul32_tm_gmtoff{static_cast<std::uint_least32_t>(static_cast<long unsigned>(tm_gmtoff))};
	std::uint_least32_t const ul32_bias{static_cast<std::uint_least32_t>(static_cast<long unsigned>(bias))};
	std::uint_least32_t const dst_timezone{ul32_tm_gmtoff+ul32_bias};
	constexpr std::uint_least32_t ul32_zero{};
	return unix_timestamp_to_iso8601_tsp_impl_internal(sub_overflow(seconds,static_cast<std::int_least64_t>(static_cast<std::int_least32_t>(dst_timezone))),
		subseconds,static_cast<std::int_least32_t>(ul32_zero-dst_timezone));
#endif
#else
	auto res{unix_timestamp_to_tm_impl<true>(seconds)};
	long tm_gmtoff{};
#if defined(_WIN32)
	#if defined(_MSC_VER) || defined(_UCRT)
	{
		auto errn{::fast_io::noexcept_call(_get_timezone,__builtin_addressof(tm_gmtoff))};
		if(errn)
			throw_posix_error(static_cast<int>(errn));
	}
	#else
		tm_gmtoff=_timezone;
	#endif
	long unsigned ulong_tm_gmtoff{static_cast<long unsigned>(tm_gmtoff)};
	long seconds{};
#if (defined(_MSC_VER) || defined(_UCRT))&& (defined(_M_IX86) || defined(_M_X64) ||defined(__i386__) || defined(__x86_64__))
	{
		auto errn{::fast_io::noexcept_call(_get_dstbias,__builtin_addressof(seconds))};
		if(errn)
			throw_posix_error(static_cast<int>(errn));
	}
#else
	seconds=_dstbias;
#endif
	auto const ulong_seconds{static_cast<long unsigned>(static_cast<unsigned>(seconds))};
	ulong_tm_gmtoff+=ulong_seconds;
	ulong_tm_gmtoff=0ul-ulong_tm_gmtoff;
	tm_gmtoff=static_cast<long>(ulong_tm_gmtoff);
#elif defined(__TM_GMTOFF)
	tm_gmtoff=res.__TM_GMTOFF;
#elif defined(__NEWLIB__) || defined(__AVR__) || defined(_PICOLIBC__) || defined(__serenity__)
	tm_gmtoff=0;
#else
	tm_gmtoff=res.tm_gmtoff;
#endif
	std::uint_least8_t month{static_cast<std::uint_least8_t>(res.tm_mon)};
	auto year{res.tm_year+1900};
	if(month==0)
	{
		--year;
		month=12;
	}
	else
	{
		++month;
	}
	return {year,
		month,
		static_cast<std::uint_least8_t>(res.tm_mday),
		static_cast<std::uint_least8_t>(res.tm_hour),
		static_cast<std::uint_least8_t>(res.tm_min),
		static_cast<std::uint_least8_t>(res.tm_sec),subseconds,static_cast<std::int_least32_t>(tm_gmtoff)};
#endif
}

#if defined(__NEWLIB__) || defined(_PICOLIBC__)
#if (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))&&defined(__CYGWIN__)
[[__gnu__::__dllimport__]]
#endif
extern void m_tzset() noexcept __asm__("tzset");
#endif
}

inline void posix_tzset() noexcept
{
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__)
	noexcept_call(_tzset);
#elif defined(__NEWLIB__) || defined(_PICOLIBC__)
	details::m_tzset();
#elif !defined(__AVR__)&&(!defined(__wasi__) || defined(__wasilibc_unmodified_upstream))
	noexcept_call(tzset);
#endif
}

#if defined(_WIN32) && !defined(__BIONIC__) && !defined(__CYGWIN__) && (defined(_UCRT) || defined(_MSC_VER))
struct win32_timezone_t
{
	std::size_t tz_name_len{};
	bool is_dst{};
};

inline bool posix_daylight() noexcept
{
	int hours;
	if(::fast_io::noexcept_call(_get_daylight,__builtin_addressof(hours)))
		return false;
	return hours!=0;
}

inline win32_timezone_t timezone_name(bool is_dst=posix_daylight())
{
	win32_timezone_t tzt{.is_dst=is_dst};
	constexpr std::size_t zero{};
	auto errn{::fast_io::noexcept_call(_get_tzname,__builtin_addressof(tzt.tz_name_len),nullptr,zero,is_dst)};
	if(errn)
		throw_posix_error(static_cast<int>(errn));
	return tzt;
}

inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char,win32_timezone_t>,win32_timezone_t tzt) noexcept
{
	return tzt.tz_name_len;
}

namespace details
{

inline std::size_t print_reserve_define_impl(char* first,win32_timezone_t tzt)
{
	auto errn{::fast_io::noexcept_call(_get_tzname,__builtin_addressof(tzt.tz_name_len),first,tzt.tz_name_len,tzt.is_dst)};
	if(errn)
		throw_posix_error(static_cast<int>(errn));
	if(tzt.tz_name_len)
		return tzt.tz_name_len-1;
	else
		return 0;
}

}

inline char* print_reserve_define(io_reserve_type_t<char,win32_timezone_t>, char* first,win32_timezone_t tzt)
{
	return details::print_reserve_define_impl(::std::to_address(first),tzt)+first;
}

#else

#if defined(__NEWLIB__) || defined(_PICOLIBC__)
extern char *m_tzname[2] __asm__("_tzname");
extern int m_daylight __asm__("_daylight");
#endif

inline bool posix_daylight()
{
#if defined(__MSDOS__) || (defined(__wasi__) &&!defined(__wasilibc_unmodified_upstream)) || defined(__AVR__)
	return 0;
#elif defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__) && !defined(__BIONIC__)
	return _daylight;
#elif defined(__NEWLIB__) && !defined(__CYGWIN__)
	return m_daylight;
#else
	std::time_t t{};
	struct tm stm{};
	if(noexcept_call(localtime_r,__builtin_addressof(t),__builtin_addressof(stm))==0)
		throw_posix_error();
	return stm.tm_isdst;
#endif
}

inline basic_io_scatter_t<char> timezone_name([[maybe_unused]] bool dst=posix_daylight()) noexcept
{
#if defined(__MSDOS__) || (defined(__wasi__) &&!defined(__wasilibc_unmodified_upstream)) || defined(__AVR__)
	return {reinterpret_cast<char const*>(u8"UTC"),3}; 
#else
#if defined(__NEWLIB__) || defined(_PICOLIBC__)
	auto nm{m_tzname[dst]};
#else
	auto nm{tzname[dst]};
#endif
	return {nm,::fast_io::cstr_len(nm)};
#endif
}

#endif


template<std::int_least64_t off_to_epoch>
inline iso8601_timestamp local(basic_timestamp<off_to_epoch> timestamp,[[maybe_unused]] bool dstadj=posix_daylight())
{
#ifdef __MSDOS__
	return utc(timestamp);
#if 0
	return details::to_iso8601_local_impl(static_cast<unix_timestamp>(timestamp));
#endif
#else
	if constexpr(off_to_epoch==0)
	{
		return details::to_iso8601_local_impl(timestamp.seconds,timestamp.subseconds,dstadj);
	}
	else
	{
		unix_timestamp stmp(static_cast<unix_timestamp>(timestamp));
		return details::to_iso8601_local_impl(stmp.seconds,stmp.subseconds,dstadj);
	}
#endif
}

inline void posix_clock_settime([[maybe_unused]] posix_clock_id pclk_id,[[maybe_unused]] unix_timestamp timestamp)
{
#if defined(_WIN32) && !defined(__NEWLIB__)
	nt_clock_settime(pclk_id,timestamp);
#elif defined(__MSDOS__)
	switch(pclk_id)
	{
	case posix_clock_id::realtime:
	case posix_clock_id::realtime_alarm:
	case posix_clock_id::realtime_coarse:
	case posix_clock_id::tai:
	{
#if 0
		if constexpr(sizeof(std::time_t)<sizeof(std::int_least64_t))
		{
			if(static_cast<std::int_least64_t>(std::numeric_limits<std::time_t>::max())<timestamp.seconds)
				throw_posix_error(EOVERFLOW);
		}
		constexpr std::uint_least64_t mul_factor{uint_least64_subseconds_per_second/1000000u};
		timeval tv{static_cast<std::time_t>(timestamp.seconds),
		static_cast<long>(timestamp.subseconds/mul_factor)};
		if(::fast_io::noexcept_call(::settimeofday,__builtin_addressof(tv), nullptr)<0)
			throw_posix_error();
#else
		details::set_dos_unix_timestamp(timestamp);
#endif
	}
	default:
		throw_posix_error(EINVAL);
	}
#elif defined(__AVR__)
	constexpr std::int_least64_t mn_unix_offset{UNIX_OFFSET};
	auto tsp_seconds{timestamp.seconds};
	if(tsp_seconds<mn_unix_offset)
		throw_posix_error(EINVAL);
	if constexpr(sizeof(time_t)>=sizeof(std::int_least64_t))
	{
		time_t t{static_cast<time_t>(tsp_seconds)};
		if(__builtin_add_overflow(t,UNIX_OFFSET,__builtin_addressof(t)))
			throw_posix_error(EINVAL);
		noexcept_call(::set_system_time,t);
	}
	else
	{
		constexpr std::int_least64_t mx_unix_offset{static_cast<std::int_least64_t>(std::numeric_limits<time_t>::max())-UNIX_OFFSET};
		if(tsp_seconds>mx_unix_offset)
			throw_posix_error(EINVAL);
		time_t t{static_cast<time_t>(tsp_seconds)};
		noexcept_call(::set_system_time,t);		
	}
#elif (!defined(__NEWLIB__) || defined(_POSIX_TIMERS)) && (!defined(__wasi__) || defined(__wasilibc_unmodified_upstream))
	constexpr std::uint_least64_t mul_factor{uint_least64_subseconds_per_second/1000000000u};
	struct timespec res{static_cast<std::time_t>(timestamp.seconds),
	static_cast<long>(timestamp.subseconds/mul_factor)};
	auto clk{details::posix_clock_id_to_native_value(pclk_id)};
#ifdef __linux__
	system_call_throw_error(system_call<__NR_clock_settime,int>(clk,__builtin_addressof(res)));
#else
	if(::fast_io::posix::libc_clock_settime(clk,__builtin_addressof(res))<0)
		throw_posix_error();
#endif
#else
	throw_posix_error(EINVAL);
#endif
}


#if 0
#ifdef _WIN32


namespace details
{

}

inline void posix_clock_sleep_abstime_complete(posix_clock_id pclk_id,unix_timestamp timestamp)
{
	switch(pclk_id)
	{
	case posix_clock_id::realtime:
	case posix_clock_id::realtime_alarm:
	case posix_clock_id::realtime_coarse:
	case posix_clock_id::tai:
		return win32::details::win32_posix_clock_gettime_tai_impl();
	case posix_clock_id::monotonic:
	case posix_clock_id::monotonic_coarse:
	case posix_clock_id::monotonic_raw:
	case posix_clock_id::boottime:
//		return win32::details::win32_posix_clock_gettime_boottime_impl();
		return win32::details::win32_posix_clock_gettime_boottime_xp_impl();
	case posix_clock_id::process_cputime_id:
		return win32::details::win32_posix_clock_gettime_process_or_thread_time_impl<false>();
	case posix_clock_id::thread_cputime_id:
		return win32::details::win32_posix_clock_gettime_process_or_thread_time_impl<true>();
	default:
		throw_win32_error(0x00000057);
	}
}

inline [[nodiscard]] bool posix_clock_sleep_abstime(posix_clock_id pclk_id,unix_timestamp timestamp)
{
	posix_clock_sleep_abstime_complete(pclk_id,timestamp);
	return false;
}

#else

inline [[nodiscard]] bool posix_clock_sleep_abstime(posix_clock_id pclk_id,unix_timestamp timestamp)
{
	constexpr std::uint_least64_t mul_factor{uint_least64_subseconds_per_second/1000000000u};
	struct timespec timestamp_spec{static_cast<std::time_t>(timestamp.seconds),static_cast<long>(timestamp.subseconds/mul_factor)};
	auto ret{::fast_io::noexcept_call(::clock_nanosleep,pclk_id,TIMER_ABSTIME,__builtin_addressof(timestamp_spec),ptr,nullptr)};
	if(ret<0)
	{
		auto ern{errno};
		if(ern!=EINTR)
			throw_posix_error(ern);
		return true;
	}
	return false;
}

inline void posix_clock_sleep_abstime_complete(posix_clock_id pclk_id,unix_timestamp timestamp)
{
	constexpr std::uint_least64_t mul_factor{uint_least64_subseconds_per_second/1000000000u};
	struct timespec timestamp_spec{static_cast<std::time_t>(timestamp.seconds),static_cast<long>(timestamp.subseconds/mul_factor)}
	for(;;)
	{
		auto ret{::fast_io::noexcept_call(::clock_nanosleep,pclk_id,TIMER_ABSTIME,__builtin_addressof(timestamp_spec),nullptr)};
		if(ret==0)[[likely]]
			return;
		auto ern{errno};
		if(ern!=EINTR)
			throw_posix_error(ern);
	}
}

#endif
#endif
}
