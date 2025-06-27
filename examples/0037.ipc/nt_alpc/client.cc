#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_dsal/string.h>

int main()
{
	try
	{
		::fast_io::io::perrln("client -> connect -> server: ", "try to connect");
		::fast_io::basic_nt_family_alpc_ipc_client<::fast_io::nt_family::nt, char> c{"test", ::fast_io::ipc_mode::out, "try to connect!\n"};
		::fast_io::basic_ibuf<::fast_io::basic_nt_family_alpc_ipc_client_observer<::fast_io::nt_family::nt, char>> c_buf{c};

		::fast_io::string str{};
		::fast_io::io::scan(c_buf, ::fast_io::mnp::line_get(str));
		::fast_io::io::perrln("server -> accept -> client: ", str);

		::fast_io::io::scan(c_buf, ::fast_io::mnp::line_get(str));
		::fast_io::io::perrln("client -> recv -> server ", str);
	}
	catch (::fast_io::error e)
	{
		::fast_io::io::perrln(e);
	}
}
