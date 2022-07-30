﻿#pragma once

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#endif
#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(push)
#pragma warning( disable : 4365 )
#endif

#if 0
//__STDC_HOSTED__==1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED==1)
namespace fast_io::freestanding
{
using ::std::find_if;
using ::std::copy_n;
using ::std::copy;
using ::std::copy_backward;
using ::std::fill;
using ::std::fill_n;
}

#else
namespace fast_io::freestanding
{

template<input_iterator Iter,typename predicate>
inline constexpr Iter find_if(Iter first,Iter last,predicate q)
{
	for(;first!=last;++first)
		if(q(*first))
			return first;
	return last;
}

template<input_iterator input_iter,input_or_output_iterator output_iter>
inline constexpr output_iter copy_n(input_iter first,std::size_t count,output_iter result)
{
	for (std::size_t i{}; i != count; ++i)
	{
		*result = *first;
		++first;
		++result;
	}
	return result;
}

template<::fast_io::freestanding::input_iterator input_iter,::fast_io::freestanding::input_or_output_iterator output_iter>
inline constexpr output_iter copy(input_iter first,input_iter last,output_iter result)
{
	for(;first!=last;)
	{
		*result=*first;
		++first;
		++result;
	}
	return result;
}


template<::fast_io::freestanding::input_iterator fwd_iter,typename T>
inline constexpr void fill(fwd_iter first,fwd_iter last,T value)
{
	for(;first!=last;++first)
		*first=value;
}


template<::fast_io::freestanding::input_iterator fwd_iter,typename T>
inline constexpr void fill_n(fwd_iter first,std::size_t n,T value)
{
	for(std::size_t i{};i!=n;++i)
	{
		*first=value;
		++first;
	}
}


template<::fast_io::freestanding::bidirectional_iterator BidirIt1, ::fast_io::freestanding::bidirectional_iterator BidirIt2 >
constexpr BidirIt2 copy_backward(BidirIt1 first, BidirIt1 last, BidirIt2 d_last)
{
	for(;first!=last;*(--d_last)=*(--last));
	return d_last;
}

}

#endif

namespace fast_io::freestanding
{

template<input_iterator Iter,std::integral T>
//requires (std::is_trivially_copyable_v<T>&&sizeof(T)<=sizeof(std::uintmax_t))
requires (std::is_trivially_copyable_v<T>)
inline constexpr Iter find(Iter first,Iter last,T t)
{
	for(;first!=last;++first)
		if(*first==t)
			return first;
	return last;
}

template<input_iterator Iter,input_iterator Iter2>
struct mismatch_result
{
	Iter first;
	Iter2 second;
};

template<input_iterator Iter,input_iterator Iter2>
inline constexpr mismatch_result<Iter,Iter2> my_mismatch(Iter first1,Iter last1,Iter2 first2,Iter2 last2)
{
	for(;first1!=last1&&first2!=last2&&*first1==*first2;++first2)
		++first1;
	return {first1,first2};
}
template<forward_iterator ForwardIt, class T >
requires (std::is_trivially_copyable_v<T>)
inline constexpr ForwardIt remove(ForwardIt first, ForwardIt last, T value)
{
	first = find(first, last, value);
	if(first != last)
		for(ForwardIt i = first; ++i != last; )
			if (!(*i == value))
			{
				*first = ::fast_io::freestanding::move(*i);
				++first;
			}
	return first;
}
}

namespace fast_io::freestanding
{

inline
#if defined(__has_builtin)
#if __has_builtin(__builtin_memcpy)
constexpr
#endif
#endif
void* my_memcpy(void* dest, void const* src, std::size_t count) noexcept
{
	return
#if defined(__has_builtin)
#if __has_builtin(__builtin_memcpy)
		__builtin_memcpy
#else
		std::memcpy
#endif
#else
		std::memcpy
#endif
		(dest, src, count);
}


inline
#if defined(__has_builtin)
#if __has_builtin(__builtin_memmove)
constexpr
#endif
#endif
void* my_memmove(void* dest, void const* src, std::size_t count) noexcept
{
	return
#if defined(__has_builtin)
#if __has_builtin(__builtin_memmove)
		__builtin_memmove
#else
		std::memmove
#endif
#else
		std::memmove
#endif
		(dest, src, count);
}

inline void* my_memset(void* dest, int ch, std::size_t count) noexcept
{
	return
#if defined(__has_builtin)
#if __has_builtin(__builtin_memset)
		__builtin_memset
#else
		std::memset
#endif
#else
		std::memset
#endif
		(dest, ch, count);
}



template<::fast_io::freestanding::input_iterator input_iter, ::fast_io::freestanding::input_or_output_iterator output_iter>
inline constexpr output_iter non_overlapped_copy_n(input_iter first, std::size_t count, output_iter result)
{
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if (__builtin_is_constant_evaluated())
#endif
	{
		return ::fast_io::freestanding::copy_n(first, count, result);
	}
	else
#endif
	{
		using input_value_type = ::fast_io::freestanding::iter_value_t<input_iter>;
		using output_value_type = ::fast_io::freestanding::iter_value_t<output_iter>;
		if constexpr
			(::fast_io::freestanding::contiguous_iterator<input_iter> &&
				::fast_io::freestanding::contiguous_iterator<output_iter> &&
				std::is_trivially_copyable_v<input_value_type> &&
				std::is_trivially_copyable_v<output_value_type> &&
				(std::same_as<input_value_type, output_value_type> ||
					(std::integral<input_value_type> && std::integral<output_value_type> &&
						sizeof(input_value_type) == sizeof(output_value_type))))
		{
			if (count)	//to avoid nullptr UB
				my_memcpy(::fast_io::freestanding::to_address(result), ::fast_io::freestanding::to_address(first), sizeof(::fast_io::freestanding::iter_value_t<input_iter>) * count);
			return result += count;
		}
		else
			return ::fast_io::freestanding::copy_n(first, count, result);
	}
}

template<::fast_io::freestanding::input_iterator input_iter, ::fast_io::freestanding::input_or_output_iterator output_iter>
inline constexpr output_iter non_overlapped_copy(input_iter first, input_iter last, output_iter result)
{
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if (__builtin_is_constant_evaluated())
#endif
	{
		return ::fast_io::freestanding::copy(first, last, result);
	}
	else
#endif
	{
		using input_value_type = ::fast_io::freestanding::iter_value_t<input_iter>;
		using output_value_type = ::fast_io::freestanding::iter_value_t<output_iter>;
		if constexpr
			(::fast_io::freestanding::contiguous_iterator<input_iter> &&
				::fast_io::freestanding::contiguous_iterator<output_iter> &&
				std::is_trivially_copyable_v<input_value_type> &&
				std::is_trivially_copyable_v<output_value_type> &&
				(std::same_as<input_value_type, output_value_type> ||
					(std::integral<input_value_type> && std::integral<output_value_type> &&
						sizeof(input_value_type) == sizeof(output_value_type))))
		{
			std::size_t count{ static_cast<std::size_t>(last - first) };
			if (count)	//to avoid nullptr UB
				my_memcpy(::fast_io::freestanding::to_address(result), ::fast_io::freestanding::to_address(first), sizeof(::fast_io::freestanding::iter_value_t<input_iter>) * count);
			return result += count;
		}
		else
		{
			return ::fast_io::freestanding::copy(first, last, result);
		}
	}
}

template<::fast_io::freestanding::input_iterator input_iter, ::fast_io::freestanding::input_or_output_iterator output_iter>
inline constexpr output_iter my_copy_n(input_iter first, std::size_t count, output_iter result)
{
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if (__builtin_is_constant_evaluated())
#endif
	{
		return ::fast_io::freestanding::copy_n(first, count, result);
	}
	else
#endif
	{
		using input_value_type = ::fast_io::freestanding::iter_value_t<input_iter>;
		using output_value_type = ::fast_io::freestanding::iter_value_t<output_iter>;
		if constexpr
			(::fast_io::freestanding::contiguous_iterator<input_iter> &&
				::fast_io::freestanding::contiguous_iterator<output_iter> &&
				std::is_trivially_copyable_v<input_value_type> &&
				std::is_trivially_copyable_v<output_value_type> &&
				(std::same_as<input_value_type, output_value_type> ||
					(std::integral<input_value_type> && std::integral<output_value_type> &&
						sizeof(input_value_type) == sizeof(output_value_type))))
		{
			if (count)	//to avoid nullptr UB
				my_memmove(::fast_io::freestanding::to_address(result), ::fast_io::freestanding::to_address(first), sizeof(::fast_io::freestanding::iter_value_t<input_iter>) * count);
			return result += count;
		}
		else
		{
			return ::fast_io::freestanding::copy_n(first, count, result);
		}
	}
}

template<::fast_io::freestanding::input_iterator input_iter, ::fast_io::freestanding::input_or_output_iterator output_iter>
inline constexpr output_iter my_copy(input_iter first, input_iter second, output_iter result)
{
	using input_value_type = ::fast_io::freestanding::iter_value_t<input_iter>;
	using output_value_type = ::fast_io::freestanding::iter_value_t<output_iter>;
	if constexpr (::fast_io::freestanding::contiguous_iterator<input_iter> && ::fast_io::freestanding::contiguous_iterator<output_iter> && std::is_trivially_copyable_v<input_value_type> &&
		std::is_trivially_copyable_v<output_value_type> &&
		(std::same_as<input_value_type, output_value_type> || (std::integral<input_value_type> && std::integral<output_value_type> && sizeof(std::is_trivially_copyable_v<input_value_type>) == sizeof(std::is_trivially_copyable_v<output_value_type>))))
	{
		my_copy_n(first, static_cast<std::size_t>(second - first), result);
		return result + (second - first);
	}
	else
		return ::fast_io::freestanding::copy(first, second, result);
}

template<::fast_io::freestanding::bidirectional_iterator input_iter, ::fast_io::freestanding::bidirectional_iterator output_iter>
inline constexpr output_iter my_copy_backward(input_iter first, input_iter last, output_iter d_last)
{
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if (__builtin_is_constant_evaluated())
#endif
	{
		return ::fast_io::freestanding::copy_backward(first, last, d_last);
	}
	else
#endif
	{
		using input_value_type = typename ::fast_io::freestanding::iter_value_t<input_iter>;
		using output_value_type = typename ::fast_io::freestanding::iter_value_t<output_iter>;
		if constexpr
			(::fast_io::freestanding::contiguous_iterator<input_iter> &&
				::fast_io::freestanding::contiguous_iterator<output_iter> &&
				std::is_trivially_copyable_v<input_value_type> &&
				std::is_trivially_copyable_v<output_value_type> &&
				(std::same_as<input_value_type, output_value_type> ||
					(std::integral<input_value_type> && std::integral<output_value_type> &&
						sizeof(input_value_type) == sizeof(output_value_type))))
		{
			std::size_t const count(last - first);
			d_last -= count;
			if (count)	//to avoid nullptr UB
				my_memmove(::fast_io::freestanding::to_address(d_last), ::fast_io::freestanding::to_address(first), sizeof(input_value_type) * count);
			return d_last;
		}
		else
			return ::fast_io::freestanding::copy_backward(first, last, d_last);
	}
}

}

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#endif
