#include<string_view>
#include<fast_io.h>
#include<fast_io_device.h>

int main()
try
{
	fast_io::http_header_buffer bf;
	fast_io::ibuf_file ibf(u8"test.txt");
	scan(ibf,bf);
	println("request:",bf.request(),"\n"
	"code:",bf.code(),"\n"
	"reason:",bf.reason());
	for(auto [key,value]:line_generator(bf))
	{
		println("key=",key," value=",value);
	}
}
catch(fast_io::parse_code c)
{
	perrln(c);
}
