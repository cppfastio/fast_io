#include <fast_io.h>

fast_io::wc_file_unlocked obf(
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

struct usefwide
{
	usefwide()
	{
		fwide(obf.fp, 1);
	}
};
#ifdef USE_FWIDE
usefwide t;
#endif

extern "C" int LLVMFuzzerTestOneInput(std::uint8_t const *pptr, std::size_t n) noexcept
{
	wchar_t const *ptr{reinterpret_cast<wchar_t const *>(pptr)};
	using namespace fast_io::mnp;
	::fast_io::operations::write_all(obf, ptr, ptr + n / sizeof(wchar_t));

	return 0;
}
