#include <fast_io.h>
#include <fast_io_driver/timer.h>

int main()
{
	constexpr ::std::size_t total_iters{100};
	constexpr ::std::size_t sc_count{32};
	char buf[4095]{};
	::fast_io::io_scatter_t scatters[sc_count];
	for (auto &e : scatters)
	{
		e = {buf, sizeof(buf)};
	}
	{
		::fast_io::timer tm(u8"nt_scatter_loop");
		::fast_io::nt_file nf(u"nt_scatter_loop.txt", ::fast_io::open_mode::out);
		for (::std::size_t i{}; i != total_iters; ++i)
		{
			for (auto const &e : scatters)
			{
				auto b{static_cast<::std::byte const *>(e.base)};
				::fast_io::operations::write_all_bytes(nf, b, b + e.len);
			}
		}
	}
	{
		::fast_io::timer tm(u8"nt_scatter_buffered");
		::fast_io::nt_file nf(u"nt_scatter_buffered.txt", ::fast_io::open_mode::out);
		for (::std::size_t i{}; i != total_iters; ++i)
		{
			::fast_io::operations::scatter_write_all_bytes(nf, scatters, sc_count);
		}
	}
}
