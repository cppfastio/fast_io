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
//fill 100 zero bytes after the end of the file. Some libraries like simdjson requires adding padding bytes after the data
	fast_io::allocation_file_loader loader(::fast_io::file_loader_extra_bytes{100},::fast_io::mnp::os_c_str(argv[1]));
	//This will load entire file to memory through memory mapping.
	/*
	This is a contiguous container of the file.
	You can do these things:
	std::size_t sum{};
	for(auto e:loader)
		sum+=e;	
	*/
	print(loader);
}
