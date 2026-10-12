#include <cstring>
#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_dsal/array.h>
#include <fast_io_dsal/string_view.h>
#include <fast_io_dsal/vector.h>

namespace
{

inline void append_expected(::fast_io::vector<::std::byte> &expected, void const *base, ::std::size_t len) throws
{
	auto first{static_cast<::std::byte const *>(base)};
	for (::std::size_t i{}; i != len; ++i)
	{
		expected.push_back(first[i]);
	}
}

inline void append_expected_at(::fast_io::vector<::std::byte> &expected, void const *base, ::std::size_t len,
							   ::std::size_t pos) throws
{
	if (expected.size() < pos + len)
	{
		expected.resize(pos + len);
	}
	auto first{static_cast<::std::byte const *>(base)};
	for (::std::size_t i{}; i != len; ++i)
	{
		expected[pos + i] = first[i];
	}
}

template <typename filetype, typename T>
inline void run(T const &filename) throws
{
	constexpr ::fast_io::string_view s1{"abc"};
	constexpr ::fast_io::string_view s2{"defg"};
	constexpr ::fast_io::string_view s3{"hi"};

	::fast_io::vector<::std::byte> big(5000);
	for (::std::size_t i{}; i != 5000; ++i)
	{
		big[i] = static_cast<::std::byte>(i & 0x7F);
	}

	::fast_io::vector<::std::byte> hundred(100);
	for (::std::size_t i{}; i != 100; ++i)
	{
		hundred[i] = static_cast<::std::byte>((i ^ 0x5A) & 0x7F);
	}

	::fast_io::vector<::std::byte> expected;
	{
		filetype file(filename, ::fast_io::open_mode::out);

		// n == 0: no-op
		::fast_io::operations::scatter_write_all_bytes(file, nullptr, 0);

		// n == 1: single scatter, expect status {1, 0}
		::fast_io::array one{::fast_io::io_scatter_t{s1.data(), s1.size()}};
		auto st1{::fast_io::operations::scatter_write_some_bytes(file, one.data(), one.size())};
		if (st1.position != 1 || st1.position_in_scatter != 0)
		{
			::fast_io::fast_terminate();
		}
		append_expected(expected, s1.data(), s1.size());

		// small scatters: coalesced through the 4KiB buffer
		::fast_io::array smalls{::fast_io::io_scatter_t{s2.data(), s2.size()},
								::fast_io::io_scatter_t{s3.data(), s3.size()}};
		::fast_io::operations::scatter_write_all_bytes(file, smalls.data(), smalls.size());
		append_expected(expected, s2.data(), s2.size());
		append_expected(expected, s3.data(), s3.size());

		// mix: small -> buffered, big(5000) -> direct write, small -> buffered
		::fast_io::array mix{::fast_io::io_scatter_t{s3.data(), s3.size()},
							 ::fast_io::io_scatter_t{big.data(), big.size()},
							 ::fast_io::io_scatter_t{s2.data(), s2.size()}};
		::fast_io::operations::scatter_write_all_bytes(file, mix.data(), mix.size());
		append_expected(expected, s3.data(), s3.size());
		append_expected(expected, big.data(), big.size());
		append_expected(expected, s2.data(), s2.size());

		// 100 x 100 bytes: forces multiple 4KiB buffer flushes
		::fast_io::vector<::fast_io::io_scatter_t> many;
		for (::std::size_t i{}; i != 100; ++i)
		{
			many.push_back({hundred.data(), hundred.size()});
			append_expected(expected, hundred.data(), hundred.size());
		}
		::fast_io::operations::scatter_write_all_bytes(file, many.data(), many.size());

		// pwrite at offset 7: "abc" + "defg" lands at [7, 14)
		::fast_io::array pw{::fast_io::io_scatter_t{s1.data(), s1.size()},
							::fast_io::io_scatter_t{s2.data(), s2.size()}};
		::fast_io::operations::scatter_pwrite_all_bytes(file, pw.data(), pw.size(), 7);
		append_expected_at(expected, s1.data(), s1.size(), 7);
		append_expected_at(expected, s2.data(), s2.size(), 7 + s1.size());

		// pwrite some: expect status {2, 0}, and sequential chunks advance offset
		::fast_io::array pw2{::fast_io::io_scatter_t{s2.data(), s2.size()},
							 ::fast_io::io_scatter_t{big.data(), big.size()},
							 ::fast_io::io_scatter_t{s3.data(), s3.size()}};
		auto st2{::fast_io::operations::scatter_pwrite_some_bytes(file, pw2.data(), pw2.size(), 20000)};
		if (st2.position != 3 || st2.position_in_scatter != 0)
		{
			::fast_io::fast_terminate();
		}
		append_expected_at(expected, s2.data(), s2.size(), 20000);
		append_expected_at(expected, big.data(), big.size(), 20000 + s2.size());
		append_expected_at(expected, s3.data(), s3.size(), 20000 + s2.size() + big.size());
	}
	{
		filetype in(filename, ::fast_io::open_mode::in);
		::fast_io::vector<::std::byte> got(expected.size());
		::fast_io::operations::read_all_bytes(in, got.data(), got.size());
		if (got.size() != expected.size() || ::std::memcmp(got.data(), expected.data(), expected.size()) != 0)
		{
			::fast_io::fast_terminate();
		}
	}
}

} // namespace

int main()
try
{
	run<::fast_io::nt_file>(u8"nt_scatter_write.txt");
	run<::fast_io::zw_file>(u8"zw_scatter_write.txt");
}
catch throws(::std::error e)
{
	::fast_io::perrln(e);
	return 1;
}
