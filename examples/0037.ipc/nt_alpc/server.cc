#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_dsal/string.h>

int main(int argc, char **argv)
{
	try
	{
		::fast_io::io::perrln("server create: ", "test");
		::fast_io::basic_nt_family_alpc_ipc_server<::fast_io::nt_family::nt, char> s{"test", ::fast_io::ipc_mode::in};
		::fast_io::basic_ibuf<::fast_io::basic_nt_family_alpc_ipc_server_observer<::fast_io::nt_family::nt, char>> s_buf{s};

		auto c{wait_for_connect(s)};
		::fast_io::string str{};
		::fast_io::io::scan(s_buf, ::fast_io::mnp::line_get(str));
		::fast_io::io::perrln("client -> connect -> server: ", str);

		::fast_io::io::perrln("server -> accept -> client: ", "good");

		::fast_io::io::print(s, "good\n");
		accept_connect(s, c, true);

		::fast_io::io::perrln("server -> send -> client: ", "hello");
		::fast_io::io::print(s, "hello\n");
	}
	catch (::fast_io::error e)
	{
		::fast_io::io::perrln(e);
	}

}
