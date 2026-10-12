#include <fast_io.h>

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>

namespace
{

struct die_only_allocator
{
	static inline void *allocate_die(::std::size_t n) noexcept
	{
		return ::fast_io::c_malloc_allocator::allocate_die(n);
	}
	static inline void deallocate(void *p) noexcept
	{
		::fast_io::c_malloc_allocator::deallocate(p);
	}
};

struct try_only_allocator
{
	static inline void *allocate_try(::std::size_t n) noexcept(false)
	{
		return ::std::malloc(n);
	}
	static inline void deallocate(void *p) noexcept
	{
		::fast_io::c_malloc_allocator::deallocate(p);
	}
};

struct handle_allocator
{
	using handle_type = ::std::size_t;
	static inline void *handle_allocate_die(handle_type, ::std::size_t n) noexcept
	{
		return ::fast_io::c_malloc_allocator::allocate_die(n);
	}
	static inline void *handle_allocate_try(handle_type, ::std::size_t n) noexcept(false)
	{
		return ::std::malloc(n);
	}
	static inline void handle_deallocate(handle_type, void *p) noexcept
	{
		::fast_io::c_malloc_allocator::deallocate(p);
	}
};

struct nontrivial_handle_allocator
{
	struct handle_type
	{
		virtual ~handle_type() {}
	};
	static inline void *handle_allocate_die(handle_type, ::std::size_t) noexcept
	{
		return nullptr;
	}
	static inline void handle_deallocate(handle_type, void *) noexcept {}
};

template <typename alloc>
concept can_allocate = requires { { alloc::allocate(1) } -> ::std::same_as<void *>; };
template <typename alloc>
concept can_allocate_die = requires { { alloc::allocate_die(1) } -> ::std::same_as<void *>; };
template <typename alloc>
concept can_allocate_try = requires { { alloc::allocate_try(1) } -> ::std::same_as<void *>; };
template <typename alloc>
concept can_handle_allocate = requires(typename alloc::handle_type h)
{
	{ alloc::handle_allocate(h, 1) } -> ::std::same_as<void *>;
};
template <typename alloc>
concept can_handle_allocate_try = requires(typename alloc::handle_type h)
{
	{ alloc::handle_allocate_try(h, 1) } -> ::std::same_as<void *>;
};
template <typename alloc>
concept can_typed_allocate_try = requires { { alloc::allocate_try(1) } -> ::std::same_as<int *>; };
template <typename alloc>
concept can_typed_allocate = requires { { alloc::allocate(1) } -> ::std::same_as<int *>; };

} // namespace

// Native allocator exposes _die and _try for every fallible api.
static_assert(::fast_io::details::has_allocate_impl<::fast_io::c_malloc_allocator>);
static_assert(::fast_io::details::has_allocate_mode_impl<::fast_io::c_malloc_allocator, true>);
static_assert(::fast_io::details::has_allocate_mode_impl<::fast_io::c_malloc_allocator, false>);
static_assert(::fast_io::details::has_reallocate_impl<::fast_io::c_malloc_allocator>);
static_assert(::fast_io::details::has_allocate_zero_impl<::fast_io::c_malloc_allocator>);

static_assert(::fast_io::details::has_allocate_mode_impl<die_only_allocator, false>);
static_assert(!::fast_io::details::has_allocate_mode_impl<die_only_allocator, true>);
static_assert(::fast_io::details::has_allocate_mode_impl<try_only_allocator, true>);

using die_adapter = ::fast_io::generic_allocator_adapter<die_only_allocator>;
static_assert(can_allocate<die_adapter>);
static_assert(can_allocate_die<die_adapter>);
static_assert(!can_allocate_try<die_adapter>);

using throwing_die_adapter =
	::fast_io::generic_allocator_adapter<die_only_allocator,
										 ::fast_io::allocator_adapter_flags::throws_on_allocation_failure>;
// die-only allocator cannot satisfy throwing mode
static_assert(!can_allocate<throwing_die_adapter>);
static_assert(!can_allocate_try<throwing_die_adapter>);
static_assert(can_allocate_die<throwing_die_adapter>);

using try_adapter = ::fast_io::generic_allocator_adapter<try_only_allocator>;
static_assert(can_allocate<try_adapter>);
static_assert(can_allocate_die<try_adapter>);
static_assert(can_allocate_try<try_adapter>);

using throwing_try_adapter =
	::fast_io::generic_allocator_adapter<try_only_allocator,
										 ::fast_io::allocator_adapter_flags::throws_on_allocation_failure>;
static_assert(can_allocate<throwing_try_adapter>);
static_assert(can_allocate_try<throwing_try_adapter>);

// Handle-based apis are gated on a non-empty trivially copyable handle_type.
static_assert(::fast_io::details::has_non_empty_handle_type<handle_allocator>);
static_assert(::fast_io::details::has_handle_allocate_impl<handle_allocator>);
static_assert(::fast_io::details::has_handle_allocate_mode_impl<handle_allocator, true>);
static_assert(!::fast_io::details::has_non_empty_handle_type<nontrivial_handle_allocator>);
// wrapping an allocator whose handle_type is not trivially copyable is a hard
// error inside generic_allocator_adapter, so it cannot be spelled here

using handle_adapter = ::fast_io::generic_allocator_adapter<handle_allocator>;
static_assert(handle_adapter::has_status);
static_assert(can_handle_allocate<handle_adapter>);
static_assert(can_handle_allocate_try<handle_adapter>);
static_assert(!can_allocate<handle_adapter>);

using die_typed_adapter = ::fast_io::typed_generic_allocator_adapter<die_adapter, int>;
static_assert(can_typed_allocate<die_typed_adapter>);
static_assert(!can_typed_allocate_try<die_typed_adapter>);
using try_typed_adapter = ::fast_io::typed_generic_allocator_adapter<try_adapter, int>;
static_assert(can_typed_allocate<try_typed_adapter>);
static_assert(can_typed_allocate_try<try_typed_adapter>);

namespace
{
inline void exercise_try_paths()
#if defined(__HERBCEPTIONS__)
	throws
#endif
{
	// native _try
	void *p{::fast_io::c_malloc_allocator::allocate_try(32)};
	p = ::fast_io::c_malloc_allocator::reallocate_try(p, 64);
	::fast_io::c_malloc_allocator::deallocate(p);
	// adapter _try
	using a = ::fast_io::generic_allocator_adapter<::fast_io::c_malloc_allocator>;
	p = a::allocate_try(24);
	a::deallocate(p);
	// throwing-flag adapter unsuffixed + _try
	using b = ::fast_io::generic_allocator_adapter<::fast_io::c_malloc_allocator,
												   ::fast_io::allocator_adapter_flags::throws_on_allocation_failure>;
	p = b::allocate(16);
	b::deallocate(p);
	p = b::allocate_try(24);
	b::deallocate_n(p, 24);
	// try-only allocator through adapter
	p = try_adapter::allocate_try(8);
	try_adapter::deallocate(p);
	// typed adapter _try
	int *q{try_typed_adapter::allocate_try(4)};
	try_typed_adapter::deallocate_n(q, 4);
	// handle _try
	p = handle_adapter::handle_allocate_try(0, 32);
	handle_adapter::handle_deallocate_n(0, p, 32);
}
} // namespace

int main()
{
	// native _die
	{
		void *p{::fast_io::c_malloc_allocator::allocate_die(16)};
		assert(p != nullptr);
		::std::memset(p, 0xAA, 16);
		::fast_io::c_malloc_allocator::deallocate(p);
	}
	// adapter unsuffixed (die mode by default)
	{
		using a = ::fast_io::generic_allocator_adapter<::fast_io::c_malloc_allocator>;
		void *p{a::allocate(16)};
		p = a::reallocate_n(p, 16, 64);
		a::deallocate_n(p, 64);
	}
	// adapter explicit die
	{
		using a = ::fast_io::generic_allocator_adapter<::fast_io::c_malloc_allocator>;
		void *p{a::allocate_die(16)};
		a::deallocate(p);
	}
	// typed adapter
	{
		using a = ::fast_io::native_typed_global_allocator<int>;
		int *p{a::allocate(8)};
		p = a::reallocate_n(p, 8, 16);
		a::deallocate_n(p, 16);
	}
	// handle api
	{
		using a = ::fast_io::generic_allocator_adapter<handle_allocator>;
		void *p{a::handle_allocate(0, 16)};
		a::handle_deallocate(0, p);
	}
#if defined(__HERBCEPTIONS__)
	try
	{
		exercise_try_paths();
	}
	catch throws(::std::error)
	{
		return 1;
	}
#else
	exercise_try_paths();
#endif
	return 0;
}
