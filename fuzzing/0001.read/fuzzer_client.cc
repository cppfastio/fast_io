#include<fast_io.h>
#include<fast_io_device.h>

fast_io::net_service serivce;
thread_local fast_io::iobuf_socket_file soc(fast_io::tcp_connect(fast_io::ipv4{{127,0,0,1},2000}));

extern "C" int LLVMFuzzerTestOneInput(std::uint8_t const* ptr,std::size_t n) noexcept
{
	write(soc,ptr,ptr+n);
	return 0;
}