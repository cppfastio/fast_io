#include <fast_io.h>

fast_io::u32c_file_unlocked obf(
#if defined(_WIN32) && !defined(__WINE__)
	"nul"
#else
	"/dev/null"
#endif
	,
	fast_io::open_mode::out);

extern "C"
	[[__gnu__::__weak__]]
	uintptr_t __sancov_lowest_stack{};

extern "C" int LLVMFuzzerTestOneInput(std::uint8_t const *pptr, std::size_t n) noexcept
{
	using chartype = char32_t;
	char32_t const *ptr{reinterpret_cast<char32_t const *>(pptr)};
	using namespace fast_io::mnp;
	::fast_io::operations::write_all(obf, ptr, ptr + n / sizeof(char32_t));

	return 0;
}
