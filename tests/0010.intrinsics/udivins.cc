
#include<fast_io.h>
#include<fast_io_core_impl/intrinsics/impl.h>

auto udivmod64(
		::std::uint_least64_t dividendlow,
		::std::uint_least64_t dividendhigh,
		::std::uint_least64_t divisorlow,
		::std::uint_least64_t divisorhigh) noexcept
{
	return ::fast_io::intrinsics::udivmod(dividendlow,dividendhigh,divisorlow,divisorhigh);
}