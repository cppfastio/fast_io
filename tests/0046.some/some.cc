#include <fast_io.h>
#include <fast_io_device.h>

#include <cstdio>

/*
the read/write/pread/pwrite *_some_ primitives return the number of
elements (bytes for the *_bytes_ forms) actually transferred, not the
end pointer.  Exercise every one of the eight public entry points and
check the returned counts.
*/

namespace
{
inline void check(bool b) noexcept
{
	if (!b)
	{
		::fast_io::fast_terminate();
	}
}
} // namespace

int main()
{
	try
	{
		::fast_io::native_file f{::fast_io::io_temp};
		char buf[64];
		for (::std::size_t i{}; i != 64; ++i)
		{
			buf[i] = static_cast<char>(i);
		}
		auto const *u8buf{reinterpret_cast<::std::byte const *>(buf)};
		auto *u8mut{reinterpret_cast<::std::byte *>(buf)};

		/* a fresh temp file is empty -- a read hits eof and transfers 0 */
		check(::fast_io::operations::read_some_bytes(f, u8mut, 64) == 0);
		check(::fast_io::operations::read_some(f, buf, 8) == 0);

		/* the byte/element counts of a successful write are returned */
		check(::fast_io::operations::write_some_bytes(f, u8buf, 64) == 64);
		check(::fast_io::operations::write_some(f, buf, 64) == 64);

		/* pwrite/pwrite bytes honour the offset and return the count */
		check(::fast_io::operations::pwrite_some_bytes(f, u8buf, 8, 0) == 8);
		check(::fast_io::operations::pwrite_some(f, buf, 8, 0) == 8);

		/* pread/pread bytes return the count too */
		check(::fast_io::operations::pread_some_bytes(f, u8mut, 8, 0) == 8);
		check(::fast_io::operations::pread_some(f, buf, 8, 0) == 8);

		/* a buffered stream routes through the same primitives */
		::fast_io::obuf_file obf{u"some_test.tmp", ::fast_io::open_mode::out | ::fast_io::open_mode::trunc |
													   ::fast_io::open_mode::creat};
		check(::fast_io::operations::write_some_bytes(obf, u8buf, 64) == 64);
		check(::fast_io::operations::write_some(obf, buf, 64) == 64);
		obf.close();
		::fast_io::ibuf_file ibf{u"some_test.tmp"};
		check(::fast_io::operations::read_some_bytes(ibf, u8mut, 64) == 64);
		check(::fast_io::operations::read_some(ibf, buf, 64) == 64);
		ibf.close();
		(void)::std::remove("some_test.tmp");

		/* transmit_some_bytes forwards between streams and returns bytes */
		::fast_io::operations::decay::input_stream_seek_bytes_decay(
			::fast_io::operations::input_stream_ref(f), 0, ::fast_io::seekdir::beg);
		::fast_io::native_file dst{::fast_io::io_temp};
		check(::fast_io::operations::transmit_some_bytes(dst, {}, f, {}, 16) == 16);
	}
	catch throws(::std::error e)
	{
		::fast_io::perrln(e);
		return 1;
	}
	return 0;
}
