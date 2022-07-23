#pragma once

namespace fast_io::details
{
template<typename char_type>
inline constexpr char_type* allocate_iobuf_space(std::size_t buffer_size) noexcept
{
#if __cpp_constexpr >=201907L && __cpp_constexpr_dynamic_alloc >= 201907L && (__cpp_lib_is_constant_evaluated >=201811L || __cpp_if_consteval >= 202106L)
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if(std::is_constant_evaluated())
#endif
	{
		return new char_type[buffer_size];
	}
	else
#endif
	{
		return static_cast<char_type*>(native_typed_thread_local_allocator<char_type>::allocate(buffer_size));
	}
}

template<bool nsecure_clear>
inline void deallocate_with_secure_clear(void* ptr,[[maybe_unused]] std::size_t buffer_bytes) noexcept
{
	if constexpr(nsecure_clear)
		secure_clear(ptr,buffer_bytes);
	native_thread_local_allocator::deallocate(ptr);
}

template<bool nsecure_clear,typename char_type>
inline constexpr void deallocate_iobuf_space(char_type* ptr,[[maybe_unused]] std::size_t buffer_size) noexcept
{
#if __cpp_constexpr >=201907L && __cpp_constexpr_dynamic_alloc >= 201907L && (__cpp_lib_is_constant_evaluated >=201811L || __cpp_if_consteval >= 202106L)
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if(std::is_constant_evaluated())
#endif
	{
		delete[] ptr;
	}
	else
#endif
	{
		if constexpr(nsecure_clear)
			secure_clear(ptr,buffer_size*sizeof(char_type));
		native_typed_thread_local_allocator<char_type>::deallocate(ptr);
	}
}

template<typename T,bool nsecure_clear>
struct buffer_alloc_arr_ptr
{
	T* ptr{};
	std::size_t size{};
	constexpr buffer_alloc_arr_ptr() noexcept = default;
	explicit
#if __cpp_constexpr >=201907L && __cpp_constexpr_dynamic_alloc >= 201907L && (__cpp_lib_is_constant_evaluated >=201811L || __cpp_if_consteval >= 202106L)
	constexpr
#endif
	buffer_alloc_arr_ptr(std::size_t sz) noexcept:ptr(allocate_iobuf_space<T>(sz)),size(sz){}

	buffer_alloc_arr_ptr(buffer_alloc_arr_ptr const&)=delete;
	buffer_alloc_arr_ptr& operator=(buffer_alloc_arr_ptr const&)=delete;
	constexpr T* get() noexcept
	{
		return ptr;
	}
	constexpr T const* get() const noexcept
	{
		return ptr;
	}
	constexpr T& operator[](std::size_t pos) noexcept
	{
		return ptr[pos];
	}
	constexpr T const& operator[](std::size_t pos) const noexcept
	{
		return ptr[pos];
	}
#if __cpp_constexpr >=201907L && __cpp_constexpr_dynamic_alloc >= 201907L && (__cpp_lib_is_constant_evaluated >=201811L || __cpp_if_consteval >= 202106L)
	constexpr
#endif
	~buffer_alloc_arr_ptr()
	{
		deallocate_iobuf_space<nsecure_clear,T>(ptr,size);
	}
};

template<typename char_type>
using local_operator_new_array_ptr=buffer_alloc_arr_ptr<char_type,false>;

}
