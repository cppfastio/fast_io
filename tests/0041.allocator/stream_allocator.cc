#include <fast_io.h>
#include <fast_io_freestanding.h>

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <type_traits>

#if defined(__HERBCEPTIONS__)
#define TEST_THROWS throws
#else
#define TEST_THROWS
#endif

namespace
{

// statusless counting allocator
struct counting_allocator
{
	static inline int unsigned allocated{};
	static inline int unsigned live{};
	static inline void *allocate_die(::std::size_t n) noexcept
	{
		++allocated;
		++live;
		return ::std::malloc(n ? n : 1);
	}
	static inline void *allocate_try(::std::size_t n)
#if defined(__HERBCEPTIONS__)
		throws
#else
		noexcept(false)
#endif
	{
		return allocate_die(n);
	}
	static inline void deallocate_n(void *p, ::std::size_t) noexcept
	{
		if (p != nullptr)
		{
			--live;
		}
		::std::free(p);
	}
};

// handle-based counting allocator: the handle selects which pool is accounted
struct pool_allocator
{
	struct handle_type
	{
		int pool;
	};
	static inline int unsigned allocated[4]{};
	static inline int unsigned live[4]{};
	static inline void *handle_allocate_die(handle_type h, ::std::size_t n) noexcept
	{
		++allocated[h.pool];
		++live[h.pool];
		return ::std::malloc(n ? n : 1);
	}
	static inline void *handle_allocate_try(handle_type h, ::std::size_t n)
#if defined(__HERBCEPTIONS__)
		throws
#else
		noexcept(false)
#endif
	{
		return handle_allocate_die(h, n);
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

// pool 3 always fails in _try
struct failing_pool_allocator : pool_allocator
{
	static inline void *handle_allocate_try(handle_type h, ::std::size_t n)
#if defined(__HERBCEPTIONS__)
		throws
#else
		noexcept(false)
#endif
	{
		if (h.pool == 3)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return pool_allocator::handle_allocate_die(h, n);
	}
};

using counting_adapter = ::fast_io::generic_allocator_adapter<counting_allocator>;
using pool_adapter = ::fast_io::generic_allocator_adapter<pool_allocator>;
using throwing_pool_adapter =
	::fast_io::generic_allocator_adapter<failing_pool_allocator,
										 ::fast_io::allocator_adapter_flags::throws_on_allocation_failure>;

// minimal output sink used as the underlying stream of basic_io_buffer
struct null_sink
{
	using char_type = char;
	using input_char_type = char;
	using output_char_type = char;
};

inline constexpr null_sink output_stream_ref_define(null_sink s) noexcept
{
	return s;
}

inline void write_all_bytes_overflow_define(null_sink, ::std::byte const *, ::std::size_t) noexcept
{
}

// raw unbuffered output stream declaring a statusless allocator (no handle API)
struct plain_stream
{
	using char_type = char;
	using input_char_type = char;
	using output_char_type = char;
	using output_allocator_type = counting_adapter;
};

inline constexpr plain_stream output_stream_ref_define(plain_stream s) noexcept
{
	return s;
}

inline void write_all_bytes_overflow_define(plain_stream, ::std::byte const *, ::std::size_t) noexcept
{
}

// raw unbuffered output stream with a handle-based allocator
struct handle_stream
{
	using char_type = char;
	using input_char_type = char;
	using output_char_type = char;
	using output_allocator_type = pool_adapter;
	typename output_allocator_type::handle_type allochdl{};
};

inline constexpr handle_stream output_stream_ref_define(handle_stream s) noexcept
{
	return s;
}

inline void write_all_bytes_overflow_define(handle_stream, ::std::byte const *, ::std::size_t) noexcept
{
}

inline constexpr typename pool_adapter::handle_type
output_stream_allocator_handle_define(handle_stream s) noexcept
{
	return s.allochdl;
}

struct failing_handle_stream
{
	using char_type = char;
	using input_char_type = char;
	using output_char_type = char;
	using output_allocator_type = throwing_pool_adapter;
	typename output_allocator_type::handle_type allochdl{};
};

inline constexpr failing_handle_stream output_stream_ref_define(failing_handle_stream s) noexcept
{
	return s;
}

inline void write_all_bytes_overflow_define(failing_handle_stream, ::std::byte const *, ::std::size_t) noexcept
{
}

inline constexpr typename throwing_pool_adapter::handle_type
output_stream_allocator_handle_define(failing_handle_stream s) noexcept
{
	return s.allochdl;
}

inline char *fill_spaces(char *first, char *last) noexcept
{
	::std::memset(first, 'x', static_cast<::std::size_t>(last - first));
	return last;
}

void test_buffered_statusless() TEST_THROWS
{
	using obuf_type = ::fast_io::basic_obuf<null_sink, counting_adapter>;
	int unsigned before{counting_allocator::allocated};
	{
		obuf_type obuf{null_sink{}};
		::fast_io::print(obuf, "hello", 42, ::fast_io::space_reserve<decltype(&fill_spaces)>{100, &fill_spaces});
		// the stream buffer itself was lazily allocated through the stream allocator
		assert(counting_allocator::allocated > before);
		assert(counting_allocator::live >= 1u);
	}
	assert(counting_allocator::live == 0u);
}

void test_buffered_handle() TEST_THROWS
{
	using obuf_type = ::fast_io::basic_obuf<null_sink, pool_adapter>;
	static_assert(!::std::is_default_constructible_v<obuf_type>);
	static_assert(::std::is_constructible_v<obuf_type, pool_allocator::handle_type, null_sink>);
	{
		obuf_type obuf{pool_allocator::handle_type{1}, null_sink{}};
		int unsigned before{pool_allocator::allocated[1]};
		::fast_io::print(obuf, "hello", 42, ::fast_io::space_reserve<decltype(&fill_spaces)>{100, &fill_spaces});
		// both the stream buffer and the print scratch space came from pool 1
		assert(pool_allocator::allocated[1] >= before + 2);
		assert(pool_allocator::live[1] >= 1u);
	}
	assert(pool_allocator::live[1] == 0u);
}

void test_unbuffered_handle() TEST_THROWS
{
	handle_stream stm{typename handle_stream::output_allocator_type::handle_type{2}};
	int unsigned before{pool_allocator::allocated[2]};
	::fast_io::print(stm, ::fast_io::space_reserve<decltype(&fill_spaces)>{100, &fill_spaces});
	assert(pool_allocator::allocated[2] > before);
	assert(pool_allocator::live[2] == 0u);
	// small prints stay on the stack; no allocation
	::fast_io::print(stm, 42);
	assert(pool_allocator::live[2] == 0u);
}

void test_unbuffered_statusless() TEST_THROWS
{
	plain_stream stm{};
	::fast_io::print(stm, ::fast_io::space_reserve<decltype(&fill_spaces)>{64, &fill_spaces});
	assert(counting_allocator::live == 0u);
}

void test_failure_propagation() TEST_THROWS
{
	static_assert(throwing_pool_adapter::throws_on_allocation_failure);
	static_assert(throwing_pool_adapter::has_status);
#if defined(__HERBCEPTIONS__)
	failing_handle_stream stm{{3}};
	try
	{
		::fast_io::print(stm, ::fast_io::space_reserve<decltype(&fill_spaces)>{100, &fill_spaces});
		assert(false);
	}
	catch throws(::std::error)
	{
	}
	assert(pool_allocator::live[3] == 0u);
#endif
}

} // namespace

int main()
{
#if defined(__HERBCEPTIONS__)
	try
	{
		test_buffered_statusless();
		test_buffered_handle();
		test_unbuffered_handle();
		test_unbuffered_statusless();
		test_failure_propagation();
	}
	catch throws(::std::error)
	{
		return 1;
	}
#else
	test_buffered_statusless();
	test_buffered_handle();
	test_unbuffered_handle();
	test_unbuffered_statusless();
	test_failure_propagation();
#endif
}
