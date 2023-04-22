#include<string>
#include<string_view>
#include<fast_io.h>
#include<fast_io_device.h>

thread_local fast_io::obuf_file obf(u8"/dev/null");
thread_local fast_io::wobuf_file wobf(u8"/dev/null");
thread_local fast_io::u8obuf_file u8obf(u8"/dev/null");
thread_local fast_io::u16obuf_file u16obf(u8"/dev/null");
thread_local fast_io::u32obuf_file u32obf(u8"/dev/null");

using namespace fast_io::io;

extern "C" int LLVMFuzzerTestOneInput(std::uint8_t const* ptr,std::size_t n) noexcept
{
	std::string_view v(reinterpret_cast<char const*>(ptr),n);
	print(obf,fast_io::concat(v));
	print(wobf,fast_io::wconcat(fast_io::mnp::code_cvt(v)));
	print(u8obf,fast_io::u8concat(fast_io::mnp::code_cvt(v)));
	print(u16obf,fast_io::u16concat(fast_io::mnp::code_cvt(v)));
	print(u32obf,fast_io::u32concat(fast_io::mnp::code_cvt(v)));
	print(obf,fast_io::concat(fast_io::mnp::code_cvt(fast_io::u32concat(fast_io::mnp::code_cvt(v))),v),
		fast_io::concat(fast_io::mnp::code_cvt(fast_io::u16concat(fast_io::mnp::code_cvt(v))),v));
	print(wobf,fast_io::wconcat(fast_io::mnp::code_cvt(v)),fast_io::wconcat(fast_io::mnp::code_cvt(v)));
	print(u8obf,fast_io::u8concat(fast_io::mnp::code_cvt(v)),fast_io::u8concat(fast_io::mnp::code_cvt(v)));
	print(u16obf,fast_io::u16concat(fast_io::mnp::code_cvt(v)),fast_io::u16concat(fast_io::mnp::code_cvt(v)));
	print(u32obf,fast_io::u32concat(fast_io::mnp::code_cvt(v)),fast_io::u32concat(fast_io::mnp::code_cvt(v)));
	return 0;
}
