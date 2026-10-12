#include <fast_io.h>
#include <fast_io_device.h>
#include <coroutine>
#include <cerrno>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>

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

using test_task = ::fast_io::io_async_task<>;

namespace fad = ::fast_io::operations;

static test_task coro_accept(::fast_io::linux_io_uring_observer sched, int listen_fd,
							 ::fast_io::posix_file *out) throws
{
	*out = co_await fad::async_accept(sched, {}, ::fast_io::posix_io_observer{listen_fd},
									  ::fast_io::open_mode{});
}

static test_task coro_close(::fast_io::linux_io_uring_observer sched, int fd) throws
{
	co_await fad::async_close(sched, {}, ::fast_io::posix_io_observer{fd});
}

static test_task coro_connect(::fast_io::linux_io_uring_observer sched, int fd, void const *addr,
							  ::std::size_t addrlen) throws
{
	co_await fad::async_connect(sched, {}, ::fast_io::posix_io_observer{fd}, addr, addrlen);
}

static test_task coro_connect_v4(::fast_io::linux_io_uring_observer sched, int fd,
								 ::fast_io::ipv4 dest) throws
{
	co_await fad::async_connect(sched, {}, ::fast_io::posix_io_observer{fd}, dest);
}

static test_task coro_main(::fast_io::linux_io_uring_observer sched, int fd_in, int fd_out) throws
{
	::std::byte buf[128]{};
	/* positioned read */
	auto n{co_await fad::async_pread_some_bytes(sched, {}, ::fast_io::posix_io_observer{fd_in}, buf,
												sizeof(buf), ::fast_io::intfpos_opt{0})};
	CHECK(n == 128);
	CHECK(buf[0] == ::std::byte{'A'});
	/* positioned write_all through the coroutine form */
	co_await fad::async_pwrite_all_bytes(sched, {}, ::fast_io::posix_io_observer{fd_out}, buf, 64,
										 ::fast_io::intfpos_opt{0});
}

int main()
{
	namespace fi = fast_io;
	fi::linux_io_uring ring{fi::native_interface, 32, 0};
	fi::linux_io_uring_observer sched{ring.native_handle()};

	::std::byte filedata[128];
	for (::std::size_t i{}; i != sizeof(filedata); ++i)
	{
		filedata[i] = static_cast<::std::byte>('A' + (i & 15));
	}
	fi::posix_file fin{"/tmp/uring_in.bin",
					   fi::open_mode::out | fi::open_mode::in | fi::open_mode::creat | fi::open_mode::trunc};
	fi::posix_file fout{"/tmp/uring_out.bin",
						fi::open_mode::out | fi::open_mode::in | fi::open_mode::creat | fi::open_mode::trunc};
	fi::operations::write_all_bytes(fi::posix_io_observer{fin.native_handle()}, filedata, sizeof(filedata));

	/* coroutine forms */
	{
		auto t{coro_main(sched, fin.native_handle(), fout.native_handle())};
		t.resume();
		while (!t.done())
		{
			fi::io_async_wait(sched);
		}
		try
		{
			t.rethrow_if_error();
		}
		catch throws(::std::error)
		{
			CHECK(false);
		}
		char check[64]{};
		auto got{::pread(fout.native_handle(), check, 64, 0)};
		CHECK(got == 64);
		CHECK(::std::memcmp(check, filedata, 64) == 0);
	}

	/* callback form: read_all at current position */
	{
		CHECK(::lseek(fin.native_handle(), 0, SEEK_SET) == 0);
		bool fired{};
		::std::cxx_std_error err{};
		::std::byte buf[128]{};
		fad::async_pread_all_bytes_callback(
			sched, {}, fi::posix_io_observer{fin.native_handle()}, buf, sizeof(buf), {},
			[&](::std::cxx_std_error e) noexcept {
				fired = true;
				err = e;
			});
		while (!fired)
		{
			fi::io_async_wait(sched);
		}
		CHECK(err.domain == nullptr);
		CHECK(::std::memcmp(buf, filedata, 128) == 0);
	}

	/* native scatter writev/readv */
	{
		::std::byte a[16], b[16];
		for (::std::size_t i{}; i != 16; ++i)
		{
			a[i] = ::std::byte{0x11};
			b[i] = ::std::byte{0x22};
		}
		fi::io_scatter_t wv[2]{{a, 16}, {b, 16}};
		bool fired{};
		fad::async_scatter_pwrite_all_bytes_callback(
			sched, {}, fi::posix_io_observer{fout.native_handle()}, wv, 2, ::fast_io::intfpos_opt{64},
			[&](::std::cxx_std_error e) noexcept {
				fired = true;
				CHECK(e.domain == nullptr);
			});
		while (!fired)
		{
			fi::io_async_wait(sched);
		}
		char vbuf[32]{};
		auto got{::pread(fout.native_handle(), vbuf, 32, 64)};
		CHECK(got == 32 && vbuf[0] == 0x11 && vbuf[16] == 0x22);

		fi::io_scatter_t rv[2]{{a, 16}, {b, 16}};
		::std::memset(a, 0, 16);
		::std::memset(b, 0, 16);
		fired = false;
		fi::io_scatter_status_t st{};
		fad::async_scatter_pread_some_bytes_callback(
			sched, {}, fi::posix_io_observer{fout.native_handle()}, rv, 2, ::fast_io::intfpos_opt{64},
			[&](::std::cxx_std_error e, fi::io_scatter_status_t s) noexcept {
				fired = true;
				st = s;
				CHECK(e.domain == nullptr);
			});
		while (!fired)
		{
			fi::io_async_wait(sched);
		}
		CHECK(st.position == 2 && st.position_in_scatter == 0);
		CHECK(a[0] == ::std::byte{0x11} && b[0] == ::std::byte{0x22});
	}

	/* transmit regular->regular: splice cannot serve it -> bounce fallback */
	{
		bool fired{};
		fad::async_transmit_some_bytes_callback(
			sched, {}, fi::posix_io_observer{fout.native_handle()}, ::fast_io::intfpos_opt{200},
			fi::posix_io_observer{fin.native_handle()}, ::fast_io::intfpos_opt{0},
			fi::size_t_opt{64},
			[&](::std::cxx_std_error e, ::std::size_t n) noexcept {
				fired = true;
				CHECK(e.domain == nullptr);
				CHECK(n == 64);
			});
		while (!fired)
		{
			fi::io_async_wait(sched);
		}
		char tbuf[64]{};
		auto got{::pread(fout.native_handle(), tbuf, 64, 200)};
		CHECK(got == 64);
		CHECK(::std::memcmp(tbuf, filedata, 64) == 0);
	}

	/* transmit pipe->file: real SPLICE fast path */
	{
		int pfds[2];
		CHECK(::pipe(pfds) == 0);
		char const pipemsg[] = "splice-pipe-payload-0123456789abcdef";
		CHECK(::write(pfds[1], pipemsg, sizeof(pipemsg) - 1) == static_cast<long>(sizeof(pipemsg) - 1));
		bool fired{};
		::std::size_t moved{};
		fad::async_transmit_some_bytes_callback(
			sched, {}, fi::posix_io_observer{fout.native_handle()}, ::fast_io::intfpos_opt{400},
			fi::posix_io_observer{pfds[0]}, {},
			fi::size_t_opt{sizeof(pipemsg) - 1},
			[&](::std::cxx_std_error e, ::std::size_t n) noexcept {
				fired = true;
				moved = n;
				CHECK(e.domain == nullptr);
			});
		while (!fired)
		{
			fi::io_async_wait(sched);
		}
		CHECK(moved == sizeof(pipemsg) - 1);
		char pbuf[sizeof(pipemsg)]{};
		auto got{::pread(fout.native_handle(), pbuf, sizeof(pipemsg) - 1, 400)};
		CHECK(got == static_cast<long>(sizeof(pipemsg) - 1));
		CHECK(::std::memcmp(pbuf, pipemsg, sizeof(pipemsg) - 1) == 0);
		::close(pfds[0]);
		::close(pfds[1]);
	}

	/* operation timeout: a pipe read that can never complete must come
	 * back errc::timed_out once the armed LINK_TIMEOUT expires */
	{
		int pfds[2];
		CHECK(::pipe(pfds) == 0);
		::std::byte buf[8]{};
		bool fired{};
		::std::cxx_std_error err{};
		fad::async_pread_some_bytes_callback(
			sched, fi::posix_statx_timestamp_opt{fi::posix_statx_timestamp64{0, 50000000}},
			fi::posix_io_observer{pfds[0]}, buf, sizeof(buf), {},
			[&](::std::cxx_std_error e, ::std::size_t) noexcept {
				fired = true;
				err = e;
			});
		while (!fired)
		{
			fi::io_async_wait(sched);
		}
		CHECK(fired);
		CHECK(err.domain == ::std::error_domain<::std::errc>::domain());
		CHECK(err.code == static_cast<::std::size_t>(::std::errc::timed_out));
		::close(pfds[0]);
		::close(pfds[1]);
	}

	/* buffered input over a real posix fd: window underflow, inline
	 * drain, explicit-offset bypass */
	{
		CHECK(::lseek(fin.native_handle(), 0, SEEK_SET) == 0);
		::fast_io::basic_io_buffer<fi::posix_io_observer,
								   ::fast_io::basic_io_buffer_traits<::fast_io::buffer_mode::in,
																	 ::fast_io::native_global_allocator,
																	 char, void, 64>>
			ibf{fi::posix_io_observer{fin.native_handle()}};
		::std::byte buf[8]{};
		bool fired{};
		fad::async_pread_some_bytes_callback(
			sched, {}, ibf, buf, sizeof(buf), {},
			[&](::std::cxx_std_error e, ::std::size_t n) noexcept {
				fired = true;
				CHECK(e.domain == nullptr);
				CHECK(n == 8);
			});
		while (!fired)
		{
			fi::io_async_wait(sched);
		}
		CHECK(::std::memcmp(buf, filedata, 8) == 0);
		CHECK(::lseek(fin.native_handle(), 0, SEEK_CUR) == 64);
	}

	/* bad fd: EBADF rides the callback, not an exception */
	{
		bool fired{};
		::std::cxx_std_error err{};
		::std::byte buf[8]{};
		fad::async_pread_some_bytes_callback(
			sched, {}, fi::posix_io_observer{-1}, buf, sizeof(buf), {},
			[&](::std::cxx_std_error e, ::std::size_t) noexcept {
				fired = true;
				err = e;
			});
		while (!fired)
		{
			fi::io_async_wait(sched);
		}
		CHECK(err.code == static_cast<::std::size_t>(EBADF));
	}

	/* async_close: the kernel closes the fd; the file object is released
	 * at submission so its destructor cannot double-close */
	{
		fi::posix_file cf{"/tmp/uring_close.bin",
						  fi::open_mode::out | fi::open_mode::in | fi::open_mode::creat |
							  fi::open_mode::trunc};
		int const cfd{cf.native_handle()};
		bool fired{};
		::std::cxx_std_error err{};
		fad::async_close_callback(
			sched, {}, cf,
			[&](::std::cxx_std_error e) noexcept {
				fired = true;
				err = e;
			});
		CHECK(cf.native_handle() == -1); /* released at submission */
		while (!fired)
		{
			fi::io_async_wait(sched);
		}
		CHECK(err.domain == nullptr);
		CHECK(::fcntl(cfd, F_GETFD) == -1 && errno == EBADF);
	}

	/* buffered async_close: pending output must reach the device before
	 * the underlying close runs — the async analog of close() */
	{
		fi::posix_file bf{"/tmp/uring_close_buf.bin",
						  fi::open_mode::out | fi::open_mode::in | fi::open_mode::creat |
							  fi::open_mode::trunc};
		::fast_io::basic_io_buffer<fi::posix_file,
								   ::fast_io::basic_io_buffer_traits<::fast_io::buffer_mode::out,
																	 ::fast_io::native_global_allocator,
																	 void, char, 0, 64>>
			obf{::std::move(bf)};
		::std::byte wdata[8];
		for (::std::size_t i{}; i != sizeof(wdata); ++i)
		{
			wdata[i] = static_cast<::std::byte>('q' + i);
		}
		bool fired{};
		fad::async_pwrite_all_bytes_callback(
			sched, {}, obf, wdata, sizeof(wdata), {},
			[&](::std::cxx_std_error e) noexcept {
				fired = true;
				CHECK(e.domain == nullptr);
			});
		while (!fired)
		{
			fi::io_async_wait(sched);
		}
		char peek{};
		CHECK(::pread(obf.handle.native_handle(), &peek, 1, 0) == 0); /* still buffered */
		fired = false;
		fad::async_close_callback(
			sched, {}, obf,
			[&](::std::cxx_std_error e) noexcept {
				fired = true;
				CHECK(e.domain == nullptr);
			});
		while (!fired)
		{
			fi::io_async_wait(sched);
		}
		/* reopen and confirm the pending bytes beat the close */
		char vbuf[8]{};
		{
			fi::posix_file chk{"/tmp/uring_close_buf.bin", fi::open_mode::in};
			CHECK(::read(chk.native_handle(), vbuf, 8) == 8);
		}
		CHECK(::std::memcmp(vbuf, wdata, 8) == 0);
	}

	/* coroutine async_close on a socket pair fd */
	{
		int sv[2];
		CHECK(::socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == 0);
		auto t{coro_close(sched, sv[0])};
		t.resume();
		while (!t.done())
		{
			fi::io_async_wait(sched);
		}
		try
		{
			t.rethrow_if_error();
		}
		catch throws(::std::error)
		{
			CHECK(false);
		}
		CHECK(::fcntl(sv[0], F_GETFD) == -1 && errno == EBADF);
		::close(sv[1]);
	}

	/* async accept: real loopback listener, connect from a helper socket */
	{
		fi::posix_file listener{fi::tcp_listen(0)};
		/* discover the bound port */
		::fast_io::posix_sockaddr_in bound{};
		::fast_io::posix_socklen_t blen{sizeof(bound)};
		::getsockname(listener.native_handle(), reinterpret_cast<struct sockaddr *>(&bound), &blen);
		auto port{::fast_io::big_endian(bound.sin_port)};

		fi::posix_file accepted{};
		auto t{coro_accept(sched, listener.native_handle(), __builtin_addressof(accepted))};
		t.resume();

		/* synchronous connect drives the completion */
		fi::posix_file client{fi::tcp_connect(fi::ipv4{{127, 0, 0, 1}, port})};
		while (!t.done())
		{
			fi::io_async_wait(sched);
		}
		try
		{
			t.rethrow_if_error();
		}
		catch throws(::std::error)
		{
			CHECK(false);
		}
		CHECK(accepted.native_handle() >= 0);
	}

	/* async connect: unconnected socket → IORING_OP_CONNECT to the
	 * loopback listener, then traffic through the connected socket */
	{
		fi::posix_file listener{fi::tcp_listen(0)};
		::fast_io::posix_sockaddr_in bound{};
		::fast_io::posix_socklen_t blen{sizeof(bound)};
		::getsockname(listener.native_handle(), reinterpret_cast<struct sockaddr *>(&bound), &blen);

		fi::posix_file client{fi::sock_family::inet, fi::sock_type::stream, fi::open_mode{},
							  fi::sock_protocol::tcp};
		auto t{coro_connect(sched, client.native_handle(), __builtin_addressof(bound),
							sizeof(bound))};
		t.resume();
		while (!t.done())
		{
			fi::io_async_wait(sched);
		}
		try
		{
			t.rethrow_if_error();
		}
		catch throws(::std::error)
		{
			CHECK(false);
		}
		/* the socket is usable after connect: accept the peer and do io */
		int serverfd{::accept(listener.native_handle(), nullptr, nullptr)};
		CHECK(serverfd >= 0);
		::std::byte wbuf[4]{::std::byte{'p'}, ::std::byte{'i'}, ::std::byte{'n'}, ::std::byte{'g'}};
		CHECK(::write(client.native_handle(), wbuf, 4) == 4);
		::std::byte rbuf[4]{};
		CHECK(::read(serverfd, rbuf, 4) == 4);
		CHECK(::std::memcmp(wbuf, rbuf, 4) == 0);
		::close(serverfd);

		/* the ipv4 convenience overload builds the sockaddr itself */
		fi::posix_file client2{fi::sock_family::inet, fi::sock_type::stream, fi::open_mode{},
							   fi::sock_protocol::tcp};
		auto port{::fast_io::big_endian(bound.sin_port)};
		auto t2{coro_connect_v4(sched, client2.native_handle(),
								fi::ipv4{{127, 0, 0, 1}, port})};
		t2.resume();
		while (!t2.done())
		{
			fi::io_async_wait(sched);
		}
		try
		{
			t2.rethrow_if_error();
		}
		catch throws(::std::error)
		{
			CHECK(false);
		}
	}

	/* async connect callback form + error path: a dead loopback port
	 * reports ECONNREFUSED through the callback */
	{
		fi::posix_file client{fi::sock_family::inet, fi::sock_type::stream, fi::open_mode{},
							  fi::sock_protocol::tcp};
		::fast_io::posix_sockaddr_in dead{};
		dead.sin_family = AF_INET;
		dead.sin_port = ::fast_io::big_endian(::std::uint_least16_t{1});
		dead.sin_addr.address[0] = 127;
		dead.sin_addr.address[3] = 1;
		bool fired{};
		fad::async_connect_callback(
			sched, {}, ::fast_io::posix_io_observer{client.native_handle()},
			__builtin_addressof(dead), sizeof(dead),
			[&](::std::cxx_std_error e) noexcept {
				fired = true;
				CHECK(e.domain != nullptr);
			});
		while (!fired)
		{
			fi::io_async_wait(sched);
		}
	}

	::std::fprintf(stderr, "io_uring test: %s (failures=%d)\n", failures == 0 ? "all ok" : "FAILED",
				   failures);
	return failures == 0 ? 0 : 1;
}
