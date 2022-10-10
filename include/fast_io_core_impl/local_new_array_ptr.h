#pragma once

namespace fast_io::details
{
template<typename char_type, typename allocator = ::fast_io::native_thread_local_allocator>
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
		return static_cast<char_type*>(typed_generic_allocator_adapter<allocator,char_type>::allocate(buffer_size));
	}
}

template<bool nsecure_clear, typename allocator = ::fast_io::native_thread_local_allocator>
inline void deallocate_with_secure_clear(void* ptr,[[maybe_unused]] std::size_t buffer_bytes) noexcept
{
	if constexpr(nsecure_clear)
		secure_clear(ptr,buffer_bytes);
	if constexpr(allocator::has_deallocate)
		allocator::deallocate(ptr);
	else
	 	allocator::deallocate_n(ptr,buffer_bytes);
}

template<bool nsecure_clear ,typename char_type, typename allocator = ::fast_io::native_thread_local_allocator>
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
		using typed_allocator = typed_generic_allocator_adapter<allocator,char_type>;
		if constexpr(nsecure_clear)
			secure_clear(ptr,buffer_size*sizeof(char_type));
		if constexpr(typed_allocator::has_deallocate)
			typed_allocator::deallocate(ptr);
		else
		 	typed_allocator::deallocate_n(ptr,buffer_size);
	}
}

template<typename T,bool nsecure_clear,typename Allocator=native_thread_local_allocator>
struct buffer_alloc_arr_ptr
{
	using allocator_type = Allocator;
	T* ptr{};
	std::size_t size{};
	constexpr buffer_alloc_arr_ptr() noexcept = default;
	explicit
#if __cpp_constexpr >=201907L && __cpp_constexpr_dynamic_alloc >= 201907L && (__cpp_lib_is_constant_evaluated >=201811L || __cpp_if_consteval >= 202106L)
	constexpr
#endif
	buffer_alloc_arr_ptr(std::size_t sz) noexcept:ptr(allocate_iobuf_space<T,allocator_type>(sz)),size(sz){}

	buffer_alloc_arr_ptr(buffer_alloc_arr_ptr const&)=delete;
	buffer_alloc_arr_ptr& operator=(buffer_alloc_arr_ptr const&)=delete;
	constexpr T* allocate_new(::std::size_t n) noexcept
	{
		return (ptr=allocate_iobuf_space<T,allocator_type>(size=n));
	}
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
		deallocate_iobuf_space<nsecure_clear,T,allocator_type>(ptr,size);
	}
};

template<typename char_type,typename Allocator=native_thread_local_allocator>
using local_operator_new_array_ptr=buffer_alloc_arr_ptr<char_type,false,Allocator>;

}
