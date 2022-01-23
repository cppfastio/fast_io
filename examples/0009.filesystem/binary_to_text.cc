#include<fast_io_device.h>
#include<fast_io.h>

int main(int argc,char const** argv)
try
{
	if(argc<3)
	{
		perr("Usage: ",fast_io::mnp::os_c_str(*argv)," <source directory> <dest directory>\n");
		return 1;
	}
	fast_io::dir_file df(::fast_io::mnp::os_c_str(argv[1]));
	fast_io::dir_file df_crlf(::fast_io::mnp::os_c_str(argv[2]));
	for(auto ent: current(at(df)))
		if(extension(ent)==fast_io::u8cstring_view(u8".cc"))
		{
			fast_io::native_file pf(drt(ent),fast_io::open_mode::in|fast_io::open_mode::text);//sorry i am stupid
			fast_io::c_file_unlocked pf2(at(df_crlf),native_filename(ent),fast_io::open_mode::out|fast_io::open_mode::text);
			transmit(pf2,pf);
		}
}
catch(fast_io::error e)
{
	perrln(e);
}