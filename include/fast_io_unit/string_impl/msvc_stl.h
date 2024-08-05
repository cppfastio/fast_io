#pragma once

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#endif

namespace fast_io::details::string_hack
{

/*
https://github.com/microsoft/STL/blob/master/stl/inc/xstring
*/

template <class _Elem, class _Traits = ::std::char_traits<_Elem>, class _Alloc = ::std::allocator<_Elem>>
struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	model
{
	using _Alty = ::std::_Rebind_alloc_t<_Alloc, _Elem>;
	using _Alty_traits = ::std::allocator_traits<_Alty>;
	using _Scary_val = ::std::_String_val<::std::conditional_t<
		::std::_Is_simple_alloc_v<_Alty>, ::std::_Simple_types<_Elem>,
		::std::_String_iter_types<_Elem, typename _Alty_traits::size_type, typename _Alty_traits::difference_type,
								  typename _Alty_traits::pointer, typename _Alty_traits::const_pointer
#if _MSVC_STL_UPDATE < 202306L
								  ,
								  _Elem &, _Elem const &
#endif
								  >>>;
	using compress_pair_type = ::std::_Compressed_pair<_Alty, _Scary_val>;
	compress_pair_type _Mypair;
};

template <typename elem, typename traits, typename alloc>
inline constexpr decltype(auto) hack_scary_val(::std::basic_string<elem, traits, alloc> &str) noexcept
{
	using model_t = model<elem, traits, alloc>;
	using compress_pair_type = typename model_t::compress_pair_type;
	using scary_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= typename model_t::_Scary_val *;
	return *reinterpret_cast<scary_ptr>(reinterpret_cast<::std::byte *>(__builtin_addressof(str)) +
										__builtin_offsetof(model_t, _Mypair) + __builtin_offsetof(compress_pair_type, _Myval2));
}

#if !defined(_DISABLE_STL_ANNOTATION) && !defined(_DISABLE_STRING_ANNOTATION)
#ifdef __SANITIZE_ADDRESS__
#define FAST_IO_MSVC_STL_INSERT_STRING_ANNOTATION
#elif defined(__has_feature)
#if __has_feature(address_sanitizer)
#define FAST_IO_MSVC_STL_INSERT_STRING_ANNOTATION
#endif
#endif
#endif

// we leave this FAST_IO_MSVC_STL_INSERT_STRING_ANNOTATION macro for users to use

inline constexpr bool msvc_stl_xstring_activate_string_annotation{
#ifdef FAST_IO_MSVC_STL_INSERT_STRING_ANNOTATION
	true
#endif
};

#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline _CONSTEXPR20 bool msvc_stl_xstring_get_asan_string_should_annotate() noexcept
{
#ifdef FAST_IO_MSVC_STL_INSERT_STRING_ANNOTATION
	if constexpr (::fast_io::details::string_hack::msvc_stl_xstring_activate_string_annotation)
	{
		if (!__builtin_is_constant_evaluated())
		{
			return ::_Asan_string_should_annotate;
		}
	}
#endif
	return false;
}

#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline _CONSTEXPR20 void msvc_stl_sanitizer_annotate_contiguous_container(
	[[maybe_unused]] void const *_First, [[maybe_unused]] void const *_End, [[maybe_unused]] void const *_Old_last, [[maybe_unused]] void const *_New_last)
{
#ifdef FAST_IO_MSVC_STL_INSERT_STRING_ANNOTATION
	if (!__builtin_is_constant_evaluated())
	{
		_CSTD __sanitizer_annotate_contiguous_container(_First, _End, _Old_last, _New_last);
	}
#endif
}
/*
https://github.com/microsoft/STL/blob/a357ff1750d3f6dffd54b10d537e93e0accfcc92/stl/inc/xstring#L617
*/
template <typename elem, typename traits, typename alloc>
inline _CONSTEXPR20 void msvc_stl_xstring_Apply_annotation(typename ::std::basic_string<elem, traits, alloc>::value_type const *const _First,
														   typename ::std::basic_string<elem, traits, alloc>::size_type const _Capacity,
														   typename ::std::basic_string<elem, traits, alloc>::size_type const _Old_size,
														   typename ::std::basic_string<elem, traits, alloc>::size_type const _New_size) noexcept
{
#if _HAS_CXX20
	if (_STD is_constant_evaluated())
	{
		return;
	}
#endif // _HAS_CXX20
	using model_t = model<elem, traits, alloc>;
	using _Scary_val = typename model_t::_Scary_val;
	using size_type = typename ::std::basic_string<elem, traits, alloc>::size_type;
	constexpr size_type _Small_string_capacity = _Scary_val::_BUF_SIZE - 1;

	// Don't annotate small strings; only annotate on the heap.
	if (_Capacity <= _Small_string_capacity || !::fast_io::details::string_hack::msvc_stl_xstring_get_asan_string_should_annotate())
	{
		return;
	}

	// Note that `_Capacity`, `_Old_size`, and `_New_size` do not include the null terminator
	void const *const _End = _First + _Capacity + 1;
	void const *const _Old_last = _First + _Old_size + 1;
	void const *const _New_last = _First + _New_size + 1;

	constexpr bool _Large_string_always_asan_aligned =
		(_STD _Container_allocation_minimum_asan_alignment<::std::basic_string<elem, traits, alloc>>) >= _STD _Asan_granularity;

	// for the non-aligned buffer options, the buffer must always have size >= 9 bytes,
	// so it will always end at least one shadow memory section.

	_STD _Asan_aligned_pointers _Aligned;
	if constexpr (_Large_string_always_asan_aligned)
	{
		_Aligned = {_First, _STD _Get_asan_aligned_after(_End)};
	}
	else
	{
		_Aligned = _STD _Get_asan_aligned_first_end(_First, _End);
	}
	void const *const _Old_fixed = _Aligned._Clamp_to_end(_Old_last);
	void const *const _New_fixed = _Aligned._Clamp_to_end(_New_last);

	// --- always aligned case ---
	// old state:
	//   [_First, _Old_last) valid
	//   [_Old_last, asan_aligned_after(_End)) poison
	// new state:
	//   [_First, _New_last) valid
	//   [_New_last, asan_aligned_after(_End)) poison

	// --- sometimes non-aligned case ---
	// old state:
	//   [_Aligned._First, _Old_fixed) valid
	//   [_Old_fixed, _Aligned._End) poison
	//   [_Aligned._End, _End) valid
	// new state:
	//   [_Aligned._First, _New_fixed) valid
	//   [_New_fixed, _Aligned._End) poison
	//   [_Aligned._End, _End) valid
	::fast_io::details::string_hack::msvc_stl_sanitizer_annotate_contiguous_container(_Aligned._First, _Aligned._End, _Old_fixed, _New_fixed);
}

template <typename elem, typename traits, typename alloc>
inline _CONSTEXPR20 void msvc_stl_xstring_Modify_annotation(::std::basic_string<elem, traits, alloc> &str,
															typename ::std::basic_string<elem, traits, alloc>::size_type const _Old_size,
															typename ::std::basic_string<elem, traits, alloc>::size_type const _New_size) noexcept
{
	if (_Old_size == _New_size)
	{
		return;
	}
	decltype(auto) _My_data{::fast_io::details::string_hack::hack_scary_val(str)};
	::fast_io::details::string_hack::msvc_stl_xstring_Apply_annotation<elem, traits, alloc>(_My_data._Myptr(), _My_data._Myres, _Old_size, _New_size);
}

template <typename T>
inline constexpr void set_end_ptr(T &str, typename T::value_type *ptr) noexcept
{
	decltype(auto) scv{hack_scary_val(str)};
	::std::size_t newsize{static_cast<::std::size_t>(ptr - str.data())};
	if constexpr (::fast_io::details::string_hack::msvc_stl_xstring_activate_string_annotation)
	{
		::fast_io::details::string_hack::msvc_stl_xstring_Modify_annotation(str, scv._Mysize, newsize);
	}
	scv._Mysize = newsize;
}

template <typename T>
inline constexpr ::std::size_t local_capacity() noexcept
{
	using model_t = model<typename T::value_type, typename T::traits_type, typename T::allocator_type>;
	using _Scary_val = typename model_t::_Scary_val;
	return _Scary_val::_BUF_SIZE - 1;
}

} // namespace fast_io::details::string_hack
