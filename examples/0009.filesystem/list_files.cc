/*
 * Usage:
 *     ./list_files [directory]
 */
#include <fast_io.h>
#include <fast_io_device.h>

using namespace fast_io::io;

int main(int argc, char **argv)
{
	using namespace fast_io::mnp;
	auto dname = os_c_str(".");
	if (argc > 1)
	{
		dname = os_c_str(argv[1]);
	}
	try
	{
		fast_io::dir_file df(dname);
		for (auto const &ent : current(at(df)))
		{
			if (is_dot(ent)) // ignore . and ..
			{
				continue;
			}
			switch (type(ent))
			{
				using enum ::fast_io::file_type;
			case directory:
				print("D: ");
				break;
			default:
				print("F: ");
			}
			println(code_cvt(u8filename(ent)));
		}
	}
	catch (fast_io::error &e)
	{
		perr("Fatal error: ", e);
		return 1;
	}
	return 0;
}
