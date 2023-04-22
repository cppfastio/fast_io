#define FAST_IO_SANITIZE_IO_BUFFER
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
	auto buffer{ fast_io::concat(test_struct) };
	auto buffer_size{ buffer.size() };
	auto split_index{ ptr[size_of_struct + 1] };
	split_index = split_index > buffer_size ? buffer_size : split_index;
	std::string_view sv1{ buffer.c_str(), split_index };
	std::string_view sv2{ buffer.c_str() + split_index, buffer_size - split_index };
try
{
	println(obf, fast_io::to<decltype(test_struct)>(sv1, sv2));
}
catch(...)
{
}
	return 0;
}

