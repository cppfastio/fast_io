#include <fast_io.h>
#include <fast_io_device.h>
#include <random>
#include <vector>
#include <limits>
#include <cstring>
#include <fast_io_driver/timer.h>
#include <fast_io_unit/floating/roundtrip.h>
#include <fast_io_unit/floating/punning.h>
#include <teju/float.h>
#include <teju/double.h>
#include <dragonbox/dragonbox.h>
#include <dragonbox/dragonbox_to_chars.h>

using namespace fast_io::io;

template <class T>
static std::vector<T> make_random_values(std::size_t n)
{
	std::mt19937_64 eng{123456789u};
	std::uniform_real_distribution<T> dist(std::numeric_limits<T>::denorm_min(), std::numeric_limits<T>::max());
	std::vector<T> v;
	v.reserve(n);
	for (std::size_t i = 0; i < n; ++i)
	{
		v.push_back(dist(eng)); // strictly positive finite by construction
	}
	return v;
}

static void bench_float(std::size_t n)
{
	auto values = make_random_values<float>(n);

	if(!values.empty())
	{
		char fio_buf[128];
		char dbx_buf[128];
		char teju_buf[128];
		auto const x0 = values.front();
		char* fio_p = fast_io::pr_rsv_to_c_array(fio_buf, fast_io::mnp::scientific(x0));
		char* dbx_p = jkj::dragonbox::to_chars(x0, dbx_buf);
		char* teju_p = jkj::dragonbox::to_chars(x0, teju_buf);
		fast_io::println(
			"sample fast_io=", fast_io::mnp::strvw(fio_buf, fio_p),
			" dragonbox=", fast_io::mnp::strvw(dbx_buf, dbx_p),
			" teju=", fast_io::mnp::strvw(teju_buf, teju_p));
	}

	{
		fast_io::timer t(u8"fastio_float");
		std::uint64_t acc{};
		for (auto const x : values)
		{
			// auto const [mantissa, exponent, sign] = fast_io::details::get_punned_result(x);
			// (void)sign;
			// auto const r = fast_io::details::dragonbox_impl<float>(mantissa, static_cast<::std::int_least32_t>(exponent));
			// acc += static_cast<std::uint64_t>(r.m10) + static_cast<std::uint64_t>(r.e10);
			char buf[128];
			auto *p = fast_io::pr_rsv_to_c_array(buf, fast_io::mnp::scientific(x));
			acc += static_cast<std::uint64_t>(p - buf);
		}
		std::uint64_t volatile sink = acc;
		(void)sink;
	}

	{
		using namespace jkj::dragonbox;
		fast_io::timer t(u8"dragonbox_float");
		std::uint64_t acc{};
		for (auto const x : values)
		{
			char buf[128]{0};
			auto *p = to_chars(x, buf);
			acc += static_cast<std::uint64_t>(p - buf);
		}
		std::uint64_t volatile sink = acc;
		(void)sink;
	}

	{
		fast_io::timer t(u8"teju_float");
		std::uint64_t acc{};
		for (auto const x : values)
		{
			char buf[128]{0};
			auto *p = jkj::dragonbox::to_chars(x, buf);
			acc += static_cast<std::uint64_t>(p - buf);
		}
		std::uint64_t volatile sink = acc;
		(void)sink;
	}
}

static void bench_double(std::size_t n)
{
	auto values = make_random_values<double>(n);

	if(!values.empty())
	{
		char fio_buf[128];
		char dbx_buf[128];
		char teju_buf[128];
		auto const x0 = values.front();
		char* fio_p = fast_io::pr_rsv_to_c_array(fio_buf, fast_io::mnp::scientific(x0));
		char* dbx_p = jkj::dragonbox::to_chars(x0, dbx_buf);
		char* teju_p = jkj::dragonbox::to_chars(x0, teju_buf);
		fast_io::println(
			"sample fast_io=", fast_io::mnp::strvw(fio_buf, fio_p),
			" dragonbox=", fast_io::mnp::strvw(dbx_buf, dbx_p),
			" teju=", fast_io::mnp::strvw(teju_buf, teju_p));
	}

	{
		// fast_io core (dragonbox_impl only, no string assembly)
		fast_io::timer t(u8"fastio_double");
		std::uint64_t acc{};
		for (auto const x : values)
		{
			char buf[128]{0};
			auto *p = fast_io::pr_rsv_to_c_array(buf, fast_io::mnp::scientific(x));
			acc += static_cast<std::uint64_t>(p - buf);
		}
		std::uint64_t volatile sink = acc;
		(void)sink;
	}

	{
		using namespace jkj::dragonbox;
		fast_io::timer t(u8"dragonbox_double");
		std::uint64_t acc{};
		for (auto const x : values)
		{
			char buf[128]{0};
			auto *p = to_chars(x, buf);
			acc += static_cast<std::uint64_t>(p - buf);
		}
		std::uint64_t volatile sink = acc;
		(void)sink;
	}

	{
		fast_io::timer t(u8"teju_double");
		std::uint64_t acc{};
		for (auto const x : values)
		{
			char buf[128]{0};
			auto *p = jkj::dragonbox::to_chars(x, buf);
			acc += static_cast<std::uint64_t>(p - buf);
		}
		std::uint64_t volatile sink = acc;
		(void)sink;
	}
}

int main()
{
	constexpr std::size_t N = 1u << 20; // ~1M samples
	bench_float(N);
	print("\n");
	bench_double(N);
}
