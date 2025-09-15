#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_dsal/string.h>
#include <fmt/core.h>
#include <fmt/compile.h>

#define ROUNDS 5

#if __has_include(<format>)
#include <format>
#define ENABLE_STD_FORMAT_BENCH 1
#endif

#if __has_include(<fmt/core.h>)
#include <fmt/core.h>
#if __has_include(<fmt/compile.h>)
#include <fmt/compile.h>
#define ENABLE_FMT_BENCH 1
#endif
#endif

using namespace fast_io::io;
using namespace fast_io::mnp;

struct benchmark_result
{
	std::size_t total_size{};
	fast_io::unix_timestamp elapsed{};
};

template <typename Func>
inline benchmark_result run_bench(Func f, std::uint32_t iterations, std::uint32_t rounds = ROUNDS)
{
	// More thorough warmup
	for (std::uint32_t w = 0; w < 1000; ++w)
	{
		auto warmup = f(w);
		(void)warmup;
	}

	std::vector<fast_io::unix_timestamp> times;
	times.reserve(rounds);
	std::size_t total_size{};

	for (std::uint32_t round = 0; round < rounds; ++round)
	{
		auto start = fast_io::posix_clock_gettime(fast_io::posix_clock_id::monotonic_raw);
		for (std::uint32_t i{}; i != iterations; ++i)
		{
			auto tmp = f(i);
			if (round == 0) // Only count size once
			{
				total_size += tmp.size();
			}
		}
		auto end = fast_io::posix_clock_gettime(fast_io::posix_clock_id::monotonic_raw);
		times.push_back(end - start);
	}

	// Return the best (fastest) time
	auto best_time = *std::min_element(times.begin(), times.end());
	return {total_size, best_time};
}


inline ::fast_io::string make_record_fastio(std::uint32_t i)
{
	std::uint32_t id = i * 2654435761u + 0x9e3779b9u;
	std::uint64_t val = 0xDEADBEEFCAFEBABEull ^ (std::uint64_t)id * 1315423911ull;
	std::uint32_t score = (id % 10007) + 3141;
	std::uint32_t rate = score / ((id % 97) + 1);
	::fast_io::string name{"fastio"};

	// return fast_io::concat_fast_io(
	// 	"ID=", width(scalar_placement::right, hex0xupper(id), 10, '0'),
	// 	" VAL=", width(scalar_placement::right, hex0xupper(val), 18, '0'),
	// 	" SCORE=", width(scalar_placement::right, strvw(score_s), 12),
	// 	" RATE=", width(scalar_placement::right, strvw(rate_s), 10),
	// 	" NAME=", left(strvw(name), 16, '.'));

	return fast_io::concat_fast_io(
		"ID=", width(scalar_placement::right, hex0xupper(id), 10, '0'),
		" VAL=", width(scalar_placement::right, hex0xupper(val), 18, '0'),
		" SCORE=", width(scalar_placement::right, score, 12),
		" RATE=", width(scalar_placement::right, rate, 10),
		" NAME=", left(name, 16, '.'));
}

#if defined(ENABLE_STD_FORMAT_BENCH)
inline std::string make_record_stdformat(std::uint32_t i)
{
	std::uint32_t id = i * 2654435761u + 0x9e3779b9u;
	std::uint64_t val = 0xDEADBEEFCAFEBABEull ^ static_cast<std::uint64_t>(id) * 1315423911ull;
	std::uint32_t score = (id % 10007) + 3141;
	std::uint32_t rate = score / ((id % 97) + 1);
	constexpr auto name = "fastio";
	return std::format("ID={:#010X} VAL={:#018X} SCORE={:>12} RATE={:>10} NAME={:.<16}",
					   id, val, score, rate, name);
}
#endif

#if __has_include(<fmt/core.h>) && defined(ENABLE_FMT_BENCH)
inline std::string make_record_fmt(std::uint32_t i)
{
	std::uint32_t id = i * 2654435761u + 0x9e3779b9u;
	std::uint64_t val = 0xDEADBEEFCAFEBABEull ^ static_cast<std::uint64_t>(id) * 1315423911ull;
	std::uint32_t score = (id % 10007) + 3141;
	std::uint32_t rate = score / ((id % 97) + 1);
	constexpr auto name = "fastio";

#if __has_include(<fmt/compile.h>)
	return fmt::format(FMT_COMPILE("ID={:#010X} VAL={:#018X} SCORE={:>12} RATE={:>10} NAME={:.<16}"),
					   id, val, score, rate, name);
#else
	return fmt::format("ID={:#010X} VAL={:#018X} SCORE={:>12} RATE={:>10} NAME={:.<16}",
					   id, val, score, rate, name);
#endif
}
#endif

// iostream 
inline std::string make_record_iostream(std::uint32_t i)
{
	std::uint32_t id = i * 2654435761u + 0x9e3779b9u;
	std::uint64_t val = 0xDEADBEEFCAFEBABEull ^ static_cast<std::uint64_t>(id) * 1315423911ull;
	std::uint32_t score = (id % 10007) + 3141;
	std::uint32_t rate = score / ((id % 97) + 1);
	constexpr char const *name = "fastio";

	std::ostringstream oss;
	oss.setf(std::ios::uppercase);
	oss << "ID=" << std::showbase << std::internal << std::setfill('0') << std::setw(10) << std::hex << id;
	oss << std::dec << std::setfill(' ') << std::nouppercase; // reset
	oss.setf(std::ios::uppercase);
	oss << " VAL=" << std::showbase << std::internal << std::setfill('0') << std::setw(18) << std::hex << val;
	oss << std::dec << std::setfill(' ');
	oss << " SCORE=" << std::setw(12) << std::right << score;
	oss << " RATE=" << std::setw(10) << std::right << rate;
	oss << " NAME=" << std::left << std::setw(16) << std::setfill('.') << name;
	return oss.str();
}

// -------- write benchmark (buffered/no buffered) to /dev/null, avoid disk interference --------
inline benchmark_result run_write_bench_fastio(std::uint32_t iterations, bool buffered_128k)
{
	std::size_t total_size{};
	auto start = fast_io::posix_clock_gettime(fast_io::posix_clock_id::monotonic_raw);
	if (buffered_128k)
	{
		fast_io::native_file nf("/dev/null", fast_io::open_mode::out | fast_io::open_mode::trunc);
		std::string buf;
		buf.reserve(128 * 1024);
		for (std::uint32_t i{}; i != iterations; ++i)
		{
			auto rec = make_record_fastio(i);
			total_size += rec.size();
			if (buf.size() + rec.size() + 1 > 128 * 1024)
			{
				if (!buf.empty())
				{
					::fast_io::operations::write_all(nf, buf.data(), buf.data() + buf.size());
					buf.clear();
				}
				if (rec.size() + 1 > 128 * 1024)
				{
					::fast_io::operations::write_all(nf, rec.data(), rec.data() + rec.size());
					char nl = '\n';
					::fast_io::operations::write_all(nf, &nl, &nl + 1);
					continue;
				}
			}
			buf.append(rec.data(), rec.size());
			buf.push_back('\n');
		}
		if (!buf.empty())
		{
			::fast_io::operations::write_all(nf, buf.data(), buf.data() + buf.size());
		}
	}
	else
	{
		fast_io::native_file nf("/dev/null", fast_io::open_mode::out | fast_io::open_mode::trunc);
		for (std::uint32_t i{}; i != iterations; ++i)
		{
			auto rec = make_record_fastio(i);
			total_size += rec.size();
			::fast_io::operations::write_all(nf, rec.data(), rec.data() + rec.size());
			char nl = '\n';
			::fast_io::operations::write_all(nf, &nl, &nl + 1);
		}
	}
	auto end = fast_io::posix_clock_gettime(fast_io::posix_clock_id::monotonic_raw);
	return {total_size, end - start};
}

inline benchmark_result run_write_bench_fmt(std::uint32_t iterations, bool buffered_128k)
{
	std::size_t total_size{};
	auto start = fast_io::posix_clock_gettime(fast_io::posix_clock_id::monotonic_raw);
	if (buffered_128k)
	{
		fast_io::native_file nf("/dev/null", fast_io::open_mode::out | fast_io::open_mode::trunc);
		std::string buf;
		buf.reserve(128 * 1024);
		for (std::uint32_t i{}; i != iterations; ++i)
		{
			auto rec = make_record_fmt(i);
			total_size += rec.size();
			if (buf.size() + rec.size() + 1 > 128 * 1024)
			{
				if (!buf.empty())
				{
					::fast_io::operations::write_all(nf, buf.data(), buf.data() + buf.size());
					buf.clear();
				}
				if (rec.size() + 1 > 128 * 1024)
				{
					::fast_io::operations::write_all(nf, rec.data(), rec.data() + rec.size());
					char nl = '\n';
					::fast_io::operations::write_all(nf, &nl, &nl + 1);
					continue;
				}
			}
			buf.append(rec.data(), rec.size());
			buf.push_back('\n');
		}
		if (!buf.empty())
		{
			::fast_io::operations::write_all(nf, buf.data(), buf.data() + buf.size());
		}
	}
	else
	{
		fast_io::native_file nf("/dev/null", fast_io::open_mode::out | fast_io::open_mode::trunc);
		for (std::uint32_t i{}; i != iterations; ++i)
		{
			auto rec = make_record_fmt(i);
			total_size += rec.size();
			print(nf, rec, '\n');
		}
	}
	auto end = fast_io::posix_clock_gettime(fast_io::posix_clock_id::monotonic_raw);
	return {total_size, end - start};
}

inline benchmark_result run_write_bench_iostream(std::uint32_t iterations, bool buffered_128k)
{
	std::size_t total_size{};
	auto start = fast_io::posix_clock_gettime(fast_io::posix_clock_id::monotonic_raw);
	{
		std::ofstream out("/dev/null", std::ios::binary | std::ios::trunc);
		std::vector<char> bigbuf;
		if (buffered_128k)
		{
			bigbuf.resize(128 * 1024);
			out.rdbuf()->pubsetbuf(bigbuf.data(), static_cast<std::streamsize>(bigbuf.size()));
		}
		else
		{
			out.rdbuf()->pubsetbuf(nullptr, 0);
		}
		for (std::uint32_t i{}; i != iterations; ++i)
		{
			auto rec = make_record_iostream(i);
			total_size += rec.size();
			out.write(rec.data(), static_cast<std::streamsize>(rec.size()));
			out.put('\n');
		}
		out.flush();
	}
	auto end = fast_io::posix_clock_gettime(fast_io::posix_clock_id::monotonic_raw);
	return {total_size, end - start};
}

int main(int argc, char **argv)
{
	std::uint32_t iterations = 10000000;
	std::uint32_t rounds = ROUNDS;
	if (argc >= 2)
	{
		try
		{
			iterations = ::fast_io::to<std::uint32_t>(::fast_io::mnp::os_c_str(argv[1]));
		}
		catch (...)
		{
			// ignore invalid input, keep default
		}
	}
	if (argc >= 3)
	{
		try
		{
			rounds = ::fast_io::to<std::uint32_t>(::fast_io::mnp::os_c_str(argv[2]));
		}
		catch (...)
		{
			// ignore invalid input, keep default ROUNDS
		}
	}

	auto sample_fastio = make_record_fastio(1);
#if defined(ENABLE_STD_FORMAT_BENCH)
	auto sample_stdformat = make_record_stdformat(1);
#endif
#if __has_include(<fmt/core.h>) && defined(ENABLE_FMT_BENCH)
	auto sample_fmt = make_record_fmt(1);
#endif

	using namespace fast_io::io;
	print("Sample fast_io output:    ", sample_fastio, "\n");
#if defined(ENABLE_STD_FORMAT_BENCH)
	print("Sample std::format output:", sample_stdformat, "\n");
#endif
#if __has_include(<fmt/core.h>) && defined(ENABLE_FMT_BENCH)
	print("Sample fmt output:        ", sample_fmt, "\n");
#endif
	print("\n");

	print("Running benchmarks with ", iterations, " iterations, ", rounds, " rounds each (showing best time)...\n\n");

	auto fastio_res = run_bench(make_record_fastio, iterations, rounds);
	print("fast_io completed\n");

#if defined(ENABLE_STD_FORMAT_BENCH)
	auto stdformat_res = run_bench(make_record_stdformat, iterations, rounds);
	print("std::format completed\n");
#endif

#if __has_include(<fmt/core.h>) && defined(ENABLE_FMT_BENCH)
	auto fmt_res = run_bench(make_record_fmt, iterations, rounds);
	print("fmt completed\n");
#endif

	print("\n[format benchmark results]\n");
	print("Iterations: ", iterations, ", Rounds: ", rounds, "\n");
	print("fast_io     (total size: ", fastio_res.total_size, ") took ", fastio_res.elapsed, "s\n");

#if defined(ENABLE_STD_FORMAT_BENCH)
	print("std::format (total size: ", stdformat_res.total_size, ") took ", stdformat_res.elapsed, "s");
	// Calculate speedup
	constexpr double subseconds_to_seconds = 1.0 / static_cast<double>(fast_io::uint_least64_subseconds_per_second);
	double stdformat_seconds = static_cast<double>(stdformat_res.elapsed.seconds) +
							   static_cast<double>(stdformat_res.elapsed.subseconds) * subseconds_to_seconds;
	double fastio_seconds = static_cast<double>(fastio_res.elapsed.seconds) +
							static_cast<double>(fastio_res.elapsed.subseconds) * subseconds_to_seconds;
	if (fastio_seconds > 0)
	{
		double speedup = stdformat_seconds / fastio_seconds;
		std::string speedup_str = std::format("{:.2f}", speedup);
		print(" (fast_io is ", speedup_str, "x faster)\n");
	}
	else
	{
		print("\n");
	}
#endif

#if __has_include(<fmt/core.h>) && defined(ENABLE_FMT_BENCH)
	print("fmt         (total size: ", fmt_res.total_size, ") took ", fmt_res.elapsed, "s");
	constexpr double subseconds_to_seconds2 = 1.0 / static_cast<double>(fast_io::uint_least64_subseconds_per_second);
	double fmt_seconds = static_cast<double>(fmt_res.elapsed.seconds) +
						 static_cast<double>(fmt_res.elapsed.subseconds) * subseconds_to_seconds2;
	double fastio_seconds2 = static_cast<double>(fastio_res.elapsed.seconds) +
							 static_cast<double>(fastio_res.elapsed.subseconds) * subseconds_to_seconds2;
	if (fastio_seconds2 > 0)
	{
		double fmt_speedup = fmt_seconds / fastio_seconds2;
		std::string fmt_speedup_str = std::format("{:.2f}", fmt_speedup);
		print(" (fast_io is ", fmt_speedup_str, "x faster)\n");
	}
	else
	{
		print("\n");
	}
#endif

	print("\n\n[write benchmark to /dev/null]\n");
	// fast_io write: 128KB buffered vs direct system call
	{
		auto fio_buf = run_write_bench_fastio(iterations, true);
		auto fio_nobuf = run_write_bench_fastio(iterations, false);
		print("fast_io obuf(128K)  (size: ", fio_buf.total_size, ") took ", fio_buf.elapsed, "s\n");
		print("fast_io native(no)  (size: ", fio_nobuf.total_size, ") took ", fio_nobuf.elapsed, "s\n");
	}
	// iostream write: 128KB buffered vs no buffered
	// {
	// 	auto iostream_buf = run_write_bench_iostream(iterations, true);
	// 	auto iostream_nobuf = run_write_bench_iostream(iterations, false);
	// 	print("iostream 128K buf    (size: ", iostream_buf.total_size, ") took ", iostream_buf.elapsed, "s\n");
	// 	print("iostream no buf      (size: ", iostream_nobuf.total_size, ") took ", iostream_nobuf.elapsed, "s\n");
	// }
#if __has_include(<fmt/core.h>) && defined(ENABLE_FMT_BENCH)
	// fmt write: 128KB buffered vs direct system call (format with FMT_COMPILE)
	{
		auto fmt_buf = run_write_bench_fmt(iterations, true);
		auto fmt_nobuf = run_write_bench_fmt(iterations, false);
		print("fmt(FMT_COMPILE)+buf (size: ", fmt_buf.total_size, ") took ", fmt_buf.elapsed, "s\n");
		print("fmt(FMT_COMPILE)+no  (size: ", fmt_nobuf.total_size, ") took ", fmt_nobuf.elapsed, "s\n");
	}
#endif
}
