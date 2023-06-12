#include<fast_io.h>

int main()
{
	::fast_io::io_file file(::fast_io::io_place_type<::fast_io::native_file>,u8"io_file.txt",::fast_io::open_mode::out);
	::fast_io::io::print(file,"Hello World\n");
}
