#include<string_view>
#include<fast_io.h>
#include<fast_io_device.h>

thread_local std::size_t context{};

extern "C" int LLVMFuzzerTestOneInput(std::uint8_t const* ptr,std::size_t n) noexcept
{
	::fast_io::basic_http_header_buffer<char> buffer;
	auto ret{::fast_io::details::http_header_scan_context_define_impl(context,reinterpret_cast<char const*>(ptr),reinterpret_cast<char const*>(ptr)+n,buffer)};
        return 0;
}