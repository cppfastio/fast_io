#pragma once

namespace fast_io
{

template<std::integral char_type>
requires requires(FILE* fp)
{
	{::std::posix::__libc_stdio_fp_ibuffer_begin<char_type>(fp)}->char_type*;
}
inline char_type* ibuffer_begin(basic_c_io_observer_unlocked<char_type> ciob) noexcept
{
	return ::std::posix::__libc_stdio_fp_ibuffer_begin<char_type>(ciob.fp);
}

template<std::integral char_type>
requires requires(FILE* fp)
{
	{::std::posix::__libc_stdio_fp_ibuffer_curr<char_type>(fp)}->char_type*;
}
inline char_type* ibuffer_curr(basic_c_io_observer_unlocked<char_type> ciob) noexcept
{
	return ::std::posix::__libc_stdio_fp_ibuffer_curr<char_type>(ciob.fp);
}

template<std::integral char_type>
requires requires(FILE* fp)
{
	{::std::posix::__libc_stdio_fp_ibuffer_end<char_type>(fp)}->char_type*;
}
inline char_type* ibuffer_end(basic_c_io_observer_unlocked<char_type> ciob) noexcept
{
	return ::std::posix::__libc_stdio_fp_ibuffer_end<char_type>(ciob.fp);
}

template<std::integral char_type>
requires requires(FILE* fp,char_type* ptr)
{
	{::std::posix::__libc_stdio_fp_ibuffer_set_curr<char_type>(fp,ptr)};
}
inline void ibuffer_set_curr(basic_c_io_observer_unlocked<char_type> ciob,char_type* ptr) noexcept
{
	::std::posix::__libc_stdio_fp_ibuffer_set_curr<char_type>(ciob.fp,ptr);
}

template<c_family family,std::integral char_type>
inline bool ibuffer_underflow(basic_c_io_observer_unlocked<char_type> ciob)
requires requires(FILE* fp)
{
	{::std::posix::__libc_stdio_fp_ibuffer_underflow<char_type>(fp)}->bool;
}
{
	return ::std::posix::__libc_stdio_fp_ibuffer_underflow<char_type>(ciob.fp);
}


template<std::integral char_type>
requires requires(FILE* fp)
{
	{::std::posix::__libc_stdio_fp_obuffer_begin<char_type>(fp)}->char_type*;
}
inline char_type* obuffer_begin(basic_c_io_observer_unlocked<char_type> ciob) noexcept
{
	return ::std::posix::__libc_stdio_fp_obuffer_begin<char_type>(ciob.fp);
}

template<std::integral char_type>
requires requires(FILE* fp)
{
	{::std::posix::__libc_stdio_fp_obuffer_curr<char_type>(fp)}->char_type*;
}
inline char_type* obuffer_curr(basic_c_io_observer_unlocked<char_type> ciob) noexcept
{
	return ::std::posix::__libc_stdio_fp_obuffer_curr<char_type>(ciob.fp);
}

template<std::integral char_type>
requires requires(FILE* fp)
{
	{::std::posix::__libc_stdio_fp_obuffer_end<char_type>(fp)}->char_type*;
}
inline char_type* obuffer_end(basic_c_io_observer_unlocked<char_type> ciob) noexcept
{
	return ::std::posix::__libc_stdio_fp_obuffer_end<char_type>(ciob.fp);
}

template<std::integral char_type>
requires requires(FILE* fp,char_type* ptr)
{
	{::std::posix::__libc_stdio_fp_obuffer_set_curr<char_type>(fp,ptr)}->char_type*;
}
inline void obuffer_set_curr(basic_c_io_observer_unlocked<char_type> ciob,char_type* ptr) noexcept
{
	::std::posix::__libc_stdio_fp_obuffer_set_curr<char_type>(ciob.fp,ptr);
}

template<std::integral char_type>
requires requires(FILE* fp,char_type ch)
{
	::std::posix::__libc_stdio_fp_obuffer_overflow<char_type>(fp,ch);
}
inline void obuffer_overflow(basic_c_io_observer_unlocked<char_type> ciob,char_type ch)
{
	::std::posix::__libc_stdio_fp_obuffer_overflow<char_type>(ciob.fp,ch);
}

}
