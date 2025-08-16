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
	vector_model
{
	::std::byte *begin_ptr;
	::std::byte *curr_ptr;
	::std::byte *end_ptr;
};

namespace vector
{

namespace detemplate
{

template <typename allocator>
inline void *grow_to_byte_size_iter_impl(vector_model &imp, void *iter, ::std::size_t newcap, ::std::size_t gap, ::std::size_t size, ::std::size_t alignment) noexcept
{
	::std::byte *old_begin_ptr{imp.begin_ptr};
	::std::byte *old_curr_ptr{imp.curr_ptr};
	::std::size_t const old_size{static_cast<::std::size_t>(old_curr_ptr - old_begin_ptr)};
	::std::size_t const old_capacity{static_cast<::std::size_t>(imp.end_ptr - old_begin_ptr)};

	auto newres = allocator::allocate_aligned_at_least(alignment, newcap);
	auto begin_ptr = reinterpret_cast<::std::byte *>(newres.ptr);
	auto newiter = ::fast_io::freestanding::nonoverlapped_bytes_copy(reinterpret_cast<::std::byte const *>(old_begin_ptr), reinterpret_cast<::std::byte const *>(iter),
																	 reinterpret_cast<::std::byte *>(begin_ptr));
	::fast_io::freestanding::nonoverlapped_bytes_copy(reinterpret_cast<::std::byte const *>(iter), reinterpret_cast<::std::byte const *>(old_curr_ptr),
													  reinterpret_cast<::std::byte *>(newiter + gap));
	allocator::deallocate_aligned_n(old_begin_ptr, alignment, old_capacity);
	imp.begin_ptr = begin_ptr;
	imp.curr_ptr = begin_ptr + old_size;
	imp.end_ptr = begin_ptr + (newres.count / size * size);
	return newiter;
}

template <typename allocator>
inline void *grow_to_size_iter_impl(vector_model &imp, void *iter, ::std::size_t newcap, ::std::size_t size, ::std::size_t alignment) noexcept
{
#if defined(_MSC_VER) && !defined(__clang__)
	::std::size_t mx{SIZE_MAX / size};
	if (newcap > mx) [[unlikely]]
	{
		::fast_io::fast_terminate();
	}
	newcap *= size;
#else
	if (__builtin_mul_overflow(size, newcap, __builtin_addressof(newcap))) [[unlikely]]
	{
		__builtin_trap();
	}
#endif
	return grow_to_byte_size_iter_impl<allocator>(imp, iter, newcap, size, size, alignment);
}

template <typename allocator>
inline void *grow_to_size_impl(vector_model &imp, ::std::size_t newcap, ::std::size_t size, ::std::size_t alignment) noexcept
{
	return grow_to_size_iter_impl<allocator>(imp, imp.curr_ptr, newcap, size, alignment);
}

template <typename allocator>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void *grow_twice_iter_impl(vector_model &imp, void *iter, ::std::size_t size, ::std::size_t alignment) noexcept
{
	auto begin_ptr{imp.begin_ptr};
	auto end_ptr{imp.end_ptr};
	::std::size_t toallocate{size};
	::std::size_t diff{static_cast<::std::size_t>(end_ptr - begin_ptr)};
	if (diff) [[likely]]
	{
#if defined(_MSC_VER) && !defined(__clang__)
		constexpr ::std::size_t mx{SIZE_MAX / 2};
		if (diff > mx) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		toallocate = (diff << 1u);
#else
		if (__builtin_mul_overflow(diff, 2u, __builtin_addressof(toallocate))) [[unlikely]]
		{
			__builtin_trap();
		}
#endif
	}
	return grow_to_byte_size_iter_impl<allocator>(imp, iter, toallocate, size, size, alignment);
}

template <typename allocator>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void *grow_twice_impl(vector_model &imp, ::std::size_t size, ::std::size_t alignment) noexcept
{
	return grow_twice_iter_impl<allocator>(imp, imp.curr_ptr, size, alignment);
}

template <typename allocator>
inline constexpr void *move_backward_impl(vector_model &imp, void *iter, ::std::size_t size, ::std::size_t alignment) noexcept
{
	if (imp.curr_ptr == imp.end_ptr) [[unlikely]]
	{
		return grow_twice_iter_impl<allocator>(imp, iter, size, alignment);
	}
	auto currptr{imp.curr_ptr};
	auto currptrp1{currptr + size};
	::fast_io::freestanding::uninitialized_move_backward(reinterpret_cast<::std::byte *>(iter), currptr, currptrp1);
	return iter;
}

} // namespace detemplate

template <typename allocator, ::std::size_t size, ::std::size_t alignment>
inline constexpr void *grow_to_size_iter_impl(vector_model &imp, void *iter, ::std::size_t newcap) noexcept
{
	if constexpr (alignment < allocator::default_alignment)
	{
		return ::fast_io::containers::details::vector::grow_to_size_iter_impl<allocator, size, allocator::default_alignment>(imp, iter, newcap);
	}
	else
	{
		return ::fast_io::containers::details::vector::detemplate::grow_to_size_iter_impl<allocator>(imp, iter, newcap, size, alignment);
	}
}

template <typename allocator, ::std::size_t size, ::std::size_t alignment>
inline constexpr void *grow_to_size_impl(vector_model &imp, ::std::size_t newcap) noexcept
{
	if constexpr (alignment < allocator::default_alignment)
	{
		return ::fast_io::containers::details::vector::grow_to_size_impl<allocator, size, allocator::default_alignment>(imp, newcap);
	}
	else
	{
		return ::fast_io::containers::details::vector::detemplate::grow_to_size_impl<allocator>(imp, newcap, size, alignment);
	}
}

template <typename allocator, ::std::size_t size, ::std::size_t alignment>
inline constexpr void *grow_twice_impl(vector_model &imp) noexcept
{
	if constexpr (alignment < allocator::default_alignment)
	{
		return ::fast_io::containers::details::vector::grow_twice_impl<allocator, size, allocator::default_alignment>(imp);
	}
	else
	{
		return ::fast_io::containers::details::vector::detemplate::grow_twice_impl<allocator>(imp, size, alignment);
	}
}

template <typename allocator, ::std::size_t size, ::std::size_t alignment>
inline constexpr void *grow_twice_iter_impl(vector_model &imp, void *iter) noexcept
{
	if constexpr (alignment < allocator::default_alignment)
	{
		return ::fast_io::containers::details::vector::grow_twice_iter_impl<allocator, size, allocator::default_alignment>(imp, iter);
	}
	else
	{
		return ::fast_io::containers::details::vector::detemplate::grow_twice_iter_impl<allocator>(imp, iter, size, alignment);
	}
}

template <typename allocator, ::std::size_t size, ::std::size_t alignment>
inline constexpr void *move_backward_impl(vector_model &imp, void *iter) noexcept
{
	if constexpr (alignment < allocator::default_alignment)
	{
		return ::fast_io::containers::details::vector::move_backward_impl<allocator, size, allocator::default_alignment>(imp, iter);
	}
	else
	{
		return ::fast_io::containers::details::vector::detemplate::move_backward_impl<allocator>(imp, iter, size, alignment);
	}
}

} // namespace vector

template <typename T>
struct vector_internal
{
	T *begin_ptr{};
	T *curr_ptr{};
	T *end_ptr{};
};

} // namespace details

template <typename T, typename allocator>
class vector FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
{
public:
	using allocator_type = allocator;
	using value_type = T;

private:
	using typed_allocator_type = typed_generic_allocator_adapter<allocator_type, value_type>;

public:
	using pointer = value_type *;
	using const_pointer = value_type const *;

	using reference = value_type &;
	using const_reference = value_type const &;

	using iterator = value_type *;
	using const_iterator = value_type const *;

	using reverse_iterator = ::std::reverse_iterator<iterator>;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;

	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	::fast_io::containers::details::vector_internal<T> imp;

	inline constexpr vector() noexcept = default;

private:
	inline constexpr void destroy() noexcept
	{
		clear();
		if constexpr (!typed_allocator_type::has_deallocate)
		{
			typed_allocator_type::deallocate(imp.begin_ptr);
		}
		else
		{
			typed_allocator_type::deallocate_n(imp.begin_ptr,
											   static_cast<::std::size_t>(imp.end_ptr - imp.begin_ptr));
		}
	}
	struct run_destroy
	{
		vector *thisvec{};
		inline constexpr run_destroy() noexcept = default;
		inline explicit constexpr run_destroy(vector *p) noexcept
			: thisvec(p)
		{}
		inline run_destroy(run_destroy const &) = delete;
		inline run_destroy &operator=(run_destroy const &) = delete;
		inline constexpr ~run_destroy()
		{
			if (thisvec)
			{
				thisvec->destroy();
			}
		}
	};

	inline constexpr void default_construct_impl(size_type n)
	{
		auto e{this->imp.end_ptr = (this->imp.curr_ptr = this->imp.begin_ptr =
										typed_allocator_type::allocate(n)) +
								   n};
		run_destroy des(this);
		for (; this->imp.curr_ptr != e; ++this->imp.curr_ptr)
		{
			::std::construct_at(this->imp.curr_ptr);
		}
		des.thisvec = nullptr;
	}

	template <typename Iter, typename Sentinel>
	inline constexpr void construct_vector_common_impl(Iter first, Sentinel last)
	{
		using rvaluetype = ::std::iter_value_t<Iter>;
		if constexpr (::std::same_as<Iter, Sentinel> && ::std::contiguous_iterator<Iter> && !::std::is_pointer_v<Iter>)
		{
			this->construct_vector_common_impl(::std::to_address(first), ::std::to_address(last));
		}
		else
		{
			if constexpr (::std::forward_iterator<Iter>)
			{
				size_type n{static_cast<size_type>(::std::ranges::distance(first, last))};
				auto e{this->imp.end_ptr = (this->imp.curr_ptr = this->imp.begin_ptr =
												typed_allocator_type::allocate(n)) +
										   n};
				if constexpr (
					::std::is_pointer_v<Iter> &&
					::std::is_trivially_constructible_v<value_type, rvaluetype> &&
					::std::same_as<::std::remove_cvref_t<rvaluetype>, ::std::remove_cvref_t<value_type>>)
				{
					if (n) [[likely]]
					{
#if defined(_MSC_VER) && !defined(__clang__)
						::std::memcpy
#else
						__builtin_memcpy
#endif
							(this->imp.curr_ptr, first, n * sizeof(value_type));
					}
					this->imp.curr_ptr = e;
				}
				else if constexpr (::std::is_nothrow_constructible_v<value_type, rvaluetype>)
				{
					auto curr{this->imp.begin_ptr};
					for (; curr != e; ++curr)
					{
						::std::construct_at(curr, *first);
						++first;
					}
					this->imp.curr_ptr = e;
				}
				else
				{
					run_destroy des(this);
					for (; this->imp.curr_ptr != e; ++this->imp.curr_ptr)
					{
						::std::construct_at(this->imp.curr_ptr, *first);
						++first;
					}
					des.thisvec = nullptr;
				}
			}
			else
			{
				run_destroy des(this);
				for (; first != last; ++first)
				{
					this->emplace_back(*first);
				}
				des.thisvec = nullptr;
			}
		}
	}

public:
	inline explicit constexpr vector(size_type n) noexcept(::fast_io::freestanding::is_zero_default_constructible_v<value_type> || noexcept(value_type()))
	{
		if constexpr (::fast_io::freestanding::is_zero_default_constructible_v<value_type>)
		{
			imp.begin_ptr = typed_allocator_type::allocate_zero(n);
			imp.end_ptr = imp.curr_ptr = imp.begin_ptr + n;
		}
		else
		{
			this->default_construct_impl(n);
		}
	}

	inline explicit constexpr vector(size_type n, ::fast_io::for_overwrite_t) noexcept(::std::is_trivially_default_constructible_v<value_type> || ::fast_io::freestanding::is_zero_default_constructible_v<value_type> || noexcept(value_type()))
	{
		if constexpr (::std::is_trivially_default_constructible_v<value_type>)
		{
			imp.begin_ptr = typed_allocator_type::allocate(n);
			imp.end_ptr = imp.curr_ptr = imp.begin_ptr + n;
		}
		else if constexpr (::fast_io::freestanding::is_zero_default_constructible_v<value_type>)
		{
			imp.begin_ptr = typed_allocator_type::allocate_zero(n);
			imp.end_ptr = imp.curr_ptr = imp.begin_ptr + n;
		}
		else
		{
			this->default_construct_impl(n);
		}
	}

	inline explicit constexpr vector(size_type n, const_reference val) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		auto e{this->imp.end_ptr = (this->imp.curr_ptr = this->imp.begin_ptr =
										typed_allocator_type::allocate(n)) +
								   n};
		run_destroy des(this);
		for (; this->imp.curr_ptr != e; ++this->imp.curr_ptr)
		{
			::std::construct_at(this->imp.curr_ptr, val);
		}
		des.thisvec = nullptr;
	}

	template <::std::ranges::range R>
	inline explicit constexpr vector(::fast_io::freestanding::from_range_t, R &&rg)
	{
		this->construct_vector_common_impl(::std::ranges::begin(rg), ::std::ranges::end(rg));
	}

	inline explicit constexpr vector(::std::initializer_list<value_type> ilist) noexcept(::std::is_nothrow_move_constructible_v<value_type>)
	{
		this->construct_vector_common_impl(ilist.begin(), ilist.end());
	}

	inline constexpr vector(vector const &vec)
		requires(::std::is_copy_constructible_v<value_type>)
	{
		std::size_t const vecsize{static_cast<std::size_t>(vec.imp.curr_ptr - vec.imp.begin_ptr)};
		if (vecsize == 0)
		{
			return;
		}
		imp.begin_ptr = typed_allocator_type::allocate(vecsize);
		if constexpr (::std::is_trivially_copyable_v<value_type>)
		{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
#endif
			{
				::fast_io::freestanding::nonoverlapped_bytes_copy(
					reinterpret_cast<::std::byte const *>(vec.imp.begin_ptr),
					reinterpret_cast<::std::byte const *>(vec.imp.curr_ptr),
					reinterpret_cast<::std::byte *>(this->imp.begin_ptr));
				imp.end_ptr = imp.curr_ptr = imp.begin_ptr + vecsize;
				return;
			}
		}
		run_destroy des(this);
		this->imp.curr_ptr = this->imp.begin_ptr;
		this->imp.end_ptr = this->imp.begin_ptr + vecsize;
		for (auto i{vec.imp.begin_ptr}; i != vec.imp.curr_ptr; ++i)
		{
			::std::construct_at(this->imp.curr_ptr, *i);
			++this->imp.curr_ptr;
		}
		des.thisvec = nullptr;
	}
	inline constexpr vector(vector const &vec) = delete;
	inline constexpr vector &operator=(vector const &vec)
		requires(::std::copyable<value_type>)
	{
		if (__builtin_addressof(vec) == this) [[unlikely]]
		{
			return *this;
		}
		vector newvec(vec);
		this->operator=(::std::move(newvec));
		return *this;
	}
	inline constexpr vector &operator=(vector const &vec) = delete;
	inline constexpr vector(vector &&vec) noexcept
		: imp(vec.imp)
	{
		vec.imp = {};
	}
	inline constexpr vector &operator=(vector &&vec) noexcept
	{
		if (__builtin_addressof(vec) == this) [[unlikely]]
		{
			return *this;
		}
		this->destroy();
		this->imp = vec.imp;
		vec.imp = {};
		return *this;
	}
	inline constexpr ~vector()
	{
		destroy();
	}

	template <typename... Args>
		requires std::constructible_from<value_type, Args...>
	inline constexpr reference emplace_back_unchecked(Args &&...args) noexcept(::std::is_nothrow_constructible_v<value_type, Args...>)
	{
		auto p{::std::construct_at(imp.curr_ptr, ::std::forward<Args>(args)...)};
		++imp.curr_ptr;
		return *p;
	}

private:
	inline constexpr pointer grow_to_size_iter_impl(size_type newcap, pointer iter, size_type n) noexcept
	{
		if constexpr (::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<value_type>)
		{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
#endif
			{
				return reinterpret_cast<pointer>(::fast_io::containers::details::vector::grow_to_size_iter_impl<allocator_type, sizeof(value_type), alignof(value_type)>(*reinterpret_cast<::fast_io::containers::details::vector_model *>(__builtin_addressof(imp)),
																																										 iter, n));
			}
		}
		auto newres = typed_allocator_type::allocate_at_least(newcap);
		auto new_begin_ptr = newres.ptr;
		auto old_begin_ptr{imp.begin_ptr};
		auto old_curr_ptr{imp.curr_ptr};
		size_type const old_size{static_cast<size_type>(old_curr_ptr - old_begin_ptr)};
		auto newiter{::fast_io::freestanding::uninitialized_relocate(old_begin_ptr, iter, new_begin_ptr)};
		::fast_io::freestanding::uninitialized_relocate(iter, old_curr_ptr, newiter + n);
		if constexpr (typed_allocator_type::has_deallocate)
		{
			typed_allocator_type::deallocate(old_begin_ptr);
		}
		else
		{
			typed_allocator_type::deallocate_n(old_begin_ptr, static_cast<size_type>(imp.end_ptr - old_begin_ptr));
		}
		imp.begin_ptr = new_begin_ptr;
		imp.curr_ptr = new_begin_ptr + old_size;
		imp.end_ptr = new_begin_ptr + newres.count;
		return newiter;
	}

#if __has_cpp_attribute(__gnu__::__cold__)
	[[__gnu__::__cold__]]
#endif
	inline constexpr pointer grow_twice_iter_impl(pointer iter) noexcept
	{
		if constexpr (::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<value_type>)
		{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
#endif
			{
				return reinterpret_cast<pointer>(::fast_io::containers::details::vector::grow_twice_iter_impl<allocator_type, sizeof(value_type), alignof(value_type)>(*reinterpret_cast<::fast_io::containers::details::vector_model *>(__builtin_addressof(imp)),
																																									   iter));
			}
		}
		std::size_t const cap{static_cast<size_type>(imp.end_ptr - imp.begin_ptr)};
		return this->grow_to_size_iter_impl(::fast_io::containers::details::cal_grow_twice_size<sizeof(value_type), false>(cap), iter, 1);
	}
	inline constexpr pointer move_backward_common_impl(pointer iter) noexcept
	{
		if constexpr (::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<value_type>)
		{
#ifdef __cpp_if_consteval
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
			{
				return reinterpret_cast<pointer>(::fast_io::containers::details::vector::move_backward_impl<allocator, sizeof(value_type), alignof(value_type)>(
					*reinterpret_cast<::fast_io::containers::details::vector_model *>(__builtin_addressof(imp)),
					iter));
			}
		}
		if (imp.curr_ptr == imp.end_ptr) [[unlikely]]
		{
			return this->grow_twice_iter_impl(iter);
		}
		auto currptr{imp.curr_ptr};
		auto currptrp1{currptr + 1};
		::fast_io::freestanding::uninitialized_move_backward(iter, currptr, currptrp1);
		return iter;
	}

	inline constexpr void grow_to_size_impl(size_type newcap) noexcept
	{
		if constexpr (::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<value_type>)
		{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
#endif
			{
				::fast_io::containers::details::vector::grow_to_size_impl<allocator_type, sizeof(value_type), alignof(value_type)>(
					*reinterpret_cast<::fast_io::containers::details::vector_model *>(__builtin_addressof(imp)), newcap);
				return;
			}
		}
		this->grow_to_size_iter_impl(newcap, imp.curr_ptr, 1);
	}
#if __has_cpp_attribute(__gnu__::__cold__)
	[[__gnu__::__cold__]]
#endif
	inline constexpr void grow_twice_impl() noexcept
	{
		if constexpr (::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<value_type>)
		{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
#endif
			{
				::fast_io::containers::details::vector::grow_twice_impl<allocator_type, sizeof(value_type), alignof(value_type)>(
					*reinterpret_cast<::fast_io::containers::details::vector_model *>(__builtin_addressof(imp)));
				return;
			}
		}
		std::size_t const cap{static_cast<size_type>(imp.end_ptr - imp.begin_ptr)};
		grow_to_size_impl(::fast_io::containers::details::cal_grow_twice_size<sizeof(value_type), false>(cap));
	}

public:
	inline constexpr void reserve(size_type n) noexcept
	{
		if (n <= static_cast<std::size_t>(imp.end_ptr - imp.begin_ptr))
		{
			return;
		}
		grow_to_size_impl(n);
	}

	inline constexpr void shrink_to_fit() noexcept
	{
		if (imp.curr_ptr == imp.end_ptr)
		{
			return;
		}
		grow_to_size_impl(static_cast<std::size_t>(imp.curr_ptr - imp.begin_ptr));
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr void pop_back() noexcept
	{
		if (imp.curr_ptr == imp.begin_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		(--imp.curr_ptr)->~value_type();
	}

	inline constexpr void pop_back_unchecked() noexcept
	{
		(--imp.curr_ptr)->~value_type();
	}

	inline constexpr void push_back(T const &value) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		this->emplace_back(value);
	}
	inline constexpr void push_back(T &&value) noexcept(::std::is_nothrow_move_constructible_v<value_type>)
	{
		this->emplace_back(::std::move(value));
	}
	inline constexpr void push_back_unchecked(T const &value) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		this->emplace_back_unchecked(value);
	}
	inline constexpr void push_back_unchecked(T &&value) noexcept(::std::is_nothrow_move_constructible_v<value_type>)
	{
		this->emplace_back_unchecked(::std::move(value));
	}

	[[nodiscard]] inline constexpr pointer data() noexcept
	{
		return imp.begin_ptr;
	}
	[[nodiscard]] inline constexpr const_pointer data() const noexcept
	{
		return imp.begin_ptr;
	}
	[[nodiscard]] inline constexpr bool is_empty() const noexcept
	{
		return imp.begin_ptr == imp.curr_ptr;
	}

	[[nodiscard]] inline constexpr bool empty() const noexcept
	{
		return imp.begin_ptr == imp.curr_ptr;
	}
	inline constexpr void clear() noexcept
	{
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			::std::destroy(imp.begin_ptr, imp.curr_ptr);
		}
		imp.curr_ptr = imp.begin_ptr;
	}
	[[nodiscard]] inline constexpr size_type size() const noexcept
	{
		return static_cast<size_type>(imp.curr_ptr - imp.begin_ptr);
	}
	[[nodiscard]] inline constexpr size_type size_bytes() const noexcept
	{
		return static_cast<size_type>(imp.curr_ptr - imp.begin_ptr) * sizeof(value_type);
	}
	[[nodiscard]] inline constexpr size_type capacity() const noexcept
	{
		return static_cast<size_type>(imp.end_ptr - imp.begin_ptr);
	}
	[[nodiscard]] inline constexpr size_type capacity_bytes() const noexcept
	{
		return static_cast<size_type>(imp.end_ptr - imp.begin_ptr) * sizeof(value_type);
	}
	[[nodiscard]] static inline constexpr size_type max_size() noexcept
	{
		constexpr size_type mx{::std::numeric_limits<size_type>::max() / sizeof(value_type)};
		return mx;
	}
	[[nodiscard]] static inline constexpr size_type max_size_bytes() noexcept
	{
		constexpr size_type mx{::std::numeric_limits<size_type>::max() / sizeof(value_type) * sizeof(value_type)};
		return mx;
	}
	[[nodiscard]] inline constexpr const_reference index_unchecked(size_type pos) const noexcept
	{
		return imp.begin_ptr[pos];
	}
	[[nodiscard]] inline constexpr reference index_unchecked(size_type pos) noexcept
	{
		return imp.begin_ptr[pos];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]] inline constexpr const_reference
	operator[](size_type pos) const noexcept
	{
		auto begin_ptr{imp.begin_ptr}, curr_ptr{imp.curr_ptr};
		if (static_cast<::std::size_t>(curr_ptr - begin_ptr) <= pos) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return begin_ptr[pos];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]] inline constexpr reference
	operator[](size_type pos) noexcept
	{
		auto begin_ptr{imp.begin_ptr}, curr_ptr{imp.curr_ptr};
		if (static_cast<::std::size_t>(curr_ptr - begin_ptr) <= pos) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return begin_ptr[pos];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]] inline constexpr const_reference
	front() const noexcept
	{
		auto begin_ptr{imp.begin_ptr}, curr_ptr{imp.curr_ptr};
		if (begin_ptr == curr_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return *begin_ptr;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]] inline constexpr reference
	front() noexcept
	{
		auto begin_ptr{imp.begin_ptr}, curr_ptr{imp.curr_ptr};
		if (begin_ptr == curr_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return *begin_ptr;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]] inline constexpr const_reference
	back() const noexcept
	{
		auto begin_ptr{imp.begin_ptr}, curr_ptr{imp.curr_ptr};
		if (begin_ptr == curr_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return curr_ptr[-1];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]] inline constexpr reference
	back() noexcept
	{
		auto begin_ptr{imp.begin_ptr}, curr_ptr{imp.curr_ptr};
		if (begin_ptr == curr_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return curr_ptr[-1];
	}

	[[nodiscard]] inline constexpr const_reference front_unchecked() const noexcept
	{
		return *imp.begin_ptr;
	}
	[[nodiscard]] inline constexpr reference front_unchecked() noexcept
	{
		return *imp.begin_ptr;
	}
	[[nodiscard]] inline constexpr const_reference back_unchecked() const noexcept
	{
		return imp.curr_ptr[-1];
	}
	[[nodiscard]] inline constexpr reference back_unchecked() noexcept
	{
		return imp.curr_ptr[-1];
	}

	[[nodiscard]] inline constexpr iterator begin() noexcept
	{
		return imp.begin_ptr;
	}
	[[nodiscard]] inline constexpr iterator end() noexcept
	{
		return imp.curr_ptr;
	}
	[[nodiscard]] inline constexpr const_iterator begin() const noexcept
	{
		return imp.begin_ptr;
	}
	[[nodiscard]] inline constexpr const_iterator end() const noexcept
	{
		return imp.curr_ptr;
	}
	[[nodiscard]] inline constexpr const_iterator cbegin() const noexcept
	{
		return imp.begin_ptr;
	}
	[[nodiscard]] inline constexpr const_iterator cend() const noexcept
	{
		return imp.curr_ptr;
	}

	[[nodiscard]] inline constexpr reverse_iterator rbegin() noexcept
	{
		return reverse_iterator{imp.curr_ptr};
	}
	[[nodiscard]] inline constexpr reverse_iterator rend() noexcept
	{
		return reverse_iterator{imp.begin_ptr};
	}
	[[nodiscard]] inline constexpr const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator{imp.curr_ptr};
	}
	[[nodiscard]] inline constexpr const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator{imp.begin_ptr};
	}
	[[nodiscard]] inline constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator{imp.curr_ptr};
	}
	[[nodiscard]] inline constexpr const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator{imp.begin_ptr};
	}

	inline constexpr void clear_destroy() noexcept
	{
		this->destroy();
		imp = {};
	}

	template <typename... Args>
		requires std::constructible_from<value_type, Args...>
	inline constexpr reference emplace_back(Args &&...args) noexcept(::std::is_nothrow_constructible_v<value_type, Args...>)
	{
		if (imp.curr_ptr == imp.end_ptr)
#if __has_cpp_attribute(unlikely)
			[[unlikely]]
#endif
		{
			grow_twice_impl();
		}
		auto p{::std::construct_at(imp.curr_ptr, ::std::forward<Args>(args)...)};
		++imp.curr_ptr;
		return *p;
	}

private:
	inline constexpr iterator insert_impl(pointer iter, value_type &&tmp) noexcept
	{
		auto ret = ::std::construct_at(this->move_backward_common_impl(iter), ::std::move(tmp));
		++imp.curr_ptr;
		return ret;
	}

public:
	template <typename... Args>
		requires std::constructible_from<value_type, Args...>
	inline constexpr iterator emplace(const_iterator iter, Args &&...args) noexcept(::std::is_nothrow_constructible_v<value_type, Args...>)
	{
		if constexpr (::std::is_nothrow_constructible_v<value_type, Args...>)
		{
			pointer ret;
			if (__builtin_is_constant_evaluated())
			{
				auto beginptr{imp.begin_ptr};
				ret = ::std::construct_at(this->move_backward_common_impl(iter - beginptr + beginptr), ::std::forward<Args>(args)...);
			}
			else
			{
				ret = ::std::construct_at(this->move_backward_common_impl(const_cast<pointer>(iter)), ::std::forward<Args>(args)...);
			}
			++imp.curr_ptr;
			return ret;
		}
		else
		{
			if (__builtin_is_constant_evaluated())
			{
				auto beginptr{imp.begin_ptr};
				return this->insert_impl(iter - beginptr + beginptr, value_type(::std::forward<Args>(args)...));
			}
			else
			{
				return this->insert_impl(const_cast<pointer>(iter), value_type(::std::forward<Args>(args)...));
			}
		}
	}

	template <typename... Args>
		requires std::constructible_from<value_type, Args...>
	inline constexpr reference emplace_index(size_type idx, Args &&...args) noexcept(::std::is_nothrow_constructible_v<value_type, Args...>)
	{
		auto beginptr{imp.begin_ptr};
		size_type sz{static_cast<size_type>(imp.curr_ptr - beginptr)};
		if (sz < idx)
		{
			::fast_io::fast_terminate();
		}
		return *this->emplace(beginptr + idx, ::std::forward<Args>(args)...);
	}

	inline constexpr iterator insert(const_iterator iter, const_reference val) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		return this->emplace(iter, val);
	}

	inline constexpr iterator insert(const_iterator iter, value_type &&val) noexcept(::std::is_nothrow_move_constructible_v<value_type>)
	{
		return this->emplace(iter, ::std::move(val));
	}

	inline constexpr reference insert_index(size_type idx, const_reference val) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		return this->emplace_index(idx, val);
	}

	inline constexpr reference insert_index(size_type idx, value_type &&val) noexcept(::std::is_nothrow_move_constructible_v<value_type>)
	{
		return this->emplace_index(idx, ::std::move(val));
	}

private:
	inline constexpr pointer erase_common(pointer it) noexcept
	{
		auto lastele{imp.curr_ptr};
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			::std::destroy_at(it);
		}
		::fast_io::freestanding::uninitialized_relocate(it + 1, lastele, it);
		imp.curr_ptr = lastele - 1;
		return it;
	}

	inline constexpr pointer erase_iters_common(pointer first, pointer last) noexcept
	{
		auto currptr{imp.curr_ptr};
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			::std::destroy(first, last);
		}
		imp.curr_ptr = ::fast_io::freestanding::uninitialized_relocate(last, currptr, first);
		return first;
	}

public:
	inline constexpr iterator erase(const_iterator it) noexcept
	{
		if (__builtin_is_constant_evaluated())
		{
			return this->erase_common(it - imp.begin_ptr + imp.begin_ptr);
		}
		else
		{
			return this->erase_common(const_cast<pointer>(it));
		}
	}

	inline constexpr void erase_index(size_type idx) noexcept
	{
		auto beginptr{imp.begin_ptr};
		auto currptr{imp.curr_ptr};
		size_type sz{static_cast<size_type>(currptr - beginptr)};
		if (sz <= idx)
		{
			::fast_io::fast_terminate();
		}
		this->erase_common(beginptr + idx);
	}

	inline constexpr iterator erase(const_iterator first, const_iterator last) noexcept
	{
		if (__builtin_is_constant_evaluated())
		{
			return this->erase_iters_common(first - imp.begin_ptr + imp.begin_ptr, last - imp.begin_ptr + imp.begin_ptr);
		}
		else
		{
			return this->erase_iters_common(const_cast<pointer>(first), const_cast<pointer>(last));
		}
	}

	inline constexpr void erase_index(size_type firstidx, size_type lastidx) noexcept
	{
		auto beginptr{imp.begin_ptr};
		auto currptr{imp.curr_ptr};
		size_type sz{static_cast<size_type>(currptr - beginptr)};
		if (lastidx < firstidx || sz < lastidx)
		{
			::fast_io::fast_terminate();
		}
		this->erase_iters_common(beginptr + firstidx, beginptr + lastidx);
	}

	inline constexpr void resize(size_type n) noexcept(::std::is_nothrow_default_constructible_v<value_type>)
	{
		auto beginptr{imp.begin_ptr};
		auto currptr{imp.curr_ptr};
		size_type sz{static_cast<size_type>(currptr - beginptr)};
		if (sz < n)
		{
			this->reserve(n);
			::fast_io::freestanding::uninitialized_default_construct(imp.curr_ptr, imp.begin_ptr + n);
			imp.curr_ptr = imp.begin_ptr + n;
		}
		else if (n < sz)
		{
			if constexpr (!::std::is_trivially_destructible_v<value_type>)
			{
				::std::destroy(imp.begin_ptr + n, imp.curr_ptr);
			}
			imp.curr_ptr = imp.begin_ptr + n;
		}
	}

	inline constexpr void resize(size_type n, const_reference val) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		auto beginptr{imp.begin_ptr};
		auto currptr{imp.curr_ptr};
		size_type sz{static_cast<size_type>(currptr - beginptr)};
		if (sz < n)
		{
			this->reserve(n);
			::fast_io::freestanding::uninitialized_fill(imp.curr_ptr, imp.begin_ptr + n, val);
			imp.curr_ptr = imp.begin_ptr + n;
		}
		else if (n < sz)
		{
			if constexpr (!::std::is_trivially_destructible_v<value_type>)
			{
				::std::destroy(imp.begin_ptr + n, imp.curr_ptr);
			}
			imp.curr_ptr = imp.begin_ptr + n;
		}
	}

	inline constexpr void assign(size_type n, const_reference val) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		this->clear();
		this->reserve(n);
		::fast_io::freestanding::uninitialized_fill(imp.begin_ptr, imp.begin_ptr + n, val);
		imp.curr_ptr = imp.begin_ptr + n;
	}

	inline constexpr void swap(vector &other) noexcept
	{
		::std::swap(imp, other.imp);
	}
};

template <typename T, typename allocator1, typename allocator2>
	requires ::std::equality_comparable<T>
inline constexpr bool operator==(vector<T, allocator1> const &lhs, vector<T, allocator2> const &rhs) noexcept
{
	return ::std::equal(lhs.imp.begin_ptr, lhs.imp.curr_ptr, rhs.imp.begin_ptr, rhs.imp.curr_ptr);
}

#if __cpp_impl_three_way_comparison >= 201907L
template <typename T, typename allocator1, typename allocator2>
	requires ::std::three_way_comparable<T>
inline constexpr auto operator<=>(vector<T, allocator1> const &lhs, vector<T, allocator2> const &rhs) noexcept
{
	return ::fast_io::freestanding::lexicographical_compare_three_way(lhs.imp.begin_ptr, lhs.imp.curr_ptr, rhs.imp.begin_ptr, rhs.imp.curr_ptr, ::std::compare_three_way{});
}
#endif

template <typename T, typename allocator>
inline constexpr void swap(vector<T, allocator> &lhs, vector<T, allocator> &rhs) noexcept
{
	lhs.swap(rhs);
}

template <typename ValueType, typename Alloc, typename U>
inline constexpr ::fast_io::containers::vector<ValueType, Alloc>::size_type erase(::fast_io::containers::vector<ValueType, Alloc> &c, U const &value)
{
	auto it = ::std::remove(c.begin(), c.end(), value);
	auto r = c.end() - it;
	c.erase(it, c.end());
	return static_cast<::fast_io::containers::vector<ValueType, Alloc>::size_type>(r);
}

template <typename ValueType, typename Alloc, typename Pred>
inline constexpr ::fast_io::containers::vector<ValueType, Alloc>::size_type erase_if(::fast_io::containers::vector<ValueType, Alloc> &c, Pred pred)
{
	auto it = ::std::remove_if(c.begin(), c.end(), pred);
	auto r = c.end() - it;
	c.erase(it, c.end());
	return static_cast<::fast_io::containers::vector<ValueType, Alloc>::size_type>(r);
}


} // namespace containers

namespace freestanding
{

template <typename T, typename Alloc>
struct is_trivially_copyable_or_relocatable<::fast_io::containers::vector<T, Alloc>>
{
	inline static constexpr bool value = true;
};

template <typename T, typename Alloc>
struct is_zero_default_constructible<::fast_io::containers::vector<T, Alloc>>
{
	inline static constexpr bool value = true;
};

} // namespace freestanding
} // namespace fast_io
