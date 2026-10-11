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

template <typename allochandle>
struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	vector_alloc_handle_model
{
	::std::byte *begin_ptr;
	::std::byte *curr_ptr;
	::std::byte *end_ptr;
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#else
	[[no_unique_address]]
#endif
	allochandle alloc;
};


namespace vector
{

namespace detemplate
{

template <typename allocator>
inline void *grow_to_byte_size_iter_impl(vector_model &imp, void *iter, ::std::size_t newcap, ::std::size_t gap, ::std::size_t size, ::std::size_t alignment)
	FAST_IO_HERBCEPTIONS_THROWS_IF(allocator::throws_on_allocation_failure)
{
	::std::byte *old_begin_ptr{imp.begin_ptr};
	::std::byte *old_curr_ptr{imp.curr_ptr};
	::std::size_t const old_size{static_cast<::std::size_t>(old_curr_ptr - old_begin_ptr)};
	::std::size_t const old_capacity{static_cast<::std::size_t>(imp.end_ptr - old_begin_ptr)};

	::std::byte *begin_ptr;
	::std::size_t newrescount;
	::std::byte *newiter;
#if 0
	if (iter == old_curr_ptr)
	{
		auto newres = allocator::reallocate_aligned_n_at_least(old_begin_ptr, old_capacity, alignment, newcap);
		begin_ptr = reinterpret_cast<::std::byte *>(newres.ptr);
		newrescount = newres.count;
		newiter = begin_ptr + old_size;
	}
	else
#endif
	{
		auto newres = allocator::allocate_aligned_at_least(alignment, newcap);
		begin_ptr = reinterpret_cast<::std::byte *>(newres.ptr);
		newrescount = newres.count;
		newiter = ::fast_io::freestanding::nonoverlapped_bytes_copy(reinterpret_cast<::std::byte const *>(old_begin_ptr), reinterpret_cast<::std::byte const *>(iter),
																	reinterpret_cast<::std::byte *>(begin_ptr));
		static_cast<void>(::fast_io::freestanding::nonoverlapped_bytes_copy(reinterpret_cast<::std::byte const *>(iter), reinterpret_cast<::std::byte const *>(old_curr_ptr),
																			reinterpret_cast<::std::byte *>(newiter + gap)));
		allocator::deallocate_aligned_n(old_begin_ptr, alignment, old_capacity);
	}
	imp.begin_ptr = begin_ptr;
	imp.curr_ptr = begin_ptr + old_size;
	imp.end_ptr = begin_ptr + (newrescount / size * size);
	return newiter;
}

template <typename allocator>
inline void *grow_to_size_iter_impl(vector_model &imp, void *iter, ::std::size_t newcap, ::std::size_t size, ::std::size_t alignment)
	FAST_IO_HERBCEPTIONS_THROWS_IF(allocator::throws_on_allocation_failure || allocator::throws_on_violations)
{
#if defined(_MSC_VER) && !defined(__clang__)
	::std::size_t mx{SIZE_MAX / size};
	if (newcap > mx) [[unlikely]]
	{
		::fast_io::containers::details::contract_violation_report<allocator::throws_on_violations>(::std::errc::value_too_large);
	}
	newcap *= size;
#else
	if (__builtin_mul_overflow(size, newcap, __builtin_addressof(newcap))) [[unlikely]]
	{
		::fast_io::containers::details::contract_violation_report<allocator::throws_on_violations>(::std::errc::value_too_large);
	}
#endif
	return grow_to_byte_size_iter_impl<allocator>(imp, iter, newcap, size, size, alignment);
}

template <typename allocator>
inline void *grow_to_size_impl(vector_model &imp, ::std::size_t newcap, ::std::size_t size, ::std::size_t alignment)
	FAST_IO_HERBCEPTIONS_THROWS_IF(allocator::throws_on_allocation_failure || allocator::throws_on_violations)
{
	return grow_to_size_iter_impl<allocator>(imp, imp.curr_ptr, newcap, size, alignment);
}

template <typename allocator>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void *grow_twice_iter_impl(vector_model &imp, void *iter, ::std::size_t size, ::std::size_t alignment)
	FAST_IO_HERBCEPTIONS_THROWS_IF(allocator::throws_on_allocation_failure || allocator::throws_on_violations)
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
			::fast_io::containers::details::contract_violation_report<allocator::throws_on_violations>(::std::errc::value_too_large);
		}
		toallocate = (diff << 1u);
#else
		if (__builtin_mul_overflow(diff, 2u, __builtin_addressof(toallocate))) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<allocator::throws_on_violations>(::std::errc::value_too_large);
		}
#endif
	}
	return grow_to_byte_size_iter_impl<allocator>(imp, iter, toallocate, size, size, alignment);
}

template <typename allocator>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void *grow_twice_impl(vector_model &imp, ::std::size_t size, ::std::size_t alignment)
	FAST_IO_HERBCEPTIONS_THROWS_IF(allocator::throws_on_allocation_failure || allocator::throws_on_violations)
{
	return grow_twice_iter_impl<allocator>(imp, imp.curr_ptr, size, alignment);
}

template <typename allocator>
inline constexpr void *move_backward_impl(vector_model &imp, void *iter, ::std::size_t size, ::std::size_t alignment)
	FAST_IO_HERBCEPTIONS_THROWS_IF(allocator::throws_on_allocation_failure || allocator::throws_on_violations)
{
	if (imp.curr_ptr == imp.end_ptr) [[unlikely]]
	{
		return grow_twice_iter_impl<allocator>(imp, iter, size, alignment);
	}
	auto currptr{imp.curr_ptr};
	auto currptrp1{currptr + size};
	static_cast<void>(::fast_io::freestanding::uninitialized_move_backward(reinterpret_cast<::std::byte *>(iter), currptr, currptrp1));
	return iter;
}

} // namespace detemplate

template <typename allocator, ::std::size_t size, ::std::size_t alignment>
inline constexpr void *grow_to_size_iter_impl(vector_model &imp, void *iter, ::std::size_t newcap)
	FAST_IO_HERBCEPTIONS_THROWS_IF(allocator::throws_on_allocation_failure || allocator::throws_on_violations)
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
inline constexpr void *grow_to_size_impl(vector_model &imp, ::std::size_t newcap)
	FAST_IO_HERBCEPTIONS_THROWS_IF(allocator::throws_on_allocation_failure || allocator::throws_on_violations)
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
inline constexpr void *grow_twice_impl(vector_model &imp)
	FAST_IO_HERBCEPTIONS_THROWS_IF(allocator::throws_on_allocation_failure || allocator::throws_on_violations)
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
inline constexpr void *grow_twice_iter_impl(vector_model &imp, void *iter)
	FAST_IO_HERBCEPTIONS_THROWS_IF(allocator::throws_on_allocation_failure || allocator::throws_on_violations)
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
inline constexpr void *move_backward_impl(vector_model &imp, void *iter)
	FAST_IO_HERBCEPTIONS_THROWS_IF(allocator::throws_on_allocation_failure || allocator::throws_on_violations)
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

template <::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt>
inline constexpr ::std::size_t iter_distance(Iter first, Snt last)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(++first) || !noexcept(first != last))
{
	::std::size_t n{};
	if constexpr (::std::sized_sentinel_for<Snt, Iter>)
	{
		n = static_cast<::std::size_t>(last - first);
	}
	else
	{
		for (; first != last; ++first)
		{
			++n;
		}
	}
	return n;
}

} // namespace vector

template <typename T>
struct vector_internal
{
	T *begin_ptr{};
	T *curr_ptr{};
	T *end_ptr{};
};

template <typename T, typename Alloc>
struct vector_alloc_handle_internal
{
	T *begin_ptr{};
	T *curr_ptr{};
	T *end_ptr{};
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#else
	[[no_unique_address]]
#endif
	Alloc alloc;
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

	static inline constexpr bool alloc_with_status{typed_allocator_type::has_status};
	static_assert(!alloc_with_status ||
					  ::fast_io::containers::details::defaulted_alignment<value_type, typed_allocator_type::default_alignment>,
				  "handle-based allocators do not support over-aligned element types");
	// Containers never relocate elements through a potentially throwing move
	// constructor: a throwing move would leave both buffers in an unusable
	// state. Trivially relocatable element types relocate by byte copy and
	// need no move constructor at all.
	static_assert(!::fast_io::containers::details::may_throw_move_constructible<value_type> ||
					  ::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<value_type>,
				  "vector element types must be nothrow move constructible or trivially relocatable");
	static_assert(::fast_io::containers::details::never_throw_destructible<value_type>,
				  "vector element destructors must never throw");

	static inline constexpr bool throwing_allocation{typed_allocator_type::throws_on_allocation_failure};
	static inline constexpr bool throwing_violations{typed_allocator_type::throws_on_violations};
	static inline constexpr bool throwing_any{throwing_allocation || throwing_violations};
	// moving elements into a new buffer during growth may throw when the move
	// can throw and the vector cannot fall back to copying
	// relocation never throws: element move constructors are required to be
	// nothrow (or the type is trivially relocatable), and destructors cannot
	// report herbceptions
	static inline constexpr bool throwing_relocation{false};
	// shifting elements inside the buffer (insert/erase) may throw on
	// move assignment; the move constructor is required not to throw and
	// destruction cannot throw
	static inline constexpr bool throwing_shift{
		::fast_io::containers::details::may_throw_move_assignable<value_type>};

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
	using handle_type = typename typed_allocator_type::handle_type;
	::fast_io::containers::details::vector_internal<T> imp;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
	[[no_unique_address]]
#endif
#endif
	handle_type allochdl{};

	inline constexpr vector() noexcept
		requires(!alloc_with_status)
	= default;

	inline explicit constexpr vector(handle_type hdl) noexcept
		requires(alloc_with_status)
		: allochdl{hdl}
	{}

private:
	inline constexpr pointer allocate_elements(size_type n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_allocation)
	{
		if constexpr (alloc_with_status)
		{
			return typed_allocator_type::handle_allocate(allochdl, n);
		}
		else
		{
			return typed_allocator_type::allocate(n);
		}
	}
	inline constexpr pointer allocate_zero_elements(size_type n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_allocation)
	{
		if constexpr (alloc_with_status)
		{
			return typed_allocator_type::handle_allocate_zero(allochdl, n);
		}
		else
		{
			return typed_allocator_type::allocate_zero(n);
		}
	}
	inline constexpr void deallocate_elements_n(pointer ptr, size_type n) noexcept
	{
		if constexpr (alloc_with_status)
		{
			typed_allocator_type::handle_deallocate_n(allochdl, ptr, n);
		}
		else
		{
			typed_allocator_type::deallocate_n(ptr, n);
		}
	}
	// Constructs an empty vector bound to this container's allocator handle.
	inline constexpr vector alloc_empty() noexcept
	{
		if constexpr (alloc_with_status)
		{
			return vector{this->allochdl};
		}
		else
		{
			return vector{};
		}
	}

	inline constexpr void destroy() noexcept
	{
		clear();
		if (imp.begin_ptr == nullptr)
		{
			return;
		}
		if constexpr (alloc_with_status)
		{
			typed_allocator_type::handle_deallocate_n(allochdl, imp.begin_ptr,
													  static_cast<::std::size_t>(imp.end_ptr - imp.begin_ptr));
		}
		else if constexpr (!typed_allocator_type::has_deallocate)
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
		inline constexpr ~run_destroy() noexcept
		{
			if (thisvec)
			{
				thisvec->destroy();
			}
		}
	};

	inline constexpr void default_construct_impl(size_type n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_default_constructible<value_type>)
	{
		auto e{this->imp.end_ptr = (this->imp.curr_ptr = this->imp.begin_ptr =
										this->allocate_elements(n)) +
								   n};
		run_destroy des(this);
		for (; this->imp.curr_ptr != e; ++this->imp.curr_ptr)
		{
			::new (static_cast<void *>(this->imp.curr_ptr)) value_type;
		}
		des.thisvec = nullptr;
	}

	template <typename Iter, typename Sentinel>
	inline constexpr void construct_vector_common_impl(Iter first, Sentinel last)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_constructible<value_type, decltype(*first)> ||
									   !noexcept(++first) || !noexcept(first != last))
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
				size_type n{static_cast<size_type>(::fast_io::containers::details::vector::iter_distance(first, last))};
				if (max_size() < n) [[unlikely]]
				{
					::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::value_too_large);
				}
				auto e{this->imp.end_ptr = (this->imp.curr_ptr = this->imp.begin_ptr =
												this->allocate_elements(n)) +
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
				else if constexpr (!::fast_io::containers::details::may_throw_constructible<value_type, rvaluetype>)
				{
					auto curr{this->imp.begin_ptr};
					for (; curr != e; ++curr)
					{
						::new (static_cast<void *>(curr)) value_type(*first);
						++first;
					}
					this->imp.curr_ptr = e;
				}
				else
				{
					run_destroy des(this);
					for (; this->imp.curr_ptr != e; ++this->imp.curr_ptr)
					{
						::new (static_cast<void *>(this->imp.curr_ptr)) value_type(*first);
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
					static_cast<void>(this->emplace_back(*first));
				}
				des.thisvec = nullptr;
			}
		}
	}

private:
	inline constexpr void ctor_default_size_impl(size_type n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_default_constructible<value_type>)
	{
		if (max_size() < n) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::value_too_large);
		}
		if constexpr (::fast_io::freestanding::is_zero_default_constructible_v<value_type>)
		{
			imp.begin_ptr = this->allocate_zero_elements(n);
			imp.end_ptr = imp.curr_ptr = imp.begin_ptr + n;
		}
		else
		{
			this->default_construct_impl(n);
		}
	}

	inline constexpr void ctor_overwrite_size_impl(size_type n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_default_constructible<value_type>)
	{
		if (max_size() < n) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::value_too_large);
		}
		if constexpr (::std::is_trivially_default_constructible_v<value_type>)
		{
			imp.begin_ptr = this->allocate_elements(n);
			imp.end_ptr = imp.curr_ptr = imp.begin_ptr + n;
		}
		else if constexpr (::fast_io::freestanding::is_zero_default_constructible_v<value_type>)
		{
			imp.begin_ptr = this->allocate_zero_elements(n);
			imp.end_ptr = imp.curr_ptr = imp.begin_ptr + n;
		}
		else
		{
			this->default_construct_impl(n);
		}
	}

	inline constexpr void ctor_fill_size_impl(size_type n, const_reference val)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_copy_constructible<value_type>)
	{
		if (max_size() < n) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::value_too_large);
		}
		auto e{this->imp.end_ptr = (this->imp.curr_ptr = this->imp.begin_ptr =
										this->allocate_elements(n)) +
								   n};
		run_destroy des(this);
		for (; this->imp.curr_ptr != e; ++this->imp.curr_ptr)
		{
			::new (static_cast<void *>(this->imp.curr_ptr)) value_type(val);
		}
		des.thisvec = nullptr;
	}

public:
	inline explicit constexpr vector(size_type n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_default_constructible<value_type>)
		requires(!alloc_with_status)
	{
		this->ctor_default_size_impl(n);
	}

	inline explicit constexpr vector(handle_type hdl, size_type n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_default_constructible<value_type>)
		requires(alloc_with_status)
		: allochdl{hdl}
	{
		this->ctor_default_size_impl(n);
	}

	inline explicit constexpr vector(size_type n, ::fast_io::for_overwrite_t)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_default_constructible<value_type>)
		requires(!alloc_with_status)
	{
		this->ctor_overwrite_size_impl(n);
	}

	inline explicit constexpr vector(handle_type hdl, size_type n, ::fast_io::for_overwrite_t)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_default_constructible<value_type>)
		requires(alloc_with_status)
		: allochdl{hdl}
	{
		this->ctor_overwrite_size_impl(n);
	}

	inline explicit constexpr vector(size_type n, const_reference val)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_copy_constructible<value_type>)
		requires(!alloc_with_status)
	{
		this->ctor_fill_size_impl(n, val);
	}

	inline explicit constexpr vector(handle_type hdl, size_type n, const_reference val)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_copy_constructible<value_type>)
		requires(alloc_with_status)
		: allochdl{hdl}
	{
		this->ctor_fill_size_impl(n, val);
	}

	template <::std::ranges::range R>
	inline explicit constexpr vector(::fast_io::freestanding::from_range_t, R &&rg)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(this->construct_vector_common_impl(::std::ranges::begin(rg), ::std::ranges::end(rg)))
		requires(!alloc_with_status)
	{
		this->construct_vector_common_impl(::std::ranges::begin(rg), ::std::ranges::end(rg));
	}

	template <::std::ranges::range R>
	inline explicit constexpr vector(handle_type hdl, ::fast_io::freestanding::from_range_t, R &&rg)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(this->construct_vector_common_impl(::std::ranges::begin(rg), ::std::ranges::end(rg)))
		requires(alloc_with_status)
		: allochdl{hdl}
	{
		this->construct_vector_common_impl(::std::ranges::begin(rg), ::std::ranges::end(rg));
	}

	inline explicit constexpr vector(::std::initializer_list<value_type> ilist)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_copy_constructible<value_type>)
		requires(!alloc_with_status)
	{
		this->construct_vector_common_impl(ilist.begin(), ilist.end());
	}

	inline explicit constexpr vector(handle_type hdl, ::std::initializer_list<value_type> ilist)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_copy_constructible<value_type>)
		requires(alloc_with_status)
		: allochdl{hdl}
	{
		this->construct_vector_common_impl(ilist.begin(), ilist.end());
	}

	inline constexpr vector(vector const &vec)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_copy_constructible<value_type>)
		: allochdl{vec.allochdl}
	{
		// Using static_assert instead of requires to delay the check
		// related to tests/0026.container/0001.vector/recursive.cc
		static_assert(::fast_io::containers::details::copy_constructible<value_type>, "vector's value type must be copy constructible to use copy constructor");
		std::size_t const vecsize{static_cast<std::size_t>(vec.imp.curr_ptr - vec.imp.begin_ptr)};
		if (vecsize == 0)
		{
			return;
		}
		imp.begin_ptr = this->allocate_elements(vecsize);
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
				static_cast<void>(::fast_io::freestanding::nonoverlapped_bytes_copy(
					reinterpret_cast<::std::byte const *>(vec.imp.begin_ptr),
					reinterpret_cast<::std::byte const *>(vec.imp.curr_ptr),
					reinterpret_cast<::std::byte *>(this->imp.begin_ptr)));
				imp.end_ptr = imp.curr_ptr = imp.begin_ptr + vecsize;
				return;
			}
		}
		run_destroy des(this);
		this->imp.curr_ptr = this->imp.begin_ptr;
		this->imp.end_ptr = this->imp.begin_ptr + vecsize;
		for (auto i{vec.imp.begin_ptr}; i != vec.imp.curr_ptr; ++i)
		{
			::new (static_cast<void *>(this->imp.curr_ptr)) value_type(*i);
			++this->imp.curr_ptr;
		}
		des.thisvec = nullptr;
	}

	inline constexpr vector &operator=(vector const &vec)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_copy_constructible<value_type>)
	{
		// Using static_assert instead of requires to delay the check
		// related to tests/0026.container/0001.vector/recursive.cc
		static_assert(::std::copyable<value_type>, "vector's value type must be copyable to use copy assignment operator");
		if (__builtin_addressof(vec) == this) [[unlikely]]
		{
			return *this;
		}
		vector newvec(vec);
		this->operator=(::std::move(newvec));
		return *this;
	}

	inline constexpr vector(vector &&vec) noexcept
		: imp(vec.imp), allochdl{vec.allochdl}
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
		this->allochdl = vec.allochdl;
		vec.imp = {};
		return *this;
	}
	inline constexpr ~vector() noexcept
	{
		destroy();
	}

	template <typename... Args>
		requires std::constructible_from<value_type, Args...>
	inline constexpr reference emplace_back_unchecked(Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::may_throw_constructible<value_type, Args...>)
	{
		auto p{::new (static_cast<void *>(imp.curr_ptr)) value_type(::std::forward<Args>(args)...)};
		++imp.curr_ptr;
		return *p;
	}

private:
	/*
	Guards a newly allocated buffer while elements are moved or copied into it
	around an insertion hole. When an element operation throws, the guard
	destroys the already constructed elements in both segments and frees the
	buffer so reallocation never leaks.
	*/
	struct grow_realloc_guard
	{
		vector *thisvec{};
		pointer first{};
		pointer mid{};
		pointer tail{};
		pointer last{};
		size_type cap{};
		inline constexpr ~grow_realloc_guard() noexcept
		{
			if (thisvec != nullptr)
			{
				if constexpr (!::std::is_trivially_destructible_v<value_type>)
				{
					for (auto p{first}; p != mid; ++p)
					{
						p->~value_type();
					}
					for (auto p{tail}; p != last; ++p)
					{
						p->~value_type();
					}
				}
				thisvec->deallocate_elements_n(first, cap);
			}
		}
	};

	inline constexpr pointer grow_to_size_iter_impl(size_type newcap, pointer iter, size_type n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation)
	{
		if (max_size() < newcap) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::value_too_large);
		}
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
				if constexpr (alloc_with_status)
				{
					auto new_begin_ptr{typed_allocator_type::handle_allocate(allochdl, newcap)};
					auto newiter{::fast_io::freestanding::nonoverlapped_bytes_copy(
						reinterpret_cast<::std::byte const *>(imp.begin_ptr),
						reinterpret_cast<::std::byte const *>(iter),
						reinterpret_cast<::std::byte *>(new_begin_ptr))};
					static_cast<void>(::fast_io::freestanding::nonoverlapped_bytes_copy(
						reinterpret_cast<::std::byte const *>(iter),
						reinterpret_cast<::std::byte const *>(imp.curr_ptr),
						newiter + n * sizeof(value_type)));
					auto const old_size{static_cast<size_type>(imp.curr_ptr - imp.begin_ptr)};
					if (imp.begin_ptr != nullptr)
					{
						typed_allocator_type::handle_deallocate_n(allochdl, imp.begin_ptr,
																  static_cast<size_type>(imp.end_ptr - imp.begin_ptr));
					}
					imp.begin_ptr = new_begin_ptr;
					imp.curr_ptr = new_begin_ptr + old_size;
					imp.end_ptr = new_begin_ptr + newcap;
					return reinterpret_cast<pointer>(newiter);
				}
				else
				{
					::std::size_t newcapbytes;
					if (__builtin_mul_overflow(sizeof(value_type), newcap, __builtin_addressof(newcapbytes))) [[unlikely]]
					{
						::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::value_too_large);
					}
					return reinterpret_cast<pointer>(::fast_io::containers::details::vector::detemplate::grow_to_byte_size_iter_impl<allocator_type>(
						*reinterpret_cast<::fast_io::containers::details::vector_model *>(__builtin_addressof(imp)),
						iter, newcapbytes, n * sizeof(value_type), sizeof(value_type), alignof(value_type)));
				}
			}
		}
		size_type new_count;
		pointer new_begin_ptr;
		if constexpr (alloc_with_status)
		{
			new_begin_ptr = typed_allocator_type::handle_allocate(allochdl, newcap);
			new_count = newcap;
		}
		else
		{
			auto newres{typed_allocator_type::allocate_at_least(newcap)};
			new_begin_ptr = newres.ptr;
			new_count = newres.count;
		}
		auto old_begin_ptr{imp.begin_ptr};
		auto old_curr_ptr{imp.curr_ptr};
		size_type const old_size{static_cast<size_type>(old_curr_ptr - old_begin_ptr)};
		size_type const old_capacity{static_cast<size_type>(imp.end_ptr - old_begin_ptr)};
		grow_realloc_guard guard{this, new_begin_ptr, new_begin_ptr, new_begin_ptr, new_begin_ptr, new_count};
		if constexpr (!::fast_io::containers::details::may_throw_move_constructible<value_type> || !::fast_io::containers::details::copy_constructible<value_type>)
		{
			for (auto src{old_begin_ptr}; src != iter; ++src)
			{
				::new (static_cast<void *>(guard.mid)) value_type(::std::move(*src));
				if constexpr (!::std::is_trivially_destructible_v<value_type>)
				{
					src->~value_type();
				}
				++guard.mid;
			}
			guard.tail = guard.last = guard.mid + n;
			for (auto src{iter}; src != old_curr_ptr; ++src)
			{
				::new (static_cast<void *>(guard.last)) value_type(::std::move(*src));
				if constexpr (!::std::is_trivially_destructible_v<value_type>)
				{
					src->~value_type();
				}
				++guard.last;
			}
		}
		else
		{
			// copy instead of moving so a throwing move/copy constructor can
			// never leave the original range partially destroyed
			for (auto src{old_begin_ptr}; src != iter; ++src)
			{
				::new (static_cast<void *>(guard.mid)) value_type(*src);
				++guard.mid;
			}
			guard.tail = guard.last = guard.mid + n;
			for (auto src{iter}; src != old_curr_ptr; ++src)
			{
				::new (static_cast<void *>(guard.last)) value_type(*src);
				++guard.last;
			}
		}
		auto newiter{guard.mid};
		guard.thisvec = nullptr;
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			for (auto p{old_begin_ptr}; p != old_curr_ptr; ++p)
			{
				p->~value_type();
			}
		}
		if constexpr (alloc_with_status)
		{
			if (old_begin_ptr != nullptr)
			{
				typed_allocator_type::handle_deallocate_n(allochdl, old_begin_ptr, old_capacity);
			}
		}
		else if constexpr (typed_allocator_type::has_deallocate)
		{
			typed_allocator_type::deallocate(old_begin_ptr);
		}
		else
		{
			typed_allocator_type::deallocate_n(old_begin_ptr, old_capacity);
		}
		imp.begin_ptr = new_begin_ptr;
		imp.curr_ptr = new_begin_ptr + old_size;
		imp.end_ptr = new_begin_ptr + new_count;
		return newiter;
	}

#if __has_cpp_attribute(__gnu__::__cold__)
	[[__gnu__::__cold__]]
#endif
	inline constexpr pointer grow_twice_iter_impl(pointer iter)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation)
	{
		if constexpr (::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<value_type> && !alloc_with_status)
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
		return this->grow_to_size_iter_impl(::fast_io::containers::details::cal_grow_twice_size<sizeof(value_type), false, throwing_violations>(cap), iter, 1);
	}

	/*
	Shifts [iter, curr_ptr) one element to the right into the uninitialized slot
	at curr_ptr. Requires curr_ptr != end_ptr. Elements between iter and
	curr_ptr stay live (moved-from) after the shift; the caller writes the new
	element into iter itself.
	*/
	inline constexpr void move_backward_one_impl(pointer iter)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::may_throw_move_constructible<value_type> ||
									   ::fast_io::containers::details::may_throw_move_assignable<value_type>)
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
				static_cast<void>(::fast_io::freestanding::uninitialized_move_backward(iter, imp.curr_ptr, imp.curr_ptr + 1));
				return;
			}
		}
		auto currptr{imp.curr_ptr};
		::new (static_cast<void *>(currptr)) value_type(::std::move(currptr[-1]));
		for (auto p{currptr - 1}; p != iter; --p)
		{
			*p = ::std::move(p[-1]);
		}
	}

	inline constexpr void grow_to_size_impl(size_type newcap)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation)
	{
		if (max_size() < newcap) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::value_too_large);
		}
		if constexpr (::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<value_type> && !alloc_with_status)
		{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
#endif
			{
				static_cast<void>(::fast_io::containers::details::vector::grow_to_size_impl<allocator_type, sizeof(value_type), alignof(value_type)>(
					*reinterpret_cast<::fast_io::containers::details::vector_model *>(__builtin_addressof(imp)), newcap));
				return;
			}
		}
		static_cast<void>(this->grow_to_size_iter_impl(newcap, imp.curr_ptr, 1));
	}
#if __has_cpp_attribute(__gnu__::__cold__)
	[[__gnu__::__cold__]]
#endif
	inline constexpr void grow_twice_impl()
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation)
	{
		if constexpr (::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<value_type> && !alloc_with_status)
		{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
#endif
			{
				static_cast<void>(::fast_io::containers::details::vector::grow_twice_impl<allocator_type, sizeof(value_type), alignof(value_type)>(
					*reinterpret_cast<::fast_io::containers::details::vector_model *>(__builtin_addressof(imp))));
				return;
			}
		}
		std::size_t const cap{static_cast<size_type>(imp.end_ptr - imp.begin_ptr)};
		grow_to_size_impl(::fast_io::containers::details::cal_grow_twice_size<sizeof(value_type), false, throwing_violations>(cap));
	}

public:
	inline constexpr void reserve(size_type n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation)
	{
		if (n <= static_cast<std::size_t>(imp.end_ptr - imp.begin_ptr))
		{
			return;
		}
		grow_to_size_impl(n);
	}

	inline constexpr void shrink_to_fit()
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation)
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
	inline constexpr void pop_back()
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (imp.curr_ptr == imp.begin_ptr) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		(--imp.curr_ptr)->~value_type();
	}

	inline constexpr void pop_back_unchecked() noexcept
	{
		(--imp.curr_ptr)->~value_type();
	}

	inline constexpr void push_back(T const &value)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation || ::fast_io::containers::details::may_throw_copy_constructible<value_type>)
	{
		static_cast<void>(this->emplace_back(value));
	}
	inline constexpr void push_back(T &&value)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation || ::fast_io::containers::details::may_throw_move_constructible<value_type>)
	{
		static_cast<void>(this->emplace_back(::std::move(value)));
	}
	inline constexpr void push_back_unchecked(T const &value)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::may_throw_copy_constructible<value_type>)
	{
		static_cast<void>(this->emplace_back_unchecked(value));
	}
	inline constexpr void push_back_unchecked(T &&value)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::may_throw_move_constructible<value_type>)
	{
		static_cast<void>(this->emplace_back_unchecked(::std::move(value)));
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

	inline constexpr void clear() noexcept
	{
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			for (auto p{imp.begin_ptr}; p != imp.curr_ptr; ++p)
			{
				p->~value_type();
			}
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
	operator[](size_type pos) const FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		auto begin_ptr{imp.begin_ptr}, curr_ptr{imp.curr_ptr};
		if (static_cast<::std::size_t>(curr_ptr - begin_ptr) <= pos) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		return begin_ptr[pos];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]] inline constexpr reference
	operator[](size_type pos) FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		auto begin_ptr{imp.begin_ptr}, curr_ptr{imp.curr_ptr};
		if (static_cast<::std::size_t>(curr_ptr - begin_ptr) <= pos) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		return begin_ptr[pos];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]] inline constexpr const_reference
	front() const FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		auto begin_ptr{imp.begin_ptr}, curr_ptr{imp.curr_ptr};
		if (begin_ptr == curr_ptr) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		return *begin_ptr;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]] inline constexpr reference
	front() FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		auto begin_ptr{imp.begin_ptr}, curr_ptr{imp.curr_ptr};
		if (begin_ptr == curr_ptr) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		return *begin_ptr;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]] inline constexpr const_reference
	back() const FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		auto begin_ptr{imp.begin_ptr}, curr_ptr{imp.curr_ptr};
		if (begin_ptr == curr_ptr) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		return curr_ptr[-1];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]] inline constexpr reference
	back() FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		auto begin_ptr{imp.begin_ptr}, curr_ptr{imp.curr_ptr};
		if (begin_ptr == curr_ptr) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
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
	inline constexpr reference emplace_back(Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation ||
									   ::fast_io::containers::details::may_throw_constructible<value_type, Args...>)
	{
		if (imp.curr_ptr == imp.end_ptr)
			[[unlikely]]
		{
			grow_twice_impl();
		}
		auto p{::new (static_cast<void *>(imp.curr_ptr)) value_type(::std::forward<Args>(args)...)};
		++imp.curr_ptr;
		return *p;
	}

private:
	struct append_range_guard
	{
		vector *thisvec{};
		size_type oldn{};
		constexpr ~append_range_guard() noexcept
		{
			if (thisvec)
			{
				auto newcurr{thisvec->imp.begin_ptr + static_cast<::std::ptrdiff_t>(oldn)};
				if constexpr (!::std::is_trivially_destructible_v<value_type>)
				{
					for (auto p{newcurr}; p != thisvec->imp.curr_ptr; ++p)
					{
						p->~value_type();
					}
				}
				thisvec->imp.curr_ptr = newcurr;
			}
		}
	};

public:
	template <::std::ranges::range R>
		requires ::std::constructible_from<value_type, ::std::ranges::range_value_t<R>>
	inline constexpr void append_range(R &&rg)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation || throwing_shift ||
									   ::fast_io::containers::details::may_throw_constructible<value_type, decltype(*::std::ranges::begin(rg))>)
	{
		if constexpr (::std::ranges::sized_range<R>)
		{
			size_type const rgsize{::std::ranges::size(rg)};
			if (!rgsize)
			{
				return;
			}
			size_type const old_size{static_cast<size_type>(imp.curr_ptr - imp.begin_ptr)};
			if (max_size() - old_size < rgsize) [[unlikely]]
			{
				::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::value_too_large);
			}
			size_type const new_size{old_size + rgsize};
			size_type const cap{static_cast<size_type>(imp.end_ptr - imp.begin_ptr)};
			if (new_size > cap)
			{
				this->grow_to_size_impl(new_size);
			}
			if constexpr (!::fast_io::containers::details::may_throw_constructible<value_type, decltype(*::std::ranges::begin(rg))>)
			{
				for (auto &e : rg)
				{
					::new (static_cast<void *>(imp.curr_ptr)) value_type(::std::forward<decltype(e)>(e));
					++imp.curr_ptr;
				}
			}
			else
			{
				append_range_guard guard{this, old_size};
				for (auto &e : rg)
				{
					::new (static_cast<void *>(imp.curr_ptr)) value_type(::std::forward<decltype(e)>(e));
					++imp.curr_ptr;
				}
				guard.thisvec = nullptr;
			}
		}
		else
		{
			if constexpr (!::fast_io::containers::details::may_throw_constructible<value_type, decltype(*::std::ranges::begin(rg))>)
			{
				for (auto &e : rg)
				{
					static_cast<void>(this->emplace_back(::std::forward<decltype(e)>(e)));
				}
			}
			else
			{
				append_range_guard guard{this, this->size()};
				for (auto &e : rg)
				{
					static_cast<void>(this->emplace_back(::std::forward<decltype(e)>(e)));
				}
				guard.thisvec = nullptr;
			}
		}
	}

private:
	/*
	Places a new element at it after growing or shifting. Used when the element
	can be constructed from args without throwing; the slot at it after a shift
	is a live moved-from object and is reconstructed.
	*/
	template <typename... Args>
	inline constexpr iterator emplace_slot_construct_impl(pointer it, Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation || throwing_shift)
	{
		if (imp.curr_ptr == imp.end_ptr) [[unlikely]]
		{
			it = this->grow_twice_iter_impl(it);
		}
		else if (it != imp.curr_ptr)
		{
			this->move_backward_one_impl(it);
			// it still refers to a live moved-from object
			it->~value_type();
			auto ret{::new (static_cast<void *>(it)) value_type(::std::forward<Args>(args)...)};
			++imp.curr_ptr;
			return ret;
		}
		auto ret{::new (static_cast<void *>(it)) value_type(::std::forward<Args>(args)...)};
		++imp.curr_ptr;
		return ret;
	}

	/*
	Same as emplace_slot_construct_impl but takes a pre-materialized element so
	a throwing element constructor runs before the vector is touched.
	*/
	inline constexpr iterator emplace_move_tmp_impl(pointer it, value_type &&tmp)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation || throwing_shift)
	{
		if (imp.curr_ptr == imp.end_ptr) [[unlikely]]
		{
			it = this->grow_twice_iter_impl(it);
		}
		else if (it != imp.curr_ptr)
		{
			this->move_backward_one_impl(it);
			if constexpr (::std::is_move_assignable_v<value_type>)
			{
				*it = ::std::move(tmp);
			}
			else
			{
				it->~value_type();
				::new (static_cast<void *>(it)) value_type(::std::move(tmp));
			}
			++imp.curr_ptr;
			return it;
		}
		auto ret{::new (static_cast<void *>(it)) value_type(::std::move(tmp))};
		++imp.curr_ptr;
		return ret;
	}

public:
	template <typename... Args>
		requires std::constructible_from<value_type, Args...>
	inline constexpr iterator emplace(const_iterator iter, Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation || throwing_shift ||
									   ::fast_io::containers::details::may_throw_constructible<value_type, Args...>)
	{
		pointer it;
		if (__builtin_is_constant_evaluated())
		{
			auto beginptr{imp.begin_ptr};
			it = iter - beginptr + beginptr;
		}
		else
		{
			it = const_cast<pointer>(iter);
		}
		if constexpr (::fast_io::containers::details::may_throw_constructible<value_type, Args...>)
		{
			return this->emplace_move_tmp_impl(it, value_type(::std::forward<Args>(args)...));
		}
		else
		{
			return this->emplace_slot_construct_impl(it, ::std::forward<Args>(args)...);
		}
	}

	template <typename... Args>
		requires std::constructible_from<value_type, Args...>
	inline constexpr reference emplace_index(size_type idx, Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation || throwing_shift ||
									   ::fast_io::containers::details::may_throw_constructible<value_type, Args...>)
	{
		auto beginptr{imp.begin_ptr};
		size_type sz{static_cast<size_type>(imp.curr_ptr - beginptr)};
		if (sz < idx) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		return *this->emplace(beginptr + idx, ::std::forward<Args>(args)...);
	}

	inline constexpr iterator insert(const_iterator iter, const_reference val)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation || throwing_shift ||
									   ::fast_io::containers::details::may_throw_copy_constructible<value_type>)
	{
		return this->emplace(iter, val);
	}

	inline constexpr iterator insert(const_iterator iter, value_type &&val)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation || throwing_shift)
	{
		return this->emplace(iter, ::std::move(val));
	}

	inline constexpr reference insert_index(size_type idx, const_reference val)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(this->emplace_index(idx, val))
	{
		return this->emplace_index(idx, val);
	}

	inline constexpr reference insert_index(size_type idx, value_type &&val)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(this->emplace_index(idx, ::std::move(val)))
	{
		return this->emplace_index(idx, ::std::move(val));
	}

private:
	/*
	Rolls a partially filled insertion hole back: destroys the elements that
	were constructed into the hole and moves the shifted tail back into place.
	*/
	struct insert_hole_guard
	{
		vector *thisvec;
		pointer first;
		size_type count;
		size_type filled;
		constexpr ~insert_hole_guard()
		{
			if (thisvec) [[unlikely]]
			{
				if constexpr (!::std::is_trivially_destructible_v<value_type>)
				{
					auto filllast{first + static_cast<::std::ptrdiff_t>(filled)};
					for (auto p{first}; p != filllast; ++p)
					{
						p->~value_type();
					}
				}
				::fast_io::freestanding::uninitialized_relocate(
					first + static_cast<::std::ptrdiff_t>(count), thisvec->imp.curr_ptr, first);
				thisvec->imp.curr_ptr -= static_cast<::std::ptrdiff_t>(count);
			}
		}
	};

	// Carves a count-wide uninitialized hole at it and fills it with val.
	inline constexpr iterator insert_count_impl(pointer it, size_type count, const_reference val)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation || throwing_shift ||
									   ::fast_io::containers::details::may_throw_copy_constructible<value_type>)
	{
		size_type const old_size{static_cast<size_type>(imp.curr_ptr - imp.begin_ptr)};
		if (max_size() - old_size < count) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::value_too_large);
		}
		size_type const new_size{old_size + count};
		if (static_cast<size_type>(imp.end_ptr - imp.curr_ptr) < count)
		{
			it = this->grow_to_size_iter_impl(new_size, it, count);
		}
		else
		{
			::fast_io::freestanding::uninitialized_relocate_backward(
				it, imp.curr_ptr, imp.curr_ptr + static_cast<::std::ptrdiff_t>(count));
		}
		imp.curr_ptr = imp.begin_ptr + static_cast<::std::ptrdiff_t>(new_size);
		if constexpr (::std::is_nothrow_copy_constructible_v<value_type>)
		{
			::fast_io::freestanding::uninitialized_fill_n(it, count, val);
		}
		else
		{
			insert_hole_guard guard{this, it, count, 0};
			for (; guard.filled != count; ++guard.filled)
			{
				::new (static_cast<void *>(it + static_cast<::std::ptrdiff_t>(guard.filled))) value_type(val);
			}
			guard.thisvec = nullptr;
		}
		return it;
	}

	template <::std::ranges::range R>
	inline constexpr iterator insert_range_hole_impl(pointer it, R &&rg, size_type rgsize)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation || throwing_shift ||
									   ::fast_io::containers::details::may_throw_constructible<value_type, decltype(*::std::ranges::begin(rg))>)
	{
		size_type const old_size{static_cast<size_type>(imp.curr_ptr - imp.begin_ptr)};
		if (max_size() - old_size < rgsize) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::value_too_large);
		}
		size_type const new_size{old_size + rgsize};
		if (static_cast<size_type>(imp.end_ptr - imp.curr_ptr) < rgsize)
		{
			it = this->grow_to_size_iter_impl(new_size, it, rgsize);
		}
		else
		{
			::fast_io::freestanding::uninitialized_relocate_backward(
				it, imp.curr_ptr, imp.curr_ptr + static_cast<::std::ptrdiff_t>(rgsize));
		}
		imp.curr_ptr = imp.begin_ptr + static_cast<::std::ptrdiff_t>(new_size);
		if constexpr (!::fast_io::containers::details::may_throw_constructible<value_type, decltype(*::std::ranges::begin(rg))>)
		{
			auto d{it};
			for (auto &e : rg)
			{
				::new (static_cast<void *>(d)) value_type(::std::forward<decltype(e)>(e));
				++d;
			}
		}
		else
		{
			insert_hole_guard guard{this, it, rgsize, 0};
			for (auto &e : rg)
			{
				::new (static_cast<void *>(it + static_cast<::std::ptrdiff_t>(guard.filled))) value_type(::std::forward<decltype(e)>(e));
				++guard.filled;
			}
			guard.thisvec = nullptr;
		}
		return it;
	}

	// Rotates [first, middle, last) left so that middle becomes the new first.
	inline static constexpr void rotate_range_impl(pointer first, pointer middle, pointer last)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_shift)
	{
		// reverse all three segments; swap on move-assign may throw -> covered by throwing_shift
		auto rev{[](pointer b, pointer e) {
			while (b < e)
			{
				--e;
				::fast_io::freestanding::iter_swap(b, e);
				++b;
			}
		}};
		rev(first, middle);
		rev(middle, last);
		rev(first, last);
	}

public:
	inline constexpr iterator insert(const_iterator iter, size_type count, const_reference val)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation || throwing_shift ||
									   ::fast_io::containers::details::may_throw_copy_constructible<value_type>)
	{
		pointer it;
		if (__builtin_is_constant_evaluated())
		{
			auto beginptr{imp.begin_ptr};
			it = iter - beginptr + beginptr;
		}
		else
		{
			it = const_cast<pointer>(iter);
		}
		if (!count)
		{
			return it;
		}
		return this->insert_count_impl(it, count, val);
	}

	inline constexpr size_type insert_index(size_type idx, size_type count, const_reference val)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations || throwing_any || throwing_relocation || throwing_shift ||
									   ::fast_io::containers::details::may_throw_copy_constructible<value_type>)
	{
		auto beginptr{imp.begin_ptr};
		size_type const sz{static_cast<size_type>(imp.curr_ptr - beginptr)};
		if (sz < idx) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		if (!count)
		{
			return idx;
		}
		this->insert_count_impl(beginptr + static_cast<::std::ptrdiff_t>(idx), count, val);
		return idx;
	}

	template <::std::ranges::range R>
		requires ::std::constructible_from<value_type, ::std::ranges::range_value_t<R>>
	inline constexpr iterator insert_range(const_iterator iter, R &&rg)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation || throwing_shift ||
									   ::fast_io::containers::details::may_throw_constructible<value_type, decltype(*::std::ranges::begin(rg))>)
	{
		pointer it;
		if (__builtin_is_constant_evaluated())
		{
			auto beginptr{imp.begin_ptr};
			it = iter - beginptr + beginptr;
		}
		else
		{
			it = const_cast<pointer>(iter);
		}
		if constexpr (::std::ranges::sized_range<R>)
		{
			size_type const rgsize{::std::ranges::size(rg)};
			if (!rgsize)
			{
				return it;
			}
			return this->insert_range_hole_impl(it, ::std::forward<R>(rg), rgsize);
		}
		else
		{
			size_type const pos{static_cast<size_type>(it - imp.begin_ptr)};
			size_type const old_size{static_cast<size_type>(imp.curr_ptr - imp.begin_ptr)};
			this->append_range(::std::forward<R>(rg));
			auto beginptr{imp.begin_ptr};
			this->rotate_range_impl(beginptr + static_cast<::std::ptrdiff_t>(pos),
									beginptr + static_cast<::std::ptrdiff_t>(old_size), imp.curr_ptr);
			return beginptr + static_cast<::std::ptrdiff_t>(pos);
		}
	}

	template <::std::ranges::range R>
		requires ::std::constructible_from<value_type, ::std::ranges::range_value_t<R>>
	inline constexpr size_type insert_range_index(size_type idx, R &&rg)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations || throwing_any || throwing_relocation || throwing_shift ||
									   ::fast_io::containers::details::may_throw_constructible<value_type, decltype(*::std::ranges::begin(rg))>)
	{
		size_type const sz{static_cast<size_type>(imp.curr_ptr - imp.begin_ptr)};
		if (sz < idx) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		return static_cast<size_type>(this->insert_range(imp.begin_ptr + static_cast<::std::ptrdiff_t>(idx), ::std::forward<R>(rg)) - imp.begin_ptr);
	}


private:
	inline constexpr pointer erase_common(pointer it)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_shift)
	{
		auto lastele{imp.curr_ptr};
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
				if constexpr (!::std::is_trivially_destructible_v<value_type>)
				{
					it->~value_type();
				}
				::fast_io::freestanding::uninitialized_relocate(it + 1, lastele, it);
				imp.curr_ptr = lastele - 1;
				return it;
			}
		}
		for (auto d{it}; d + 1 != lastele; ++d)
		{
			*d = ::std::move(d[1]);
		}
		imp.curr_ptr = lastele - 1;
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			lastele[-1].~value_type();
		}
		return it;
	}

	inline constexpr pointer erase_iters_common(pointer first, pointer last)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_shift)
	{
		auto currptr{imp.curr_ptr};
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
				if constexpr (!::std::is_trivially_destructible_v<value_type>)
				{
					for (auto p{first}; p != last; ++p)
					{
						p->~value_type();
					}
				}
				// relocation consumes the source images; the stale tail bytes
				// must not be destroyed
				imp.curr_ptr = ::fast_io::freestanding::uninitialized_relocate(last, currptr, first);
				return first;
			}
		}
		auto d{first};
		for (auto s{last}; s != currptr; ++s, ++d)
		{
			*d = ::std::move(*s);
		}
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			for (auto p{d}; p != currptr; ++p)
			{
				p->~value_type();
			}
		}
		imp.curr_ptr = d;
		return first;
	}

public:
	inline constexpr iterator erase(const_iterator it)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_shift)
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

	inline constexpr size_type erase_index(size_type idx)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations || throwing_shift)
	{
		auto beginptr{imp.begin_ptr};
		auto currptr{imp.curr_ptr};
		size_type sz{static_cast<size_type>(currptr - beginptr)};
		if (sz <= idx) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		static_cast<void>(this->erase_common(beginptr + idx));
		return idx;
	}

	inline constexpr iterator erase(const_iterator first, const_iterator last)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_shift)
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

	inline constexpr size_type erase_index(size_type firstidx, size_type lastidx)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations || throwing_shift)
	{
		auto beginptr{imp.begin_ptr};
		auto currptr{imp.curr_ptr};
		size_type sz{static_cast<size_type>(currptr - beginptr)};
		if (lastidx < firstidx || sz < lastidx) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		static_cast<void>(this->erase_iters_common(beginptr + firstidx, beginptr + lastidx));
		return firstidx;
	}

	inline constexpr void resize(size_type n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation ||
									   ::fast_io::containers::details::may_throw_default_constructible<value_type>)
	{
		auto beginptr{imp.begin_ptr};
		auto currptr{imp.curr_ptr};
		size_type sz{static_cast<size_type>(currptr - beginptr)};
		if (sz < n)
		{
			this->reserve(n);
			auto const e{imp.begin_ptr + n};
			// advance curr_ptr per element so a throwing default constructor
			// leaves the partially filled range owned by the vector
			for (; imp.curr_ptr != e; ++imp.curr_ptr)
			{
				::new (static_cast<void *>(imp.curr_ptr)) value_type;
			}
		}
		else if (n < sz)
		{
			if constexpr (!::std::is_trivially_destructible_v<value_type>)
			{
				for (auto p{imp.begin_ptr + n}; p != imp.curr_ptr; ++p)
				{
					p->~value_type();
				}
			}
			imp.curr_ptr = imp.begin_ptr + n;
		}
	}

	inline constexpr void resize(size_type n, const_reference val)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation ||
									   ::fast_io::containers::details::may_throw_copy_constructible<value_type>)
	{
		auto beginptr{imp.begin_ptr};
		auto currptr{imp.curr_ptr};
		size_type sz{static_cast<size_type>(currptr - beginptr)};
		if (sz < n)
		{
			this->reserve(n);
			auto const e{imp.begin_ptr + n};
			for (; imp.curr_ptr != e; ++imp.curr_ptr)
			{
				::new (static_cast<void *>(imp.curr_ptr)) value_type(val);
			}
		}
		else if (n < sz)
		{
			if constexpr (!::std::is_trivially_destructible_v<value_type>)
			{
				for (auto p{imp.begin_ptr + n}; p != imp.curr_ptr; ++p)
				{
					p->~value_type();
				}
			}
			imp.curr_ptr = imp.begin_ptr + n;
		}
	}

	inline constexpr void resize(size_type n, ::fast_io::for_overwrite_t)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation)
	{
		size_type const sz{static_cast<size_type>(imp.curr_ptr - imp.begin_ptr)};
		if (sz < n)
		{
			// growing leaves the appended elements uninitialized
			this->reserve(n);
			imp.curr_ptr = imp.begin_ptr + n;
		}
		else if (n < sz)
		{
			if constexpr (!::std::is_trivially_destructible_v<value_type>)
			{
				for (auto p{imp.begin_ptr + n}; p != imp.curr_ptr; ++p)
				{
					p->~value_type();
				}
			}
			imp.curr_ptr = imp.begin_ptr + n;
		}
	}

	inline constexpr void assign(size_type n, const_reference val)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation ||
									   ::fast_io::containers::details::may_throw_copy_constructible<value_type>)
	{
		this->clear();
		this->reserve(n);
		for (auto const e{imp.begin_ptr + n}; imp.curr_ptr != e; ++imp.curr_ptr)
		{
			::new (static_cast<void *>(imp.curr_ptr)) value_type(val);
		}
	}

	template <::std::ranges::range R>
		requires ::std::constructible_from<value_type, ::std::ranges::range_value_t<R>>
	inline constexpr void assign_range(R &&rg)
		FAST_IO_HERBCEPTIONS_THROWS_IF(
			!noexcept(this->construct_vector_common_impl(::std::ranges::begin(rg), ::std::ranges::end(rg))))
	{
		auto temp{this->alloc_empty()};
		temp.construct_vector_common_impl(::std::ranges::begin(rg), ::std::ranges::end(rg));
		this->swap(temp);
	}

	inline constexpr void swap(vector &other) noexcept
	{
		::std::swap(imp, other.imp);
		if constexpr (alloc_with_status)
		{
			::std::swap(allochdl, other.allochdl);
		}
	}
};

template <typename T, typename allocator1, typename allocator2>
	requires ::std::equality_comparable<T>
inline constexpr bool operator==(vector<T, allocator1> const &lhs, vector<T, allocator2> const &rhs)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(*lhs.imp.begin_ptr == *rhs.imp.begin_ptr)
{
	return ::fast_io::freestanding::equal(lhs.imp.begin_ptr, lhs.imp.curr_ptr, rhs.imp.begin_ptr, rhs.imp.curr_ptr);
}

#if __cpp_impl_three_way_comparison >= 201907L
template <typename T, typename allocator1, typename allocator2>
	requires ::std::three_way_comparable<T>
inline constexpr auto operator<=>(vector<T, allocator1> const &lhs, vector<T, allocator2> const &rhs)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::freestanding::lexicographical_compare_three_way(lhs.imp.begin_ptr, lhs.imp.curr_ptr, rhs.imp.begin_ptr, rhs.imp.curr_ptr, ::fast_io::freestanding::compare_three_way{}))
{
	return ::fast_io::freestanding::lexicographical_compare_three_way(lhs.imp.begin_ptr, lhs.imp.curr_ptr, rhs.imp.begin_ptr, rhs.imp.curr_ptr, ::fast_io::freestanding::compare_three_way{});
}
#endif

template <typename T, typename allocator>
inline constexpr void swap(vector<T, allocator> &lhs, vector<T, allocator> &rhs) noexcept
{
	lhs.swap(rhs);
}

template <typename ValueType, typename Alloc, typename U>
inline constexpr ::fast_io::containers::vector<ValueType, Alloc>::size_type erase(::fast_io::containers::vector<ValueType, Alloc> &c, U const &value)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(*c.begin() == value) ||
								   ::fast_io::containers::details::may_throw_move_assignable<ValueType>)
{
	auto first{c.begin()};
	auto const last{c.end()};
	for (; first != last && !(*first == value); ++first)
	{
	}
	if (first != last)
	{
		for (auto i{first}; ++i != last;)
		{
			if (!(*i == value))
			{
				*first = ::std::move(*i);
				++first;
			}
		}
	}
	auto const r{last - first};
	static_cast<void>(c.erase(first, last));
	return static_cast<::fast_io::containers::vector<ValueType, Alloc>::size_type>(r);
}

template <typename ValueType, typename Alloc, typename Pred>
inline constexpr ::fast_io::containers::vector<ValueType, Alloc>::size_type erase_if(::fast_io::containers::vector<ValueType, Alloc> &c, Pred pred)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(pred(*c.begin())) ||
								   ::fast_io::containers::details::may_throw_move_assignable<ValueType>)
{
	auto first{c.begin()};
	auto const last{c.end()};
	for (; first != last && !pred(*first); ++first)
	{
	}
	if (first != last)
	{
		for (auto i{first}; ++i != last;)
		{
			if (!pred(*i))
			{
				*first = ::std::move(*i);
				++first;
			}
		}
	}
	auto const r{last - first};
	static_cast<void>(c.erase(first, last));
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
