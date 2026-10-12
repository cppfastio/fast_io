#pragma once

/*
Shared helpers for the curve25519 benchmarks: rdtscp + lfence cycle
timing, min-of-batches reduction, hex decoding and byte compare.
Each benchmark binary is self-contained against its own library;
include this header and provide main() in each .cc.
*/

#include <cstdint>
#include <cstddef>

#if defined(__x86_64__) || defined(__i386__) || defined(_M_X64) || defined(_M_IX86)

inline constexpr char const *bench_unit() noexcept
{
	return " cycles";
}

inline ::std::uint64_t bench_start() noexcept
{
	unsigned aux;
	__asm__ volatile("lfence" ::: "memory");
	return __builtin_ia32_rdtscp(&aux);
}

inline ::std::uint64_t bench_end() noexcept
{
	unsigned aux;
	::std::uint64_t t{__builtin_ia32_rdtscp(&aux)};
	__asm__ volatile("lfence" ::: "memory");
	return t;
}

#else

inline constexpr char const *bench_unit() noexcept
{
	return " ns";
}

inline ::std::uint64_t bench_now_ns()
{
	auto const ts{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::monotonic_raw)};
	return static_cast<::std::uint64_t>(ts.tv_sec) * 1000000000u + ts.tv_nsec;
}

inline ::std::uint64_t bench_start()
{
	return bench_now_ns();
}

inline ::std::uint64_t bench_end()
{
	return bench_now_ns();
}

#endif

template <typename F>
inline ::std::uint64_t bench(F &&f, ::std::size_t iters, ::std::size_t batches = 25) noexcept
{
	/* warmup */
	for (::std::size_t i{}; i != 200; ++i)
	{
		f();
	}
	::std::uint64_t best{~0ull};
	for (::std::size_t b{}; b != batches; ++b)
	{
		::std::uint64_t const t0{bench_start()};
		for (::std::size_t i{}; i != iters; ++i)
		{
			f();
		}
		::std::uint64_t const t1{bench_end()};
		::std::uint64_t const per{(t1 - t0) / iters};
		if (per < best)
		{
			best = per;
		}
	}
	return best;
}

inline void hex2bytes(unsigned char *out, char const *hex) noexcept
{
	auto hv = [](char c) { return c <= '9' ? c - '0' : (c | ' ') - 'a' + 10; };
	for (; *hex; hex += 2)
	{
		*out++ = static_cast<unsigned char>(hv(hex[0]) << 4 | hv(hex[1]));
	}
}

inline bool bytes_eq(unsigned char const *a, unsigned char const *b, ::std::size_t n) noexcept
{
	for (::std::size_t i{}; i != n; ++i)
	{
		if (a[i] != b[i])
		{
			return false;
		}
	}
	return true;
}

/* RFC 8032 TEST 1 key pair used by every benchmark binary */
inline constexpr char const *rfc8032_sk() noexcept { return "9d61b19deffd5a60ba844af492ec2cc44449c5697b326919703bac031cae7f60"; }
inline constexpr char const *rfc8032_pk() noexcept { return "d75a980182b10ab7d54bfed3c964073a0ee172f3daa62325af021a68f707511a"; }
/* RFC 7748 alice key pair */
inline constexpr char const *rfc7748_alice_sk() noexcept { return "77076d0a7318a57d3c16c17251b26645df4c2f87ebc0992ab177fba51db92c2a"; }
inline constexpr char const *rfc7748_alice_pk() noexcept { return "8520f0098930a754748b7ddcb43ef75a0dbf3a0d26381af4eba4a98eaa9b4e6a"; }

constexpr ::std::size_t bench_iters{300};
