#pragma once

namespace fast_io::details
{
template <typename char_type, typename allocator = ::fast_io::native_thread_local_allocator>
inline constexpr char_type *allocate_iobuf_space(::std::size_t buffer_size)
	FAST_IO_HERBCEPTIONS_THROWS_IF(typed_generic_allocator_adapter<allocator, char_type>::throws_on_allocation_failure)
{
#if __cpp_constexpr >= 201907L && __cpp_constexpr_dynamic_alloc >= 201907L
	if (__builtin_is_constant_evaluated())
	{
		return ::new char_type[buffer_size];
	}
	else
#endif
	{
		return static_cast<char_type *>(typed_generic_allocator_adapter<allocator, char_type>::allocate(buffer_size));
	}
}

template <typename char_type, typename allocator>
inline constexpr char_type *allocate_iobuf_space(
	typename typed_generic_allocator_adapter<allocator, char_type>::handle_type handle, ::std::size_t buffer_size)
	FAST_IO_HERBCEPTIONS_THROWS_IF(typed_generic_allocator_adapter<allocator, char_type>::throws_on_allocation_failure)
	requires(typed_generic_allocator_adapter<allocator, char_type>::has_status &&
			 typed_generic_allocator_adapter<allocator, char_type>::has_defaulted_alignment)
{
#if __cpp_constexpr >= 201907L && __cpp_constexpr_dynamic_alloc >= 201907L
	if (__builtin_is_constant_evaluated())
	{
		return ::new char_type[buffer_size];
	}
	else
#endif
	{
		return typed_generic_allocator_adapter<allocator, char_type>::handle_allocate(handle, buffer_size);
	}
}

template <bool nsecure_clear, typename allocator = ::fast_io::native_thread_local_allocator>
inline void deallocate_with_secure_clear(void *ptr, [[maybe_unused]] ::std::size_t buffer_bytes) noexcept
{
	if constexpr (nsecure_clear && !allocator::secure_clear)
	{
		secure_clear(ptr, buffer_bytes);
	}

	if constexpr (allocator::has_deallocate && !allocator::secure_clear)
	{
		allocator::deallocate(ptr);
	}
	else
	{
		allocator::deallocate_n(ptr, buffer_bytes);
	}
}

template <bool nsecure_clear, typename char_type, typename allocator = ::fast_io::native_thread_local_allocator>
inline constexpr void deallocate_iobuf_space(char_type *ptr, [[maybe_unused]] ::std::size_t buffer_size) noexcept
{
#if __cpp_constexpr >= 201907L && __cpp_constexpr_dynamic_alloc >= 201907L
	if (__builtin_is_constant_evaluated())
	{
		::delete[] ptr;
	}
	else
#endif
	{
		using typed_allocator = typed_generic_allocator_adapter<allocator, char_type>;
		if constexpr (nsecure_clear && !typed_allocator::secure_clear)
		{
			secure_clear(ptr, buffer_size * sizeof(char_type));
		}

		if constexpr (typed_allocator::has_deallocate && !typed_allocator::secure_clear)
		{
			typed_allocator::deallocate(ptr);
		}
		else
		{
			typed_allocator::deallocate_n(ptr, buffer_size);
		}
	}
}

template <bool nsecure_clear, typename char_type, typename allocator>
inline constexpr void deallocate_iobuf_space(
	typename typed_generic_allocator_adapter<allocator, char_type>::handle_type handle, char_type *ptr,
	[[maybe_unused]] ::std::size_t buffer_size) noexcept
	requires(typed_generic_allocator_adapter<allocator, char_type>::has_status)
{
#if __cpp_constexpr >= 201907L && __cpp_constexpr_dynamic_alloc >= 201907L
	if (__builtin_is_constant_evaluated())
	{
		::delete[] ptr;
	}
	else
#endif
	{
		using typed_allocator = typed_generic_allocator_adapter<allocator, char_type>;
		if constexpr (nsecure_clear && !typed_allocator::secure_clear)
		{
			secure_clear(ptr, buffer_size * sizeof(char_type));
		}
		typed_allocator::handle_deallocate_n(handle, ptr, buffer_size);
	}
}

template <typename T, bool nsecure_clear, typename Allocator = native_thread_local_allocator>
struct buffer_alloc_arr_ptr
{
	using allocator_type = Allocator;
	using typed_allocator_type = typed_generic_allocator_adapter<allocator_type, T>;
	static inline constexpr bool alloc_with_status{typed_allocator_type::has_status};
	using handle_type =
		::std::conditional_t<alloc_with_status, typename typed_allocator_type::handle_type, ::fast_io::details::empty>;
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
	[[no_unique_address]]
#endif
	handle_type allochdl{};
	T *ptr{};
	::std::size_t size{};
	inline constexpr buffer_alloc_arr_ptr() noexcept = default;
	inline explicit
#if __cpp_constexpr >= 201907L && __cpp_constexpr_dynamic_alloc >= 201907L && \
	(__cpp_lib_is_constant_evaluated >= 201811L || __cpp_if_consteval >= 202106L)
		constexpr
#endif
		buffer_alloc_arr_ptr(::std::size_t sz)
			FAST_IO_HERBCEPTIONS_THROWS_IF(typed_allocator_type::throws_on_allocation_failure)
		requires(!alloc_with_status)
		: ptr(::fast_io::details::allocate_iobuf_space<T, allocator_type>(sz)), size(sz)
	{
	}

	inline explicit
#if __cpp_constexpr >= 201907L && __cpp_constexpr_dynamic_alloc >= 201907L && \
	(__cpp_lib_is_constant_evaluated >= 201811L || __cpp_if_consteval >= 202106L)
		constexpr
#endif
		buffer_alloc_arr_ptr(handle_type hdl, ::std::size_t sz)
			FAST_IO_HERBCEPTIONS_THROWS_IF(typed_allocator_type::throws_on_allocation_failure)
		requires(alloc_with_status)
		: allochdl(hdl), ptr(::fast_io::details::allocate_iobuf_space<T, allocator_type>(hdl, sz)), size(sz)
	{
	}

	inline buffer_alloc_arr_ptr(buffer_alloc_arr_ptr const &) = delete;
	inline buffer_alloc_arr_ptr &operator=(buffer_alloc_arr_ptr const &) = delete;
	inline constexpr T *allocate_new(::std::size_t n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(typed_allocator_type::throws_on_allocation_failure)
		requires(!alloc_with_status)
	{
		return (ptr = ::fast_io::details::allocate_iobuf_space<T, allocator_type>(size = n));
	}
	inline constexpr T *allocate_new(handle_type hdl, ::std::size_t n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(typed_allocator_type::throws_on_allocation_failure)
		requires(alloc_with_status)
	{
		allochdl = hdl;
		return (ptr = ::fast_io::details::allocate_iobuf_space<T, allocator_type>(hdl, size = n));
	}
	inline constexpr T *get() noexcept
	{
		return ptr;
	}
	inline constexpr T const *get() const noexcept
	{
		return ptr;
	}
	inline constexpr T &operator[](::std::size_t pos) noexcept
	{
		return ptr[pos];
	}
	inline constexpr T const &operator[](::std::size_t pos) const noexcept
	{
		return ptr[pos];
	}
	inline
#if __cpp_constexpr >= 201907L && __cpp_constexpr_dynamic_alloc >= 201907L && \
	(__cpp_lib_is_constant_evaluated >= 201811L || __cpp_if_consteval >= 202106L)
		constexpr
#endif
		~buffer_alloc_arr_ptr() noexcept
	{
		if (ptr) [[likely]]
		{
			if constexpr (alloc_with_status)
			{
				::fast_io::details::deallocate_iobuf_space<nsecure_clear, T, allocator_type>(allochdl, ptr, size);
			}
			else
			{
				::fast_io::details::deallocate_iobuf_space<nsecure_clear, T, allocator_type>(ptr, size);
			}
		}
	}
};

template <typename char_type, typename Allocator = native_thread_local_allocator>
using local_operator_new_array_ptr = buffer_alloc_arr_ptr<char_type, false, Allocator>;

} // namespace fast_io::details
