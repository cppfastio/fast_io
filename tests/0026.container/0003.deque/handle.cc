#include <fast_io.h>
#include <fast_io_dsal/deque.h>

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
using throwing_int_deque = ::fast_io::containers::deque<int, throwing_pool_adapter>;
static_assert(!noexcept(::std::declval<throwing_int_deque &>().emplace_back(0)));

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

using int_deque = ::fast_io::containers::deque<int, pool_adapter>;
using noisy_deque = ::fast_io::containers::deque<noisy, pool_adapter>;

static_assert(pool_adapter::has_status);
static_assert(::std::is_trivially_copyable_v<pool_adapter::handle_type>);
// handle-based deques cannot be default constructed: a handle is mandatory
static_assert(!::std::is_default_constructible_v<int_deque>);
static_assert(::std::is_constructible_v<int_deque, int_deque::handle_type>);
static_assert(::std::is_copy_constructible_v<int_deque>);
static_assert(::std::is_move_constructible_v<int_deque>);
// a plain deque still default constructs
static_assert(::std::is_default_constructible_v<::fast_io::containers::deque<int, ::fast_io::native_global_allocator>>);

int main()
{
	// empty deque through the handle ctor
	{
		int_deque d{int_deque::handle_type{1}};
		assert(d.is_empty());
		assert(d.controller.controller_block.controller_start_ptr == nullptr);
		d.push_back(3);
		d.push_back(4);
		static_cast<void>(d.emplace_back(5));
		d.push_front(2);
		assert(d.size() == 4 && d.front() == 2 && d.back() == 5);
	}
	assert(pool_handle_allocator::live[1] == 0);

	// sized ctors
	{
		int_deque a{int_deque::handle_type{0}, 8};
		assert(a.size() == 8);
		for (auto e : a)
		{
			assert(e == 0);
		}
		int_deque b{int_deque::handle_type{0}, 8, ::fast_io::for_overwrite};
		assert(b.size() == 8);
		int_deque c{int_deque::handle_type{0}, 4, 9};
		assert(c.size() == 4 && c.front() == 9 && c.back() == 9);
		int_deque d{int_deque::handle_type{0}, {1, 2, 3}};
		assert(d.size() == 3 && d[1] == 2);
		int const arr[]{7, 8, 9};
		int_deque e{int_deque::handle_type{0}, ::fast_io::freestanding::from_range, arr};
		assert(e.size() == 3 && e[2] == 9);
	}
	assert(pool_handle_allocator::live[0] == 0);

	// two pools must not cross free: allocations are freed against the same handle
	{
		int_deque a{int_deque::handle_type{1}, 3, 1};
		int_deque b{int_deque::handle_type{2}, 5, 2};
		a.push_back(10);
		a.push_front(11);
		b.push_back(20);
		b.push_front(21);
	}
	assert(pool_handle_allocator::live[1] == 0 && pool_handle_allocator::live[2] == 0);

	// growth through push_front/push_back/insert/erase/resize/assign/reserve
	{
		int_deque d{int_deque::handle_type{1}};
		for (int i{}; i != 100; ++i)
		{
			static_cast<void>(d.emplace_back(i));
		}
		for (int i{}; i != 100; ++i)
		{
			static_cast<void>(d.emplace_front(-i));
		}
		static_cast<void>(d.insert(d.cbegin() + 10, 42));
		assert(d.index_unchecked(10) == 42 && d.size() == 201);
		static_cast<void>(d.erase_index(10));
		assert(d.index_unchecked(10) == -89 && d.size() == 200);
		static_cast<void>(d.emplace_index(0, -999));
		assert(d.index_unchecked(0) == -999 && d.size() == 201);
		static_cast<void>(d.insert(d.cbegin() + 5, 3, 7));
		assert(d.index_unchecked(5) == 7 && d.index_unchecked(7) == 7 && d.size() == 204);
		int const arr[]{1, 2, 3};
		static_cast<void>(d.insert_range(d.cbegin() + 2, arr));
		assert(d.index_unchecked(2) == 1 && d.index_unchecked(4) == 3 && d.size() == 207);
		d.resize(400);
		assert(d.size() == 400);
		d.resize(500, ::fast_io::for_overwrite);
		assert(d.size() == 500);
		d.assign(7, 5);
		assert(d.size() == 7 && d.back_unchecked() == 5);
		int const rarr[]{1, 2, 3, 4};
		d.assign_range(rarr);
		assert(d.size() == 4 && d.front() == 1 && d.back() == 4);
		d.reserve(300);
		d.shrink_to_fit();
		d.clear();
		assert(d.is_empty());
	}

	// copy adopts the source handle; move copies the state then empties the source
	{
		int_deque a{int_deque::handle_type{2}, 4, 3};
		int_deque b{a};
		assert(b.size() == 4 && b.allochdl.pool == 2);
		int_deque c{::std::move(b)};
		assert(c.size() == 4 && c.allochdl.pool == 2);
		assert(b.is_empty() && b.controller.controller_block.controller_start_ptr == nullptr);
		int_deque d{int_deque::handle_type{3}};
		d = a;
		assert(d.size() == 4 && d.allochdl.pool == 2);
		int_deque e{int_deque::handle_type{3}};
		e = ::std::move(d);
		assert(e.size() == 4 && e.allochdl.pool == 2);
		assert(d.is_empty() && d.controller.controller_block.controller_start_ptr == nullptr);
	}
	assert(pool_handle_allocator::live[2] == 0);
	assert(pool_handle_allocator::live[3] == 0);

	// nontrivial elements exercise the element-wise construction paths
	{
		noisy_deque d{noisy_deque::handle_type{1}};
		for (int i{}; i != 50; ++i)
		{
			static_cast<void>(d.emplace_back(i));
			static_cast<void>(d.emplace_front(i));
		}
		noisy extra{99};
		static_cast<void>(d.insert(d.cbegin() + 5, extra));
		assert(d.index_unchecked(5).v == 99 && d.size() == 101);
		d.erase_index(0);
		assert(d.size() == 100);
		noisy_deque w{d};
		assert(w.size() == 100 && w.index_unchecked(4).v == 99);
		noisy_deque u{::std::move(w)};
		assert(u.size() == 100 && w.is_empty());
	}
	assert(pool_handle_allocator::live[1] == 0);

	// allocation failure through a throwing-flagged handle allocator propagates;
	// without the dialect the failure would terminate instead
#if defined(__HERBCEPTIONS__)
	try
	{
		throwing_int_deque d{throwing_int_deque::handle_type{7}, 4};
		(void)d;
		return 2;
	}
	catch throws(::std::error e)
	{
		assert(e.code() == static_cast<int>(::std::errc::not_enough_memory));
	}
	{
		// a good pool still allocates through the same adapter
		throwing_int_deque d{throwing_int_deque::handle_type{1}, 4};
		assert(d.size() == 4);
		static_cast<void>(d.emplace_back(1));
		assert(d.size() == 5);
	}
#endif
	return 0;
}
