#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_dsal/string.h>

int main()
{
	try
	{
		::fast_io::io::print(::fast_io::u8out(), u8"create win32 named pipe server sender!\nserver name: ");
		::fast_io::u8string pipe_name{};
		::fast_io::scan(::fast_io::u8c_stdin(), ::fast_io::mnp::line_get(pipe_name));
		::fast_io::u8win32_named_pipe_ipc_server s{pipe_name, ::fast_io::ipc_mode::out};
		::fast_io::basic_obuf<::fast_io::u8win32_named_pipe_ipc_server> s_obuf{s};
		::fast_io::io::print(::fast_io::u8out(), u8"wait for connect\n");
		wait_for_connect(s, {}, {});
		::fast_io::io::print(::fast_io::u8out(), u8"connected!\n");
		for (;;)
		{
			::fast_io::u8string str_name{};
			::fast_io::io::print(::fast_io::u8out(), u8"send: ");
			::fast_io::scan(::fast_io::u8c_stdin(), ::fast_io::mnp::line_get(str_name));
			::fast_io::println(s_obuf, str_name);
			::fast_io::details::close_basic_io_buffer(s_obuf);
		}
	}
	catch (::fast_io::error e)
	{
		::fast_io::io::perrln(e);
	}
}
