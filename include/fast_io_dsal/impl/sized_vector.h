#pragma once
namespace fast_io
{

namespace containers
{

namespace details
{

template <typename uinttype>
struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	sized_vector_model
{
	::std::byte *begin_ptr;
	uinttype size;
	uinttype capacity;
};

template <typename uinttype, typename allochandle>
struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	sized_vector_alloc_handle_model
{
	::std::byte *begin_ptr;
	uinttype size;
	uinttype capacity;
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#else
	[[no_unique_address]]
#endif
	allochandle alloc;
};


namespace sized_vector
{

namespace detemplate
{

template <typename allocator, typename uinttype>
inline void *grow_to_byte_size_iter_impl(sized_vector_model<uinttype> &imp, void *iter, ::std::size_t newcap, ::std::size_t gap, ::std::size_t size, ::std::size_t alignment)
	FAST_IO_HERBCEPTIONS_THROWS_IF(allocator::throws_on_allocation_failure)
{
	::std::byte *old_begin_ptr{imp.begin_ptr};
	::std::size_t const old_size{static_cast<::std::size_t>(imp.size) * size};
	::std::size_t const old_capacity{static_cast<::std::size_t>(imp.capacity) * size};

	::std::byte *begin_ptr;
	::std::size_t newrescount;
	::std::byte *newiter;
	{
		auto newres = allocator::allocate_aligned_at_least(alignment, newcap);
		begin_ptr = reinterpret_cast<::std::byte *>(newres.ptr);
		newrescount = newres.count;
		newiter = ::fast_io::freestanding::nonoverlapped_bytes_copy(reinterpret_cast<::std::byte const *>(old_begin_ptr), reinterpret_cast<::std::byte const *>(iter),
																	reinterpret_cast<::std::byte *>(begin_ptr));
		static_cast<void>(::fast_io::freestanding::nonoverlapped_bytes_copy(reinterpret_cast<::std::byte const *>(iter), reinterpret_cast<::std::byte const *>(old_begin_ptr + old_size),
																			reinterpret_cast<::std::byte *>(newiter + gap)));
		allocator::deallocate_aligned_n(old_begin_ptr, alignment, old_capacity);
	}
	imp.begin_ptr = begin_ptr;
	::std::size_t const newelements{newrescount / size};
	constexpr ::std::size_t uintmx{static_cast<::std::size_t>(::std::numeric_limits<uinttype>::max())};
	imp.capacity = static_cast<uinttype>(newelements < uintmx ? newelements : uintmx);
	return newiter;
}

template <typename allocator, typename uinttype>
inline void *grow_to_size_iter_impl(sized_vector_model<uinttype> &imp, void *iter, ::std::size_t newcap, ::std::size_t size, ::std::size_t alignment)
	FAST_IO_HERBCEPTIONS_THROWS_IF(allocator::throws_on_allocation_failure || allocator::throws_on_violations)
{
	if (static_cast<::std::size_t>(::std::numeric_limits<uinttype>::max()) < newcap) [[unlikely]]
	{
		::fast_io::containers::details::contract_violation_report<allocator::throws_on_overflow>(::std::errc::value_too_large);
	}
#if defined(_MSC_VER) && !defined(__clang__)
	::std::size_t mx{SIZE_MAX / size};
	if (newcap > mx) [[unlikely]]
	{
		::fast_io::containers::details::contract_violation_report<allocator::throws_on_overflow>(::std::errc::value_too_large);
	}
	newcap *= size;
#else
	if (__builtin_mul_overflow(size, newcap, __builtin_addressof(newcap))) [[unlikely]]
	{
		::fast_io::containers::details::contract_violation_report<allocator::throws_on_overflow>(::std::errc::value_too_large);
	}
#endif
	return grow_to_byte_size_iter_impl<allocator>(imp, iter, newcap, size, size, alignment);
}

template <typename allocator, typename uinttype>
inline void *grow_to_size_impl(sized_vector_model<uinttype> &imp, ::std::size_t newcap, ::std::size_t size, ::std::size_t alignment)
	FAST_IO_HERBCEPTIONS_THROWS_IF(allocator::throws_on_allocation_failure || allocator::throws_on_violations)
{
	return grow_to_size_iter_impl<allocator>(imp, imp.begin_ptr + static_cast<::std::size_t>(imp.size) * size, newcap, size, alignment);
}

template <typename allocator, typename uinttype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void *grow_twice_iter_impl(sized_vector_model<uinttype> &imp, void *iter, ::std::size_t size, ::std::size_t alignment)
	FAST_IO_HERBCEPTIONS_THROWS_IF(allocator::throws_on_allocation_failure || allocator::throws_on_violations)
{
	::std::size_t toallocate{1};
	if (imp.capacity) [[likely]]
	{
#if defined(_MSC_VER) && !defined(__clang__)
		constexpr ::std::size_t mx{SIZE_MAX / 2};
		if (static_cast<::std::size_t>(imp.capacity) > mx) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<allocator::throws_on_overflow>(::std::errc::value_too_large);
		}
		toallocate = (static_cast<::std::size_t>(imp.capacity) << 1u);
#else
		if (__builtin_mul_overflow(static_cast<::std::size_t>(imp.capacity), 2u, __builtin_addressof(toallocate))) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<allocator::throws_on_overflow>(::std::errc::value_too_large);
		}
#endif
	}
	return grow_to_size_iter_impl<allocator>(imp, iter, toallocate, size, alignment);
}

template <typename allocator, typename uinttype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void *grow_twice_impl(sized_vector_model<uinttype> &imp, ::std::size_t size, ::std::size_t alignment)
	FAST_IO_HERBCEPTIONS_THROWS_IF(allocator::throws_on_allocation_failure || allocator::throws_on_violations)
{
	return grow_twice_iter_impl<allocator>(imp, imp.begin_ptr + static_cast<::std::size_t>(imp.size) * size, size, alignment);
}

template <typename allocator, typename uinttype>
inline constexpr void *move_backward_impl(sized_vector_model<uinttype> &imp, void *iter, ::std::size_t size, ::std::size_t alignment)
	FAST_IO_HERBCEPTIONS_THROWS_IF(allocator::throws_on_allocation_failure || allocator::throws_on_violations)
{
	if (imp.size == imp.capacity) [[unlikely]]
	{
		return grow_twice_iter_impl<allocator>(imp, iter, size, alignment);
	}
	auto currptr{imp.begin_ptr + static_cast<::std::size_t>(imp.size) * size};
	static_cast<void>(::fast_io::freestanding::uninitialized_move_backward(reinterpret_cast<::std::byte *>(iter), currptr, currptr + size));
	return iter;
}

} // namespace detemplate

template <typename allocator, typename uinttype, ::std::size_t size, ::std::size_t alignment>
inline constexpr void *grow_to_size_iter_impl(sized_vector_model<uinttype> &imp, void *iter, ::std::size_t newcap)
	FAST_IO_HERBCEPTIONS_THROWS_IF(allocator::throws_on_allocation_failure || allocator::throws_on_violations)
{
	if constexpr (alignment < allocator::default_alignment)
	{
		return ::fast_io::containers::details::sized_vector::grow_to_size_iter_impl<allocator, uinttype, size, allocator::default_alignment>(imp, iter, newcap);
	}
	else
	{
		return ::fast_io::containers::details::sized_vector::detemplate::grow_to_size_iter_impl<allocator>(imp, iter, newcap, size, alignment);
	}
}

template <typename allocator, typename uinttype, ::std::size_t size, ::std::size_t alignment>
inline constexpr void *grow_to_size_impl(sized_vector_model<uinttype> &imp, ::std::size_t newcap)
	FAST_IO_HERBCEPTIONS_THROWS_IF(allocator::throws_on_allocation_failure || allocator::throws_on_violations)
{
	if constexpr (alignment < allocator::default_alignment)
	{
		return ::fast_io::containers::details::sized_vector::grow_to_size_impl<allocator, uinttype, size, allocator::default_alignment>(imp, newcap);
	}
	else
	{
		return ::fast_io::containers::details::sized_vector::detemplate::grow_to_size_impl<allocator>(imp, newcap, size, alignment);
	}
}

template <typename allocator, typename uinttype, ::std::size_t size, ::std::size_t alignment>
inline constexpr void *grow_twice_impl(sized_vector_model<uinttype> &imp)
	FAST_IO_HERBCEPTIONS_THROWS_IF(allocator::throws_on_allocation_failure || allocator::throws_on_violations)
{
	if constexpr (alignment < allocator::default_alignment)
	{
		return ::fast_io::containers::details::sized_vector::grow_twice_impl<allocator, uinttype, size, allocator::default_alignment>(imp);
	}
	else
	{
		return ::fast_io::containers::details::sized_vector::detemplate::grow_twice_impl<allocator>(imp, size, alignment);
	}
}

template <typename allocator, typename uinttype, ::std::size_t size, ::std::size_t alignment>
inline constexpr void *grow_twice_iter_impl(sized_vector_model<uinttype> &imp, void *iter)
	FAST_IO_HERBCEPTIONS_THROWS_IF(allocator::throws_on_allocation_failure || allocator::throws_on_violations)
{
	if constexpr (alignment < allocator::default_alignment)
	{
		return ::fast_io::containers::details::sized_vector::grow_twice_iter_impl<allocator, uinttype, size, allocator::default_alignment>(imp, iter);
	}
	else
	{
		return ::fast_io::containers::details::sized_vector::detemplate::grow_twice_iter_impl<allocator>(imp, iter, size, alignment);
	}
}

template <typename allocator, typename uinttype, ::std::size_t size, ::std::size_t alignment>
inline constexpr void *move_backward_impl(sized_vector_model<uinttype> &imp, void *iter)
	FAST_IO_HERBCEPTIONS_THROWS_IF(allocator::throws_on_allocation_failure || allocator::throws_on_violations)
{
	if constexpr (alignment < allocator::default_alignment)
	{
		return ::fast_io::containers::details::sized_vector::move_backward_impl<allocator, uinttype, size, allocator::default_alignment>(imp, iter);
	}
	else
	{
		return ::fast_io::containers::details::sized_vector::detemplate::move_backward_impl<allocator>(imp, iter, size, alignment);
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

} // namespace sized_vector


template <typename T, typename uinttype>
struct sized_vector_internal
{
	T *begin_ptr{};
	uinttype size{};
	uinttype capacity{};
};

template <typename T, typename uinttype, typename Alloc>
struct sized_vector_alloc_handle_internal
{
	T *begin_ptr{};
	uinttype size{};
	uinttype capacity{};
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#else
	[[no_unique_address]]
#endif
	Alloc alloc;
};

} // namespace details

template <typename uinttype, typename T, typename allocator>
	requires(::std::unsigned_integral<uinttype>)
class sized_vector FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
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
				  "sized_vector element types must be nothrow move constructible or trivially relocatable");
	static_assert(::fast_io::containers::details::never_throw_destructible<value_type>,
				  "sized_vector element destructors must never throw");

	static inline constexpr bool throwing_allocation{typed_allocator_type::throws_on_allocation_failure};
	static inline constexpr bool throwing_violations{typed_allocator_type::throws_on_violations};
	static inline constexpr bool throwing_overflow{typed_allocator_type::throws_on_overflow};
	static inline constexpr bool throwing_any{throwing_allocation || throwing_violations || throwing_overflow};
	// moving elements into a new buffer during growth may throw when the move
	// can throw and the sized_vector cannot fall back to copying
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

	using size_type = uinttype;
	using difference_type = ::std::ptrdiff_t;
	using handle_type = typename typed_allocator_type::handle_type;
	::fast_io::containers::details::sized_vector_internal<T, uinttype> imp;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
	[[no_unique_address]]
#endif
#endif
	handle_type allochdl{};

	inline constexpr sized_vector() noexcept
		requires(!alloc_with_status)
	= default;

	inline explicit constexpr sized_vector(handle_type hdl) noexcept
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
	// Constructs an empty sized_vector bound to this container's allocator handle.
	inline constexpr sized_vector alloc_empty() noexcept
	{
		if constexpr (alloc_with_status)
		{
			return sized_vector{this->allochdl};
		}
		else
		{
			return sized_vector{};
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
													  static_cast<::std::size_t>(imp.capacity));
		}
		else if constexpr (!typed_allocator_type::has_deallocate)
		{
			typed_allocator_type::deallocate(imp.begin_ptr);
		}
		else
		{
			typed_allocator_type::deallocate_n(imp.begin_ptr,
											   static_cast<::std::size_t>(imp.capacity));
		}
	}
	struct run_destroy
	{
		sized_vector *thisvec{};
		inline constexpr run_destroy() noexcept = default;
		inline explicit constexpr run_destroy(sized_vector *p) noexcept
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
		this->imp.begin_ptr = this->allocate_elements(n);
		this->imp.size = 0;
		this->imp.capacity = n;
		auto e{this->imp.begin_ptr + n};
		run_destroy des(this);
		for (; this->imp.begin_ptr + this->imp.size != e; ++this->imp.size)
		{
			::new (static_cast<void *>((this->imp.begin_ptr + this->imp.size))) value_type;
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
				::std::size_t const rgdist{::fast_io::containers::details::sized_vector::iter_distance(first, last)};
				if (static_cast<::std::size_t>(max_size()) < rgdist) [[unlikely]]
				{
					::fast_io::containers::details::contract_violation_report<throwing_overflow>(::std::errc::value_too_large);
				}
				size_type const n{static_cast<size_type>(rgdist)};
				this->imp.begin_ptr = this->allocate_elements(n);
				this->imp.size = 0;
				this->imp.capacity = n;
				auto e{this->imp.begin_ptr + n};
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
							((this->imp.begin_ptr + this->imp.size), first, n * sizeof(value_type));
					}
					this->imp.size = static_cast<size_type>(e - this->imp.begin_ptr);
				}
				else if constexpr (!::fast_io::containers::details::may_throw_constructible<value_type, rvaluetype>)
				{
					auto curr{this->imp.begin_ptr};
					for (; curr != e; ++curr)
					{
						::new (static_cast<void *>(curr)) value_type(*first);
						++first;
					}
					this->imp.size = static_cast<size_type>(e - this->imp.begin_ptr);
				}
				else
				{
					run_destroy des(this);
					for (; this->imp.begin_ptr + this->imp.size != e; ++this->imp.size)
					{
						::new (static_cast<void *>((this->imp.begin_ptr + this->imp.size))) value_type(*first);
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
			::fast_io::containers::details::contract_violation_report<throwing_overflow>(::std::errc::value_too_large);
		}
		if constexpr (::fast_io::freestanding::is_zero_default_constructible_v<value_type>)
		{
			imp.begin_ptr = this->allocate_zero_elements(n);
			imp.size = n;
			imp.capacity = n;
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
			::fast_io::containers::details::contract_violation_report<throwing_overflow>(::std::errc::value_too_large);
		}
		if constexpr (::std::is_trivially_default_constructible_v<value_type>)
		{
			imp.begin_ptr = this->allocate_elements(n);
			imp.size = n;
			imp.capacity = n;
		}
		else if constexpr (::fast_io::freestanding::is_zero_default_constructible_v<value_type>)
		{
			imp.begin_ptr = this->allocate_zero_elements(n);
			imp.size = n;
			imp.capacity = n;
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
			::fast_io::containers::details::contract_violation_report<throwing_overflow>(::std::errc::value_too_large);
		}
		this->imp.begin_ptr = this->allocate_elements(n);
		this->imp.size = 0;
		this->imp.capacity = n;
		auto e{this->imp.begin_ptr + n};
		run_destroy des(this);
		for (; this->imp.begin_ptr + this->imp.size != e; ++this->imp.size)
		{
			::new (static_cast<void *>((this->imp.begin_ptr + this->imp.size))) value_type(val);
		}
		des.thisvec = nullptr;
	}

public:
	inline explicit constexpr sized_vector(size_type n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_default_constructible<value_type>)
		requires(!alloc_with_status)
	{
		this->ctor_default_size_impl(n);
	}

	inline explicit constexpr sized_vector(handle_type hdl, size_type n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_default_constructible<value_type>)
		requires(alloc_with_status)
		: allochdl{hdl}
	{
		this->ctor_default_size_impl(n);
	}

	inline explicit constexpr sized_vector(size_type n, ::fast_io::for_overwrite_t)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_default_constructible<value_type>)
		requires(!alloc_with_status)
	{
		this->ctor_overwrite_size_impl(n);
	}

	inline explicit constexpr sized_vector(handle_type hdl, size_type n, ::fast_io::for_overwrite_t)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_default_constructible<value_type>)
		requires(alloc_with_status)
		: allochdl{hdl}
	{
		this->ctor_overwrite_size_impl(n);
	}

	inline explicit constexpr sized_vector(size_type n, const_reference val)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_copy_constructible<value_type>)
		requires(!alloc_with_status)
	{
		this->ctor_fill_size_impl(n, val);
	}

	inline explicit constexpr sized_vector(handle_type hdl, size_type n, const_reference val)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_copy_constructible<value_type>)
		requires(alloc_with_status)
		: allochdl{hdl}
	{
		this->ctor_fill_size_impl(n, val);
	}

	template <::std::ranges::range R>
	inline explicit constexpr sized_vector(::fast_io::freestanding::from_range_t, R &&rg)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(this->construct_vector_common_impl(::std::ranges::begin(rg), ::std::ranges::end(rg)))
		requires(!alloc_with_status)
	{
		this->construct_vector_common_impl(::std::ranges::begin(rg), ::std::ranges::end(rg));
	}

	template <::std::ranges::range R>
	inline explicit constexpr sized_vector(handle_type hdl, ::fast_io::freestanding::from_range_t, R &&rg)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(this->construct_vector_common_impl(::std::ranges::begin(rg), ::std::ranges::end(rg)))
		requires(alloc_with_status)
		: allochdl{hdl}
	{
		this->construct_vector_common_impl(::std::ranges::begin(rg), ::std::ranges::end(rg));
	}

	inline explicit constexpr sized_vector(::std::initializer_list<value_type> ilist)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_copy_constructible<value_type>)
		requires(!alloc_with_status)
	{
		this->construct_vector_common_impl(ilist.begin(), ilist.end());
	}

	inline explicit constexpr sized_vector(handle_type hdl, ::std::initializer_list<value_type> ilist)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_copy_constructible<value_type>)
		requires(alloc_with_status)
		: allochdl{hdl}
	{
		this->construct_vector_common_impl(ilist.begin(), ilist.end());
	}

	inline constexpr sized_vector(sized_vector const &vec)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_copy_constructible<value_type>)
		: allochdl{vec.allochdl}
	{
		// Using static_assert instead of requires to delay the check
		// related to tests/0026.container/0001.sized_vector/recursive.cc
		static_assert(::fast_io::containers::details::copy_constructible<value_type>, "sized_vector's value type must be copy constructible to use copy constructor");
		std::size_t const vecsize{static_cast<std::size_t>(vec.imp.size)};
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
					reinterpret_cast<::std::byte const *>((vec.imp.begin_ptr + vec.imp.size)),
					reinterpret_cast<::std::byte *>(this->imp.begin_ptr)));
				imp.size = vecsize;
				imp.capacity = vecsize;
				return;
			}
		}
		run_destroy des(this);
		this->imp.size = 0;
		this->imp.capacity = vecsize;
		for (auto i{vec.imp.begin_ptr}; i != (vec.imp.begin_ptr + vec.imp.size); ++i)
		{
			::new (static_cast<void *>((this->imp.begin_ptr + this->imp.size))) value_type(*i);
			++this->imp.size;
		}
		des.thisvec = nullptr;
	}

	inline constexpr sized_vector &operator=(sized_vector const &vec)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || ::fast_io::containers::details::may_throw_copy_constructible<value_type>)
	{
		// Using static_assert instead of requires to delay the check
		// related to tests/0026.container/0001.sized_vector/recursive.cc
		static_assert(::std::copyable<value_type>, "sized_vector's value type must be copyable to use copy assignment operator");
		if (__builtin_addressof(vec) == this) [[unlikely]]
		{
			return *this;
		}
		sized_vector newvec(vec);
		this->operator=(::std::move(newvec));
		return *this;
	}

	inline constexpr sized_vector(sized_vector &&vec) noexcept
		: imp(vec.imp), allochdl{vec.allochdl}
	{
		vec.imp = {};
	}
	inline constexpr sized_vector &operator=(sized_vector &&vec) noexcept
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
	inline constexpr ~sized_vector() noexcept
	{
		destroy();
	}

	template <typename... Args>
		requires std::constructible_from<value_type, Args...>
	inline constexpr reference emplace_back_unchecked(Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::may_throw_constructible<value_type, Args...>)
	{
		auto p{::new (static_cast<void *>((imp.begin_ptr + imp.size))) value_type(::std::forward<Args>(args)...)};
		++imp.size;
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
		sized_vector *thisvec{};
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
			::fast_io::containers::details::contract_violation_report<throwing_overflow>(::std::errc::value_too_large);
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
						reinterpret_cast<::std::byte const *>((imp.begin_ptr + imp.size)),
						newiter + n * sizeof(value_type)));
					auto const old_size{static_cast<size_type>(imp.size)};
					if (imp.begin_ptr != nullptr)
					{
						typed_allocator_type::handle_deallocate_n(allochdl, imp.begin_ptr,
																  static_cast<size_type>(imp.capacity));
					}
					imp.begin_ptr = new_begin_ptr;
					imp.size = old_size;
					imp.capacity = newcap;
					return reinterpret_cast<pointer>(newiter);
				}
				else
				{
					::std::size_t newcapbytes;
					if (__builtin_mul_overflow(sizeof(value_type), newcap, __builtin_addressof(newcapbytes))) [[unlikely]]
					{
						::fast_io::containers::details::contract_violation_report<throwing_overflow>(::std::errc::value_too_large);
					}
					return reinterpret_cast<pointer>(::fast_io::containers::details::sized_vector::detemplate::grow_to_byte_size_iter_impl<allocator_type>(
						*reinterpret_cast<::fast_io::containers::details::sized_vector_model<uinttype> *>(__builtin_addressof(imp)),
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
		auto old_curr_ptr{(imp.begin_ptr + imp.size)};
		size_type const old_size{static_cast<size_type>(old_curr_ptr - old_begin_ptr)};
		size_type const old_capacity{static_cast<size_type>((imp.begin_ptr + imp.capacity) - old_begin_ptr)};
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
		imp.size = old_size;
		imp.capacity = new_count;
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
				return reinterpret_cast<pointer>(::fast_io::containers::details::sized_vector::grow_twice_iter_impl<allocator_type, uinttype, sizeof(value_type), alignof(value_type)>(*reinterpret_cast<::fast_io::containers::details::sized_vector_model<uinttype> *>(__builtin_addressof(imp)),
																																													   iter));
			}
		}
		std::size_t const cap{static_cast<::std::size_t>(imp.capacity)};
		::std::size_t const ncap{::fast_io::containers::details::cal_grow_twice_size<sizeof(value_type), false, throwing_overflow>(cap)};
		if (static_cast<::std::size_t>(max_size()) < ncap) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_overflow>(::std::errc::value_too_large);
		}
		return this->grow_to_size_iter_impl(static_cast<size_type>(ncap), iter, 1);
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
				static_cast<void>(::fast_io::freestanding::uninitialized_move_backward(iter, (imp.begin_ptr + imp.size), (imp.begin_ptr + imp.size) + 1));
				return;
			}
		}
		auto currptr{(imp.begin_ptr + imp.size)};
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
			::fast_io::containers::details::contract_violation_report<throwing_overflow>(::std::errc::value_too_large);
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
				static_cast<void>(::fast_io::containers::details::sized_vector::grow_to_size_impl<allocator_type, uinttype, sizeof(value_type), alignof(value_type)>(
					*reinterpret_cast<::fast_io::containers::details::sized_vector_model<uinttype> *>(__builtin_addressof(imp)), newcap));
				return;
			}
		}
		static_cast<void>(this->grow_to_size_iter_impl(newcap, (imp.begin_ptr + imp.size), 1));
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
				static_cast<void>(::fast_io::containers::details::sized_vector::grow_twice_impl<allocator_type, uinttype, sizeof(value_type), alignof(value_type)>(
					*reinterpret_cast<::fast_io::containers::details::sized_vector_model<uinttype> *>(__builtin_addressof(imp))));
				return;
			}
		}
		std::size_t const cap{static_cast<::std::size_t>(imp.capacity)};
		::std::size_t const ncap{::fast_io::containers::details::cal_grow_twice_size<sizeof(value_type), false, throwing_overflow>(cap)};
		if (static_cast<::std::size_t>(max_size()) < ncap) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_overflow>(::std::errc::value_too_large);
		}
		grow_to_size_impl(static_cast<size_type>(ncap));
	}

public:
	inline constexpr void reserve(size_type n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation)
	{
		if (n <= static_cast<std::size_t>(imp.capacity))
		{
			return;
		}
		grow_to_size_impl(n);
	}

	inline constexpr void shrink_to_fit()
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation)
	{
		if (imp.size == imp.capacity)
		{
			return;
		}
		grow_to_size_impl(static_cast<std::size_t>(imp.size));
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr void pop_back()
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (imp.size == 0) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		(--(imp.begin_ptr + imp.size))->~value_type();
	}

	inline constexpr void pop_back_unchecked() noexcept
	{
		(--(imp.begin_ptr + imp.size))->~value_type();
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
		return imp.size == 0;
	}

	inline constexpr void clear() noexcept
	{
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			for (auto p{imp.begin_ptr}; p != (imp.begin_ptr + imp.size); ++p)
			{
				p->~value_type();
			}
		}
		imp.size = 0;
	}
	[[nodiscard]] inline constexpr size_type size() const noexcept
	{
		return static_cast<size_type>(imp.size);
	}
	[[nodiscard]] inline constexpr ::std::size_t size_bytes() const noexcept
	{
		return static_cast<::std::size_t>(imp.size) * sizeof(value_type);
	}
	[[nodiscard]] inline constexpr size_type capacity() const noexcept
	{
		return static_cast<size_type>(imp.capacity);
	}
	[[nodiscard]] inline constexpr ::std::size_t capacity_bytes() const noexcept
	{
		return static_cast<::std::size_t>(imp.capacity) * sizeof(value_type);
	}
	[[nodiscard]] static inline constexpr size_type max_size() noexcept
	{
		constexpr ::std::size_t mx{::std::numeric_limits<::std::size_t>::max() / sizeof(value_type)};
		constexpr size_type umx{::std::numeric_limits<size_type>::max()};
		return mx < static_cast<::std::size_t>(umx) ? static_cast<size_type>(mx) : umx;
	}
	[[nodiscard]] static inline constexpr ::std::size_t max_size_bytes() noexcept
	{
		return static_cast<::std::size_t>(max_size()) * sizeof(value_type);
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
		auto begin_ptr{imp.begin_ptr}, curr_ptr{(imp.begin_ptr + imp.size)};
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
		auto begin_ptr{imp.begin_ptr}, curr_ptr{(imp.begin_ptr + imp.size)};
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
		auto begin_ptr{imp.begin_ptr}, curr_ptr{(imp.begin_ptr + imp.size)};
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
		auto begin_ptr{imp.begin_ptr}, curr_ptr{(imp.begin_ptr + imp.size)};
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
		auto begin_ptr{imp.begin_ptr}, curr_ptr{(imp.begin_ptr + imp.size)};
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
		auto begin_ptr{imp.begin_ptr}, curr_ptr{(imp.begin_ptr + imp.size)};
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
		return (imp.begin_ptr + imp.size)[-1];
	}
	[[nodiscard]] inline constexpr reference back_unchecked() noexcept
	{
		return (imp.begin_ptr + imp.size)[-1];
	}

	[[nodiscard]] inline constexpr iterator begin() noexcept
	{
		return imp.begin_ptr;
	}
	[[nodiscard]] inline constexpr iterator end() noexcept
	{
		return (imp.begin_ptr + imp.size);
	}
	[[nodiscard]] inline constexpr const_iterator begin() const noexcept
	{
		return imp.begin_ptr;
	}
	[[nodiscard]] inline constexpr const_iterator end() const noexcept
	{
		return (imp.begin_ptr + imp.size);
	}
	[[nodiscard]] inline constexpr const_iterator cbegin() const noexcept
	{
		return imp.begin_ptr;
	}
	[[nodiscard]] inline constexpr const_iterator cend() const noexcept
	{
		return (imp.begin_ptr + imp.size);
	}

	[[nodiscard]] inline constexpr reverse_iterator rbegin() noexcept
	{
		return reverse_iterator{(imp.begin_ptr + imp.size)};
	}
	[[nodiscard]] inline constexpr reverse_iterator rend() noexcept
	{
		return reverse_iterator{imp.begin_ptr};
	}
	[[nodiscard]] inline constexpr const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator{(imp.begin_ptr + imp.size)};
	}
	[[nodiscard]] inline constexpr const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator{imp.begin_ptr};
	}
	[[nodiscard]] inline constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator{(imp.begin_ptr + imp.size)};
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
		if (imp.size == imp.capacity)
			[[unlikely]]
		{
			grow_twice_impl();
		}
		auto p{::new (static_cast<void *>((imp.begin_ptr + imp.size))) value_type(::std::forward<Args>(args)...)};
		++imp.size;
		return *p;
	}

private:
	struct append_range_guard
	{
		sized_vector *thisvec{};
		size_type oldn{};
		constexpr ~append_range_guard() noexcept
		{
			if (thisvec)
			{
				auto newcurr{thisvec->imp.begin_ptr + static_cast<::std::ptrdiff_t>(oldn)};
				if constexpr (!::std::is_trivially_destructible_v<value_type>)
				{
					for (auto p{newcurr}; p != (thisvec->imp.begin_ptr + thisvec->imp.size); ++p)
					{
						p->~value_type();
					}
				}
				thisvec->imp.size = oldn;
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
			::std::size_t const rgdist{::std::ranges::size(rg)};
			if (!rgdist)
			{
				return;
			}
			size_type const old_size{imp.size};
			if (static_cast<::std::size_t>(max_size() - old_size) < rgdist) [[unlikely]]
			{
				::fast_io::containers::details::contract_violation_report<throwing_overflow>(::std::errc::value_too_large);
			}
			size_type const rgsize{static_cast<size_type>(rgdist)};
			size_type const new_size{static_cast<size_type>(old_size + rgsize)};
			size_type const cap{static_cast<size_type>(imp.capacity)};
			if (new_size > cap)
			{
				this->grow_to_size_impl(new_size);
			}
			if constexpr (!::fast_io::containers::details::may_throw_constructible<value_type, decltype(*::std::ranges::begin(rg))>)
			{
				for (auto &e : rg)
				{
					::new (static_cast<void *>((imp.begin_ptr + imp.size))) value_type(::std::forward<decltype(e)>(e));
					++imp.size;
				}
			}
			else
			{
				append_range_guard guard{this, old_size};
				for (auto &e : rg)
				{
					::new (static_cast<void *>((imp.begin_ptr + imp.size))) value_type(::std::forward<decltype(e)>(e));
					++imp.size;
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
		if (imp.size == imp.capacity) [[unlikely]]
		{
			it = this->grow_twice_iter_impl(it);
		}
		else if (it != (imp.begin_ptr + imp.size))
		{
			this->move_backward_one_impl(it);
			// it still refers to a live moved-from object
			it->~value_type();
			auto ret{::new (static_cast<void *>(it)) value_type(::std::forward<Args>(args)...)};
			++imp.size;
			return ret;
		}
		auto ret{::new (static_cast<void *>(it)) value_type(::std::forward<Args>(args)...)};
		++imp.size;
		return ret;
	}

	/*
	Same as emplace_slot_construct_impl but takes a pre-materialized element so
	a throwing element constructor runs before the sized_vector is touched.
	*/
	inline constexpr iterator emplace_move_tmp_impl(pointer it, value_type &&tmp)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation || throwing_shift)
	{
		if (imp.size == imp.capacity) [[unlikely]]
		{
			it = this->grow_twice_iter_impl(it);
		}
		else if (it != (imp.begin_ptr + imp.size))
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
			++imp.size;
			return it;
		}
		auto ret{::new (static_cast<void *>(it)) value_type(::std::move(tmp))};
		++imp.size;
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
		size_type sz{static_cast<size_type>((imp.begin_ptr + imp.size) - beginptr)};
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
		sized_vector *thisvec;
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
					first + static_cast<::std::ptrdiff_t>(count), (thisvec->imp.begin_ptr + thisvec->imp.size), first);
				thisvec->imp.size -= count;
			}
		}
	};

	// Carves a count-wide uninitialized hole at it and fills it with val.
	inline constexpr iterator insert_count_impl(pointer it, size_type count, const_reference val)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation || throwing_shift ||
									   ::fast_io::containers::details::may_throw_copy_constructible<value_type>)
	{
		size_type const old_size{static_cast<size_type>(imp.size)};
		if (max_size() - old_size < count) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_overflow>(::std::errc::value_too_large);
		}
		size_type const new_size{old_size + count};
		if (static_cast<size_type>((imp.capacity - imp.size)) < count)
		{
			it = this->grow_to_size_iter_impl(new_size, it, count);
		}
		else
		{
			::fast_io::freestanding::uninitialized_relocate_backward(
				it, (imp.begin_ptr + imp.size), (imp.begin_ptr + imp.size) + static_cast<::std::ptrdiff_t>(count));
		}
		imp.size = new_size;
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
		size_type const old_size{static_cast<size_type>(imp.size)};
		if (max_size() - old_size < rgsize) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_overflow>(::std::errc::value_too_large);
		}
		size_type const new_size{old_size + rgsize};
		if (static_cast<size_type>((imp.capacity - imp.size)) < rgsize)
		{
			it = this->grow_to_size_iter_impl(new_size, it, rgsize);
		}
		else
		{
			::fast_io::freestanding::uninitialized_relocate_backward(
				it, (imp.begin_ptr + imp.size), (imp.begin_ptr + imp.size) + static_cast<::std::ptrdiff_t>(rgsize));
		}
		imp.size = new_size;
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
		size_type const sz{static_cast<size_type>((imp.begin_ptr + imp.size) - beginptr)};
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
			::std::size_t const rgdist{::std::ranges::size(rg)};
			if (!rgdist)
			{
				return it;
			}
			if (static_cast<::std::size_t>(max_size()) < rgdist) [[unlikely]]
			{
				::fast_io::containers::details::contract_violation_report<throwing_overflow>(::std::errc::value_too_large);
			}
			return this->insert_range_hole_impl(it, ::std::forward<R>(rg), static_cast<size_type>(rgdist));
		}
		else
		{
			size_type const pos{static_cast<size_type>(it - imp.begin_ptr)};
			size_type const old_size{static_cast<size_type>(imp.size)};
			this->append_range(::std::forward<R>(rg));
			auto beginptr{imp.begin_ptr};
			this->rotate_range_impl(beginptr + static_cast<::std::ptrdiff_t>(pos),
									beginptr + static_cast<::std::ptrdiff_t>(old_size), (imp.begin_ptr + imp.size));
			return beginptr + static_cast<::std::ptrdiff_t>(pos);
		}
	}

	template <::std::ranges::range R>
		requires ::std::constructible_from<value_type, ::std::ranges::range_value_t<R>>
	inline constexpr size_type insert_range_index(size_type idx, R &&rg)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations || throwing_any || throwing_relocation || throwing_shift ||
									   ::fast_io::containers::details::may_throw_constructible<value_type, decltype(*::std::ranges::begin(rg))>)
	{
		size_type const sz{static_cast<size_type>(imp.size)};
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
		auto lastele{(imp.begin_ptr + imp.size)};
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
				imp.size = static_cast<size_type>(lastele - imp.begin_ptr) - 1;
				return it;
			}
		}
		for (auto d{it}; d + 1 != lastele; ++d)
		{
			*d = ::std::move(d[1]);
		}
		imp.size = static_cast<size_type>(lastele - imp.begin_ptr) - 1;
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			lastele[-1].~value_type();
		}
		return it;
	}

	inline constexpr pointer erase_iters_common(pointer first, pointer last)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_shift)
	{
		auto currptr{(imp.begin_ptr + imp.size)};
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
				imp.size = static_cast<size_type>(::fast_io::freestanding::uninitialized_relocate(last, currptr, first) - imp.begin_ptr);
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
		imp.size = static_cast<size_type>(d - imp.begin_ptr);
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
		auto currptr{(imp.begin_ptr + imp.size)};
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
		auto currptr{(imp.begin_ptr + imp.size)};
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
		auto currptr{(imp.begin_ptr + imp.size)};
		size_type sz{static_cast<size_type>(currptr - beginptr)};
		if (sz < n)
		{
			this->reserve(n);
			auto const e{imp.begin_ptr + n};
			// advance curr_ptr per element so a throwing default constructor
			// leaves the partially filled range owned by the sized_vector
			for (; imp.begin_ptr + imp.size != e; ++imp.size)
			{
				::new (static_cast<void *>((imp.begin_ptr + imp.size))) value_type;
			}
		}
		else if (n < sz)
		{
			if constexpr (!::std::is_trivially_destructible_v<value_type>)
			{
				for (auto p{imp.begin_ptr + n}; p != (imp.begin_ptr + imp.size); ++p)
				{
					p->~value_type();
				}
			}
			imp.size = n;
		}
	}

	inline constexpr void resize(size_type n, const_reference val)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation ||
									   ::fast_io::containers::details::may_throw_copy_constructible<value_type>)
	{
		auto beginptr{imp.begin_ptr};
		auto currptr{(imp.begin_ptr + imp.size)};
		size_type sz{static_cast<size_type>(currptr - beginptr)};
		if (sz < n)
		{
			this->reserve(n);
			auto const e{imp.begin_ptr + n};
			for (; imp.begin_ptr + imp.size != e; ++imp.size)
			{
				::new (static_cast<void *>((imp.begin_ptr + imp.size))) value_type(val);
			}
		}
		else if (n < sz)
		{
			if constexpr (!::std::is_trivially_destructible_v<value_type>)
			{
				for (auto p{imp.begin_ptr + n}; p != (imp.begin_ptr + imp.size); ++p)
				{
					p->~value_type();
				}
			}
			imp.size = n;
		}
	}

	inline constexpr void resize(size_type n, ::fast_io::for_overwrite_t)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation)
	{
		size_type const sz{static_cast<size_type>(imp.size)};
		if (sz < n)
		{
			// growing leaves the appended elements uninitialized
			this->reserve(n);
			imp.size = n;
		}
		else if (n < sz)
		{
			if constexpr (!::std::is_trivially_destructible_v<value_type>)
			{
				for (auto p{imp.begin_ptr + n}; p != (imp.begin_ptr + imp.size); ++p)
				{
					p->~value_type();
				}
			}
			imp.size = n;
		}
	}

	inline constexpr void assign(size_type n, const_reference val)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || throwing_relocation ||
									   ::fast_io::containers::details::may_throw_copy_constructible<value_type>)
	{
		this->clear();
		this->reserve(n);
		for (auto const e{imp.begin_ptr + n}; imp.begin_ptr + imp.size != e; ++imp.size)
		{
			::new (static_cast<void *>((imp.begin_ptr + imp.size))) value_type(val);
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

	inline constexpr void swap(sized_vector &other) noexcept
	{
		::std::swap(imp, other.imp);
		if constexpr (alloc_with_status)
		{
			::std::swap(allochdl, other.allochdl);
		}
	}
};

template <typename uinttype1, typename uinttype2, typename T, typename allocator1, typename allocator2>
	requires ::std::equality_comparable<T>
inline constexpr bool operator==(sized_vector<uinttype1, T, allocator1> const &lhs, sized_vector<uinttype2, T, allocator2> const &rhs)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(*lhs.imp.begin_ptr == *rhs.imp.begin_ptr)
{
	return ::fast_io::freestanding::equal(lhs.imp.begin_ptr, (lhs.imp.begin_ptr + lhs.imp.size), rhs.imp.begin_ptr, (rhs.imp.begin_ptr + rhs.imp.size));
}

#if __cpp_impl_three_way_comparison >= 201907L
template <typename uinttype1, typename uinttype2, typename T, typename allocator1, typename allocator2>
	requires ::std::three_way_comparable<T>
inline constexpr auto operator<=>(sized_vector<uinttype1, T, allocator1> const &lhs, sized_vector<uinttype2, T, allocator2> const &rhs)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::freestanding::lexicographical_compare_three_way(lhs.imp.begin_ptr, (lhs.imp.begin_ptr + lhs.imp.size), rhs.imp.begin_ptr, (rhs.imp.begin_ptr + rhs.imp.size), ::fast_io::freestanding::compare_three_way{}))
{
	return ::fast_io::freestanding::lexicographical_compare_three_way(lhs.imp.begin_ptr, (lhs.imp.begin_ptr + lhs.imp.size), rhs.imp.begin_ptr, (rhs.imp.begin_ptr + rhs.imp.size), ::fast_io::freestanding::compare_three_way{});
}
#endif

template <typename uinttype, typename T, typename allocator>
inline constexpr void swap(sized_vector<uinttype, T, allocator> &lhs, sized_vector<uinttype, T, allocator> &rhs) noexcept
{
	lhs.swap(rhs);
}

template <typename uinttype, typename ValueType, typename Alloc, typename U>
inline constexpr ::fast_io::containers::sized_vector<uinttype, ValueType, Alloc>::size_type erase(::fast_io::containers::sized_vector<uinttype, ValueType, Alloc> &c, U const &value)
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
	return static_cast<typename ::fast_io::containers::sized_vector<uinttype, ValueType, Alloc>::size_type>(r);
}

template <typename uinttype, typename ValueType, typename Alloc, typename Pred>
inline constexpr ::fast_io::containers::sized_vector<uinttype, ValueType, Alloc>::size_type erase_if(::fast_io::containers::sized_vector<uinttype, ValueType, Alloc> &c, Pred pred)
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
	return static_cast<typename ::fast_io::containers::sized_vector<uinttype, ValueType, Alloc>::size_type>(r);
}


} // namespace containers

namespace freestanding
{

template <typename T, typename uinttype, typename Alloc>
struct is_trivially_copyable_or_relocatable<::fast_io::containers::sized_vector<uinttype, T, Alloc>>
{
	inline static constexpr bool value = true;
};

template <typename T, typename uinttype, typename Alloc>
struct is_zero_default_constructible<::fast_io::containers::sized_vector<uinttype, T, Alloc>>
{
	inline static constexpr bool value = true;
};

} // namespace freestanding
} // namespace fast_io
