#include<fast_io.h>
#include<fast_io_driver/openssl_driver.h>

int main()
{
	fast_io::bio_file bf("bio.txt",fast_io::open_mode::out);
	print(bf,"Hello World to bio from fast_io\n");
	BIO_printf(bf.bio,"hello %s\n","world from openssl bio");
}