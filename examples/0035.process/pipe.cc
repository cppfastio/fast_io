#include <fast_io.h>
#include <fast_io_device.h>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		if (argc == 0) [[unlikely]]
		{
			return 1;
		}
		::fast_io::io::perr("Usage: ", ::fast_io::mnp::os_c_str(*argv), " <exe>\n");
		return 1;
	}
	try
	{
		::fast_io::io::perr(::fast_io::out(), "Note that there is no input:\n");

		::fast_io::iobuf_pipe pipe_out;
		::fast_io::iobuf_pipe pipe_err;
		::fast_io::native_process p{
			::fast_io::mnp::os_c_str(argv[1]),
			{},
			{},
			{::fast_io::posix_dev_null(), pipe_out.handle, pipe_err.handle}};

		pipe_out.handle.out().close();
		pipe_err.handle.out().close();


		::fast_io::io::perr(::fast_io::out(), "child stdout:\n");

		for (;;)
		{
			auto res{::fast_io::operations::transmit_until_eof(::fast_io::out(), pipe_out)};
			if (!res.transmitted)
			{
				break;
			}
		}

		::fast_io::io::perr(::fast_io::out(), "child stderr:\n");

		for (;;)
		{
			auto res{::fast_io::operations::transmit_until_eof(::fast_io::out(), pipe_err)};
			if (!res.transmitted)
			{
				break;
			}
		}

	}
	catch (fast_io::error e)
	{
		perrln(e);
		return 1;
	}
}
