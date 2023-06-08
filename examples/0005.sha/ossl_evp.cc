#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_hosted_crypto.h>

using namespace fast_io::io;

int main(int argc,char** argv)
try
{
	using namespace fast_io::mnp;
	if(argc!=3)
	{
		if(argv==0)
		{
			return 1;
		}
		perr("Usage: ",os_c_str(*argv)," <hash algorithm> <file>\n");
		return 1;
	}
	auto t0{fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime)};
	fast_io::ossl_evp_hash_file ctx(::fast_io::mnp::os_c_str(argv[1]));
	fast_io::ibuf_file ibf(os_c_str(argv[2]));
	auto transmitted{::fast_io::operations::transmit_bytes_until_eof(as_file(ctx),ibf)};
	ctx.do_final();
	println(hash_digest(ctx)," *",os_c_str(argv[2]),"\nAlgorithm:",::fast_io::mnp::os_c_str(argv[1]),"\nTransmitted:",transmitted," bytes\tElapsed Time:",fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime)-t0);
}
catch(fast_io::error e)
{
	perrln(e);
	return 1;
}
