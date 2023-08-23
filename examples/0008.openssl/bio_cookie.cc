#include<fast_io.h>
#include<fast_io_driver/openssl_driver.h>

using namespace fast_io::io;

int main()
{
	fast_io::bio_file bf(fast_io::io_cookie_type<fast_io::native_file>,"bio_cookie.txt",fast_io::open_mode::out);
	print(bf,"Hello World to bio from fast_io\n");
	BIO_printf(bf.bio,"hello %s\n","world from openssl bio");
}
