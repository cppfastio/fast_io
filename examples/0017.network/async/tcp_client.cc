#include <fast_io.h>
#include <fast_io_device.h>

static ::fast_io::io_async_task<> pump(::fast_io::io_async_observer sched,
									   ::fast_io::native_socket_file nsf) throws
{
	using namespace ::fast_io::iomnp;
	co_await ::fast_io::io::async_print(sched, {}, nsf, "Hello World\n");
	co_await ::fast_io::operations::async_transmit_all_bytes(sched, {},
															 ::fast_io::c_stdout(), {}, nsf, {}, {});
}

int main()
try
{
	::fast_io::net_service service;
	::fast_io::io_async_scheduler sched{::fast_io::io_async};
	auto pumping{pump(sched,
					  ::fast_io::native_socket_file(::fast_io::tcp_connect(::fast_io::ipv4{{127, 0, 0, 1}, 2000},
																		   ::fast_io::open_mode::no_block)))};
	for (pumping.resume(); !pumping.done(); ::fast_io::io_async_wait(sched))
	{
	}
	pumping.rethrow_if_error();
}
catch throws(::std::error e)
{
	::fast_io::io::perrln(e);
	return 1;
}
