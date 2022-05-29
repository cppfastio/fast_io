#pragma once

namespace fast_io
{

namespace details
{
/*
Referenced from https://github.com/SerenityOS/serenity/blob/master/Userland/Libraries/LibC/stdio.cpp

*/
struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
serenity_file_buffer_model
{
	std::uint_least8_t* m_data{};
	std::size_t m_capacity{BUFSIZ};
	std::size_t m_begin{};
	std::size_t m_end{};
	int m_mode{-1};
	std::uint_least8_t m_unget_buffer{};
	bool m_ungotten:1 {};
	bool m_data_is_malloced:1 {};
	bool m_empty: 1{true};
};

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
serenity_file_model
{
	int m_fd{-1};
	int m_mode{};
	std::uint_least8_t m_flags{};
	int m_error{};
	bool m_eof{};
	pid_t m_popen_child{-1};
	serenity_file_buffer_model m_buffer;
	__pthread_mutex_t m_mutex;
};

inline char* serenity_obuffer_begin(serenity_file_model*
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
fp) noexcept
{
	return reinterpret_cast<char*>(fp->m_buffer.m_data);
}

inline char* serenity_obuffer_curr(serenity_file_model*
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
fp) noexcept
{
	return reinterpret_cast<char*>(fp->m_buffer.m_data)+fp->m_buffer.m_begin;
}

inline char* serenity_obuffer_end(serenity_file_model*
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
fp) noexcept
{
	return reinterpret_cast<char*>(fp->m_buffer.m_data)+fp->m_buffer.m_end;
}

inline void serenity_obuffer_set_curr(serenity_file_model*
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
fp,char * ptr) noexcept
{
	fp->m_buffer.m_begin=static_cast<std::size_t>(ptr-reinterpret_cast<char*>(fp->m_buffer.m_data));
}

inline void serenity_obuffer_overflow(serenity_file_model*
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
fp,char ch)
{
	using file_ptr_may_alias
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
	= FILE*;
	int ret{noexcept_call(::fputc,ch,reinterpret_cast<file_ptr_may_alias>(fp))};
	if(ret==EOF)
		throw_posix_error();
}


inline char* serenity_ibuffer_begin(serenity_file_model*
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
fp) noexcept
{
	return reinterpret_cast<char*>(fp->m_buffer.m_data);
}

inline char* serenity_ibuffer_curr(serenity_file_model*
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
fp) noexcept
{
	return reinterpret_cast<char*>(fp->m_buffer.m_data)+fp->m_buffer.m_begin;
}

inline char* serenity_ibuffer_end(serenity_file_model*
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
fp) noexcept
{
	return reinterpret_cast<char*>(fp->m_buffer.m_data)+fp->m_buffer.m_end;
}

inline void serenity_ibuffer_set_curr(serenity_file_model*
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
fp,char* ptr) noexcept
{
	fp->m_buffer.m_begin=static_cast<std::size_t>(ptr-reinterpret_cast<char*>(fp->m_buffer.m_data));
}

inline bool serenity_ibuffer_underflow(serenity_file_model*
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
fp)
{
	fp->m_buffer.m_begin=fp->m_buffer.m_end;
	using file_ptr_may_alias
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
	= FILE*;
	int c{noexcept_call(fgetc,reinterpret_cast<file_ptr_may_alias>(fp))};
	if(c==EOF)
	{
		if(fp->m_error)
			throw_posix_error();
		return false;
	}
	fp->m_buffer.m_begin=0;
	return true;
}

}

template<std::integral char_type>
requires (sizeof(char_type)==1)
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char_type* ibuffer_begin(basic_c_io_observer_unlocked<char_type> bciob) noexcept
{
	return ::fast_io::details::serenity_ibuffer_begin(reinterpret_cast<::fast_io::details::serenity_file_model*>(bciob.fp));
}

template<std::integral char_type>
requires (sizeof(char_type)==1)
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char_type* ibuffer_curr(basic_c_io_observer_unlocked<char_type> bciob) noexcept
{
	return ::fast_io::details::serenity_ibuffer_curr(reinterpret_cast<::fast_io::details::serenity_file_model*>(bciob.fp));
}

template<std::integral char_type>
requires (sizeof(char_type)==1)
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char_type* ibuffer_end(basic_c_io_observer_unlocked<char_type> bciob) noexcept
{
	return ::fast_io::details::serenity_ibuffer_end(reinterpret_cast<::fast_io::details::serenity_file_model*>(bciob.fp));
}

template<std::integral char_type>
requires (sizeof(char_type)==1)
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline void ibuffer_set_curr(basic_c_io_observer_unlocked<char_type> bciob,char_type*
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
ptr
) noexcept
{
	::fast_io::details::serenity_ibuffer_set_curr(reinterpret_cast<::fast_io::details::serenity_file_model*>(bciob.fp),reinterpret_cast<char*>(ptr));
}

template<std::integral char_type>
inline bool ibuffer_underflow(basic_c_io_observer_unlocked<char_type> bciob) noexcept
{
	return ::fast_io::details::serenity_ibuffer_underflow(reinterpret_cast<::fast_io::details::serenity_file_model*>(bciob.fp));
}

template<std::integral char_type>
requires (sizeof(char_type)==1)
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char_type* obuffer_begin(basic_c_io_observer_unlocked<char_type> bciob) noexcept
{
	return ::fast_io::details::serenity_obuffer_begin(reinterpret_cast<::fast_io::details::serenity_file_model*>(bciob.fp));
}

template<std::integral char_type>
requires (sizeof(char_type)==1)
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char_type* obuffer_curr(basic_c_io_observer_unlocked<char_type> bciob) noexcept
{
	return ::fast_io::details::serenity_obuffer_curr(reinterpret_cast<::fast_io::details::serenity_file_model*>(bciob.fp));
}

template<std::integral char_type>
requires (sizeof(char_type)==1)
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char_type* obuffer_end(basic_c_io_observer_unlocked<char_type> bciob) noexcept
{
	return ::fast_io::details::serenity_obuffer_end(reinterpret_cast<::fast_io::details::serenity_file_model*>(bciob.fp));
}

template<std::integral char_type>
requires (sizeof(char_type)==1)
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline void obuffer_set_curr(basic_c_io_observer_unlocked<char_type> bciob,char_type*
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
ptr
) noexcept
{
	::fast_io::details::serenity_obuffer_set_curr(reinterpret_cast<::fast_io::details::serenity_file_model*>(bciob.fp),reinterpret_cast<char*>(ptr));
}

template<std::integral char_type>
requires (sizeof(char_type)==1)
inline void obuffer_overflow(basic_c_io_observer_unlocked<char_type> bciob,char_type ch) noexcept
{
	return ::fast_io::details::serenity_obuffer_overflow(reinterpret_cast<::fast_io::details::serenity_file_model*>(bciob.fp),ch);
}

}
