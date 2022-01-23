#include<fast_io.h>
#include<fast_io_device.h>

int main(int argc,char** argv)
{
	if(argc!=2)
	{
		perr("Usage: ",fast_io::mnp::os_c_str(*argv)," <file>\n");
		return 1;
	}
	fast_io::ibuf_file ibf(fast_io::mnp::os_c_str(argv[1]));
	transmit(fast_io::c_stdout(),ibf);//Transmit all bytes to FILE* stdout
}
