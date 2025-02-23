#pragma once

namespace fast_io
{

template <::std::integral char_type, typename traits_type, typename allocator_type>
inline constexpr auto
strlike_construct_define(io_strlike_type_t<char_type, ::std::basic_string<char_type, traits_type, allocator_type>>,
						 char_type const *first, char_type const *last)
{
	return ::std::basic_string<char_type, traits_type, allocator_type>(first, last);
}

template <::std::integral char_type, typename traits_type, typename allocator_type>
inline constexpr auto strlike_construct_single_character_define(
	io_strlike_type_t<char_type, ::std::basic_string<char_type, traits_type, allocator_type>>, char_type ch)
{
	return ::std::basic_string<char_type, traits_type, allocator_type>(1, ch);
}

#if (defined(__GLIBCXX__) && !defined(_LIBCPP_VERSION) && !defined(_GLIBCXX_USE_CXX11_ABI)) || \
	(defined(_LIBCPP_VERSION) &&                                                               \
	 !((_LIBCPP_VERSION < 20 && !defined(_LIBCPP_HAS_NO_ASAN) || _LIBCPP_HAS_ASAN) && defined(_LIBCPP_INSTRUMENTED_WITH_ASAN)))

template <::std::integral char_type, typename traits_type, typename allocator_type>
inline constexpr char_type *
strlike_begin(io_strlike_type_t<char_type, ::std::basic_string<char_type, traits_type, allocator_type>>,
			  ::std::basic_string<char_type, traits_type, allocator_type> &str) noexcept
{
	return str.data();
}

template <::std::integral char_type, typename traits_type, typename allocator_type>
inline constexpr char_type *
strlike_curr(io_strlike_type_t<char_type, ::std::basic_string<char_type, traits_type, allocator_type>>,
			 ::std::basic_string<char_type, traits_type, allocator_type> &str) noexcept
{
	return str.data() + str.size();
}

template <::std::integral char_type, typename traits_type, typename allocator_type>
inline constexpr char_type *
strlike_end(io_strlike_type_t<char_type, ::std::basic_string<char_type, traits_type, allocator_type>>,
			::std::basic_string<char_type, traits_type, allocator_type> &str) noexcept
{
	return str.data() + str.capacity();
}
#if __cpp_lib_string_resize_and_overwrite >= 202110L
namespace details
{

template <::std::integral char_type, typename size_type>
struct empty_string_set_ptr
{
	::std::size_t realsize{};
	inline constexpr ::std::size_t operator()(char_type const *, size_type) noexcept
	{
		return realsize;
	}
};

} // namespace details

template <::std::integral char_type, typename traits_type, typename allocator_type>
inline constexpr ::std::size_t
strlike_sso_size(io_strlike_type_t<char_type, ::std::basic_string<char_type, traits_type, allocator_type>>) noexcept
{
	return ::fast_io::details::string_hack::local_capacity<
		::std::basic_string<char_type, traits_type, allocator_type>>();
}
#endif

template <::std::integral char_type, typename traits_type, typename allocator_type>
inline constexpr void
strlike_set_curr(io_strlike_type_t<char_type, ::std::basic_string<char_type, traits_type, allocator_type>>,
				 ::std::basic_string<char_type, traits_type, allocator_type> &str, char_type *p)
{
#if (__cpp_lib_string_resize_and_overwrite >= 202110L || __cpp_constexpr_dynamic_alloc >= 201907L)
	if (__builtin_is_constant_evaluated())
	{
		auto old_ptr{str.data()};
		::std::size_t const sz{static_cast<::std::size_t>(p - str.data())};
#if __cpp_lib_string_resize_and_overwrite >= 202110L

		str.resize_and_overwrite(
			sz, ::fast_io::details::empty_string_set_ptr<
					char_type, typename ::std::basic_string<char_type, traits_type, allocator_type>::size_type>{sz});
#else
		auto curr_ptr{str.data() + str.size()};
		if (p < curr_ptr)
		{
			str.resize(sz);
		}
		else if (curr_ptr < p)
		{
			::std::size_t const oldsz{str.size()};
			::std::size_t const diff{static_cast<::std::size_t>(p - curr_ptr)};
			::fast_io::details::local_operator_new_array_ptr<char_type> buffer(diff);
			for (::std::size_t i{}; i != diff; ++i)
			{
				buffer[i] = curr_ptr[i];
			}
			str.append(diff, 0);
			auto newp{str.data() + oldsz};
			for (::std::size_t i{}; i != diff; ++i)
			{
				newp[i] = buffer[i];
			}
		}
#endif
		if (old_ptr != str.data())
		{
			::fast_io::fast_terminate();
		}
	}
	else
#endif
	{
		::fast_io::details::string_hack::set_end_ptr(str, p);
		traits_type::assign(*p, char_type());
	}
}

template <::std::integral char_type, typename traits_type, typename allocator_type>
inline constexpr void
strlike_reserve(io_strlike_type_t<char_type, ::std::basic_string<char_type, traits_type, allocator_type>>,
				::std::basic_string<char_type, traits_type, allocator_type> &str, ::std::size_t n)
{
	str.reserve(n);
}
#endif
template <::std::integral char_type, typename traits_type, typename allocator_type>
inline constexpr void
strlike_append(io_strlike_type_t<char_type, ::std::basic_string<char_type, traits_type, allocator_type>>,
			   ::std::basic_string<char_type, traits_type, allocator_type> &str, char_type const *first,
			   char_type const *last)
{
	str.append(first, last);
}

template <::std::integral char_type, typename traits_type, typename allocator_type>
inline constexpr void
strlike_push_back(io_strlike_type_t<char_type, ::std::basic_string<char_type, traits_type, allocator_type>>,
				  ::std::basic_string<char_type, traits_type, allocator_type> &str, char_type ch)
{
	str.push_back(ch);
}
template <::std::integral char_type, typename traits_type, typename allocator_type>
inline constexpr io_strlike_reference_wrapper<char_type, ::std::basic_string<char_type, traits_type, allocator_type>>
io_strlike_ref(io_alias_t, ::std::basic_string<char_type, traits_type, allocator_type> &str) noexcept
{
	return {__builtin_addressof(str)};
}

template <::std::integral CharT, typename Traits = ::std::char_traits<CharT>,
		  typename Allocator = ::std::allocator<CharT>>
using basic_ostring_ref_std = io_strlike_reference_wrapper<CharT, ::std::basic_string<CharT, Traits, Allocator>>;
using ostring_ref_std = basic_ostring_ref_std<char>;
using wostring_ref_std = basic_ostring_ref_std<wchar_t>;
using u8ostring_ref_std = basic_ostring_ref_std<char8_t>;
using u16ostring_ref_std = basic_ostring_ref_std<char16_t>;
using u32ostring_ref_std = basic_ostring_ref_std<char32_t>;

template <::std::integral CharT, typename Traits = ::std::char_traits<CharT>,
		  typename Allocator = ::std::allocator<CharT>>
using basic_ostring_ref [[deprecated("Please use basic_ostring_ref_std or basic_ostring_ref_fast_io instead.")]] = ::fast_io::basic_ostring_ref_std<CharT, Traits, Allocator>;
using ostring_ref [[deprecated("Please use ostring_ref_std or ostring_ref_fast_io instead.")]] = ::fast_io::ostring_ref_std;
using wostring_ref [[deprecated("Please use wostring_ref_std or wostring_ref_fast_io instead.")]] = ::fast_io::wostring_ref_std;
using u8ostring_ref [[deprecated("Please use u8ostring_ref_std or u8ostring_ref_fast_io instead.")]] = ::fast_io::u8ostring_ref_std;
using u16ostring_ref [[deprecated("Please use u16ostring_ref_std or u16ostring_ref_fast_io instead.")]] = ::fast_io::u16ostring_ref_std;
using u32ostring_ref [[deprecated("Please use u32ostring_ref_std or u32ostring_ref_fast_io instead.")]] = ::fast_io::u32ostring_ref_std;

} // namespace fast_io
