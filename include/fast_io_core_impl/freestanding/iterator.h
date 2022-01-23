#pragma once

#if __STDC_HOSTED__==1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED==1)

#ifdef __GLIBCXX__

#if __has_include(<bits/ranges_base.h>) && __has_include(<bits/stl_iterator.h>)
#include<bits/stl_iterator.h>
#include<bits/ranges_base.h>
#else
#include<iterator>
#endif
#else
#include<iterator>
#endif

namespace fast_io::freestanding
{
using ::std::input_or_output_iterator;
using ::std::input_iterator;
using ::std::forward_iterator;
using ::std::bidirectional_iterator;
using ::std::random_access_iterator;
using ::std::contiguous_iterator;
using ::std::iterator_traits;
using ::std::input_iterator_tag;
using ::std::forward_iterator_tag;
using ::std::bidirectional_iterator_tag;
using ::std::random_access_iterator_tag;
using ::std::contiguous_iterator_tag;

using ::std::iter_value_t;
using ::std::iter_difference_t;
using ::std::iter_rvalue_reference_t;

using ::std::default_sentinel_t;
using ::std::default_sentinel;

}

#else

namespace fast_io::freestanding
{

struct input_iterator_tag { };
struct forward_iterator_tag : public input_iterator_tag { };
struct bidirectional_iterator_tag : public forward_iterator_tag { };
struct random_access_iterator_tag : public bidirectional_iterator_tag { };
struct contiguous_iterator_tag: public random_access_iterator_tag { };

template<typename>
struct iterator_traits;

template<typename T >
struct iterator_traits<T*>
{
	using difference_type = std::ptrdiff_t;
	using value_type = std::remove_cv_t<T>;
	using pointer = T*;
	using reference = T&;
	using iterator_category = random_access_iterator_tag;
	using iterator_concept = contiguous_iterator_tag;
};

/*
naive implementation just to make compilation works
*/

template<typename Iter>
concept input_or_output_iterator = requires(Iter i)
{ 
	*i;
	++i;
};

template<typename Iter>
concept input_iterator = input_or_output_iterator<Iter>;
template<typename Iter>
concept forward_iterator = input_or_output_iterator<Iter>;
template<typename Iter>
concept bidirectional_iterator = forward_iterator<Iter>&&requires(Iter i)
{
	{--i} -> std::same_as<Iter&>;
	{i--} -> std::same_as<Iter>;
};

template<typename T>
using iter_value_t = typename iterator_traits<std::remove_cvref_t<T>>::value_type;

template<typename T>
using iter_reference_t = decltype(*std::declval<T&>());

template<typename T>
using iter_difference_t = typename iterator_traits<std::remove_cvref_t<T>>::difference_type;

template<typename Iter>
concept random_access_iterator = bidirectional_iterator<Iter>&&
	requires(Iter i,Iter const j,iter_difference_t<Iter> const n)
{
	{ i += n } -> std::same_as<Iter&>;
	{ j +  n } -> std::same_as<Iter>;
	{ n +  j } -> std::same_as<Iter>;
	{ i -= n } -> std::same_as<Iter&>;
	{ j -  n } -> std::same_as<Iter>;
	{  j[n]  } -> std::same_as<iter_reference_t<Iter>>;
};

template<typename Iter>
concept contiguous_iterator = random_access_iterator<Iter>&&requires(Iter iter)
{
	{ to_address(iter) } -> std::same_as<std::add_pointer_t<iter_reference_t<Iter>>>;
};

struct default_sentinel_t { };
inline constexpr default_sentinel_t default_sentinel{};

}

#endif

namespace fast_io::freestanding
{

template<input_or_output_iterator Iter>
constexpr iter_difference_t<Iter> distance(Iter first, Iter last) noexcept
{
	if constexpr(random_access_iterator<Iter>)
		return last-first;
	else
	{
		iter_difference_t<Iter> result{};
		for (;first != last;++first)
			++result;
		return result;
	}
}
}