#pragma once

/*
https://github.com/llvm-mirror/libcxx/blob/78d6a7767ed57b50122a161b91f59f19c9bd0d19/include/string#L703

*/
/*
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#endif
*/
namespace fast_io::details::string_hack
{

template<class _CharT, class _Traits, class _Allocator>
struct
#if __has_cpp_attribute(gnu::may_alias)
[[gnu::may_alias]]
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
        size_type __cap_;
    };

#ifdef _LIBCPP_BIG_ENDIAN
    static const size_type __short_mask = 0x01;
    static const size_type __long_mask  = 0x1ul;
#else  // _LIBCPP_BIG_ENDIAN
    static const size_type __short_mask = 0x80;
    static const size_type __long_mask  = ~(size_type(~0) >> 1);
#endif  // _LIBCPP_BIG_ENDIAN

    enum {__min_cap = (sizeof(__long) - 1)/sizeof(value_type) > 2 ?
                      (sizeof(__long) - 1)/sizeof(value_type) : 2};

    struct __short
    {
        value_type __data_[__min_cap];
        struct __pad
            : std::__padding<value_type>
        {
            unsigned char __size_;
        }pad;
    };

#else

    struct __long
    {
        size_type __cap_;
        size_type __size_;
        pointer   __data_;
    };

#ifdef _LIBCPP_BIG_ENDIAN
    static const size_type __short_mask = 0x80;
    static const size_type __long_mask  = ~(size_type(~0) >> 1);
#else  // _LIBCPP_BIG_ENDIAN
    static const size_type __short_mask = 0x01;
    static const size_type __long_mask  = 0x1ul;
#endif  // _LIBCPP_BIG_ENDIAN

    enum {__min_cap = (sizeof(__long) - 1)/sizeof(value_type) > 2 ?
                      (sizeof(__long) - 1)/sizeof(value_type) : 2};

    struct __short
    {
        union
        {
            unsigned char __size_;
            value_type __lx;
        };
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
#if __has_cpp_attribute(gnu::may_alias)
[[gnu::may_alias]]
#endif
    = typename ::std::__compressed_pair<__rep, alloc>*;
	return reinterpret_cast<alias_pointer>(reinterpret_cast<std::byte*>(__builtin_addressof(str))+__builtin_offsetof(model_t,__r_))->first();
}

template<typename elem,typename traits,typename alloc>
inline bool is_long(std::basic_string<elem,traits,alloc>& str) noexcept
{
	using model_t = model<elem,traits,alloc>;
	decltype(auto) __r_{hack_rep(str)};
	{return bool(__r_.__s.__size_ & model_t::__short_mask);}
}

template<typename elem,typename traits,typename alloc>
inline void set_size(std::basic_string<elem,traits,alloc>& str,typename std::basic_string<elem,traits,alloc>::size_type s) noexcept
{
	decltype(auto) __r_{hack_rep(str)};
	if (is_long(str))
		__r_.__l.__size_=s;
	else
	{
#ifdef _LIBCPP_ABI_ALTERNATE_STRING_LAYOUT
#ifdef _LIBCPP_BIG_ENDIAN
        	__r_.__s.__size_ = (unsigned char)(s << 1);
#else
	        __r_.__s.__size_ = (unsigned char)(s);
#endif
#else
#ifdef _LIBCPP_BIG_ENDIAN
        	__r_.__s.__size_ = (unsigned char)(s);
#else
	        __r_.__s.__size_ = (unsigned char)(s << 1);
#endif
#endif
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
