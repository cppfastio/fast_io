#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	constexpr char8_t const testbuffer[]{u8"412741298414 1235321525"};
	::std::uint_least64_t val;
	auto [position,code] = ::fast_io::parse_by_scan(testbuffer,testbuffer+sizeof(testbuffer),val);
	if(code!=::fast_io::parse_code::ok)
	{
		perr("parse failed\n");
		return 1;
	}
	println("val=",val," position:",position-testbuffer);
}