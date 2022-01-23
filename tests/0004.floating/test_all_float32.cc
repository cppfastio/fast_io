#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_crypto.h>
#include<fast_io_driver/timer.h>

int main()
{
	fast_io::timer tm(u8"test float32 ");
	fast_io::sha256 sha;
	{
	fast_io::basic_obuf<fast_io::hash_processor<fast_io::sha256>> sha_obf(sha);
	constexpr std::uint64_t v{UINT32_MAX};
	for(std::uint64_t i{};i<=v;++i)
	{
		std::uint32_t low32bits{static_cast<std::uint32_t>(i)};
		println(sha_obf,fast_io::mnp::scientific(std::bit_cast<float>(low32bits)));
	}
	flush(sha_obf);
	sha_obf.handle.do_final();
	}
	println("sha256=",sha);
}
