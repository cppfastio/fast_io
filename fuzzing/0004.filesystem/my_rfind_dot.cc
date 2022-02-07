#include<fast_io.h>

thread_local fast_io::obuf_file obf(u8"/dev/null");

extern "C" int LLVMFuzzerTestOneInput(std::uint8_t const* ptr,std::size_t n) noexcept
{
	auto e{::fast_io::details::my_rfind_dot(ptr,ptr+n)};
	write(obf,ptr,e);
        return 0;
}