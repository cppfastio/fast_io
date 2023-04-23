#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/openssl_driver.h>

thread_local fast_io::ossl::sha512_context ctx;
thread_local fast_io::obuf_file obf(u8"/dev/null");

using namespace fast_io::io;

extern "C" int LLVMFuzzerTestOneInput(std::uint8_t const* ptr,std::size_t n) noexcept
{
	ctx.reset();
	for(std::size_t i{};i!=16;++i)
	{
	        write(::fast_io::mnp::as_file(ctx),ptr,ptr+n);
	}
	ctx.do_final();
	println(obf,::fast_io::mnp::hash_digest(ctx));
        return 0;
}
