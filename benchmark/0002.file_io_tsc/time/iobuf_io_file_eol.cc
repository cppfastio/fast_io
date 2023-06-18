#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/timer.h>
#include<vector>
using namespace fast_io::io;

int main()
{
	constexpr std::size_t N(10000000);
	{
	fast_io::timer t(u8"output");
	fast_io::iobuf_io_file obf(::fast_io::io_place_type<::fast_io::native_file>,"iobuf_io_file_eol.txt",::fast_io::open_mode::out);
	fast_io::operations::add_io_decos(obf,::fast_io::decorators::native_eol{});
	for(std::size_t i{};i!=N;++i)
		println(obf,i);
	}
	std::vector<std::size_t> vec(N);
	{
	fast_io::timer t(u8"input");
	fast_io::iobuf_io_file ibf(::fast_io::io_place_type<::fast_io::native_file>,"iobuf_io_file_eol.txt",::fast_io::open_mode::in);
	fast_io::operations::add_io_decos(ibf,::fast_io::decorators::native_eol{});
	for(std::size_t i{};i!=N;++i)
		scan(ibf,vec[i]);
	}
}

