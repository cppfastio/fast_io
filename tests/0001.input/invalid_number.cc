#include<fast_io.h>
#include<fast_io_device.h>

int main()
try
{
	fast_io::u8ibuf_file ibf(u8"invalid.txt");
	std::size_t s{};
	scan(ibf,s);
	println(s);
}
catch(fast_io::parse_code code)
{
	perrln(code);
	return 1;
}