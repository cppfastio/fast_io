#include <string>
#include <limits>
#include <source_location>
#include <fast_io.h>

/*
https://github.com/sunfishcode/wasm-reference-manual/blob/master/WebAssembly.md#primitive-encoding-types
*/

using namespace fast_io::io;

template<typename T>
inline void test(T u1)
{
	T u2;
	using namespace fast_io::mnp;
	auto buffer{fast_io::concat(wasm_float_put(u1))};
	scan(fast_io::ibuffer_view{ buffer }, wasm_float_get(u2));
	println(std::source_location::current(), "\tu1 == u2: ", boolalpha(u1 == u2));
}

template<typename T>
inline void testvarint(T u1)
{
	T u2;
	using namespace fast_io::mnp;
	auto buffer{fast_io::concat(wasm_varint_put(u1))};
	scan(fast_io::ibuffer_view{ buffer }, wasm_varint_get(u2));
	println(std::source_location::current(),"\tu1=",u1,"\tu2=",u2, "\tu1 == u2: ", boolalpha(u1 == u2));
}

template<typename T>
inline void testuint32(T u1)
{
	T u2;
	using namespace fast_io::mnp;
	auto buffer{fast_io::concat(wasm_uint32_put(u1))};
	scan(fast_io::ibuffer_view{ buffer }, wasm_uint32_get(u2));
	println(std::source_location::current(),"\tu1=",u1,"\tu2=",u2, "\tu1 == u2: ", boolalpha(u1 == u2));
}

int main()
{
#if __STDCPP_BFLOAT16_T__
	test(124.4264bf16);
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

	testvarint(12421);
	testvarint(12421235);
	testvarint(UINT64_C(124212351253253253));
	testvarint(UINT32_C(5235252));
	testvarint(INT64_C(-50));
	testuint32(UINT32_C(3253));
}


