#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_dsal/str_swiss_set.h>

thread_local fast_io::u32obuf_file obf(u8"/dev/null");

using namespace fast_io::io;

extern "C" int LLVMFuzzerTestOneInput(std::uint8_t const *ptr, std::size_t n) noexcept
{
	::std::uint_least64_t h{::fast_io::details::xxh3_64bits_fio(ptr, n)};
	println(obf, h);
	return 0;
}
