#include <fast_io.h>
#include <fast_io_dsal/vector.h>

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <type_traits>
#include <utility>

namespace
{

// A handle-based allocator mock: the handle selects which accounting bucket an
// allocation belongs to, mimicking kernel allocators that take a pool/zone
// parameter per call.
struct pool_handle_allocator
{
	struct handle_type
	{
		::std::size_t pool;
	};
	static inline ::std::size_t live[4]{};
	static inline void *handle_allocate_die(handle_type h, ::std::size_t n) noexcept
	{
		void *p{::std::malloc(n ? n : 1)};
		if (p != nullptr)
		{
			++live[h.pool];
		}
		return p;
	}
	static inline void *handle_allocate_try(handle_type h, ::std::size_t n) noexcept(false)
	{
		return handle_allocate_die(h, n);
	}
	static inline void *handle_allocate_zero_die(handle_type h, ::std::size_t n) noexcept
	{
		void *p{::std::calloc(1, n ? n : 1)};
		if (p != nullptr)
		{
			++live[h.pool];
		}
		return p;
	}
	static inline void *handle_allocate_zero_try(handle_type h, ::std::size_t n) noexcept(false)
	{
		return handle_allocate_zero_die(h, n);
	}
	static inline void handle_deallocate_n(handle_type h, void *p, ::std::size_t) noexcept
	{
		if (p != nullptr)
		{
			--live[h.pool];
		}
		::std::free(p);
	}
};

using pool_adapter = ::fast_io::generic_allocator_adapter<pool_handle_allocator>;

// _try variant that reports pool 7 allocation failures through herbceptions
struct failing_pool_allocator : pool_handle_allocator
{
	static inline void *handle_allocate_try(handle_type h, ::std::size_t n)
#if defined(__HERBCEPTIONS__)
		throws
#else
		noexcept(false)
#endif
	{
		if (h.pool == 7)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return pool_handle_allocator::handle_allocate_die(h, n);
	}
	static inline void *handle_allocate_zero_try(handle_type h, ::std::size_t n)
#if defined(__HERBCEPTIONS__)
		throws
#else
		noexcept(false)
#endif
	{
		if (h.pool == 7)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return pool_handle_allocator::handle_allocate_zero_die(h, n);
	}
};
using throwing_pool_adapter = ::fast_io::generic_allocator_adapter<
	failing_pool_allocator,
	::fast_io::allocator_adapter_flags::throws_on_allocation_failure>;
using throwing_int_vec = ::fast_io::containers::vector<int, throwing_pool_adapter>;
static_assert(!noexcept(::std::declval<throwing_int_vec &>().emplace_back(0)));

struct noisy
{
	int v;
	inline noisy(int x) noexcept
		: v{x}
	{}
	inline noisy(noisy const &o) noexcept
		: v{o.v}
	{}
	inline noisy &operator=(noisy const &o) noexcept
	{
		v = o.v;
		return *this;
	}
	inline ~noisy() noexcept
	{
		v = -1;
	}
};

} // namespace

using int_vec = ::fast_io::containers::vector<int, pool_adapter>;
using noisy_vec = ::fast_io::containers::vector<noisy, pool_adapter>;

static_assert(pool_adapter::has_status);
static_assert(::std::is_trivially_copyable_v<pool_adapter::handle_type>);
// handle-based vectors cannot be default constructed: a handle is mandatory
static_assert(!::std::is_default_constructible_v<int_vec>);
static_assert(::std::is_constructible_v<int_vec, int_vec::handle_type>);
static_assert(::std::is_copy_constructible_v<int_vec>);
static_assert(::std::is_move_constructible_v<int_vec>);
// a plain vector still default constructs
static_assert(::std::is_default_constructible_v<::fast_io::containers::vector<int, ::fast_io::native_global_allocator>>);

int main()
{
	// empty vector through the handle ctor
	{
		int_vec v{int_vec::handle_type{1}};
		assert(v.is_empty());
		assert(v.imp.begin_ptr == nullptr);
		v.push_back(3);
		v.push_back(4);
		static_cast<void>(v.emplace_back(5));
		assert(v.size() == 3 && v[0] == 3 && v[2] == 5);
	}
	assert(pool_handle_allocator::live[1] == 0);

	// sized ctors
	{
		int_vec a{int_vec::handle_type{0}, 8};
		assert(a.size() == 8);
		for (auto e : a)
		{
			assert(e == 0);
		}
		int_vec b{int_vec::handle_type{0}, 8, ::fast_io::for_overwrite};
		assert(b.size() == 8);
		int_vec c{int_vec::handle_type{0}, 4, 9};
		assert(c.size() == 4 && c.front() == 9 && c.back() == 9);
		int_vec d{int_vec::handle_type{0}, {1, 2, 3}};
		assert(d.size() == 3 && d[1] == 2);
		int const arr[]{7, 8, 9};
		int_vec e{int_vec::handle_type{0}, ::fast_io::freestanding::from_range, arr};
		assert(e.size() == 3 && e[2] == 9);
	}
	assert(pool_handle_allocator::live[0] == 0);

	// two pools must not cross free: allocations are freed against the same handle
	{
		int_vec a{int_vec::handle_type{1}, 3, 1};
		int_vec b{int_vec::handle_type{2}, 5, 2};
		a.push_back(10);
		b.push_back(20);
		assert(pool_handle_allocator::live[1] == 1);
		assert(pool_handle_allocator::live[2] == 1);
	}
	assert(pool_handle_allocator::live[1] == 0 && pool_handle_allocator::live[2] == 0);

	// growth through insert/erase/resize/assign/reserve
	{
		int_vec v{int_vec::handle_type{1}};
		for (int i{}; i != 100; ++i)
		{
			static_cast<void>(v.emplace_back(i));
		}
		static_cast<void>(v.insert(v.begin() + 10, 42));
		assert(v.index_unchecked(10) == 42 && v.size() == 101);
		static_cast<void>(v.erase_index(10));
		assert(v.index_unchecked(10) == 10 && v.size() == 100);
		static_cast<void>(v.emplace_index(0, -1));
		assert(v.index_unchecked(0) == -1 && v.size() == 101);
		v.resize(200);
		assert(v.size() == 200);
		v.assign(7, 5);
		assert(v.size() == 7 && v.back_unchecked() == 5);
		v.reserve(300);
		assert(v.capacity() >= 300);
		v.shrink_to_fit();
		assert(v.capacity() == 7);
		v.clear();
		assert(v.is_empty() && pool_handle_allocator::live[1] == 1);
	}
	assert(pool_handle_allocator::live[1] == 0);

	// copy adopts the source handle; move copies the state then empties the source
	{
		int_vec a{int_vec::handle_type{2}, 4, 3};
		int_vec b{a};
		assert(b.size() == 4 && b.allochdl.pool == 2);
		assert(pool_handle_allocator::live[2] == 2);
		int_vec c{::std::move(b)};
		assert(c.size() == 4 && c.allochdl.pool == 2);
		assert(b.is_empty() && b.imp.begin_ptr == nullptr);
		int_vec d{int_vec::handle_type{3}};
		d = a;
		assert(d.size() == 4 && d.allochdl.pool == 2);
		assert(pool_handle_allocator::live[2] == 3);
		int_vec e{int_vec::handle_type{3}};
		e = ::std::move(d);
		assert(e.size() == 4 && e.allochdl.pool == 2);
		assert(d.is_empty() && d.imp.begin_ptr == nullptr);
	}
	assert(pool_handle_allocator::live[2] == 0);
	assert(pool_handle_allocator::live[3] == 0);

	// nontrivial elements still go through the element-wise growth path
	{
		noisy_vec v{noisy_vec::handle_type{1}};
		v.reserve(64);
		for (int i{}; i != 50; ++i)
		{
			static_cast<void>(v.emplace_back(i));
		}
		noisy extra{99};
		static_cast<void>(v.insert(v.begin() + 5, extra));
		assert(v.index_unchecked(5).v == 99 && v.size() == 51);
		v.erase_index(0);
		assert(v.index_unchecked(0).v == 1 && v.size() == 50);
		noisy_vec w{v};
		assert(w.size() == 50 && w.index_unchecked(4).v == 99 && w.index_unchecked(5).v == 5);
		noisy_vec u{::std::move(w)};
		assert(u.size() == 50 && w.is_empty());
	}
	assert(pool_handle_allocator::live[1] == 0);

	// allocation failure through a throwing-flagged handle allocator propagates;
	// without the dialect the failure would terminate instead
#if defined(__HERBCEPTIONS__)
	try
	{
		throwing_int_vec v{throwing_int_vec::handle_type{7}, 4};
		(void)v;
		return 2;
	}
	catch throws(::std::error e)
	{
		assert(e.code() == static_cast<int>(::std::errc::not_enough_memory));
	}
	{
		// a good pool still allocates through the same adapter
		throwing_int_vec v{throwing_int_vec::handle_type{1}, 4};
		assert(v.size() == 4);
		static_cast<void>(v.emplace_back(1));
		assert(v.size() == 5);
	}
#endif
	return 0;
}
