#include<string_view>
#include<fast_io.h>
#include<fast_io_device.h>
#include<algorithm>

using namespace fast_io::io;

int main(int argc,char** argv)
{
	if(argc<2)
	{
		if(argc==0)
		{
			return 1;
		}
		perr("Usage: ",::fast_io::mnp::os_c_str(*argv)," <directory>\n");
		return 1;
	}
	using namespace std::string_view_literals;
	fast_io::dir_file dir(::fast_io::mnp::os_c_str(argv[1]));
	std::uint_least64_t lines{};
	for(auto ent : recursive(at(dir)))
	{
		std::u8string_view ext(u8extension(ent));
		if(type(ent)==fast_io::file_type::regular&&
			(ext==u8".h"sv ||
			ext==u8".cc"sv||
			ext==u8".cpp"sv||
			ext==u8".hpp"sv||
			ext==u8".cxx"sv))
		{
			fast_io::native_file_loader loader(drt(ent));
			lines+=std::ranges::count(loader,u8'\n');
		}
	}
	println("lines of C/C++:",lines);
}
