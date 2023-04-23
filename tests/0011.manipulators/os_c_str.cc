#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	char const* ptr{"Hello World from char const*\n"};
	char8_t const* u8ptr{u8"Hello World from char8_t const*\n"};
	char32_t const* u32ptr{U"Hello World from char32_t const*\n"};
	print(fast_io::mnp::os_c_str(ptr),
		fast_io::mnp::code_cvt(fast_io::mnp::os_c_str(u8ptr)),
		fast_io::mnp::code_cvt(fast_io::mnp::os_c_str(u32ptr)));
}