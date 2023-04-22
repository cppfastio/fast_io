#pragma once

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

template<::std::input_iterator Iter,typename predicate>
inline constexpr Iter find_if(Iter first,Iter last,predicate q)
{
	for(;first!=last;++first)
		if(q(*first))
			return first;
	return last;
}

template<::std::input_iterator input_iter,::std::input_or_output_iterator output_iter>
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

template<::std::input_iterator input_iter,::std::input_or_output_iterator output_iter>
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


template<::std::input_iterator fwd_iter,typename T>
inline constexpr void fill(fwd_iter first,fwd_iter last,T value)
{
	for(;first!=last;++first)
		*first=value;
}


template<::std::input_iterator fwd_iter,typename T>
inline constexpr void fill_n(fwd_iter first,std::size_t n,T value)
{
	for(std::size_t i{};i!=n;++i)
	{
		*first=value;
		++first;
	}
}


template<::std::bidirectional_iterator BidirIt1,::std::bidirectional_iterator BidirIt2 >
constexpr BidirIt2 copy_backward(BidirIt1 first, BidirIt1 last, BidirIt2 d_last)
{
	for(;first!=last;*(--d_last)=*(--last));
	return d_last;
}

}

#endif

namespace fast_io::freestanding
{

template<::std::input_iterator Iter,::std::integral T>
//requires (std::is_trivially_copyable_v<T>&&sizeof(T)<=sizeof(std::uintmax_t))
requires (::std::is_trivially_copyable_v<T>)
inline constexpr Iter find(Iter first,Iter last,T t)
{
	for(;first!=last;++first)
		if(*first==t)
			return first;
	return last;
}

template<::std::input_iterator Iter,::std::integral T>
//requires (std::is_trivially_copyable_v<T>&&sizeof(T)<=sizeof(std::uintmax_t))
requires (::std::is_trivially_copyable_v<T>)
inline constexpr Iter find_not(Iter first,Iter last,T t)
{
	for(;first!=last;++first)
		if(*first!=t)
			return first;
	return last;
}

template<::std::input_iterator Iter,::std::input_iterator Iter2>
struct mismatch_result
{
	Iter first;
	Iter2 second;
};

template<::std::input_iterator Iter,::std::input_iterator Iter2>
inline constexpr mismatch_result<Iter,Iter2> my_mismatch(Iter first1,Iter last1,Iter2 first2,Iter2 last2)
{
	for(;first1!=last1&&first2!=last2&&*first1==*first2;++first2)
		++first1;
	return {first1,first2};
}
template<::std::forward_iterator ForwardIt,class T >
requires (::std::is_trivially_copyable_v<T>)
inline constexpr ForwardIt remove(ForwardIt first, ForwardIt last, T value)
{
	first = find(first, last, value);
	if(first != last)
		for(ForwardIt i = first; ++i != last; )
			if (!(*i == value))
			{
				*first = ::std::move(*i);
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

inline
#if defined(__has_builtin)
#if __has_builtin(__builtin_memcpy)
constexpr
#endif
#endif
int my_memcmp(void const* dest, void const* src, std::size_t count) noexcept
{
	return
#if defined(__has_builtin)
#if __has_builtin(__builtin_memcmp)
		__builtin_memcmp
#else
		std::memcmp
#endif
#else
		std::memcmp
#endif
		(dest, src, count);
}

template<::std::input_iterator input_iter,::std::input_or_output_iterator output_iter>
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
		using input_value_type = ::std::iter_value_t<input_iter>;
		using output_value_type = ::std::iter_value_t<output_iter>;
		if constexpr
			(::std::contiguous_iterator<input_iter> &&
				::std::contiguous_iterator<output_iter> &&
				std::is_trivially_copyable_v<input_value_type> &&
				std::is_trivially_copyable_v<output_value_type> &&
				(std::same_as<input_value_type, output_value_type> ||
					(std::integral<input_value_type> && std::integral<output_value_type> &&
						sizeof(input_value_type) == sizeof(output_value_type))))
		{
			if (count)	//to avoid nullptr UB
				my_memcpy(::std::to_address(result), ::std::to_address(first), sizeof(::std::iter_value_t<input_iter>) * count);
			return result += count;
		}
		else
			return ::fast_io::freestanding::copy_n(first, count, result);
	}
}

template<::std::input_iterator input_iter,::std::input_or_output_iterator output_iter>
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
		using input_value_type = ::std::iter_value_t<input_iter>;
		using output_value_type = ::std::iter_value_t<output_iter>;
		if constexpr
			(::std::contiguous_iterator<input_iter> &&
				::std::contiguous_iterator<output_iter> &&
				std::is_trivially_copyable_v<input_value_type> &&
				std::is_trivially_copyable_v<output_value_type> &&
				(std::same_as<input_value_type, output_value_type> ||
					(std::integral<input_value_type> && std::integral<output_value_type> &&
						sizeof(input_value_type) == sizeof(output_value_type))))
		{
			std::size_t count{ static_cast<std::size_t>(last - first) };
			if (count)	//to avoid nullptr UB
				my_memcpy(::std::to_address(result), ::std::to_address(first), sizeof(::std::iter_value_t<input_iter>) * count);
			return result += count;
		}
		else
		{
			return ::fast_io::freestanding::copy(first, last, result);
		}
	}
}

template<::std::input_iterator input_iter,::std::input_or_output_iterator output_iter>
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
		using input_value_type = ::std::iter_value_t<input_iter>;
		using output_value_type = ::std::iter_value_t<output_iter>;
		if constexpr
			(::std::contiguous_iterator<input_iter> &&
				::std::contiguous_iterator<output_iter> &&
				std::is_trivially_copyable_v<input_value_type> &&
				std::is_trivially_copyable_v<output_value_type> &&
				(std::same_as<input_value_type, output_value_type> ||
					(std::integral<input_value_type> && std::integral<output_value_type> &&
						sizeof(input_value_type) == sizeof(output_value_type))))
		{
			if (count)	//to avoid nullptr UB
				my_memmove(::std::to_address(result), ::std::to_address(first), sizeof(::std::iter_value_t<input_iter>) * count);
			return result += count;
		}
		else
		{
			return ::fast_io::freestanding::copy_n(first, count, result);
		}
	}
}

template<::std::input_iterator input_iter,::std::input_or_output_iterator output_iter>
inline constexpr output_iter my_copy(input_iter first, input_iter second, output_iter result)
{
	using input_value_type = ::std::iter_value_t<input_iter>;
	using output_value_type = ::std::iter_value_t<output_iter>;
	if constexpr (::std::contiguous_iterator<input_iter> && ::std::contiguous_iterator<output_iter> && std::is_trivially_copyable_v<input_value_type> &&
		std::is_trivially_copyable_v<output_value_type> &&
		(std::same_as<input_value_type, output_value_type> || (std::integral<input_value_type> && std::integral<output_value_type> && sizeof(std::is_trivially_copyable_v<input_value_type>) == sizeof(std::is_trivially_copyable_v<output_value_type>))))
	{
		my_copy_n(first, static_cast<std::size_t>(second - first), result);
		return result + (second - first);
	}
	else
		return ::fast_io::freestanding::copy(first, second, result);
}

template<::std::bidirectional_iterator input_iter,::std::bidirectional_iterator output_iter>
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
		using input_value_type = typename ::std::iter_value_t<input_iter>;
		using output_value_type = typename ::std::iter_value_t<output_iter>;
		if constexpr
			(::std::contiguous_iterator<input_iter> &&
				::std::contiguous_iterator<output_iter> &&
				std::is_trivially_copyable_v<input_value_type> &&
				std::is_trivially_copyable_v<output_value_type> &&
				(std::same_as<input_value_type, output_value_type> ||
					(std::integral<input_value_type> && std::integral<output_value_type> &&
						sizeof(input_value_type) == sizeof(output_value_type))))
		{
			std::size_t const count(last - first);
			d_last -= count;
			if (count)	//to avoid nullptr UB
				my_memmove(::std::to_address(d_last), ::std::to_address(first), sizeof(input_value_type) * count);
			return d_last;
		}
		else
			return ::fast_io::freestanding::copy_backward(first, last, d_last);
	}
}

template<::std::random_access_iterator input_iter,::std::random_access_iterator output_iter>
inline constexpr bool my_compare_iter_n(input_iter first,std::size_t n,output_iter outier) noexcept
{
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if (__builtin_is_constant_evaluated())
#endif
	{
		for(auto last{first+n};first!=last;++first)
		{
			if(*first!=*outier)
			{
				return false;
			}
			++outier;
		}
		return true;
	}
	else
#endif
	{
		using input_value_type = typename ::std::iter_value_t<input_iter>;
		using output_value_type = typename ::std::iter_value_t<output_iter>;
		if constexpr
		(::std::contiguous_iterator<input_iter> &&
			::std::contiguous_iterator<output_iter> &&
			std::is_trivially_copyable_v<input_value_type> &&
			std::is_trivially_copyable_v<output_value_type> &&
			(std::same_as<input_value_type, output_value_type> ||
				(std::integral<input_value_type> && std::integral<output_value_type> &&
					sizeof(input_value_type) == sizeof(output_value_type))))
		{
			return my_memcmp(std::to_address(first),std::to_address(outier),n)==0;
		}
		else
		{
			for(auto last{first+n};first!=last;++first)
			{
				if(*first!=outier)
				{
					return false;
				}
				++outier;
			}
			return true;
		}
	}
}


template<::std::input_iterator InputIt1,::std::input_iterator InputIt2>
inline constexpr bool lexicographical_compare(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
{
	for (; (first1 != last1) && (first2 != last2); ++first2)
	{
		if (*first1 < *first2)
			return true;
		if (*first2 < *first1)
			return false;
		++first1;
	}
	return (first1 == last1) && (first2 != last2);
}

template<::std::input_iterator InputIt1,::std::input_iterator InputIt2>
inline constexpr bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
{
	for(;first1!=last1&&first2!=last2&&*first1==*first2;++first2)
	{
		++first1;
	}
	return first1==last1&&first2==last2;
}

template<std::forward_iterator ForwardIt, class T, class Compare>
inline constexpr ForwardIt lower_bound(ForwardIt first, ForwardIt last, T const& value, Compare comp)
{
	ForwardIt it;
	typename ::std::iterator_traits<ForwardIt>::difference_type count, step;
	count = last-first;

	while (count > 0)
	{
		it = first;
		step = count / 2;
		it+= step;

		if (comp(*it, value))
		{
			first = ++it;
			count -= step + 1;
		}
		else
			count = step;
	}
	return first;
}

template<::std::input_iterator InputIt, ::std::forward_iterator NoThrowForwardIt>
constexpr NoThrowForwardIt uninitialized_copy(InputIt first, InputIt last, NoThrowForwardIt d_first)
{
    using T = typename std::iterator_traits<NoThrowForwardIt>::value_type;
	struct destroyer
	{
		NoThrowForwardIt d_first;
		NoThrowForwardIt current;
		constexpr
			~destroyer()
		{
			for (;d_first != current;++d_first)
			{
				d_first->~T();
			}
		}
	};
	using input_iter = InputIt;
	using output_iter = NoThrowForwardIt;
	using input_value_type = ::std::iter_value_t<input_iter>;
	using output_value_type = ::std::iter_value_t<output_iter>;
	if constexpr
		(::std::contiguous_iterator<input_iter> &&
			::std::contiguous_iterator<output_iter> &&
			std::is_trivially_copyable_v<input_value_type> &&
			std::is_trivially_copyable_v<output_value_type> &&
			(std::same_as<input_value_type, output_value_type> ||
				(std::integral<input_value_type> && std::integral<output_value_type> &&
					sizeof(input_value_type) == sizeof(output_value_type))))
	{
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
		if !consteval
#else
		if (!__builtin_is_constant_evaluated())
#endif
		{
			std::size_t count{ static_cast<std::size_t>(last - first) };
			if (count)	//to avoid nullptr UB
				my_memmove(::std::to_address(d_first), ::std::to_address(first), sizeof(input_value_type) * count);
			return d_first += count;
		}
#endif
	}
	destroyer d{d_first,d_first};
	for(;first!=last;++first)
	{
		::new (__builtin_addressof(*d.current)) T(*first);
		++d.current;
	}
	NoThrowForwardIt current = d.current;
	d.current = d.d_first;
	return current;
}

}

namespace fast_io::details
{
using ::fast_io::freestanding::my_memcpy;
using ::fast_io::freestanding::my_memmove;
using ::fast_io::freestanding::my_memset;
using ::fast_io::freestanding::non_overlapped_copy_n;
using ::fast_io::freestanding::non_overlapped_copy;
using ::fast_io::freestanding::my_copy;
using ::fast_io::freestanding::my_copy_backward;
using ::fast_io::freestanding::my_copy_n;

}

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#endif
