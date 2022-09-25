#include<fast_io.h>
#include<fast_io_device.h>

thread_local fast_io::obuf_file obf(u8"/dev/null");

extern "C" int LLVMFuzzerTestOneInput(char16_t const* ptr,std::size_t n) noexcept
{
	auto e{::fast_io::details::find_dot_and_sep<true,char16_t,char16_t>(ptr,n/2)};
	write(obf,e.ptr,e.ptr+e.n);
        return 0;
}