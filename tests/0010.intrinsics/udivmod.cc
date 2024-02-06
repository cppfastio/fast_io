#include <fast_io.h>
#include <fast_io_core_impl/intrinsics/impl.h>

int main()
{
	using namespace fast_io::mnp;

	constexpr __uint128_t a{static_cast<__uint128_t>(-1)}, b{(static_cast<__uint128_t>(500) << 64u) | INT64_MAX};
	auto [qlow, qhigh, rlow, rhigh] = ::fast_io::intrinsics::udivmod<::std::uint_least64_t>(
		static_cast<::std::uint_least64_t>(a),
		static_cast<::std::uint_least64_t>(a >> 64u),
		static_cast<::std::uint_least64_t>(b),
		static_cast<::std::uint_least64_t>(b >> 64u));

	::fast_io::io::println(
#if 0
		"qlow=",addrvw(qlow),"\n"
		"qhigh=",addrvw(qhigh),"\n"
		"rlow=",addrvw(rlow),"\n"
		"rhigh=",addrvw(rhigh),"\n"
#endif
		"divnext=", static_cast<__uint128_t>((static_cast<__uint128_t>(qhigh) << 64u) | qlow), "\n"
																							   "modnext=",
		static_cast<__uint128_t>((static_cast<__uint128_t>(rhigh) << 64u) | rlow), "\n"
																				   "div=",
		a / b,
		"\tmod=",
		a % b);
}
