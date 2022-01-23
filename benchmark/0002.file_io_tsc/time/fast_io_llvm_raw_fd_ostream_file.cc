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
	fast_io::llvm::raw_fd_ostream_file fsof(u8"fast_io_llvm_raw_fd_ostream_file.txt",fast_io::open_mode::out);
	for(std::size_t i{};i!=N;++i)
		println(fsof,i);
	}
}
