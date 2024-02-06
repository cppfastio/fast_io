#include <fast_io.h>
#include <fast_io_device.h>

int main()
{
	fast_io::obuf_file obf(u8"obf.txt");
	print(obf, "Hello World\n");
}
