#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_dsal/array.h>
#include <fast_io_dsal/string.h>
#include <fast_io_dsal/string_view.h>

int main()
{
	::fast_io::ibuf_file nf(u8"wsme.txt");
	::fast_io::array<char, 10> buffer;
	::fast_io::string str(10);

	::fast_io::array<::fast_io::io_scatter_t, 2> scat{::fast_io::io_scatter_t{buffer.data(), buffer.size()}, ::fast_io::io_scatter_t{str.data(), str.size()}};

	::fast_io::operations::scatter_pread_all_bytes_span(nf, scat, 2);
	::fast_io::io::println("buffer:", ::fast_io::string_view(::fast_io::freestanding::from_range, buffer),
						   "\nstr:", str);
}
