#include <fast_io.h>

#include <fast_io_core_impl/allocation/adapters.h>
#include <fast_io_core_impl/allocation/c_malloc.h>

#include <cassert>
#include <cstddef>
#include <cstring>

namespace
{

struct allocate_only_allocator
{
	static inline void *allocate_die(::std::size_t n) noexcept
	{
		return ::fast_io::c_malloc_allocator::allocate_die(n);
	}

	static inline void deallocate_n(void *p, ::std::size_t) noexcept
	{
		::fast_io::c_malloc_allocator::deallocate(p);
	}
};

} // namespace

int main()
{
	using adapter = ::fast_io::generic_allocator_adapter<allocate_only_allocator>;

	// Trigger fallback path for allocators that have allocate but no reallocate.
	// Under ASan/LSan this should detect leaks if old blocks are not freed.

	// Case 1: grow and verify content is preserved.
	constexpr ::std::size_t oldn1{16};
	constexpr ::std::size_t newn1{32};
	void *p1 = adapter::allocate(oldn1);
	auto *b1 = static_cast<unsigned char *>(p1);
	for (::std::size_t i{}; i != oldn1; ++i)
	{
		b1[i] = static_cast<unsigned char>(i);
	}
	p1 = adapter::reallocate_n(p1, oldn1, newn1);
	b1 = static_cast<unsigned char *>(p1);
	for (::std::size_t i{}; i != oldn1; ++i)
	{
		assert(b1[i] == static_cast<unsigned char>(i));
	}
	adapter::deallocate_n(p1, newn1);

	// Case 1b: same-size reallocate returns original pointer.
	constexpr ::std::size_t samen1{24};
	void *p1b = adapter::allocate(samen1);
	auto *b1b = static_cast<unsigned char *>(p1b);
	for (::std::size_t i{}; i != samen1; ++i)
	{
		b1b[i] = static_cast<unsigned char>(0x5A ^ i);
	}
	void *p1b2 = adapter::reallocate_n(p1b, samen1, samen1);
	assert(p1b2 == p1b);
	b1b = static_cast<unsigned char *>(p1b2);
	for (::std::size_t i{}; i != samen1; ++i)
	{
		assert(b1b[i] == static_cast<unsigned char>(0x5A ^ i));
	}
	adapter::deallocate_n(p1b2, samen1);

	// Case 2: shrink-to-zero request (fast_io allocators typically treat 0 as 1).
	constexpr ::std::size_t oldn2{64};
	void *p2 = adapter::allocate(oldn2);
	::std::memset(p2, 0xAB, oldn2);
	p2 = adapter::reallocate_n(p2, oldn2, 0);
	assert(p2 != nullptr);
	static_cast<unsigned char *>(p2)[0] = 0xCD;
	adapter::deallocate_n(p2, 1);

	return 0;
}

