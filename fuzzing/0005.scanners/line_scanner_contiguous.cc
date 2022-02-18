#include<string_view>
#include<fast_io.h>
#include<fast_io_device.h>

thread_local fast_io::obuf_file obf("/dev/null");

extern "C" int LLVMFuzzerTestOneInput(std::uint8_t const* ptr,std::size_t n) noexcept
{
	fast_io::ibuffer_view view(reinterpret_cast<char const*>(ptr),reinterpret_cast<char const*>(ptr)+n);
	for(std::string_view line:line_scanner(view))
	{
		write(obf,ptr,ptr+n);
	}
	return 0;
}