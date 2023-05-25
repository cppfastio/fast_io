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

inline constexpr foo output_stream_ref_define(foo f) noexcept
{
	return f;
}
extern "C" void fake_scatters_write(
	::fast_io::io_scatter_t const* base,::std::size_t len) noexcept;

inline ::std::byte const* write_some_bytes_overflow_define(foo f,::std::byte const* first,::std::byte const* last)
{
	auto res{::fast_io::noexcept_call(::write,f.fd,first,static_cast<::std::size_t>(last-first))};
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
#endif

struct bar
{};

#if 0
template<::fast_io::output_stream optstmtype>
inline constexpr void print_define(::fast_io::io_reserve_type_t<char8_t,bar>,optstmtype opt,bar)
{
	::fast_io::operations::print_freestanding(opt,u8"\nbar",1,u8"\nbar2\n",u8"bar3\n");
}

static_assert(::fast_io::printable<char8_t,bar>);
#endif

int main()
{
	foo f{1};
	::fast_io::operations::println_freestanding(f,u8"Hello");
//	static_assert(::fast_io::byte_output_stream<foo>);
//	:fast_io::operations::decay::scatter_write_all_bytes_decay()
//	::fast_io::operations::println_freestanding(f,::std::source_location::current(),u8"\nPara1\n",u8"Para2\n",u8"Para3\n",u8"Para4\nPara",5,u8"\nPara",6,bar{},u8"Para7\n",u8"Para8");
}
