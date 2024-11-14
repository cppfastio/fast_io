#include <fast_io.h>

int main()
{
	try
	{
		::fast_io::win32_pipe pipe_out{};
		::fast_io::win32_process_io npi{.out{pipe_out}};
		::fast_io::win32_process child{
#ifdef _WIN32
			u8"child.exe"
#else
			u8"child"
#endif
			,
			{},
			{},
			npi};
		for (;;)
		{
			char buffer[1024];
			::fast_io::operations::read_all(pipe_out.in(), buffer, buffer + 1024);
			::fast_io::io::perr("read!\n");
		}

	}
	catch (fast_io::error e)
	{
		::fast_io::io::perrln(e);
	}
}
