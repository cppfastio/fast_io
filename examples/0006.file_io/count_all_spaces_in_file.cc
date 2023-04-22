#include<algorithm>
#include<fast_io.h>

using namespace fast_io::io;

int main(int argc,char** argv)
{
	if(argc!=2)
	{
		if(argc==0)
		{
			return 1;
		}
		perr("Usage: ",fast_io::mnp::os_c_str(*argv)," <file>\n");
		return 1;
	}
	fast_io::native_file_loader loader(::fast_io::mnp::os_c_str(argv[1]));
	println(std::ranges::count_if(loader,[](char8_t ch) noexcept
	{
		return fast_io::char_category::is_c_space(ch);
	}));
}
