#pragma once

namespace fast_io::details::string_hack
{

/*
https://github.com/gcc-mirror/gcc/blob/47fe96341d28ff9e68990038b1beb8a760ff26d0/libstdc%2B%2B-v3/include/bits/basic_string.h#L150
*/

template<typename T>
struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
model
{
using value_type = typename T::value_type;
using size_type = typename T::size_type;
using pointer = typename T::pointer;
using const_pointer = typename T::const_pointer;
using allocator_type = typename T::allocator_type;
struct _Alloc_hider : allocator_type // TODO check __is_final
{
#if __cplusplus < 201103L
_Alloc_hider(pointer __dat, allocator_type const& __a = allocator_type())
: allocator_type(__a), _M_p(__dat) { }
#else
_Alloc_hider(pointer __dat, allocator_type const& __a)
: allocator_type(__a), _M_p(__dat) { }

_Alloc_hider(pointer __dat, allocator_type&& __a = allocator_type())
: allocator_type(::std::move(__a)), _M_p(__dat) { }
#endif

pointer _M_p; // The actual data.
};

_Alloc_hider	_M_dataplus;
size_type		_M_string_length;

enum { _S_local_capacity = 15 / sizeof(value_type) };

union
{
value_type        _M_local_buf[_S_local_capacity + 1];
size_type        _M_allocated_capacity;
};

};


template<typename T>
inline decltype(auto) hack_M_data(T& str) noexcept
{
	using model_t = model<T>;
	using alloc_hider = typename model_t::_Alloc_hider;
	using pointer = typename T::pointer;
	return *reinterpret_cast<pointer*>(reinterpret_cast<std::byte*>(__builtin_addressof(str))+__builtin_offsetof(model_t,_M_dataplus)+__builtin_offsetof(alloc_hider,_M_p));
}

template<typename T>
inline decltype(auto) hack_M_string_length(T& str) noexcept
{
	using model_t = model<T>;
	using size_type = typename T::size_type;
	return *reinterpret_cast<size_type*>(reinterpret_cast<std::byte*>(__builtin_addressof(str))+__builtin_offsetof(model_t,_M_string_length));
}

template<typename T>
inline constexpr void set_end_ptr(T& str,typename T::value_type* ptr) noexcept
{
	hack_M_string_length(str)=static_cast<std::size_t>(ptr-::std::to_address(hack_M_data(str)));
}

template<typename T>
inline constexpr std::size_t local_capacity() noexcept
{
	using model_type = model<T>;
	return model_type::_S_local_capacity;
}

}
