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

template <typename filetype, typename T>
inline void run(T const &filename) throws
{
	constexpr ::std::size_t total{8192};
	::fast_io::vector<::std::byte> data(total);
	for (::std::size_t i{}; i != total; ++i)
	{
		data[i] = static_cast<::std::byte>(i & 0xFF);
	}

	// pieces straddle the direct-write threshold: tiny/buffered, 3071 edge, 4000 direct, 1114 tail
	constexpr ::std::size_t pieces[]{7, 1000, 3071, 4000, 1114}; // sums to 8192
	::fast_io::vector<::fast_io::io_scatter_t> scatters;
	::std::size_t off{};
	for (auto sz : pieces)
	{
		scatters.push_back({data.data() + off, sz});
		off += sz;
	}

	{
		filetype file(filename, ::fast_io::open_mode::out);
		::fast_io::operations::scatter_write_all_bytes(file, scatters.data(), scatters.size());
		// same pattern again at offset 100 -> file ends at 8292
		::fast_io::operations::scatter_pwrite_all_bytes(file, scatters.data(), scatters.size(), 100);
	}
	{
		filetype in(filename, ::fast_io::open_mode::in);
		::fast_io::vector<::std::byte> got(total + 100);
		::fast_io::operations::read_all_bytes(in, got.data(), got.size());
		check_seq(got.data(), 100, 0);           // [0,100): leftover of the first write
		check_seq(got.data() + 100, 8092, 0);    // [100,8192): pwrite seq
		check_seq(got.data() + 8192, 100, 8092); // [8192,8292): pwrite tail
	}
}

} // namespace

int main()
try
{
	run<::fast_io::nt_file>(u8"nt_scatter_seq.txt");
	run<::fast_io::zw_file>(u8"zw_scatter_seq.txt");
}
catch throws(::std::error e)
{
	::fast_io::perrln(e);
	return 1;
}
