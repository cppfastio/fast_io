#include <fast_io.h>
#include <fast_io_device.h>

namespace fi = ::fast_io;

/*
 * Async a+b from a file — the async mirror of examples/0002.a+b:
 * async_scan parses two integers through the input buffer, refilling it
 * asynchronously under the hood; async_println formats the sum into the
 * output buffer and async_output_stream_flush drains it. The scan and
 * the print suspend the coroutine instead of blocking the pump.
 */
static fi::io_async_task<> work(fi::io_async_observer sched) throws
{
	/* real files opened with no_block — win32 maps it to
	 * FILE_FLAG_OVERLAPPED, which IOCP requires on every handle; std
	 * handles can never be async on Windows (consoles can't do overlapped
	 * I/O at all), so in()/out() are unusable there — files keep the
	 * example portable */
	::fast_io::ibuf_file inb{"input.txt", ::fast_io::open_mode::in | ::fast_io::open_mode::no_block};
	::fast_io::obuf_file outb{"output.txt", ::fast_io::open_mode::out | ::fast_io::open_mode::no_block};
	::std::size_t a{}, b{};
	co_await fi::async_scan(sched, {}, inb, a, b);
	co_await fi::async_println(sched, {}, outb, a + b);
	co_await fi::operations::async_output_stream_flush(sched, {}, outb);
}

int main()
try
{
	fi::io_async_scheduler scheduler{fi::io_async};

	auto task{work(scheduler)};
	task.resume();
	while (!task.done())
	{
		fi::io_async_wait(scheduler);
	}
	task.rethrow_if_error();
}
catch throws(::std::error e)
{
	fi::perrln(e);
	return 1;
}
