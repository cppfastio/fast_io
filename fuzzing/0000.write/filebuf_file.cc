#include <fast_io.h>
#include <fast_io_legacy.h>

fast_io::filebuf_file obf(
#if defined(_WIN32) && !defined(__WINE__)
	"nul"
#else
	"/dev/null"
#endif
	,
	fast_io::open_mode::out);


extern "C" int LLVMFuzzerTestOneInput(std::uint8_t const *pptr, std::size_t n) noexcept
{
	char const *ptr{reinterpret_cast<char const *>(pptr)};
	using namespace fast_io::mnp;
	::fast_io::operations::write_all(obf, ptr, ptr + n);

	return 0;
}
