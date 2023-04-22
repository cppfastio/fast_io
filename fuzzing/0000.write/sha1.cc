#include<fast_io.h>
#include<fast_io_crypto.h>
#include<fast_io_device.h>

thread_local fast_io::sha1_context ctx;
thread_local fast_io::u32obuf_file obf(u8"/dev/null");

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
