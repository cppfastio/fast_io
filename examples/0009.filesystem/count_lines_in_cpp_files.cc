#include<fast_io.h>
#include<fast_io_device.h>
#include<algorithm>

int main(int argc,char** argv)
{
	if(argc<2)
	{
		perr("Usage: ",fast_io::mnp::os_c_str(*argv)," <directory>\n");
		return 1;
	}
	fast_io::dir_file dir(fast_io::mnp::os_c_str(argv[1]));
	std::uint_least64_t lines{};
	for(auto ent : recursive(at(dir)))
	{
		if(type(ent)==fast_io::file_type::regular&&
			(extension(ent)==fast_io::u8cstring_view(u8".h")||
			extension(ent)==fast_io::u8cstring_view(u8".cc")||
			extension(ent)==fast_io::u8cstring_view(u8".cpp")||
			extension(ent)==fast_io::u8cstring_view(u8".hpp")||
			extension(ent)==fast_io::u8cstring_view(u8".cxx")))
		{
			fast_io::native_file_loader loader(drt(ent));
			lines+=std::ranges::count(loader,u8'\n');
		}
	}
	println("lines of C/C++:",lines);
}
