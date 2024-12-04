#include <fast_io.h>

fast_io::c_file_unlocked obf("/dev/null", fast_io::open_mode::out);

extern "C" int LLVMFuzzerTestOneInput(std::uint8_t const *pptr, std::size_t n) noexcept
{
	::std::byte const* ptr{reinterpret_cast<::std::byte const*>(pptr)};
	::fast_io::operations::write_all_bytes(obf, ptr, ptr + n);
	return 0;
}
