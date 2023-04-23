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
