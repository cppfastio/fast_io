#include <fast_io.h>

#include <cassert>
#include <cstddef>
#include <cstring>

namespace
{

template <typename alloc>
concept can_deallocate = requires { { alloc::deallocate(nullptr) } -> ::std::same_as<void>; };
template <typename alloc>
concept can_deallocate_n = requires { { alloc::deallocate_n(nullptr, 0) } -> ::std::same_as<void>; };
template <typename alloc>
concept can_deallocate_aligned = requires { { alloc::deallocate_aligned(nullptr, 0) } -> ::std::same_as<void>; };
template <typename alloc>
concept can_deallocate_aligned_n =
	requires { { alloc::deallocate_aligned_n(nullptr, 0, 0) } -> ::std::same_as<void>; };
template <typename alloc>
concept can_handle_deallocate = requires(typename alloc::handle_type h)
{
	{ alloc::handle_deallocate(h, nullptr) } -> ::std::same_as<void>;
};
template <typename alloc>
concept can_handle_deallocate_n = requires(typename alloc::handle_type h)
{
	{ alloc::handle_deallocate_n(h, nullptr, 0) } -> ::std::same_as<void>;
};

struct handle_allocator
{
	using handle_type = ::std::size_t;
	static inline void *handle_allocate_die(handle_type, ::std::size_t n) noexcept
	{
		return ::fast_io::c_malloc_allocator::allocate_die(n);
	}
	static inline void handle_deallocate(handle_type, void *p) noexcept
	{
		::fast_io::c_malloc_allocator::deallocate(p);
	}
	static inline void handle_deallocate_n(handle_type, void *p, ::std::size_t) noexcept
	{
		::fast_io::c_malloc_allocator::deallocate(p);
	}
};

} // namespace

using secure_adapter = ::fast_io::generic_allocator_adapter<::fast_io::c_malloc_allocator,
														  ::fast_io::allocator_adapter_flags::secure_clear>;

// unsized deallocation is unavailable under secure_clear; sized apis wipe then free
static_assert(!can_deallocate<secure_adapter>);
static_assert(!can_deallocate_aligned<secure_adapter>);
static_assert(can_deallocate_n<secure_adapter>);
static_assert(can_deallocate_aligned_n<secure_adapter>);

using plain_adapter = ::fast_io::generic_allocator_adapter<::fast_io::c_malloc_allocator>;
static_assert(can_deallocate<plain_adapter>);
static_assert(can_deallocate_n<plain_adapter>);

using secure_handle_adapter =
	::fast_io::generic_allocator_adapter<handle_allocator, ::fast_io::allocator_adapter_flags::secure_clear>;
static_assert(!can_handle_deallocate<secure_handle_adapter>);
static_assert(can_handle_deallocate_n<secure_handle_adapter>);

int main()
{
	// sized deallocation under secure_clear must wipe the buffer before freeing
	{
		constexpr ::std::size_t n{64};
		void *p{secure_adapter::allocate(n)};
		::std::memset(p, 0x55, n);
		secure_adapter::deallocate_n(p, n);
	}
	{
		constexpr ::std::size_t n{64};
		void *p{secure_adapter::allocate_aligned(alignof(::std::max_align_t), n)};
		::std::memset(p, 0x77, n);
		secure_adapter::deallocate_aligned_n(p, alignof(::std::max_align_t), n);
	}
	{
		void *p{secure_handle_adapter::handle_allocate(0, 64)};
		::std::memset(p, 0x99, 64);
		secure_handle_adapter::handle_deallocate_n(0, p, 64);
	}
	return 0;
}
