#include<string>
#include<fast_io.h>
#include<fast_io_device.h>

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
	for(auto ent : recursive(at(dir)))
	{
		if(type(ent)==fast_io::file_type::regular||type(ent)==fast_io::file_type::symlink)
		{
			std::u8string_view u8fn(u8filename(ent));
			std::u8string filename(u8fn);
			for(auto & e : filename)
			{
				e=fast_io::char_category::to_c_lower(e);
			}
			if(filename==u8fn)
			{
				continue;
			}
			native_renameat(drt(ent),at(ent),filename);
			println(fast_io::mnp::code_cvt(u8fn),"->",fast_io::mnp::code_cvt(filename));
				
		}
	}
}
