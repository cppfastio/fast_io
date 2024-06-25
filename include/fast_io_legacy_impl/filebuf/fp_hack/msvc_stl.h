#pragma once

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#endif

namespace fast_io::details::streambuf_hack
{

/*
https://github.com/microsoft/STL/blob/master/stl/inc/fstream#L775

	const _Cvt* _Pcvt; // pointer to codecvt facet (may be null)
	_Elem _Mychar; // putback character, when _Ungetc fails
	bool _Wrotesome; // true if homing sequence may be needed
	typename _Traits::state_type _State; // current conversion state
	bool _Closef; // true if C stream must be closed
	FILE* _Myfile; // pointer to C stream
*/

template <class _Elem, class _Traits>
class basic_filebuf_model : public ::std::basic_streambuf<_Elem, _Traits>
{
public:
	using _Mysb = ::std::basic_streambuf<_Elem, _Traits>;
	using _Cvt = ::std::codecvt<_Elem, char, typename _Traits::state_type>;
	using int_type = typename _Traits::int_type;
	using pos_type = typename _Traits::pos_type;
	using off_type = typename _Traits::off_type;
	_Cvt const *_Pcvt;                   // pointer to codecvt facet (may be null)
	_Elem _Mychar;                       // putback character, when _Ungetc fails
	bool _Wrotesome;                     // true if homing sequence may be needed
	typename _Traits::state_type _State; // current conversion state
	bool _Closef;                        // true if C stream must be closed
	FILE *_Myfile;                       // pointer to C stream

	_Elem *_Set_eback; // saves eback() during one-element putback
	_Elem *_Set_egptr; // saves egptr()
};

template <typename char_type, typename traits_type>
#if __has_cpp_attribute(__gnu__::__pure__)
[[__gnu__::__pure__]]
#endif
inline FILE *fp_hack_impl(::std::basic_filebuf<char_type, traits_type> *fbuf) noexcept
{
	using filebuf_model_type = basic_filebuf_model<char_type, traits_type>;
	static_assert(sizeof(filebuf_model_type) == sizeof(::std::basic_filebuf<char_type, traits_type>),
				  "unmatched ::std::basic_filebuf model");
	return *reinterpret_cast<FILE **>(reinterpret_cast<::std::byte *>(fbuf) + __builtin_offsetof(filebuf_model_type, _Myfile));
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
#ifdef __cpp_rtti
			auto fbfb{dynamic_cast<::std::basic_filebuf<char_type, traits_type> *>(fb)};
			if (fbfb)
			{
				return ::fast_io::details::streambuf_hack::fp_hack_impl(fbfb);
			}
#else
			if constexpr (::std::same_as<char_type, char> || ::std::same_as<char_type, wchar_t> ||
						  ::std::same_as<char_type, char8_t> || ::std::same_as<char_type, char16_t> ||
						  ::std::same_as<char_type, char32_t>)
			{
				char const *my_type{::fast_io::rtti_hack::abi_type_info_name_or_nullptr(fb)};
				if (my_type)
				{
					::std::size_t mytypelen{::fast_io::cstr_len(my_type)};
					if (::fast_io::details::symbol_cmp_equal(::fast_io::details::libstdcxx_streambufname<0, char_type>, my_type, mytypelen))
					{
						return ::fast_io::details::streambuf_hack::fp_hack_impl(static_cast<::std::basic_filebuf<char_type, traits_type> *>(fb));
					}
				}
			}
#endif
		}
	}
	return nullptr;
}

template <typename CharT, typename Traits>
#if __has_cpp_attribute(nodiscard)
[[nodiscard]]
#endif
inline ::std::basic_filebuf<CharT, Traits> *open_msvc_hacked_basic_filebuf_impl(FILE *fp)
{
	using filebuf_model_type = basic_filebuf_model<CharT, Traits>;
	static_assert(sizeof(filebuf_model_type) == sizeof(::std::basic_filebuf<CharT, Traits>),
				  "unmatched ::std::basic_filebuf model");
	filebuf_guard<CharT, Traits> guard{new ::std::basic_filebuf<CharT, Traits>(fp)};
	::std::basic_filebuf<CharT, Traits> *fb{guard.new_filebuf};
	if (!fb->is_open())
	{
		throw_posix_error();
	}
	reinterpret_cast<char unsigned *>(fb)[__builtin_offsetof(filebuf_model_type, _Closef)] = 1;
	return guard.release();
}

template <typename CharT, typename Traits>
#if __has_cpp_attribute(nodiscard)
[[nodiscard]]
#endif
inline ::std::basic_filebuf<CharT, Traits> *open_hacked_basic_filebuf(FILE *fp, ::fast_io::open_mode)
{
	return open_msvc_hacked_basic_filebuf_impl<CharT, Traits>(fp);
}

} // namespace fast_io::details::streambuf_hack
