#pragma once
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#endif
/*
https://github.com/llvm-mirror/libcxx/blob/78d6a7767ed57b50122a161b91f59f19c9bd0d19/include/string#L703

*/

namespace fast_io::details::string_hack
{

template<class _CharT, class _Traits, class _Allocator>
struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
model
{
    typedef _Traits                                      traits_type;
    typedef _CharT                                       value_type;
    typedef _Allocator                                   allocator_type;
    typedef std::allocator_traits<allocator_type>             __alloc_traits;
    typedef typename __alloc_traits::size_type           size_type;
    typedef typename __alloc_traits::difference_type     difference_type;
    typedef value_type&                                  reference;
    typedef value_type const&                            const_reference;
    typedef typename __alloc_traits::pointer             pointer;
    typedef typename __alloc_traits::const_pointer       const_pointer;
#ifdef _LIBCPP_ABI_ALTERNATE_STRING_LAYOUT
    struct __long
    {
        pointer   __data_;
        size_type __size_;
        size_type __cap_ : sizeof(size_type) * CHAR_BIT - 1;
        size_type __is_long_ : 1;
    };

    enum {__min_cap = (sizeof(__long) - 1)/sizeof(value_type) > 2 ?
                      (sizeof(__long) - 1)/sizeof(value_type) : 2};

    struct __short
    {
        value_type __data_[__min_cap];
        unsigned char __padding_[sizeof(value_type) - 1];
        unsigned char __size_ : 7;
        unsigned char __is_long_ : 1;
    };


#else

    struct __long
    {
        size_type __is_long_ : 1;
        size_type __cap_ : sizeof(size_type) * CHAR_BIT - 1;
        size_type __size_;
        pointer   __data_;
    };

    enum {__min_cap = (sizeof(__long) - 1)/sizeof(value_type) > 2 ?
                      (sizeof(__long) - 1)/sizeof(value_type) : 2};

    struct __short
    {
        unsigned char __is_long_ : 1;
        unsigned char __size_ : 7;
        char __padding_[sizeof(value_type) - 1];
        value_type __data_[__min_cap];
    };

#endif  // _LIBCPP_ABI_ALTERNATE_STRING_LAYOUT

    union __ulx{__long __lx; __short __lxx;};

    enum {__n_words = sizeof(__ulx) / sizeof(size_type)};

    struct __raw
    {
        size_type __words[__n_words];
    };

    struct __rep
    {
        union
        {
            __long  __l;
            __short __s;
            __raw   __r;
        };
    };
    ::std::__compressed_pair<__rep, allocator_type> __r_;
};

template<typename elem,typename traits,typename alloc>
inline decltype(auto) hack_rep(std::basic_string<elem,traits,alloc>& str) noexcept
{
	using model_t = model<elem,traits,alloc>;
	using __rep = typename model_t::__rep;
	using alias_pointer
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
	= typename ::std::__compressed_pair<__rep, alloc>*;
	return reinterpret_cast<alias_pointer>(reinterpret_cast<std::byte*>(__builtin_addressof(str))+__builtin_offsetof(model_t,__r_))->first();
}

template<typename elem,typename traits,typename alloc>
inline void set_size(std::basic_string<elem,traits,alloc>& str,typename std::basic_string<elem,traits,alloc>::size_type s) noexcept
{
	decltype(auto) __r_{hack_rep(str)};
	if (bool(__r_.__s.__is_long_))
		__r_.__l.__size_=s;
	else
	{
		__r_.__s.__size_ = (unsigned char)(s);
	}
}

template<typename T>
inline constexpr void set_end_ptr(T& str,typename T::value_type* ptr) noexcept
{
	set_size(str,static_cast<std::size_t>(ptr-str.data()));
}

template<typename T>
inline constexpr std::size_t local_capacity() noexcept
{
	using model_type = model<typename T::value_type,typename T::traits_type,typename T::allocator_type>;
	constexpr std::size_t mcapminus1{static_cast<std::size_t>(model_type::__min_cap-static_cast<std::size_t>(1u))};
	return mcapminus1;
}

}
