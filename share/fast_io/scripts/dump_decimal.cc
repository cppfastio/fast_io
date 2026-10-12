// Dump tool for verify_decimal.py: prints the raw (mantissa, exponent-field)
// inputs and the (m10, e10) results of each floating conversion entry point
// declared in fast_io_unit/floating/roundtrip.h.  Build with the floating
// module object linked:
//
//   clang++ -std=c++2c -O3 -c fast_io_floating.cppm -o fio_floating.o -I<include>
//   clang++ -O3 -o dump_decimal dump_decimal.cc fio_floating.o -I<include>
//   ./dump_decimal f16  > f16.out   && python3 verify_decimal.py f16 f16.out
//   ./dump_decimal bf16 > bf16.out  && python3 verify_decimal.py bf16 bf16.out
//   ./dump_decimal f80  > f80.out   && python3 verify_decimal.py f80 f80.out
//   ./dump_decimal f128 > f128.out  && python3 verify_decimal.py f128 f128.out

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <random>
#include <fast_io_freestanding.h>

int main(int argc, char **argv)
{
	using namespace ::fast_io::details;
	if (argc < 2)
	{
		return 1;
	}
	auto const mode{argv[1]};
	if (!::std::strcmp(mode, "f16") || !::std::strcmp(mode, "bf16"))
	{
		bool const is16{!::std::strcmp(mode, "f16")};
		for (::std::uint_least32_t bits{}; bits < 65536u; ++bits)
		{
			::std::uint_least32_t e2, m2;
			if (is16)
			{
				e2 = (bits >> 10u) & 0x1fu;
				m2 = bits & 0x3ffu;
				if (e2 == 0x1fu)
				{
					continue;
				}
			}
			else
			{
				e2 = (bits >> 7u) & 0xffu;
				m2 = bits & 0x7fu;
				if (e2 == 0xffu)
				{
					continue;
				}
			}
			auto const r{is16 ? to_decimal_binary16(m2, e2) : to_decimal_bfloat16(m2, e2)};
			::std::printf("%x %u %d\n", bits, r.m10, r.e10);
		}
	}
	else if (!::std::strcmp(mode, "f32") || !::std::strcmp(mode, "f64"))
	{
		::std::mt19937_64 rng(7);
		bool const is32{!::std::strcmp(mode, "f32")};
		for (long i{}; i != 20000000; ++i)
		{
			auto const bits{rng()};
			if (is32)
			{
				auto const b32{static_cast<::std::uint_least32_t>(bits)};
				auto const e2{(b32 >> 23u) & 0xffu};
				auto const m2{b32 & 0x7fffffu};
				if (e2 == 0xffu)
				{
					continue;
				}
				auto const r{to_decimal_binary32(m2, e2)};
				::std::printf("%x %u %d\n", b32, r.m10, r.e10);
			}
			else
			{
				auto const e2{static_cast<::std::uint_least32_t>((bits >> 52u) & 0x7ffu)};
				auto const m2{bits & ((::std::uint_least64_t{1} << 52u) - 1u)};
				if (e2 == 0x7ffu)
				{
					continue;
				}
				auto const r{to_decimal_binary64(m2, e2)};
				::std::printf("%llx %llu %d\n", static_cast<unsigned long long>(bits),
							  static_cast<unsigned long long>(r.m10), r.e10);
			}
		}
	}
	else if (!::std::strcmp(mode, "f80"))
	{
		::std::mt19937_64 rng(3);
		for (long i{}; i != 100000; ++i)
		{
			auto const m2{rng() & ((::std::uint_least64_t{1} << 63u) - 1u)};
			auto const e2{static_cast<::std::uint_least32_t>(rng() & 0x7fffu)};
			if (e2 == 0x7fffu)
			{
				continue;
			}
			auto const r{to_decimal_binary80(m2, e2)};
			::std::printf("%llx:%x %llx:%llx %d\n", static_cast<unsigned long long>(m2), e2,
						  static_cast<unsigned long long>(static_cast<::std::uint_least64_t>(r.m10 >> 64u)),
						  static_cast<unsigned long long>(static_cast<::std::uint_least64_t>(r.m10)), r.e10);
		}
	}
	else if (!::std::strcmp(mode, "f128"))
	{
		::std::mt19937_64 rng(3);
		for (long i{}; i != 100000; ++i)
		{
			auto m2{(static_cast<__uint128_t>(rng()) << 64u) | rng()};
			m2 &= (static_cast<__uint128_t>(1) << 112u) - 1;
			auto const e2{static_cast<::std::uint_least32_t>(rng() & 0x7fffu)};
			if (e2 == 0x7fffu)
			{
				continue;
			}
			auto const r{to_decimal_binary128(m2, e2)};
			::std::printf("%llx:%llx:%x %llx:%llx %d\n",
						  static_cast<unsigned long long>(static_cast<::std::uint_least64_t>(m2 >> 64u)),
						  static_cast<unsigned long long>(static_cast<::std::uint_least64_t>(m2)), e2,
						  static_cast<unsigned long long>(static_cast<::std::uint_least64_t>(r.m10 >> 64u)),
						  static_cast<unsigned long long>(static_cast<::std::uint_least64_t>(r.m10)), r.e10);
		}
	}
	return 0;
}
