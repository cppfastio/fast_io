#pragma once

namespace fast_io::details
{
#if __has_cpp_attribute(gnu::always_inline)
[[gnu::always_inline]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::std::byte* bytes_copy_naive_n_impl(::std::byte const* first,std::size_t n,::std::byte* dest) noexcept
{
	for(std::size_t i{};i!=n;++i)
	{
		dest[i]=first[i];
	}
	return dest+n;
}

}

namespace fast_io::freestanding
{
#if __has_cpp_attribute(gnu::flatten)
[[gnu::flatten]]
#endif
inline constexpr ::std::byte* bytes_copy_n(::std::byte const* first,::std::size_t n,::std::byte* dest) noexcept
{
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L
	if (__builtin_is_constant_evaluated())
#endif
	{
		return ::fast_io::details::bytes_copy_naive_n_impl(first,n,dest);
	}
	else
#endif
	{
		if(n)
#if __has_cpp_attribute(likely)
		[[likely]]
#endif
		{
#if !defined(__has_builtin)
			::std::memmove(dest,first,n);
#elif __has_builtin(__builtin_memmove)
			__builtin_memmove(dest,first,n);
#else
			::std::memmove(dest,first,n);
#endif
		}
		return dest+n;
	}
}

inline constexpr ::std::byte* nonoverlapped_bytes_copy_n(::std::byte const* __restrict first,::std::size_t n,::std::byte* __restrict dest) noexcept
{
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L
	if (__builtin_is_constant_evaluated())
#endif
	{
		return ::fast_io::details::bytes_copy_naive_n_impl(first,n,dest);
	}
	else
#endif
	{
		if(n)
#if __has_cpp_attribute(likely)
		[[likely]]
#endif
		{
#if !defined(__has_builtin)
			::std::memcpy(dest,first,n);
#elif __has_builtin(__builtin_memcpy)
			__builtin_memcpy(dest,first,n);
#else
			::std::memcpy(dest,first,n);
#endif
		}
	}
	return dest+n;
}

inline constexpr ::std::byte* nonoverlapped_bytes_copy(::std::byte const* first,::std::byte const* last,::std::byte* __restrict dest) noexcept
{
	return nonoverlapped_bytes_copy_n(first,static_cast<std::size_t>(last-first),dest);
}

inline constexpr ::std::byte* bytes_clear_n(::std::byte* data,std::size_t size) noexcept
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
	return data+size;
}

inline constexpr ::std::byte* bytes_clear(::std::byte* first,::std::byte* last) noexcept
{
	return bytes_clear_n(first,static_cast<std::size_t>(last-first));
}

inline constexpr ::std::byte* bytes_fill_n(::std::byte* data,::std::size_t size,std::byte val) noexcept
{
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L
	if (__builtin_is_constant_evaluated())
#endif
	{
		for(std::size_t i{};i!=size;++i)
			data[i]=val;
	}
	else
#endif
	{
#if defined(__has_builtin)
#if __has_builtin(__builtin_memset)
		__builtin_memset(data,static_cast<char unsigned>(val),size);
#else
		::std::memset(data,static_cast<char unsigned>(val),size);
#endif
#else
		::std::memset(data,static_cast<char unsigned>(val),size);
#endif
	}
	return data+size;
}

inline constexpr ::std::byte* bytes_fill(::std::byte* first,::std::byte* last,std::byte val) noexcept
{
	return bytes_fill_n(first,static_cast<std::size_t>(last-first),val);
}

}
