#include <string>
#include <limits>
#include <source_location>
#include <fast_io.h>

using namespace fast_io::io;

template<typename T>
inline void test(T u1)
{
	T u2;
	using namespace fast_io::mnp;
	auto buffer{fast_io::concat(iec559_le_put(u1))};
	scan(fast_io::ibuffer_view{ buffer }, iec559_le_get(u2));
	println(std::source_location::current(), "\tu1 == u2: ", boolalpha(u1 == u2));
}

int main()
{
	std::string buffer(10000, '\0');

#if __STDCPP_BFLOAT_T__
	test(124.4264bf);
#endif

#if __STDCPP_FLOAT16_T__
	test(124.4264f16);
#endif

#if __STDCPP_FLOAT32_T__
	test(124.4264325f32);
#endif

#if __STDCPP_FLOAT64_T__
	test(124.4264325f64);
#endif

#if __STDCPP_FLOAT128_T__
	test(142112424.4264325f128);
#endif

	test(1245.35f);

	test(12421421.35);
}

