#include <fast_io.h>
#include <fast_io_device.h>

namespace fi = ::fast_io;
namespace fop = ::fast_io::operations;

/*
 * One detached coroutine per connection: a single transmit_some round —
 * read what arrived, write it back, done. No buffer management anywhere;
 * the transmit engine owns its scratch space. A detached frame reports
 * errors to nobody: an escaping herbception is stored in the promise and
 * released when the frame self-destroys at final_suspend.
 */
static fi::io_async_task<> echo_session(fi::io_async_observer sched, fi::native_socket_file sock) throws
{
	co_await fop::async_transmit_some_bytes(sched, {}, sock, {}, sock, {}, {});
}

/*
 * The accept coroutine: same shape as asio's listener() — one suspended
 * coroutine accepts connections and detaches a session task per client.
 * async_accept needs no nonblocking listener: the io_uring sqe /
 * AcceptEx completion carries the accepted handle.
 */
static fi::io_async_task<> accept_loop(fi::io_async_observer sched, fi::native_socket_file listener) throws
{
	for (;;)
	{
		/* accepted sockets need async capability: no_block marks the
		 * socket overlapped on win32; posix schedulers need nothing.
		 * async_accept yields the owning native_socket_file — the
		 * handle can never escape ownership */
		echo_session(sched,
					 co_await fop::async_accept(sched, {}, listener, fi::open_mode::no_block))
			.detach();
	}
}

int main()
{
	fi::net_service service;
	fi::io_async_scheduler scheduler{fi::io_async};

	/* supervisor: when the accept loop dies to a herbception (listener
	 * error, accept failure), the frame unwinds here — log and rebuild
	 * it instead of letting one dead listener take the server down */
	for (;;)
	try
	{
		/* the listener itself must be async-capable too: no_block maps to
		 * WSA_FLAG_OVERLAPPED on win32, which AcceptEx requires on the
		 * listen socket */
		auto acceptor{accept_loop(scheduler, fi::native_socket_file{
									   fi::tcp_listen(2000, fi::open_mode::no_block)})};
		acceptor.resume();
		while (!acceptor.done())
		{
			fi::io_async_wait(scheduler);
		}
		acceptor.rethrow_if_error();
	}
	catch throws(::std::error e)
	{
		fi::perrln(e);
	}
}
