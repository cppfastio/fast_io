#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_dsal/string.h>

int main()
{
	try
	{
		::fast_io::io::print(::fast_io::u8out(), u8"connect to win32 named pipe server sender!\nserver name: ");
		::fast_io::u8string pipe_name{};
		::fast_io::scan(::fast_io::u8c_stdin(), ::fast_io::mnp::line_get(pipe_name));
		::fast_io::u8win32_named_pipe_ipc_client s{pipe_name, ::fast_io::ipc_mode::in};
		::fast_io::basic_ibuf<::fast_io::u8win32_named_pipe_ipc_client_observer> s_ibuf{s};
		::fast_io::io::print(::fast_io::u8out(), u8"connected!\n");
		for (;;)
		{
			::fast_io::u8string str_name{};
			::fast_io::scan(s_ibuf, ::fast_io::mnp::line_get(str_name));
			::fast_io::println(::fast_io::u8out(), str_name);
		}
	}
	catch (::fast_io::error e)
	{
		::fast_io::io::perrln(e);
	}
}
