#include <fast_io.h>
#include <coroutine>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <limits>

struct mock_scheduler
{
	using native_handle_type = int;
	using allocator_type = ::fast_io::native_global_allocator;
	native_handle_type h{};
	constexpr native_handle_type native_handle() const noexcept
	{
		return h;
	}
};

struct mock_file
{
	using input_char_type = char;
	using output_char_type = char;
	::std::byte *data;
	::std::size_t size;
	::std::size_t *pos; /* shared current position, fd-like */
};

inline constexpr mock_file input_stream_ref_define(mock_file f) noexcept
{
	return f;
}

inline constexpr mock_file output_stream_ref_define(mock_file f) noexcept
{
	return f;
}

inline constexpr mock_file io_stream_ref_define(mock_file f) noexcept
{
	return f;
}

/* sync ops: basic_io_buffer's destructor/close flushes pending output
 * through the synchronous write path */
inline void write_all_bytes_overflow_define(mock_file out, ::std::byte const *first,
											::std::size_t count) noexcept
{
	::std::size_t pos{*out.pos};
	::std::size_t room{pos < out.size ? out.size - pos : 0};
	::std::size_t n{room < count ? room : count};
	::std::memcpy(out.data + pos, first, n);
	*out.pos = pos + n;
}

static ::std::size_t mock_advance(::fast_io::intfpos_opt &off, ::std::size_t *pos, ::std::size_t n)
{
	if (off.has_opt)
	{
		off.opt += static_cast<::fast_io::intfpos_t>(n);
	}
	else
	{
		*pos += n;
	}
	return n;
}

/* synchronous inline-completion backend: the callback fires during the
 * define call, which also exercises the awaiter inline path */
template <typename func>
inline void async_pread_some_bytes_underflow_callback_define(mock_scheduler, ::fast_io::posix_statx_timestamp_opt,
															 mock_file in, ::std::byte *first,
															 ::std::size_t count, ::fast_io::intfpos_opt off,
															 func cb) noexcept
{
	::std::size_t pos{off.has_opt ? static_cast<::std::size_t>(off.opt) : *in.pos};
	::std::size_t avail{pos < in.size ? in.size - pos : 0};
	::std::size_t n{avail < count ? avail : count};
	::std::memcpy(first, in.data + pos, n);
	if (off.has_opt == false)
	{
		*in.pos += n;
	}
	cb(::std::cxx_std_error{}, n);
}

template <typename func>
inline void async_pwrite_some_bytes_overflow_callback_define(mock_scheduler, ::fast_io::posix_statx_timestamp_opt,
															 mock_file out,
															 ::std::byte const *first, ::std::size_t count,
															 ::fast_io::intfpos_opt off,
															 func cb) noexcept
{
	::std::size_t pos{off.has_opt ? static_cast<::std::size_t>(off.opt) : *out.pos};
	::std::size_t avail{pos < out.size ? out.size - pos : 0};
	::std::size_t n{avail < count ? avail : count};
	::std::memcpy(out.data + pos, first, n);
	if (off.has_opt == false)
	{
		*out.pos += n;
	}
	cb(::std::cxx_std_error{}, n);
}


/* a second stream type with native scatter/transmit defines: the decay
 * functions must prefer them over the generic emulation */
struct mock_file2 : mock_file
{
};

inline constexpr mock_file2 input_stream_ref_define(mock_file2 f) noexcept
{
	return f;
}

inline constexpr mock_file2 output_stream_ref_define(mock_file2 f) noexcept
{
	return f;
}
static int native_scatter_calls{};
static int native_transmit_calls{};

template <typename func>
inline void async_scatter_pwrite_some_bytes_overflow_callback_define(mock_scheduler, ::fast_io::posix_statx_timestamp_opt,
																	 mock_file2 out,
																	 ::fast_io::io_scatter_t const *scatters,
																	 ::std::size_t n,
																	 ::fast_io::intfpos_opt off,
																	 func cb) noexcept
{
	++native_scatter_calls;
	::std::size_t pos{off.has_opt ? static_cast<::std::size_t>(off.opt) : *out.pos};
	for (::std::size_t i{}; i != n; ++i)
	{
		::std::size_t const room{pos < out.size ? out.size - pos : 0};
		::std::size_t const take{room < scatters[i].len ? room : scatters[i].len};
		::std::memcpy(out.data + pos, scatters[i].base, take);
		pos += take;
	}
	if (!off.has_opt)
	{
		*out.pos = pos;
	}
	cb(::std::cxx_std_error{}, ::fast_io::io_scatter_status_t{n, 0});
}

template <typename func>
inline void async_transmit_some_bytes_overflow_underflow_callback_define(mock_scheduler, ::fast_io::posix_statx_timestamp_opt,
																		 mock_file2 out,
																		 ::fast_io::intfpos_opt off_out, mock_file2 in,
																		 ::fast_io::intfpos_opt off_in,
																		 ::fast_io::size_t_opt bound,
																		 func cb) noexcept
{
	++native_transmit_calls;
	::std::size_t ipos{off_in.has_opt ? static_cast<::std::size_t>(off_in.opt) : *in.pos};
	::std::size_t opos{off_out.has_opt ? static_cast<::std::size_t>(off_out.opt) : *out.pos};
	::std::size_t want{bound.has_opt ? bound.opt : in.size - ipos};
	::std::size_t const iroom{ipos < in.size ? in.size - ipos : 0};
	::std::size_t const oroom{opos < out.size ? out.size - opos : 0};
	::std::size_t n{iroom < oroom ? iroom : oroom};
	if (want < n)
	{
		n = want;
	}
	::std::memcpy(out.data + opos, in.data + ipos, n);
	if (!off_in.has_opt)
	{
		*in.pos += n;
	}
	if (!off_out.has_opt)
	{
		*out.pos += n;
	}
	cb(::std::cxx_std_error{}, n);
}

/* a stream with an async close define: marks the close as consumed */
template <typename func>
inline void async_close_define(mock_scheduler, ::fast_io::posix_statx_timestamp_opt,
							   mock_file f, func cb) noexcept
{
	*f.pos = ::std::numeric_limits<::std::size_t>::max();
	cb(::std::cxx_std_error{});
}

/* a stream with an async connect define: copies the peer address into
 * the stream's buffer like a real backend does at submission */
template <typename func>
inline void async_connect_define(mock_scheduler, ::fast_io::posix_statx_timestamp_opt,
								 mock_file f, void const *addr, ::std::size_t addrlen,
								 func cb) noexcept
{
	::std::size_t n{addrlen < f.size ? addrlen : f.size};
	::std::memcpy(f.data, addr, n);
	*f.pos = n;
	cb(::std::cxx_std_error{});
}

/* a stream with only synchronous byte operations — no async defines; the
 * generic transmit engine must drive it inline */
struct sync_only_file
{
	using input_char_type = char;
	using output_char_type = char;
	::std::byte *data;
	::std::size_t size;
	::std::size_t *pos;
};

inline constexpr sync_only_file input_stream_ref_define(sync_only_file f) noexcept
{
	return f;
}

inline constexpr sync_only_file output_stream_ref_define(sync_only_file f) noexcept
{
	return f;
}

inline ::std::byte *read_some_bytes_underflow_define(sync_only_file in, ::std::byte *first,
													 ::std::size_t count) noexcept
{
	::std::size_t const room{*in.pos < in.size ? in.size - *in.pos : 0};
	::std::size_t const n{room < count ? room : count};
	::std::memcpy(first, in.data + *in.pos, n);
	*in.pos += n;
	return first + n;
}

inline ::std::byte const *write_some_bytes_overflow_define(sync_only_file out,
														   ::std::byte const *first,
														   ::std::size_t count) noexcept
{
	::std::size_t const room{*out.pos < out.size ? out.size - *out.pos : 0};
	::std::size_t const n{room < count ? room : count};
	::std::memcpy(out.data + *out.pos, first, n);
	*out.pos += n;
	return first + n;
}

/* sync input whose read always fails — checks error propagation through
 * the synchronous side of the transmit engine */
struct sync_fail_file
{
	using input_char_type = char;
};

inline constexpr sync_fail_file input_stream_ref_define(sync_fail_file f) noexcept
{
	return f;
}

inline ::std::byte *read_some_bytes_underflow_define(sync_fail_file, ::std::byte *,
													 ::std::size_t) throws
{
	::fast_io::herbceptions::throws_errc(::std::errc::io_error);
}

using test_task = ::fast_io::io_async_task<>;

static int failures{};

#define CHECK(cond)                                                       \
	do                                                                    \
	{                                                                     \
		if (!(cond))                                                      \
		{                                                                 \
			++failures;                                                   \
			::std::fprintf(stderr, "FAIL %s line %d\n", #cond, __LINE__); \
		}                                                                 \
	} while (0)

static test_task coro_read(mock_scheduler sched, mock_file in, ::std::byte *buf, ::std::size_t n,
						   ::fast_io::intfpos_opt off) throws
{
	auto got{co_await ::fast_io::operations::async_pread_some_bytes(sched, {}, in, buf, n, off)};
	if (got != n)
	{
		++failures;
	}
}

static test_task coro_write_all(mock_scheduler sched, mock_file out, ::std::byte const *buf,
								::std::size_t n, ::fast_io::intfpos_opt off) throws
{
	co_await ::fast_io::operations::async_pwrite_all_bytes(sched, {}, out, buf, n, off);
}

static test_task coro_main(mock_scheduler sched) throws
{
	/* source file */
	::std::byte srcdata[1024];
	for (::std::size_t i{}; i != sizeof(srcdata); ++i)
	{
		srcdata[i] = static_cast<::std::byte>(i & 0xff);
	}
	::std::size_t srcpos{};
	mock_file src{srcdata, sizeof(srcdata), __builtin_addressof(srcpos)};

	/* coroutine some-read at explicit offset */
	::std::byte rbuf[200]{};
	co_await coro_read(sched, src, rbuf, sizeof(rbuf), ::fast_io::intfpos_opt{100});
	CHECK(::std::memcmp(rbuf, srcdata + 100, 200) == 0);
	CHECK(srcpos == 0);

	/* coroutine all-write at explicit offset into dst */
	::std::byte dstdata[1024]{};
	::std::size_t dstpos{};
	mock_file dst{dstdata, sizeof(dstdata), __builtin_addressof(dstpos)};
	co_await coro_write_all(sched, dst, rbuf, sizeof(rbuf), ::fast_io::intfpos_opt{50});
	CHECK(::std::memcmp(dstdata + 50, srcdata + 100, 200) == 0);
	CHECK(dstpos == 0);
}

template <typename stmtype>
static test_task coro_close_out(mock_scheduler sched, stmtype &&stm) throws
{
	co_await ::fast_io::operations::async_close(sched, {}, stm);
}

static test_task coro_connect(mock_scheduler sched, mock_file stm, void const *addr,
							  ::std::size_t addrlen) throws
{
	co_await ::fast_io::operations::async_connect(sched, {}, stm, addr, addrlen);
}

static test_task coro_scan(mock_scheduler sched, ::std::byte *data, ::std::size_t size) throws
{
	::std::size_t pos{};
	::fast_io::basic_io_buffer<mock_file,
							   ::fast_io::basic_io_buffer_traits<::fast_io::buffer_mode::in,
																 ::fast_io::native_global_allocator,
																 char, void, 32>>
		ibf{mock_file{data, size, __builtin_addressof(pos)}};
	::fast_io::http_header_buffer hdr{};
	co_await ::fast_io::async_scan(sched, ::fast_io::posix_statx_timestamp_opt{}, ibf, hdr);
	CHECK(hdr.code().size() == 3);
	CHECK(::std::memcmp(hdr.code().data(), "200", 3) == 0);
	CHECK(hdr.request().size() == 8);
	CHECK(::std::memcmp(hdr.request().data(), "HTTP/1.1", 8) == 0);
}

int main()
{
	mock_scheduler sched{};

	/* run coroutine test */
	{
		auto t{coro_main(sched)};
		t.resume();
		try
		{
			t.rethrow_if_error();
		}
		catch throws(::std::error)
		{
			CHECK(false);
		}
	}

	/* callback: pread_all current position */
	{
		::std::byte srcdata[512];
		for (::std::size_t i{}; i != sizeof(srcdata); ++i)
		{
			srcdata[i] = static_cast<::std::byte>((i * 7) & 0xff);
		}
		::std::size_t srcpos{};
		mock_file src{srcdata, sizeof(srcdata), __builtin_addressof(srcpos)};
		::std::byte buf[512]{};
		bool called{};
		::fast_io::operations::async_pread_all_bytes_callback(
			sched, {}, src, buf, sizeof(buf), {},
			[&](::std::cxx_std_error e) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
			});
		CHECK(called);
		CHECK(::std::memcmp(buf, srcdata, 512) == 0);
		CHECK(srcpos == 512);
	}

	/* callback: scatter_pwrite_all (generic lio emulation) */
	{
		::std::byte dstdata[256]{};
		::std::size_t dstpos{};
		mock_file dst{dstdata, sizeof(dstdata), __builtin_addressof(dstpos)};
		::std::byte a[10], b[20], c[30];
		for (::std::size_t i{}; i != sizeof(a); ++i)
		{
			a[i] = ::std::byte{0xAA};
		}
		for (::std::size_t i{}; i != sizeof(b); ++i)
		{
			b[i] = ::std::byte{0xBB};
		}
		for (::std::size_t i{}; i != sizeof(c); ++i)
		{
			c[i] = ::std::byte{0xCC};
		}
		::fast_io::io_scatter_t scs[3]{{a, sizeof(a)}, {b, sizeof(b)}, {c, sizeof(c)}};
		bool called{};
		::fast_io::operations::async_scatter_pwrite_all_bytes_callback(
			sched, {}, dst, scs, 3, ::fast_io::intfpos_opt{4},
			[&](::std::cxx_std_error e) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
			});
		CHECK(called);
		CHECK(::std::memcmp(dstdata + 4, a, 10) == 0);
		CHECK(::std::memcmp(dstdata + 14, b, 20) == 0);
		CHECK(::std::memcmp(dstdata + 34, c, 30) == 0);
	}

	/* callback: transmit_all stream->stream through the bounce chain */
	{
		::std::byte srcdata[1000];
		for (::std::size_t i{}; i != sizeof(srcdata); ++i)
		{
			srcdata[i] = static_cast<::std::byte>((i * 13) & 0xff);
		}
		::std::size_t srcpos{}, dstpos{};
		mock_file src{srcdata, sizeof(srcdata), __builtin_addressof(srcpos)};
		::std::byte dstdata[1024]{};
		mock_file dst{dstdata, sizeof(dstdata), __builtin_addressof(dstpos)};
		bool called{};
		::fast_io::operations::async_transmit_all_bytes_callback(
			sched, {}, dst, {}, src, {}, {},
			[&](::std::cxx_std_error e) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
			});
		CHECK(called);
		CHECK(::std::memcmp(dstdata, srcdata, 1000) == 0);
		CHECK(srcpos == 1000 && dstpos == 1000);
	}

	/* transmit_some bounded at explicit offsets */
	{
		::std::byte srcdata[300];
		for (::std::size_t i{}; i != sizeof(srcdata); ++i)
		{
			srcdata[i] = static_cast<::std::byte>(i);
		}
		::std::size_t srcpos{}, dstpos{};
		mock_file src{srcdata, sizeof(srcdata), __builtin_addressof(srcpos)};
		::std::byte dstdata[512]{};
		mock_file dst{dstdata, sizeof(dstdata), __builtin_addressof(dstpos)};
		bool called{};
		::fast_io::operations::async_transmit_some_bytes_callback(
			sched, {}, dst, ::fast_io::intfpos_opt{10}, src, ::fast_io::intfpos_opt{50},
			::fast_io::size_t_opt{200},
			[&](::std::cxx_std_error e, ::std::size_t n) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
				CHECK(n == 200);
			});
		CHECK(called);
		CHECK(::std::memcmp(dstdata + 10, srcdata + 50, 200) == 0);
	}

	/* all-loop EOF: read more than available must fail end_of_file */
	{
		::std::byte srcdata[64]{};
		::std::size_t srcpos{};
		mock_file src{srcdata, sizeof(srcdata), __builtin_addressof(srcpos)};
		::std::byte buf[128]{};
		bool called{};
		::fast_io::operations::async_pread_all_bytes_callback(
			sched, {}, src, buf, sizeof(buf), {},
			[&](::std::cxx_std_error e) noexcept {
				called = true;
				CHECK(e.domain != nullptr);
				CHECK(e.code == static_cast<::std::size_t>(
									::fast_io::freestanding::parse_errc::end_of_file));
			});
		CHECK(called);
	}


	/* native transmit define preferred over bounce emulation */
	{
		::std::byte srcdata[200];
		for (::std::size_t i{}; i != sizeof(srcdata); ++i)
		{
			srcdata[i] = static_cast<::std::byte>(i);
		}
		::std::size_t srcpos{}, dstpos{};
		mock_file2 src{{srcdata, sizeof(srcdata), __builtin_addressof(srcpos)}};
		::std::byte dstdata[256]{};
		mock_file2 dst{{dstdata, sizeof(dstdata), __builtin_addressof(dstpos)}};
		bool called{};
		::fast_io::operations::async_transmit_all_bytes_callback(
			sched, {}, dst, {}, src, {}, {},
			[&](::std::cxx_std_error e) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
			});
		CHECK(called);
		CHECK(native_transmit_calls > 0);
		CHECK(::std::memcmp(dstdata, srcdata, 200) == 0);
	}

	/* hybrid transmit: synchronous-only input into an async output — the
	 * read side runs inline, the write side goes through the scheduler */
	{
		::std::byte srcdata[600];
		for (::std::size_t i{}; i != sizeof(srcdata); ++i)
		{
			srcdata[i] = static_cast<::std::byte>((i * 7) & 0xff);
		}
		::std::size_t srcpos{}, dstpos{};
		sync_only_file src{srcdata, sizeof(srcdata), __builtin_addressof(srcpos)};
		::std::byte dstdata[1024]{};
		mock_file dst{dstdata, sizeof(dstdata), __builtin_addressof(dstpos)};
		bool called{};
		::fast_io::operations::async_transmit_all_bytes_callback(
			sched, {}, dst, {}, src, {}, {},
			[&](::std::cxx_std_error e) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
			});
		CHECK(called);
		CHECK(::std::memcmp(dstdata, srcdata, 600) == 0);
		CHECK(srcpos == 600 && dstpos == 600);
	}

	/* hybrid transmit: async input into a synchronous-only output (e.g. a
	 * hash context or in-memory sink) */
	{
		::std::byte srcdata[600];
		for (::std::size_t i{}; i != sizeof(srcdata); ++i)
		{
			srcdata[i] = static_cast<::std::byte>((i * 11) & 0xff);
		}
		::std::size_t srcpos{}, dstpos{};
		mock_file src{srcdata, sizeof(srcdata), __builtin_addressof(srcpos)};
		::std::byte dstdata[1024]{};
		sync_only_file dst{dstdata, sizeof(dstdata), __builtin_addressof(dstpos)};
		bool called{};
		::fast_io::operations::async_transmit_all_bytes_callback(
			sched, {}, dst, {}, src, {}, {},
			[&](::std::cxx_std_error e) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
			});
		CHECK(called);
		CHECK(::std::memcmp(dstdata, srcdata, 600) == 0);
		CHECK(srcpos == 600 && dstpos == 600);
	}

	/* both sides synchronous — the fully-sync shortcut runs transmit inline */
	{
		::std::byte srcdata[300];
		for (::std::size_t i{}; i != sizeof(srcdata); ++i)
		{
			srcdata[i] = static_cast<::std::byte>((i * 5) & 0xff);
		}
		::std::size_t srcpos{}, dstpos{};
		sync_only_file src{srcdata, sizeof(srcdata), __builtin_addressof(srcpos)};
		::std::byte dstdata[512]{};
		sync_only_file dst{dstdata, sizeof(dstdata), __builtin_addressof(dstpos)};
		bool called{};
		::fast_io::operations::async_transmit_all_bytes_callback(
			sched, {}, dst, {}, src, {}, {},
			[&](::std::cxx_std_error e) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
			});
		CHECK(called);
		CHECK(::std::memcmp(dstdata, srcdata, 300) == 0);
		CHECK(srcpos == 300 && dstpos == 300);
	}

	/* hybrid transmit_some: synchronous-only input, async output, bound */
	{
		::std::byte srcdata[256];
		for (::std::size_t i{}; i != sizeof(srcdata); ++i)
		{
			srcdata[i] = static_cast<::std::byte>((i * 3) & 0xff);
		}
		::std::size_t srcpos{}, dstpos{};
		sync_only_file src{srcdata, sizeof(srcdata), __builtin_addressof(srcpos)};
		::std::byte dstdata[512]{};
		mock_file dst{dstdata, sizeof(dstdata), __builtin_addressof(dstpos)};
		bool called{};
		::fast_io::operations::async_transmit_some_bytes_callback(
			sched, {}, dst, {}, src, {}, ::fast_io::size_t_opt{100},
			[&](::std::cxx_std_error e, ::std::size_t n) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
				CHECK(n == 100);
			});
		CHECK(called);
		CHECK(::std::memcmp(dstdata, srcdata, 100) == 0);
		CHECK(dstpos == 100);
	}

	/* a synchronous read error is delivered through the callback */
	{
		sync_fail_file src{};
		::std::byte dstdata[64]{};
		::std::size_t dstpos{};
		mock_file dst{dstdata, sizeof(dstdata), __builtin_addressof(dstpos)};
		bool called{};
		::fast_io::operations::async_transmit_all_bytes_callback(
			sched, {}, dst, {}, src, {}, {},
			[&](::std::cxx_std_error e) noexcept {
				called = true;
				CHECK(e.domain != nullptr);
			});
		CHECK(called);
	}

	/* native scatter define preferred over lio emulation */
	{
		::std::byte dstdata[64]{};
		::std::size_t dstpos{};
		mock_file2 dst{{dstdata, sizeof(dstdata), __builtin_addressof(dstpos)}};
		::std::byte a[8], b[8];
		::std::memset(a, 0x11, sizeof(a));
		::std::memset(b, 0x22, sizeof(b));
		::fast_io::io_scatter_t scs[2]{{a, sizeof(a)}, {b, sizeof(b)}};
		bool called{};
		::fast_io::operations::async_scatter_pwrite_some_bytes_callback(
			sched, {}, dst, scs, 2, ::fast_io::intfpos_opt{0},
			[&](::std::cxx_std_error e, ::fast_io::io_scatter_status_t st) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
				CHECK(st.position == 2);
			});
		CHECK(called);
		CHECK(native_scatter_calls == 1);
		CHECK(::std::memcmp(dstdata, a, 8) == 0);
		CHECK(::std::memcmp(dstdata + 8, b, 8) == 0);
	}

	/* buffered input: a small read underflows the whole window, later
	 * reads drain it without touching the mock; explicit offsets bypass */
	{
		::std::byte srcdata[256];
		for (::std::size_t i{}; i != sizeof(srcdata); ++i)
		{
			srcdata[i] = static_cast<::std::byte>(i);
		}
		::std::size_t srcpos{};
		::fast_io::basic_io_buffer<mock_file,
								   ::fast_io::basic_io_buffer_traits<::fast_io::buffer_mode::in,
																	 ::fast_io::native_global_allocator,
																	 char, void, 64>>
			ibf{mock_file{srcdata, sizeof(srcdata), __builtin_addressof(srcpos)}};
		::std::byte buf[8]{};
		bool called{};
		::fast_io::operations::async_pread_some_bytes_callback(
			sched, {}, ibf, buf, sizeof(buf), {},
			[&](::std::cxx_std_error e, ::std::size_t n) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
				CHECK(n == 8);
			});
		CHECK(called);
		CHECK(::std::memcmp(buf, srcdata, 8) == 0);
		CHECK(srcpos == 64); /* the underflow pulled the whole window */
		::std::byte buf2[16]{};
		called = false;
		::fast_io::operations::async_pread_some_bytes_callback(
			sched, {}, ibf, buf2, sizeof(buf2), {},
			[&](::std::cxx_std_error e, ::std::size_t n) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
				CHECK(n == 16);
			});
		CHECK(called);
		CHECK(::std::memcmp(buf2, srcdata + 8, 16) == 0);
		CHECK(srcpos == 64); /* drained from the window, no device read */
		called = false;
		::fast_io::operations::async_pread_some_bytes_callback(
			sched, {}, ibf, buf2, 4, ::fast_io::intfpos_opt{100},
			[&](::std::cxx_std_error e, ::std::size_t n) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
				CHECK(n == 4);
			});
		CHECK(called);
		CHECK(::std::memcmp(buf2, srcdata + 100, 4) == 0);
		CHECK(srcpos == 64); /* explicit offset left the position alone */
	}

	/* buffered output: writes land in the buffer and report inline;
	 * async_output_stream_flush pushes them to the device */
	{
		::std::byte dstdata[256]{};
		::std::size_t dstpos{};
		::fast_io::basic_io_buffer<mock_file,
								   ::fast_io::basic_io_buffer_traits<::fast_io::buffer_mode::out,
																	 ::fast_io::native_global_allocator,
																	 void, char, 0, 64>>
			obf{mock_file{dstdata, sizeof(dstdata), __builtin_addressof(dstpos)}};
		::std::byte wdata[8];
		::std::memset(wdata, 0x5A, sizeof(wdata));
		bool called{};
		::fast_io::operations::async_pwrite_some_bytes_callback(
			sched, {}, obf, wdata, sizeof(wdata), {},
			[&](::std::cxx_std_error e, ::std::size_t n) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
				CHECK(n == 8);
			});
		CHECK(called);
		CHECK(dstpos == 0); /* still buffered — no device write */
		called = false;
		::fast_io::operations::async_output_stream_flush_callback(
			sched, {}, obf,
			[&](::std::cxx_std_error e) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
			});
		CHECK(called);
		CHECK(dstpos == 8);
		CHECK(::std::memcmp(dstdata, wdata, 8) == 0);
	}

	/* buffered output overflow: a write that does not fit flushes the
	 * pending bytes, then the oversized range goes to the device */
	{
		::std::byte dstdata[256]{};
		::std::size_t dstpos{};
		::fast_io::basic_io_buffer<mock_file,
								   ::fast_io::basic_io_buffer_traits<::fast_io::buffer_mode::out,
																	 ::fast_io::native_global_allocator,
																	 void, char, 0, 64>>
			obf{mock_file{dstdata, sizeof(dstdata), __builtin_addressof(dstpos)}};
		::std::byte wdata[60];
		::std::memset(wdata, 0x11, sizeof(wdata));
		bool called{};
		::fast_io::operations::async_pwrite_some_bytes_callback(
			sched, {}, obf, wdata, sizeof(wdata), {},
			[&](::std::cxx_std_error e, ::std::size_t n) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
				CHECK(n == 60);
			});
		CHECK(called);
		CHECK(dstpos == 0);
		::std::byte big[100];
		for (::std::size_t i{}; i != sizeof(big); ++i)
		{
			big[i] = static_cast<::std::byte>(i);
		}
		called = false;
		::fast_io::operations::async_pwrite_some_bytes_callback(
			sched, {}, obf, big, sizeof(big), {},
			[&](::std::cxx_std_error e, ::std::size_t n) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
				CHECK(n == 100);
			});
		CHECK(called);
		CHECK(dstpos == 160); /* pending 60 flushed + 100 written directly */
		CHECK(::std::memcmp(dstdata, wdata, 60) == 0);
		CHECK(::std::memcmp(dstdata + 60, big, 100) == 0);
	}

	/* tied iobuf: a read flushes pending output to the device first */
	{
		::std::byte data[256]{};
		::std::size_t pos{};
		::fast_io::basic_io_buffer<mock_file,
								   ::fast_io::basic_io_buffer_traits<
									   ::fast_io::buffer_mode::in | ::fast_io::buffer_mode::out |
										   ::fast_io::buffer_mode::tie,
									   ::fast_io::native_global_allocator, char, char, 64, 64>>
			iobf{mock_file{data, sizeof(data), __builtin_addressof(pos)}};
		::std::byte wdata[8];
		::std::memset(wdata, 0x77, sizeof(wdata));
		bool called{};
		::fast_io::operations::async_pwrite_some_bytes_callback(
			sched, {}, iobf, wdata, sizeof(wdata), {},
			[&](::std::cxx_std_error e, ::std::size_t n) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
				CHECK(n == 8);
			});
		CHECK(called);
		CHECK(pos == 0); /* sitting in the output buffer */
		::std::byte rbuf[8]{};
		called = false;
		::fast_io::operations::async_pread_some_bytes_callback(
			sched, {}, iobf, rbuf, sizeof(rbuf), {},
			[&](::std::cxx_std_error e, ::std::size_t n) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
				CHECK(n == 8);
			});
		CHECK(called);
		CHECK(::std::memcmp(data, wdata, 8) == 0); /* flush wrote first */
		CHECK(pos == 8 + 64);                      /* flush 8, then underflow read 64 */
	}

	/* async_scan over a buffered stream: the mock serves the header
	 * in pieces across underflows */
	{
		char const text[]{"HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\n"};
		auto t{coro_scan(sched, reinterpret_cast<::std::byte *>(const_cast<char *>(text)),
						 sizeof(text) - 1)};
		t.resume();
		try
		{
			t.rethrow_if_error();
		}
		catch throws(::std::error)
		{
			CHECK(false);
		}
	}

	/* buffered transmit out: pending output flushes to the device first;
	 * the payload bypasses the buffer entirely (sync parity) */
	{
		::std::byte dstdata[512]{};
		::std::size_t dstpos{};
		::fast_io::basic_io_buffer<mock_file,
								   ::fast_io::basic_io_buffer_traits<::fast_io::buffer_mode::out,
																	 ::fast_io::native_global_allocator,
																	 void, char, 0, 64>>
			obf{mock_file{dstdata, sizeof(dstdata), __builtin_addressof(dstpos)}};
		::std::byte wdata[8];
		::std::memset(wdata, 0x33, sizeof(wdata));
		bool called{};
		::fast_io::operations::async_pwrite_some_bytes_callback(
			sched, {}, obf, wdata, sizeof(wdata), {},
			[&](::std::cxx_std_error e, ::std::size_t n) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
				CHECK(n == 8);
			});
		CHECK(called);
		CHECK(dstpos == 0);
		::std::byte srcdata[200];
		for (::std::size_t i{}; i != sizeof(srcdata); ++i)
		{
			srcdata[i] = static_cast<::std::byte>(i);
		}
		::std::size_t srcpos{};
		mock_file src{srcdata, sizeof(srcdata), __builtin_addressof(srcpos)};
		called = false;
		::fast_io::operations::async_transmit_some_bytes_callback(
			sched, {}, obf, {}, src, {}, ::fast_io::size_t_opt{200},
			[&](::std::cxx_std_error e, ::std::size_t n) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
				CHECK(n == 200);
			});
		CHECK(called);
		CHECK(dstpos == 208); /* pending 8 flushed + 200 transferred */
		CHECK(::std::memcmp(dstdata, wdata, 8) == 0);
		CHECK(::std::memcmp(dstdata + 8, srcdata, 200) == 0);
	}

	/* buffered transmit in: pending input drains through the transfer
	 * before the handle is read */
	{
		::std::byte srcdata[256];
		for (::std::size_t i{}; i != sizeof(srcdata); ++i)
		{
			srcdata[i] = static_cast<::std::byte>(i);
		}
		::std::size_t srcpos{};
		::fast_io::basic_io_buffer<mock_file,
								   ::fast_io::basic_io_buffer_traits<::fast_io::buffer_mode::in,
																	 ::fast_io::native_global_allocator,
																	 char, void, 64>>
			ibf{mock_file{srcdata, sizeof(srcdata), __builtin_addressof(srcpos)}};
		::std::byte warm[8]{};
		bool called{};
		::fast_io::operations::async_pread_some_bytes_callback(
			sched, {}, ibf, warm, sizeof(warm), {},
			[&](::std::cxx_std_error e, ::std::size_t n) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
				CHECK(n == 8);
			});
		CHECK(called);
		CHECK(srcpos == 64); /* window holds 56 pending */
		::std::byte dstdata[256]{};
		::std::size_t dstpos{};
		mock_file dst{dstdata, sizeof(dstdata), __builtin_addressof(dstpos)};
		called = false;
		::fast_io::operations::async_transmit_some_bytes_callback(
			sched, {}, dst, {}, ibf, {}, ::fast_io::size_t_opt{120},
			[&](::std::cxx_std_error e, ::std::size_t n) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
				CHECK(n == 56); /* one round served the pending window */
			});
		CHECK(called);
		CHECK(::std::memcmp(dstdata, srcdata + 8, 56) == 0); /* pending first */
		CHECK(dstpos == 56);
	}

	/* async_close on a plain stream: the define marks the handle
	 * consumed; the callback reports success */
	{
		::std::byte data[8]{};
		::std::size_t pos{};
		mock_file f{data, sizeof(data), __builtin_addressof(pos)};
		bool called{};
		::fast_io::operations::async_close_callback(
			sched, {}, f,
			[&](::std::cxx_std_error e) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
			});
		CHECK(called);
		CHECK(pos == ::std::numeric_limits<::std::size_t>::max());
	}

	/* async_close on a buffered output stream: pending bytes flush to
	 * the device BEFORE the handle close runs */
	{
		::std::byte dstdata[256]{};
		::std::size_t dstpos{};
		::fast_io::basic_io_buffer<mock_file,
								   ::fast_io::basic_io_buffer_traits<::fast_io::buffer_mode::out,
																	 ::fast_io::native_global_allocator,
																	 void, char, 0, 64>>
			obf{mock_file{dstdata, sizeof(dstdata), __builtin_addressof(dstpos)}};
		::std::byte wdata[8];
		::std::memset(wdata, 0x42, sizeof(wdata));
		bool called{};
		::fast_io::operations::async_pwrite_some_bytes_callback(
			sched, {}, obf, wdata, sizeof(wdata), {},
			[&](::std::cxx_std_error e, ::std::size_t n) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
				CHECK(n == 8);
			});
		CHECK(called);
		CHECK(dstpos == 0); /* still buffered */
		called = false;
		::fast_io::operations::async_close_callback(
			sched, {}, obf,
			[&](::std::cxx_std_error e) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
			});
		CHECK(called);
		CHECK(dstpos == ::std::numeric_limits<::std::size_t>::max());
		CHECK(::std::memcmp(dstdata, wdata, 8) == 0); /* flush beat the close */
	}

	/* coroutine form of async_close on a buffered in+out stream */
	{
		::std::byte dstdata[256]{};
		::std::size_t dstpos{};
		::fast_io::basic_io_buffer<mock_file,
								   ::fast_io::basic_io_buffer_traits<
									   ::fast_io::buffer_mode::in | ::fast_io::buffer_mode::out,
									   ::fast_io::native_global_allocator, char, char, 64, 64>>
			iobf{mock_file{dstdata, sizeof(dstdata), __builtin_addressof(dstpos)}};
		::std::byte wdata[4];
		::std::memset(wdata, 0x99, sizeof(wdata));
		bool called{};
		::fast_io::operations::async_pwrite_some_bytes_callback(
			sched, {}, iobf, wdata, sizeof(wdata), {},
			[&](::std::cxx_std_error e, ::std::size_t) noexcept {
				called = true;
				CHECK(e.domain == nullptr);
			});
		CHECK(called);
		/* the buffered close flushes pending output then runs the
		 * handle's own close define — the coroutine form */
		auto t{coro_close_out(sched, iobf)};
		t.resume();
		try
		{
			t.rethrow_if_error();
		}
		catch throws(::std::error)
		{
			CHECK(false);
		}
		CHECK(dstpos == ::std::numeric_limits<::std::size_t>::max());
		CHECK(::std::memcmp(dstdata, wdata, 4) == 0);
	}

	/* async_connect: the mock copies the peer address at submission */
	{
		::std::byte conbuf[16]{};
		::std::size_t conpos{};
		mock_file con{conbuf, sizeof(conbuf), __builtin_addressof(conpos)};
		::std::uint_least32_t dest{0x7f000001u};
		bool fired{};
		::fast_io::operations::async_connect_callback(
			sched, {}, con, __builtin_addressof(dest), sizeof(dest),
			[&](::std::cxx_std_error e) noexcept {
				fired = true;
				CHECK(e.domain == nullptr);
			});
		CHECK(fired);
		CHECK(conpos == sizeof(dest));
		CHECK(::std::memcmp(conbuf, __builtin_addressof(dest), sizeof(dest)) == 0);

		/* coroutine form: the awaiter carries its own address copy */
		conpos = 0;
		::std::memset(conbuf, 0, sizeof(conbuf));
		auto t{coro_connect(sched, con, __builtin_addressof(dest), sizeof(dest))};
		t.resume();
		try
		{
			t.rethrow_if_error();
		}
		catch throws(::std::error)
		{
			CHECK(false);
		}
		CHECK(conpos == sizeof(dest));
		CHECK(::std::memcmp(conbuf, __builtin_addressof(dest), sizeof(dest)) == 0);
	}

	if (failures == 0)
	{
		::std::fprintf(stderr, "async generic: all ok\n");
		return 0;
	}
	::std::fprintf(stderr, "%d failures\n", failures);
	return 1;
}
