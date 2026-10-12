#include <fast_io.h>
#include <fast_io_device.h>
#include <sys/socket.h>
#include <unistd.h>

int main()
{
	using namespace ::fast_io;
	constexpr ::std::size_t insize{100000};

	native_file in(io_temp);
	for (::std::size_t i{}; i < insize; i += 4096)
	{
		char buf[4096];
		for (::std::size_t j{}; j != 4096; ++j)
		{
			buf[j] = static_cast<char>((i + j) & 0x7f);
		}
		::fast_io::operations::write_all_range(in, buf);
	}
	::fast_io::operations::decay::input_stream_seek_bytes_decay(
		::fast_io::operations::input_stream_ref(in), 0, ::fast_io::seekdir::beg);

	// pipe -> file (splice path)
	{
		posix_pipe pp;
		char msg[60000];
		for (::std::size_t i{}; i != sizeof(msg); ++i)
		{
			msg[i] = static_cast<char>(i & 0x7f);
		}
		::fast_io::operations::write_all_range(pp.out(), msg);
		pp.out().close();
		native_file out(io_temp);
		::fast_io::operations::transmit_all_bytes(out, {}, pp.in(), {}, {});
		auto sz{static_cast<::std::uintmax_t>(::fast_io::operations::decay::input_stream_seek_bytes_decay(
			::fast_io::operations::input_stream_ref(out), 0, ::fast_io::seekdir::end))};
		if (sz != sizeof(msg))
		{
			::fast_io::fast_terminate();
		}
		char got[8];
		auto it{::fast_io::operations::decay::pread_some_bytes_decay(
			::fast_io::operations::input_stream_ref(out), reinterpret_cast<::std::byte *>(got), 8, 100)};
		if (it != reinterpret_cast<::std::byte *>(got) + 8)
		{
			::fast_io::fast_terminate();
		}
		for (int i{}; i != 8; ++i)
		{
			if (got[i] != static_cast<char>((100 + i) & 0x7f))
			{
				::fast_io::fast_terminate();
			}
		}
	}

	// file -> pipe (splice path, output side is fifo)
	{
		posix_pipe pp;
		::fast_io::operations::decay::input_stream_seek_bytes_decay(
			::fast_io::operations::input_stream_ref(in), 0, ::fast_io::seekdir::beg);
		::std::size_t n{::fast_io::operations::transmit_some_bytes(pp.out(), {}, in, {}, 30000)};
		if (n == 0 || n > 30000)
		{
			::fast_io::fast_terminate();
		}
		char got[8];
		auto r{::fast_io::noexcept_call(::read, pp.in().fd, got, 8)};
		if (r != 8)
		{
			::fast_io::fast_terminate();
		}
		for (int i{}; i != 8; ++i)
		{
			if (got[i] != static_cast<char>(i & 0x7f))
			{
				::fast_io::fast_terminate();
			}
		}
		pp.out().close();
		pp.in().close();
	}

	// file -> socket (sendfile path)
	{
		int sv[2]{-1, -1};
		if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sv) != 0)
		{
			::fast_io::fast_terminate();
		}
		::fast_io::operations::decay::input_stream_seek_bytes_decay(
			::fast_io::operations::input_stream_ref(in), 0, ::fast_io::seekdir::beg);
		posix_io_observer sout{sv[0]};
		::std::size_t n{::fast_io::operations::transmit_some_bytes(sout, {}, in, {}, 12345)};
		if (n == 0 || n > 12345)
		{
			::fast_io::fast_terminate();
		}
		char got[8];
		auto r{::fast_io::noexcept_call(::read, sv[1], got, 8)};
		if (r != 8)
		{
			::fast_io::fast_terminate();
		}
		for (int i{}; i != 8; ++i)
		{
			if (got[i] != static_cast<char>(i & 0x7f))
			{
				::fast_io::fast_terminate();
			}
		}
		::close(sv[0]);
		::close(sv[1]);
	}

	// socket -> file (no applicable zero-copy syscall -> read/write emulation)
	{
		int sv[2]{-1, -1};
		if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sv) != 0)
		{
			::fast_io::fast_terminate();
		}
		char msg[4096];
		for (::std::size_t i{}; i != sizeof(msg); ++i)
		{
			msg[i] = 'q';
		}
		if (::fast_io::noexcept_call(::write, sv[1], msg, sizeof(msg)) != static_cast<::std::ptrdiff_t>(sizeof(msg)))
		{
			::fast_io::fast_terminate();
		}
		posix_io_observer sin{sv[0]};
		native_file out(io_temp);
		::std::size_t n{::fast_io::operations::transmit_some_bytes(out, {}, sin, {}, sizeof(msg))};
		if (n != sizeof(msg))
		{
			::fast_io::fast_terminate();
		}
		::close(sv[0]);
		::close(sv[1]);
	}

	// explicit offset on a pipe is invalid -> EINVAL herbception
	{
		posix_pipe pp;
		::fast_io::print(pp.out(), "x");
		pp.out().close();
		native_file out(io_temp);
		::fast_io::intfpos_t badoff{};
		bool threw{};
		try
		{
			::fast_io::operations::transmit_some_bytes(out, {}, pp.in(), badoff, 10);
		}
		catch throws(::std::error)
		{
			threw = true;
		}
		if (!threw)
		{
			::fast_io::fast_terminate();
		}
		pp.in().close();
	}
}
