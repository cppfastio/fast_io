#include <fast_io.h>
#include <fast_io_dsal/vector.h>

#include <cassert>
#include <utility>

namespace
{

using throwing_allocator = ::fast_io::generic_allocator_adapter<
	::fast_io::c_malloc_allocator,
	::fast_io::allocator_adapter_flags::throws_on_violations |
		::fast_io::allocator_adapter_flags::throws_on_allocation_failure>;

using normal_vector = ::fast_io::containers::vector<int, ::fast_io::native_global_allocator>;
using throwing_vector = ::fast_io::containers::vector<int, throwing_allocator>;

// violations and allocation failures stay terminating without the flags
static_assert(noexcept(::std::declval<normal_vector &>()[0]));
static_assert(noexcept(::std::declval<normal_vector &>().front()));
static_assert(noexcept(::std::declval<normal_vector &>().pop_back()));
static_assert(noexcept(::std::declval<normal_vector &>().emplace_back(0)));
static_assert(noexcept(::std::declval<normal_vector &>().reserve(4)));

// with throws_on_violations the checked apis are throwing apis
static_assert(!noexcept(::std::declval<throwing_vector &>()[0]));
static_assert(!noexcept(::std::declval<throwing_vector &>().front()));
static_assert(!noexcept(::std::declval<throwing_vector &>().back()));
static_assert(!noexcept(::std::declval<throwing_vector &>().pop_back()));
static_assert(!noexcept(::std::declval<throwing_vector &>().emplace_index(0, 0)));
static_assert(!noexcept(::std::declval<throwing_vector &>().erase_index(0)));
// allocation-related apis are throwing too since the allocator throws on failure
static_assert(!noexcept(::std::declval<throwing_vector &>().emplace_back(0)));
static_assert(!noexcept(::std::declval<throwing_vector &>().reserve(4)));
// unchecked apis never validate
static_assert(noexcept(::std::declval<normal_vector &>().index_unchecked(0)));

#if defined(__HERBCEPTIONS__)

inline void throwing_element_use() throws
{
	throwing_vector v;
	for (int i{}; i != 64; ++i)
	{
		static_cast<void>(v.emplace_back(i));
	}
	static_cast<void>(v.insert(v.begin() + 3, 42));
	static_cast<void>(v.erase_index(3));
}

#endif

} // namespace

int main()
{
	// die-mode vector behaves exactly as before
	{
		normal_vector v{1, 2, 3};
		v.push_back(4);
		assert(v.size() == 4);
		assert(v[3] == 4);
		assert(v.front() == 1);
		assert(v.back() == 4);
		v.pop_back();
		assert(v.size() == 3);
	}
	// flagged allocator vector: normal usage path must still work
	{
		throwing_vector v{1, 2, 3};
		v.push_back(4);
		assert(v.size() == 4);
		assert(v.index_unchecked(0) == 1);
		v.resize(8, 9);
		assert(v.back() == 9);
	}
#if defined(__HERBCEPTIONS__)
	try
	{
		throwing_element_use();
	}
	catch throws(::std::error)
	{
		return 1;
	}
	// out of bounds operator[] throws
	try
	{
		throwing_vector v{1, 2, 3};
		(void)v[3];
		return 2;
	}
	catch throws(::std::error)
	{
	}
	// front()/back()/pop_back() on empty vector throw
	try
	{
		throwing_vector v;
		(void)v.front();
		return 3;
	}
	catch throws(::std::error)
	{
	}
	try
	{
		throwing_vector v;
		v.pop_back();
		return 4;
	}
	catch throws(::std::error)
	{
	}
	// size overflow throws
	try
	{
		throwing_vector v;
		v.reserve(throwing_vector::max_size() + 1);
		return 5;
	}
	catch throws(::std::error)
	{
	}
	try
	{
		throwing_vector v(throwing_vector::max_size() + 1, ::fast_io::for_overwrite);
		return 6;
	}
	catch throws(::std::error)
	{
	}
	// index api out of range throws
	try
	{
		throwing_vector v{1, 2};
		v.emplace_index(4, 0);
		return 7;
	}
	catch throws(::std::error)
	{
	}
	try
	{
		throwing_vector v{1, 2};
		v.erase_index(5);
		return 8;
	}
	catch throws(::std::error)
	{
	}
	try
	{
		throwing_vector v{1, 2};
		v.erase_index(2, 1);
		return 9;
	}
	catch throws(::std::error)
	{
	}
#endif
	return 0;
}
