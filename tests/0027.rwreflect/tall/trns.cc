#include<stdio.h>
#include<fast_io_core.h>
#if __has_include(<sys/uio.h>)
#include<sys/uio.h>
#endif
#include<unistd.h>
#include<cstdlib>

struct foo
{
	using input_char_type = char8_t;
	using output_char_type = char8_t;
	int fd{1};
};

inline constexpr foo io_stream_ref_define(foo f) noexcept
{
	return f;
}

inline ::std::byte const* write_some_bytes_overflow_define(foo f,::std::byte const* first,::std::byte const* last)
{
	auto res{::fast_io::noexcept_call(::write,f.fd,first,static_cast<::std::size_t>(last-first))};
	if(res<0)
	{
		::std::abort();
	}
	return first+res;
}

inline ::std::byte* read_some_bytes_underflow_define(foo f,::std::byte* first,::std::byte* last)
{
	auto res{::fast_io::noexcept_call(::read,f.fd,first,static_cast<::std::size_t>(last-first))};
	if(res<0)
	{
		::std::abort();
	}
	return first+res;
}

#if __has_include(<sys/uio.h>)

inline ::fast_io::io_scatter_status_t scatter_write_some_bytes_overflow_define(
	foo f,
	::fast_io::io_scatter_t const* base,::std::size_t len)
{
	auto res{::fast_io::noexcept_call(::writev,f.fd,reinterpret_cast<struct iovec const*>(base),len)};
	if(res<0)
	{
		::std::abort();
	}
	return ::fast_io::scatter_size_to_status(static_cast<::std::size_t>(res),base,len);
}

inline ::fast_io::io_scatter_status_t scatter_read_some_bytes_underflow_define(
	foo f,
	::fast_io::io_scatter_t const* base,::std::size_t len)
{
	auto res{::fast_io::noexcept_call(::readv,f.fd,reinterpret_cast<struct iovec const*>(base),len)};
	if(res<0)
	{
		::std::abort();
	}
	return ::fast_io::scatter_size_to_status(static_cast<::std::size_t>(res),base,len);
}
#endif

int main()
{
	foo f0{0};
	foo f1{1};
	::fast_io::operations::println_freestanding(f1,::fast_io::operations::transmit_until_eof(f1,f0));
}
