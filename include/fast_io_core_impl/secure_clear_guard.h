#pragma once

namespace fast_io
{

/*
Based on security paper Dead Store Elimination (Still) Considered Harmful

https://www.usenix.org/system/files/conference/usenixsecurity17/sec17-yang.pdf

https://www.usenix.org/sites/default/files/conference/protected-files/usenixsecurity17_slides_zhaomo_yang.pdf

Existing Technique: Using memory barrier
GCC supports a memory barrier expressed using an inline assembly statement.
According to GCC’s documentation, the clobber argument "memory" tells the
compiler that the inline assembly statement may read or write memory that is not
specified in the input or output arguments.

Used in: zap from Kerberos, memzero_explicit from Linux.
Availability: GCC and Clang.
Effectiveness: effective
Not effective on Clang:
	std::memset(data,0,size);
	__asm__ __volatile__("" ::: "memory");


Effective on Clang
	std::memset(data,0,size);
	__asm__ __volatile__("" ::"r"(data): "memory");

How difficult to create a reliable scrubbing function
However, it does not work with Clang.
A more reliable and portable memory barrier is shown below (which is also used
used in memzero_explicit):


*/

inline
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
constexpr
#endif
void none_secure_clear(std::byte* data,std::size_t size) noexcept
{
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L
	if (__builtin_is_constant_evaluated())
#endif
	{
		for(std::size_t i{};i!=size;++i)
			data[i]=std::byte{};
	}
	else
#endif
	{
#if defined(__has_builtin)
#if __has_builtin(__builtin_memset)
		__builtin_memset(data,0,size);
#else
		::std::memset(data,0,size);
#endif
#else
		::std::memset(data,0,size);
#endif
	}
}

inline void secure_clear(void* data,std::size_t size) noexcept
{
#if defined(_MSC_VER) && !defined(__clang__)
/*
Referenced from Microsoft's winnt.h.
forceinline for this is nonsense
*/
	
#if defined(_M_AMD64)
	__stosb(__builtin_bit_cast(unsigned char*,data), 0, size);
#else
	volatile char *vptr = (volatile char *)data;
	while (size)
	{
#if !defined(_M_CEE) && (defined(_M_ARM) || defined(_M_ARM64))
		__iso_volatile_store8(vptr, 0);
#else
		*vptr = 0;
#endif
		++vptr;
		--size;
	}
#endif // _M_AMD64
#else
/*
https://github.com/bminor/glibc/blob/master/string/explicit_bzero.c
Referenced from glibc

*/
	details::my_memset(data,0,size);
	__asm__ __volatile__("" ::"r"(data): "memory");
#endif
}

namespace freestanding
{


inline constexpr ::std::byte* bytes_secure_clear_n(::std::byte* data,std::size_t size) noexcept
{
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L
	if (__builtin_is_constant_evaluated())
#endif
	{
		for(std::size_t i{};i!=size;++i)
			data[i]=std::byte{};
	}
	else
#endif
	{
#if defined(_MSC_VER) && !defined(__clang__)
/*
Referenced from Microsoft's winnt.h.
forceinline for this is nonsense
*/
	
#if defined(_M_AMD64)
	__stosb(__builtin_bit_cast(unsigned char*,data), 0, size);
#else
	volatile char *vptr = (volatile char *)data;
	while (size)
	{
#if !defined(_M_CEE) && (defined(_M_ARM) || defined(_M_ARM64))
		__iso_volatile_store8(vptr, 0);
#else
		*vptr = 0;
#endif
		++vptr;
		--size;
	}
#endif // _M_AMD64
#else
/*
https://github.com/bminor/glibc/blob/master/string/explicit_bzero.c
Referenced from glibc
*/
#if defined(__has_builtin)
#if __has_builtin(__builtin_memset)
		__builtin_memset(data,0,size);
#else
		::std::memset(data,0,size);
#endif
#else
		::std::memset(data,0,size);
#endif
		__asm__ __volatile__("" ::"r"(data): "memory");
#endif
	}
	return data+size;
}

inline constexpr ::std::byte* bytes_secure_clear(::std::byte* first,::std::byte const* last) noexcept
{
	return bytes_secure_clear_n(first,static_cast<std::size_t>(last-first));
}

}

template<typename T>
class secure_clear_guard
{
public:
	T* region;
	std::size_t count;
	secure_clear_guard(T* rg,std::size_t bts):region(rg),count(bts){}
	secure_clear_guard(secure_clear_guard const&)=delete;
	secure_clear_guard& operator=(secure_clear_guard const&)=delete;
	~secure_clear_guard()
	{
		secure_clear(region,count*sizeof(T));
	}
};

template<typename T>
class secure_clear_no_op
{
public:
	constexpr secure_clear_no_op(T*,std::size_t){};
};

template<typename T,bool condition>
using conditional_secure_clear_guard=std::conditional_t<condition,secure_clear_guard<T>,secure_clear_no_op<T>>;

}