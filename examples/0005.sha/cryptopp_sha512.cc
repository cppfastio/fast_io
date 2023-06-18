#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/cryptopp_driver.h>
#include<cryptopp/sha.h>

/*
Need to install CryptoPP first
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
	fast_io::ibuf_file ibf(os_c_str(argv[1]));
	fast_io::cryptopp::iterated_hash_context<CryptoPP::SHA512> ctx;
	auto transmitted{::fast_io::operations::transmit_bytes_until_eof(as_file(ctx),ibf)};
	ctx.do_final();
	println(hash_digest(ctx)," *",os_c_str(argv[1]),"\nTransmitted:",transmitted," bytes\tElapsed Time:",fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime)-t0);
}
