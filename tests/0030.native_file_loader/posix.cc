#include <fast_io.h>
#include <fast_io_device.h>

int main(int argc, char** argv) {
	if (argc != 2) {
		if (argc == 0) {
			return 1;
		}
		::fast_io::io::perr("Usage: ", ::fast_io::mnp::os_c_str(*argv), " <file>\n");
		return 1;
	}
	::fast_io::posix_file_loader a{::fast_io::mnp::os_c_str(argv[1])};
	::fast_io::io::print(a);
}
