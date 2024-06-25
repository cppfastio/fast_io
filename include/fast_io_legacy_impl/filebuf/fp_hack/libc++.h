#pragma once

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#endif

/*
https://github.com/llvm/llvm-project/blob/main/libcxx/src/std_stream.h
*/

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

template <typename T>
inline FILE *stdinbuf_stdoutbuf_fp_hack(T *stdbuf) noexcept
{
	FILE *fp{};
	// we can only do this or ubsanitizer will complain. Do not do down_cast
	::fast_io::details::my_memcpy(__builtin_addressof(fp),
								  reinterpret_cast<::std::byte *>(stdbuf) +
									  sizeof(::std::basic_streambuf<typename T::char_type, typename T::traits_type>),
								  sizeof(fp));
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
class libcxx_basic_filebuf_model : public ::std::basic_streambuf<_CharT, _Traits>
{
public:
	typedef _CharT char_type;
	typedef _Traits traits_type;
	typedef typename traits_type::int_type int_type;
	typedef typename traits_type::pos_type pos_type;
	typedef typename traits_type::off_type off_type;
	typedef typename traits_type::state_type state_type;
	char *__extbuf_;
	char const *__extbufnext_;
	char const *__extbufend_;
	char __extbuf_min_[8];
	::std::size_t __ebs_;
	char_type *__intbuf_;
	::std::size_t __ibs_;
	FILE *__file_;
	::std::codecvt<char_type, char, state_type> const *__cv_;
	state_type __st_;
	state_type __st_last_;
	::std::ios_base::openmode __om_;
	::std::ios_base::openmode __cm_;
	bool __owns_eb_;
	bool __owns_ib_;
	bool __always_noconv_;
};

template <typename char_type, typename traits_type>
inline constexpr ::std::size_t libcxx_fp_location{
	__builtin_offsetof(libcxx_basic_filebuf_model<char_type, traits_type>, __file_)};

template <typename char_type, typename traits_type>
inline constexpr ::std::size_t libcxx_om_location{
	__builtin_offsetof(libcxx_basic_filebuf_model<char_type, traits_type>, __om_)};

template <typename char_type, typename traits_type>
#if __has_cpp_attribute(__gnu__::__pure__)
[[__gnu__::__pure__]]
#endif
inline FILE *fp_hack_impl(::std::basic_filebuf<char_type, traits_type> *fbuf) noexcept
{
	return *reinterpret_cast<FILE **>(reinterpret_cast<::std::byte *>(fbuf) + libcxx_fp_location<char_type, traits_type>);
}

template <typename char_type, typename traits_type>
#if __has_cpp_attribute(__gnu__::__pure__)
[[__gnu__::__pure__]]
#endif
inline FILE *fp_hack_fbf(::std::basic_filebuf<char_type, traits_type> *fbuf) noexcept
{
	if (fbuf == nullptr)
	{
		return nullptr;
	}
	return ::fast_io::details::streambuf_hack::fp_hack_impl(fbuf);
}

template <typename T>
#if __has_cpp_attribute(__gnu__::__pure__)
[[__gnu__::__pure__]]
#endif
inline FILE *fp_hack(T *fb) noexcept
{
	using char_type = typename T::char_type;
	using traits_type = typename T::traits_type;
	if constexpr (::std::derived_from<T, ::std::basic_filebuf<char_type, traits_type>>)
	{
		return ::fast_io::details::streambuf_hack::fp_hack_fbf(fb);
	}
	else if constexpr (::std::same_as<T, ::std::basic_streambuf<char_type, traits_type>> &&
					   ::std::same_as<::std::char_traits<char_type>, traits_type>)
	{
		if (fb) [[likely]]
		{
			if constexpr (::std::same_as<char_type, char> || ::std::same_as<char_type, wchar_t> ||
						  ::std::same_as<char_type, char8_t> || ::std::same_as<char_type, char16_t> ||
						  ::std::same_as<char_type, char32_t>)
			{
				char const *my_type{::fast_io::rtti_hack::abi_type_info_name_or_nullptr(fb)};
				if (my_type)
				{
					::std::size_t mytypelen{::std::strlen(my_type)};
					if (::fast_io::details::symbol_cmp_equal(::fast_io::details::libcxx_streambufname<0, char_type>, my_type, mytypelen) ||
						::fast_io::details::symbol_cmp_equal(::fast_io::details::libcxx_streambufname<1, char_type>, my_type, mytypelen))
					{
						return ::fast_io::details::streambuf_hack::stdinbuf_stdoutbuf_fp_hack(fb);
					}
					else if (::fast_io::details::symbol_cmp_equal(::fast_io::details::libcxx_streambufname<2, char_type>, my_type, mytypelen))
					{
						return ::fast_io::details::streambuf_hack::fp_hack_impl(static_cast<::std::basic_filebuf<char_type, traits_type> *>(fb));
					}
				}
			}
		}
	}
	return nullptr;
}

template <typename char_type, typename traits_type>
inline void fp_hack_open(::std::basic_filebuf<char_type, traits_type> *fb, FILE *fp, ::std::ios::openmode mode) noexcept
{
	::fast_io::details::my_memcpy(reinterpret_cast<::std::byte *>(fb) + libcxx_om_location<char_type, traits_type>,
								  __builtin_addressof(mode), sizeof(mode));
	::fast_io::details::my_memcpy(reinterpret_cast<::std::byte *>(fb) + libcxx_fp_location<char_type, traits_type>,
								  __builtin_addressof(fp), sizeof(fp));
}

template <typename CharT, typename Traits>
#if __has_cpp_attribute(nodiscard)
[[nodiscard]]
#endif
inline ::std::basic_filebuf<CharT, Traits> *open_libcxx_hacked_basic_filebuf_impl(FILE *fp, ::std::ios::openmode mode)
{
	filebuf_guard<CharT, Traits> guard(new ::std::basic_filebuf<CharT, Traits>);
	::std::basic_filebuf<CharT, Traits> *fb{guard.new_filebuf};
	::fast_io::details::my_memcpy(reinterpret_cast<::std::byte *>(fb) + libcxx_fp_location<CharT, Traits>,
								  __builtin_addressof(fp), sizeof(fp));
	::fast_io::details::my_memcpy(reinterpret_cast<::std::byte *>(fb) + libcxx_om_location<CharT, Traits>,
								  __builtin_addressof(mode), sizeof(mode));
	return guard.release();
}

template <typename CharT, typename Traits>
#if __has_cpp_attribute(nodiscard)
[[nodiscard]]
#endif
inline ::std::basic_filebuf<CharT, Traits> *open_hacked_basic_filebuf(FILE *fp, ::fast_io::open_mode mode)
{
	return open_libcxx_hacked_basic_filebuf_impl<CharT, Traits>(
		fp, ::fast_io::details::calculate_fstream_file_open_mode(mode));
}

} // namespace fast_io::details::streambuf_hack
