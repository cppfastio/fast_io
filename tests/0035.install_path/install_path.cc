#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_tools/install_path.h>

int main()
{
	try
	{
		auto [module_name, path_name]{::fast_io::get_module_install_path()};
		::fast_io::io::perrln(::fast_io::u8out(),u8"path: ",  path_name, u8", name: ", module_name);
		::fast_io::dir_file dir{path_name};
	}
	catch (::fast_io::error e)
	{
		::fast_io::perrln(e);
	}
}
