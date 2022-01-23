#include<fast_io.h>


extern "C" int LLVMFuzzerTestOneInput(std::uint8_t const* ptr,std::size_t n) noexcept
{
	::fast_io::details::basic_concat_buffer<char> buffer;
	auto oref{io_strlike_ref(fast_io::io_alias,buffer)};
	for(std::size_t i{};i!=16;++i)
		write(oref,ptr,ptr+n);
        return 0;
}