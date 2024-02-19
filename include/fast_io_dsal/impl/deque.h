#pragma once

namespace fast_io
{
namespace containers
{
namespace details
{

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	deque_control_block_common
{
	void **controller_ptr;
	void *begin_ptr, *curr_ptr, *end_ptr;
};

template <typename T>
struct deque_control_block
{
	T **controller_ptr;
	T *begin_ptr, *curr_ptr, *end_ptr;
};

template <::std::size_t sz>
inline constexpr ::std::size_t deque_block_size{sz <= 4096u ? (4096u / sz) : 1u};

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	deque_controller_block_common
{
	void **controller_start_ptr;
	void **controller_start_reserved_ptr;
	void **controller_after_ptr;
	void **controller_after_reserved_ptr;
};

template <typename T>
struct deque_controller_block
{
	T **controller_start_ptr;
	T **controller_start_reserved_ptr;
	T **controller_after_ptr;
	T **controller_after_reserved_ptr;
};

template <typename T>
struct deque_controller
{
	::fast_io::containers::details::deque_control_block<T> front_block;
	::fast_io::containers::details::deque_control_block<T> back_block;
	::fast_io::containers::details::deque_controller_block<T> controller_block;
};

struct
	[[__gnu__::__may_alias__]] deque_controller_common
{
	::fast_io::containers::details::deque_control_block_common front_block;
	::fast_io::containers::details::deque_control_block_common back_block;
	::fast_io::containers::details::deque_controller_block_common controller_block;
};

template <typename allocator, typename controllerblocktype>
inline constexpr void deque_destroy_controller(controllerblocktype *controllerptr) noexcept
{
	auto &controller{*controllerptr};
	if constexpr (allocator::has_deallocate)
	{
		allocator::deallocate(controller.controller_start_ptr);
	}
	else
	{
		::std::size_t n{static_cast<::std::size_t>(controller.controller_after_ptr - controller.controller_start_ptr) * sizeof(void *)};
		allocator::deallocate_n(controller.controller_start_ptr, n);
	}
}

template <typename allocator, typename controllerblocktype>
inline constexpr void deque_destroy_trivial_common_align(controllerblocktype *controllerptr, ::std::size_t aligns, ::std::size_t totalsz) noexcept
{
	auto &controller{*controllerptr};
	for (auto i{controller.controller_start_reserved_ptr}, e{controller.controller_after_reserved_ptr}; i != e; ++i)
	{
		allocator::deallocate_aligned_n(*i, aligns, totalsz);
	}
	deque_destroy_controller(controllerptr);
}

template <typename allocator, typename controllerblocktype>
inline constexpr void deque_destroy_trivial_common_no_align(controllerblocktype *controllerptr, ::std::size_t totalsz) noexcept
{
	auto &controller{*controllerptr};
	for (auto i{controller.controller_start_reserved_ptr}, e{controller.controller_after_reserved_ptr}; i != e; ++i)
	{
		allocator::deallocate_n(*i, totalsz);
	}
	deque_destroy_controller(controllerptr);
}

template <typename allocator, ::std::size_t align, ::std::size_t sz, typename controllerblocktype>
inline constexpr void deque_destroy_trivial_common_impl(controllerblocktype *controllerptr) noexcept
{
	auto &controller{*controllerptr};
	constexpr ::std::size_t totalsz{::fast_io::containers::details::deque_block_size<sz>};
	if constexpr (align <= allocator::default_alignment)
	{
		deque_destroy_trivial_common_align(controllerptr, align, totalsz);
	}
	else
	{
		deque_destroy_trivial_common_no_align(controllerptr, totalsz);
	}
}

template <typename allocator, ::std::size_t align, ::std::size_t sz, typename controllerblocktype>
inline constexpr void deque_destroy_trivial_common(controllerblocktype &controllerptr) noexcept
{
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if (__builtin_is_constant_evaluated())
#endif
	{
		::fast_io::containers::details::deque_destroy_trivial_common_impl<allocator, align, sz, controllerblocktype>(__builtin_addressof(controllerptr));
	}
	else
	{
		::fast_io::containers::details::deque_destroy_trivial_common_impl<allocator, align, sz, ::fast_io::containers::details::deque_controller_block_common>(
			reinterpret_cast<::fast_io::containers::details::deque_controller_block_common *>(__builtin_addressof(controllerptr)));
	}
}
#if 0
template<typename allocator,::std::size_t align,::std::size_t sz, typename dqcontrollerblocktype>
inline constexpr void* deque_allocate_block_memory_common_impl() noexcept
{

}
#endif

template <typename allocator, ::std::size_t align, ::std::size_t sz, typename dqcontrollerblocktype>
inline constexpr void deque_grow_trivial_common_impl(dqcontrollerblocktype *controllerptr) noexcept
{
	auto &controller{*controllerptr};
	auto bbcptr{controller.back_block.controller_ptr};
	auto ctafterprs{controller.controller_block.controller_after_reserved_ptr};
	::std::size_t diff{static_cast<::std::size_t>(ctafterprs - bbcptr)};

	constexpr ::std::size_t dqblksz{::fast_io::containers::details::deque_block_size<sz>};

	if (diff < 2u)
	{
		auto controllerafterptr{controller.controller_block.controller_after_ptr};
		if (ctafterprs == controller.controller_block.controller_after_ptr)
		{
			::std::size_t oldsize{static_cast<::std::size_t>(controllerafterptr - controller.controller_block.controller_start_ptr)};

			::std::size_t newsize{};
			if (oldsize == 0)
			{
				newsize = 1;
			}
			else
			{
			}
			allocator::reallocate_n();
		}
	}
	controller.back_block.end_ptr = ((controller.back_block.curr_ptr = controller.back_block.begin_ptr = *(controller.back_block.controller_ptr = bbcptr + 1))) + dqblksz;
}

template <typename allocator, ::std::size_t align, ::std::size_t sz, typename dqcontrollerblocktype>
inline constexpr void deque_grow_trivial_common(dqcontrollerblocktype &controller) noexcept
{
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if (__builtin_is_constant_evaluated())
#endif
	{
		::fast_io::containers::details::deque_grow_trivial_common_impl<allocator, align, sz, dqcontrollerblocktype>(__builtin_addressof(controller));
	}
	else
	{
		::fast_io::containers::details::deque_grow_trivial_common_impl<allocator, align, sz, deque_controller_common>(reinterpret_cast<deque_controller_common *>(__builtin_addressof(controller)));
	}
}

} // namespace details

template <typename T, typename allocator>
class deque
{
public:
	using value_type = T;
	using pointer = value_type *;
	using reference = value_type &;
	using const_reference = value_type const &;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	::fast_io::containers::details::deque_controller<T> controller;
	static inline constexpr size_type block_size{::fast_io::containers::details::deque_block_size<sizeof(value_type)>};
	constexpr deque() noexcept
		: controller{{}, {}, nullptr, nullptr, nullptr, nullptr}
	{
	}
	constexpr deque(deque const &) = delete;
	constexpr deque &operator=(deque const &) = delete;

private:
	static inline constexpr void destroy_block_element(pointer first, pointer last) noexcept
	{
		for (; first != last; ++first)
		{
			first->~value_type();
		}
	}
	constexpr void destroy_all_elements() noexcept
	{
		destroy_block_element(controller.front_block.curr_ptr, controller.front_block.end_ptr);
		auto front_controller_ptr{controller.front_block.controller_ptr};
		auto back_controller_ptr{controller.back_block.controller_ptr};
		if (front_controller_ptr != back_controller_ptr)
		{
			for (T **it{front_controller_ptr + 1}, **ed{back_controller_ptr}; it != ed; ++it)
			{
				T *blockptr{*it};
				destroy_block_element(blockptr, blockptr + block_size);
			}
		}
		destroy_block_element(controller.back_block.begin_ptr, controller.back_block.curr_ptr);
	}
	constexpr void destroy() noexcept
	{
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			this->destroy_all_elements();
		}
		::fast_io::containers::details::deque_destroy_trivial_common<allocator, alignof(value_type), sizeof(value_type)>(controller);
	}
	constexpr void grow_back() noexcept
	{
	}

public:
#if 0
	constexpr void clear() noexcept
	{
		if(controller.controller_block.start_ptr == controller.controller_block.start_ptr)
		{
			return;
		}
		this->destroy();
		::std::size_t n{(controller.back_block.controller_ptr-controller.front_block.controller_ptr)};
		n>>=1u;
		auto mid{controller.front_block.controller_ptr+n};
		*mid;
	}
#endif
	template <typename... Args>
		requires ::std::constructible_from<value_type, Args...>
	constexpr reference emplace_back(Args &&...args)
	{
		if (controller.back_block.curr_ptr == controller.back_block.end_ptr)
		{
			grow_back();
		}
		::std::construct_at(controller.back_block.curr_ptr, ::std::forward<Args>(args)...);
		++controller.back_block.curr_ptr;
	}
	constexpr reference back() noexcept
	{
		return controller.back_block.curr_ptr[-1];
	}

	constexpr const_reference back() const noexcept
	{
		return controller.back_block.curr_ptr[-1];
	}

	constexpr reference front() noexcept
	{
		return *controller.front_block.curr_ptr;
	}

	constexpr const_reference front() const noexcept
	{
		return *controller.front_block.curr_ptr;
	}

	static inline constexpr size_type max_size() noexcept
	{
		constexpr size_type mxval{SIZE_MAX / sizeof(value_type)};
		return mxval;
	}

	constexpr bool empty() const noexcept
	{
		return controller.front_block.curr_ptr == controller.back_block.curr_ptr;
	}

	constexpr bool is_empty() const noexcept
	{
		return controller.front_block.curr_ptr == controller.back_block.curr_ptr;
	}

	constexpr ~deque()
	{
		this->destroy();
	}
};

} // namespace containers

namespace freestanding
{

template <typename T, typename allocator>
struct is_trivially_relocatable<::fast_io::containers::deque<T, allocator>>
{
	inline static constexpr bool value = true;
};

template <typename T, typename allocator>
struct is_zero_default_constructible<::fast_io::containers::deque<T, allocator>>
{
	inline static constexpr bool value = true;
};

} // namespace freestanding

} // namespace fast_io