#include <fast_io.h>
#include <fast_io_device.h>
#include <string_view>

#if defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__)
#include <io.h>
#define FAST_IO_TEST_READ ::_read
#define FAST_IO_TEST_WRITE ::_write
#else
#include <unistd.h>
#define FAST_IO_TEST_READ ::read
#define FAST_IO_TEST_WRITE ::write
#endif

/*
Custom fd-backed streams with no transmit handle — forces the generic
read/write emulation path.
*/
struct fd_in
{
	using input_char_type = char;
	int fd{-1};
};

struct fd_out
{
	using output_char_type = char;
	int fd{-1};
};

inline constexpr fd_in io_stream_ref_define(fd_in f) noexcept
{
	return f;
}

inline constexpr fd_out io_stream_ref_define(fd_out f) noexcept
{
	return f;
}

inline ::std::byte *read_some_bytes_underflow_define(fd_in f, ::std::byte *first, ::std::size_t count)
	throws
{
	auto res{::fast_io::noexcept_call(FAST_IO_TEST_READ, f.fd, first, count)};
	if (res < 0)
	{
		::fast_io::fast_terminate();
	}
	return first + res;
}

inline ::std::byte const *write_some_bytes_overflow_define(fd_out f, ::std::byte const *first,
														   ::std::size_t count)
	throws
{
	auto res{::fast_io::noexcept_call(FAST_IO_TEST_WRITE, f.fd, first, count)};
	if (res < 0)
	{
		::fast_io::fast_terminate();
	}
	return first + res;
}

constexpr ::std::size_t insize{200000};

template <typename T>
inline void fill_pattern(T &f)
	throws
{
	char buf[4096];
	for (::std::size_t i{}; i < insize;)
	{
		::std::size_t chunk{insize - i < sizeof(buf) ? insize - i : sizeof(buf)};
		for (::std::size_t j{}; j != chunk; ++j)
		{
			buf[j] = static_cast<char>((i + j) & 0x7f);
		}
		::fast_io::operations::write_all_range(f, ::std::string_view{buf, chunk});
		i += chunk;
	}
}

template <typename T>
inline ::std::uintmax_t checked_size(T &f)
	throws
{
	return static_cast<::std::uintmax_t>(::fast_io::operations::decay::input_stream_seek_bytes_decay(
		::fast_io::operations::input_stream_ref(f), 0, ::fast_io::seekdir::end));
}

template <typename T>
inline void reset_in(T &f)
	throws
{
	::fast_io::operations::decay::input_stream_seek_bytes_decay(::fast_io::operations::input_stream_ref(f), 0,
															  ::fast_io::seekdir::beg);
}

template <typename T>
inline void check_at(T &f, ::fast_io::intfpos_t pos, ::std::size_t expect_base, ::std::size_t n)
	throws
{
	char got[16];
	auto it{::fast_io::operations::decay::pread_some_bytes_decay(
		::fast_io::operations::input_stream_ref(f), reinterpret_cast<::std::byte *>(got), n, pos)};
	if (it != reinterpret_cast<::std::byte *>(got) + n)
	{
		::fast_io::fast_terminate();
	}
	for (::std::size_t i{}; i != n; ++i)
	{
		if (got[i] != static_cast<char>((expect_base + i) & 0x7f))
		{
			::fast_io::fast_terminate();
		}
	}
}

int main()
{
	using namespace ::fast_io;

	native_file in(io_temp);
	fill_pattern(in);
	reset_in(in);

	// 1. file -> file, null offsets (current positions)
	{
		native_file out(io_temp);
		::fast_io::operations::transmit_all_bytes(out, {}, in, {}, {});
		if (checked_size(out) != insize)
		{
			::fast_io::fast_terminate();
		}
		check_at(out, 4096, 4096, 8);
	}

	// 2. explicit offsets on both sides, bounded size. Needs real seekable
	// files — io_temp fds are O_APPEND so pwrite to an explicit offset would
	// correctly fail.
	{
		native_file in2(u8"transmit_in2.tmp", open_mode::out | open_mode::in | open_mode::trunc | open_mode::creat);
		fill_pattern(in2);
		native_file out(u8"transmit_out2.tmp", open_mode::out | open_mode::in | open_mode::trunc | open_mode::creat);
		::fast_io::intfpos_t offin{1000}, offout{5000};
		::fast_io::operations::transmit_all_bytes(out, offout, in2, offin, 4000);
		if (offin != 5000 || offout != 9000 || checked_size(out) != 9000)
		{
			::fast_io::fast_terminate();
		}
		check_at(out, 5000, 1000, 8);
	}

	// 3. some: bounded single call
	{
		reset_in(in);
		native_file out(io_temp);
		::std::size_t n{::fast_io::operations::transmit_some_bytes(out, {}, in, {}, 7000)};
		if (n == 0 || n > 7000 || checked_size(out) != n)
		{
			::fast_io::fast_terminate();
		}
	}

	// 4. buffered input stream: read-ahead drained before fd-level copy
	{
		::fast_io::ibuf_file inb(io_temp);
		fill_pattern(inb.handle);
		reset_in(inb.handle);
		char tmp[512];
		auto it{::fast_io::operations::decay::read_some_bytes_decay(
			::fast_io::operations::input_stream_ref(inb), reinterpret_cast<::std::byte *>(tmp), 512)};
		if (static_cast<::std::size_t>(it - reinterpret_cast<::std::byte *>(tmp)) != 512)
		{
			::fast_io::fast_terminate();
		}
		native_file out(io_temp);
		::fast_io::operations::transmit_all_bytes(out, {}, inb, {}, {});
		if (checked_size(out) != insize - 512)
		{
			::fast_io::fast_terminate();
		}
		check_at(out, 0, 512, 8);
	}

	// 5. buffered output stream: pending data flushed before fd-level transmit
	{
		reset_in(in);
		::fast_io::obuf_file out(io_temp);
		print(out, "PENDING");
		::fast_io::operations::transmit_some_bytes(out, {}, in, {}, 100);
		print(out, "TAIL");
		if (status(out.handle).size != 107)
		{
			::fast_io::fast_terminate();
		}
	}

	// 6. emulation path via custom streams (no transmit handle)
	{
		reset_in(in);
		native_file out(io_temp);
		fd_in fin{static_cast<native_io_observer>(in).fd};
		fd_out fout{static_cast<native_io_observer>(out).fd};
		::std::size_t total{};
		for (::std::size_t n{};
			 (n = ::fast_io::operations::transmit_some_bytes(fout, {}, fin, {},
															 ::std::numeric_limits<::std::size_t>::max())) != 0;)
		{
			total += n;
		}
		if (total != insize)
		{
			::fast_io::fast_terminate();
		}
		check_at(out, insize - 8, insize - 8, 8);
	}

	// 7. all bounded by size_t_opt
	{
		reset_in(in);
		native_file out(io_temp);
		::fast_io::operations::transmit_all_bytes(out, {}, in, {}, ::fast_io::size_t_opt{3333});
		if (checked_size(out) != 3333)
		{
			::fast_io::fast_terminate();
		}
	}
}
