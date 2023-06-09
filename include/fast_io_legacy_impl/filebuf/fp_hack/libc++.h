#pragma once

#ifdef __GNUC__
#pragma GCC system_header
#endif

/*
https://github.com/llvm/llvm-project/blob/main/libcxx/src/std_stream.h
*/

#include"libc++symbol.h"

namespace fast_io::details::streambuf_hack
{

/*
https://github.com/llvm-mirror/libcxx/blob/master/include/__std_stream
private:
    FILE* __file_;
    const codecvt<char_type, char, state_type>* __cv_;
    state_type* __st_;
    int __encoding_;
    int_type __last_consumed_;
    bool __last_consumed_is_next_;
    bool __always_noconv_;
*/

template<typename T>
inline FILE* stdinbuf_stdoutbuf_fp_hack(T* stdbuf) noexcept
{
	FILE* fp{};
	// we can only do this or ubsanitizer will complain. Do not do down_cast
	::fast_io::details::my_memcpy(__builtin_addressof(fp),reinterpret_cast<std::byte*>(stdbuf)+
	sizeof(std::basic_streambuf<typename T::char_type, typename T::traits_type>),sizeof(fp));
	return fp;
}

/*
https://github.com/llvm-mirror/libcxx/blob/master/include/fstream
private:
  char* __extbuf_;
  const char* __extbufnext_;
  const char* __extbufend_;
  char __extbuf_min_[8];
  size_t __ebs_;
  char_type* __intbuf_;
  size_t __ibs_;
  FILE* __file_;
  const codecvt<char_type, char, state_type>* __cv_;
  state_type __st_;
  state_type __st_last_;
  ios_base::openmode __om_;
  ios_base::openmode __cm_;
  bool __owns_eb_;
  bool __owns_ib_;
  bool __always_noconv_;
*/

template <class _CharT, class _Traits>
class libcxx_basic_filebuf_model
    : public std::basic_streambuf<_CharT, _Traits>
{
public:
	typedef _CharT                           char_type;
	typedef _Traits                          traits_type;
	typedef typename traits_type::int_type   int_type;
	typedef typename traits_type::pos_type   pos_type;
	typedef typename traits_type::off_type   off_type;
	typedef typename traits_type::state_type state_type;
	char* __extbuf_;
	const char* __extbufnext_;
	const char* __extbufend_;
	char __extbuf_min_[8];
	std::size_t __ebs_;
	char_type* __intbuf_;
	std::size_t __ibs_;
	FILE* __file_;
	const std::codecvt<char_type, char, state_type>* __cv_;
	state_type __st_;
	state_type __st_last_;
	std::ios_base::openmode __om_;
	std::ios_base::openmode __cm_;
	bool __owns_eb_;
	bool __owns_ib_;
	bool __always_noconv_;
};


template<typename char_type,typename traits_type>
inline constexpr std::size_t libcxx_fp_location{__builtin_offsetof(libcxx_basic_filebuf_model<char_type,traits_type>,__file_)};

template<typename char_type,typename traits_type>
inline constexpr std::size_t libcxx_om_location{__builtin_offsetof(libcxx_basic_filebuf_model<char_type,traits_type>,__om_)};

template<typename char_type,typename traits_type>
inline FILE* fp_hack_impl(std::basic_filebuf<char_type,traits_type>* fbuf) noexcept
{
	FILE* fp{};
	// we can only do this or ubsanitizer will complain. Do not do down_cast
	::fast_io::details::my_memcpy(__builtin_addressof(fp),reinterpret_cast<std::byte*>(fbuf)+libcxx_fp_location<char_type,traits_type>,sizeof(fp));
	return fp;
}

template<typename char_type,typename traits_type>
inline FILE* fp_hack(std::basic_filebuf<char_type,traits_type>* fbuf) noexcept
{
	if(fbuf==nullptr)
		return nullptr;
	return fp_hack_impl(fbuf);
}


template<typename T>
requires (std::same_as<T,std::basic_streambuf<typename T::char_type,typename T::traits_type>>)
inline FILE* fp_hack([[maybe_unused]] T* stdbuf) noexcept
{
#ifdef __cpp_rtti
	using char_type = typename T::char_type;
	using traits_type = typename T::traits_type;
#ifdef __cpp_exceptions
	try
	{
#endif
		if(stdbuf)[[likely]]
		{
			if constexpr(
				::std::same_as<char_type,char>||::std::same_as<char_type,wchar_t>||
				::std::same_as<char_type,char8_t>||::std::same_as<char_type,char16_t>||
				::std::same_as<char_type,char32_t>
			)
			{
				char const* my_type{typeid(*stdbuf).name()};
				::std::size_t mytypelen{::std::strlen(my_type)};
				if((mytypelen==::fast_io::details::libcxx_stdinoutbufname<false,char_type>.size()&&
					::std::memcmp(my_type,
					::fast_io::details::libcxx_stdinoutbufname<false,char_type>.data(),
					mytypelen)==0)||
				(mytypelen==::fast_io::details::libcxx_stdinoutbufname<true,char_type>.size()&&
					::std::memcmp(my_type,
					::fast_io::details::libcxx_stdinoutbufname<true,char_type>.data(),
					mytypelen)==0))
				{
					return stdinbuf_stdoutbuf_fp_hack(stdbuf);
				}
			}
			auto fbf{dynamic_cast<std::basic_filebuf<char_type,traits_type>*>(stdbuf)};
			if(fbf)
			{
				return fp_hack_impl(fbf);
			}
		}
#ifdef __cpp_exceptions
	}
	catch(...){}
#endif
#endif
	return nullptr;
}

template<typename char_type,typename traits_type>
inline void fp_hack_open(std::basic_filebuf<char_type,traits_type>* fb,FILE* fp,std::ios::openmode mode) noexcept
{
	::fast_io::details::my_memcpy(reinterpret_cast<std::byte*>(fb)+libcxx_om_location<char_type,traits_type>,__builtin_addressof(mode),sizeof(mode));
	::fast_io::details::my_memcpy(reinterpret_cast<std::byte*>(fb)+libcxx_fp_location<char_type,traits_type>,__builtin_addressof(fp),sizeof(fp));
}

}