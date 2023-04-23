#include <cstring>
#include <string>
#include <fast_io.h>
#include <fast_io_device.h>

thread_local fast_io::obuf_file obf("/dev/null");

using namespace fast_io::io;

extern "C" int LLVMFuzzerTestOneInput(std::uint8_t const* ptr, std::size_t n) noexcept
{
	fast_io::
#if defined(TEST_INADDR)
		posix_in_addr
#elif defined(IPV4)
		ipv4
#elif defined(TEST_IN6ADDR)
		posix_in6_addr
#elif defined(IPV6)
		ipv6
#else
		posix_in6_addr
#endif
		test_struct;
	constexpr auto size_of_struct{ sizeof(test_struct) };
	if (n < size_of_struct + 10)
		return 0;
	std::memcpy(&test_struct, ptr, size_of_struct);
	print(obf,test_struct);
	return 0;
}
