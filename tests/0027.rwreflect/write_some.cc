#include <fast_io.h>
#include <fast_io_dsal/array.h>
#include <fast_io_dsal/string_view.h>
#include <fast_io_dsal/span.h>
#include <fast_io_device.h>

int main()
{
	::fast_io::obuf_file nf(u8"wsme.txt");

	constexpr ::fast_io::string_view view1("Hello World\n");
	constexpr ::fast_io::string_view view2("Do something\n");

	::fast_io::array scat{::fast_io::io_scatter_t{view1.data(), view1.size()}, ::fast_io::io_scatter_t{view2.data(), view2.size()}};

	::fast_io::operations::scatter_write_all_bytes_span(nf, scat);
	constexpr ::fast_io::string_view view("Goodby, Cruel World\n");
	::fast_io::operations::write_all_span(nf, ::fast_io::span(view));

	constexpr ::fast_io::string_view view4("Yes, Offset World\n");
	::fast_io::operations::pwrite_all_span(nf, ::fast_io::span(view1), 10);
	::fast_io::operations::scatter_pwrite_all_bytes_span(nf, scat, 30);
}
