#include<fast_io.h>

int main()
{
	::fast_io::u8iobuf_io_file file(::fast_io::io_place_type<::fast_io::u8native_file>,u8"io_file_deco.txt",::fast_io::open_mode::out);

	::fast_io::operations::add_io_decos(file,::fast_io::decorators::u8lf_crlf{});

	using namespace ::fast_io::io;

	print(file,u8"Hello World\n",u8"14214",u8"\nHello World\n",u8"asfasfs");
	print(file,u8"Hello World\n",u8"14214",u8"\nHello World\n");
	print(file,u8"Hello World\n",u8"14214",u8"\nHello World\n",u8"124");
	print(file,u8"Hello World\n",u8"14214",u8"\nHello World\n",u8"asfasfasf",u8"asgasgasg");
	print(file,u8"Hello World\n",u8"14214",u8"\nHello World\n",u8"asfasfasf",u8"asgasgasg",u8"2632");
	print(file,u8"Hello World\n",u8"14214",u8"\nHello World\n",u8"asfasfasf",u8"asgasgasg",u8"2632",u8"1235");
	print(file,u8"Hello World\n",u8"14214",u8"\nHello World\n",u8"asfasfasf",u8"asgasgasg",u8"2632",u8"1235",u8"safas");
	print(file,u8"Hello World\n",u8"14214",u8"\nHello World\n",u8"asfasfasf",u8"asgasgasg",u8"2632",u8"1235",u8"safas",u8"asfasfsafasfsa\n");
}
