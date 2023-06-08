#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_crypto.h>

/*
This is just for demo purpose. You should avoid md5 in general because it is insecure.
*/

using namespace fast_io::io;

int main(int argc,char** argv)
{
	using namespace fast_io::mnp;
	if(argc!=2)
	{
		if(argc==0)
		{
			return 1;
		}
		perr("Usage: ",os_c_str(*argv)," <file>\n");
		return 1;
	}
	auto t0{fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime)};
	fast_io::md5_context md;
	fast_io::ibuf_file ibf(os_c_str(argv[1]));
	auto transmitted{::fast_io::operations::transmit_bytes_until_eof(as_file(md),ibf)};
	md.do_final();
	println(hash_digest(md)," *",os_c_str(argv[1]),"\nTransmitted:",transmitted," bytes\tElapsed Time:",fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime)-t0);
}
