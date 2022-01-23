#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/llvm.h>
#include<fast_io_driver/timer.h>
#include<vector>

int main()
{
	constexpr std::size_t N(10000000);
	{
	fast_io::timer t(u8"output");
	fast_io::llvm::raw_fd_ostream_file fsof(u8"llvm_raw_fd_ostream.txt",fast_io::open_mode::out);
	auto& os(*fsof.os);
	for(std::size_t i{};i!=N;++i)
	{
		os<<i<<'\n';
	}
	}
}
