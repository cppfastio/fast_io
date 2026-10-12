#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_driver/timer.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;

namespace fi = ::fast_io;

static constexpr ::std::size_t N{10000000};

static fi::io_async_task<> output(fi::io_async_observer sched) throws
{
	/* no_block keeps the file async-capable where the platform needs it
	 * (win32 maps it to FILE_FLAG_OVERLAPPED, which IOCP requires) */
	::fast_io::obuf_file obf("iobuf_file_async.txt",
							 fi::open_mode::out | fi::open_mode::no_block);
	for (::std::size_t i{}; i != N; ++i)
	{
		co_await fi::async_println(sched, {}, obf, i);
	}
	co_await fi::operations::async_output_stream_flush(sched, {}, obf);
}

static fi::io_async_task<> input(fi::io_async_observer sched,
								 ::fast_io::vector<::std::size_t> &vec) throws
{
	::fast_io::ibuf_file ibf("iobuf_file_async.txt",
							 fi::open_mode::in | fi::open_mode::no_block);
	for (::std::size_t i{}; i != N; ++i)
	{
		co_await fi::async_scan(sched, {}, ibf, vec[i]);
	}
}

int main()
try
{
	fi::io_async_scheduler scheduler{fi::io_async};
	{
		::fast_io::timer t(u8"output");
		auto task{output(scheduler)};
		task.resume();
		while (!task.done())
		{
			fi::io_async_wait(scheduler);
		}
		task.rethrow_if_error();
	}
	::fast_io::vector<::std::size_t> vec(N);
	{
		::fast_io::timer t(u8"input");
		auto task{input(scheduler, vec)};
		task.resume();
		while (!task.done())
		{
			fi::io_async_wait(scheduler);
		}
		task.rethrow_if_error();
	}
}
catch throws(::std::error e)
{
	fi::perrln(e);
	return 1;
}
