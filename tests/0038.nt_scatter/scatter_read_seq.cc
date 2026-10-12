#include <cstring>
#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_dsal/array.h>
#include <fast_io_dsal/vector.h>

namespace
{

inline void check_seq(::std::byte const *got, ::std::size_t n, ::std::size_t start)
{
	for (::std::size_t i{}; i != n; ++i)
	{
		if (got[i] != static_cast<::std::byte>((start + i) & 0xFF))
		{
			::fast_io::fast_terminate();
		}
	}
}

inline void check_fill(::std::byte const *got, ::std::size_t n, ::std::byte v)
{
	for (::std::size_t i{}; i != n; ++i)
	{
		if (got[i] != v)
		{
			::fast_io::fast_terminate();
		}
	}
}

template <typename filetype, typename T>
inline void run(T const &filename) throws
{
	constexpr ::std::size_t total{8192};
	::fast_io::vector<::std::byte> data(total);
	for (::std::size_t i{}; i != total; ++i)
	{
		data[i] = static_cast<::std::byte>(i & 0xFF);
	}

	{
		filetype file(filename, ::fast_io::open_mode::out);
		::fast_io::operations::write_all_bytes(file, data.data(), data.size());
	}

	// pieces straddle the direct-read threshold: tiny/buffered, 3071 edge, 4000 direct, 1114 tail
	constexpr ::std::size_t pieces[]{7, 1000, 3071, 4000, 1114}; // sums to 8192
	constexpr ::std::size_t npieces{sizeof(pieces) / sizeof(*pieces)};

	{
		filetype in(filename, ::fast_io::open_mode::in);
		::fast_io::vector<::std::byte> got(total, ::std::byte{0xAA});
		::fast_io::array<::fast_io::io_scatter_t, npieces> scatters;
		::std::size_t off{};
		for (::std::size_t i{}; i != npieces; ++i)
		{
			scatters[i] = {got.data() + off, pieces[i]};
			off += pieces[i];
		}
		auto status{::fast_io::operations::scatter_read_some_bytes(in, scatters.data(), scatters.size())};
		if (status.position != npieces || status.position_in_scatter != 0)
		{
			::fast_io::fast_terminate();
		}
		check_seq(got.data(), total, 0);
	}

	{
		// scatter_pread at offset 100: chunks must land at sequential offsets
		filetype in(filename, ::fast_io::open_mode::in);
		constexpr ::std::size_t sub{4096};
		::fast_io::vector<::std::byte> got(sub, ::std::byte{0xAA});
		constexpr ::std::size_t subpieces[]{3, 2050, 2043}; // sums to 4096
		::fast_io::array<::fast_io::io_scatter_t, 3> scatters;
		::std::size_t off{};
		for (::std::size_t i{}; i != 3; ++i)
		{
			scatters[i] = {got.data() + off, subpieces[i]};
			off += subpieces[i];
		}
		auto status{::fast_io::operations::scatter_pread_some_bytes(in, scatters.data(), scatters.size(), 100)};
		if (status.position != 3 || status.position_in_scatter != 0)
		{
			::fast_io::fast_terminate();
		}
		check_seq(got.data(), sub, 100);
	}

	{
		// EOF partial read: 92 bytes available at offset 8100, demand is 250
		filetype in(filename, ::fast_io::open_mode::in);
		::fast_io::array<::std::byte, 50> head{};
		::fast_io::array<::std::byte, 200> tail{};
		head.fill(::std::byte{0xAA});
		tail.fill(::std::byte{0xAA});
		::fast_io::io_scatter_t scatters[]{{head.data(), head.size()}, {tail.data(), tail.size()}};
		auto status{::fast_io::operations::scatter_pread_some_bytes(in, scatters, 2, 8100)};
		if (status.position != 1 || status.position_in_scatter != 42)
		{
			::fast_io::fast_terminate();
		}
		check_seq(head.data(), 50, 8100);
		check_seq(tail.data(), 42, 8150);
		check_fill(tail.data() + 42, 158, ::std::byte{0xAA});
	}

	{
		// scatter_read_all over the same pieces: fills everything or throws
		filetype in(filename, ::fast_io::open_mode::in);
		::fast_io::vector<::std::byte> got(total, ::std::byte{0xAA});
		::fast_io::array<::fast_io::io_scatter_t, npieces> scatters;
		::std::size_t off{};
		for (::std::size_t i{}; i != npieces; ++i)
		{
			scatters[i] = {got.data() + off, pieces[i]};
			off += pieces[i];
		}
		::fast_io::operations::scatter_read_all_bytes(in, scatters.data(), scatters.size());
		check_seq(got.data(), total, 0);
	}

	{
		// scatter_pread_all at offset 200
		filetype in(filename, ::fast_io::open_mode::in);
		constexpr ::std::size_t sub{4096};
		::fast_io::vector<::std::byte> got(sub, ::std::byte{0xAA});
		constexpr ::std::size_t subpieces[]{3, 2050, 2043}; // sums to 4096
		::fast_io::array<::fast_io::io_scatter_t, 3> scatters;
		::std::size_t off{};
		for (::std::size_t i{}; i != 3; ++i)
		{
			scatters[i] = {got.data() + off, subpieces[i]};
			off += subpieces[i];
		}
		::fast_io::operations::scatter_pread_all_bytes(in, scatters.data(), scatters.size(), 200);
		check_seq(got.data(), sub, 200);
	}

	{
		// scatter_read_all past EOF must throw
		filetype in(filename, ::fast_io::open_mode::in);
		::fast_io::array<::std::byte, total + 1> buf{};
		::fast_io::io_scatter_t sc{buf.data(), buf.size()};
		bool caught{};
		try
		{
			::fast_io::operations::scatter_read_all_bytes(in, __builtin_addressof(sc), 1);
		}
		catch throws(::std::error)
		{
			caught = true;
		}
		if (!caught)
		{
			::fast_io::fast_terminate();
		}
	}
}

} // namespace

int main()
try
{
	run<::fast_io::nt_file>(u8"nt_scatter_read_seq.txt");
	run<::fast_io::zw_file>(u8"zw_scatter_read_seq.txt");
}
catch throws(::std::error e)
{
	::fast_io::perrln(e);
	return 1;
}
