#include<fast_io_core.h>
#if __has_include(<sys/uio.h>)
#include<sys/uio.h>
#endif
#include<unistd.h>
#include<cstdlib>

struct foo
{
	using input_char_type = char;
	using output_char_type = char;
};

#if 0
extern "C" ::std::byte const* fakewrite(
	::std::byte const* first,
	::std::byte const* last) noexcept;
#endif
inline constexpr foo output_stream_ref_define(foo f) noexcept
{
	return f;
}
extern "C" void fake_scatters_write(
	::fast_io::io_scatter_t const* base,::std::size_t len) noexcept;

inline ::std::byte const* write_some_bytes_overflow_define(foo,::std::byte const* first,::std::byte const* last)
{
	auto res{::fast_io::noexcept_call(::write,2,first,static_cast<::std::size_t>(last-first))};
	if(res<0)
	{
		::std::abort();
	}
	return first+res;
}
#if __has_include(<sys/uio.h>)
inline ::fast_io::io_scatter_status_t scatter_write_some_bytes_overflow_define(
	foo,
	::fast_io::io_scatter_t const* base,::std::size_t len)
{
	auto res{::fast_io::noexcept_call(::writev,2,reinterpret_cast<struct iovec const*>(base),len)};
	if(res<0)
	{
		::std::abort();
	}
	return ::fast_io::scatter_size_to_status(static_cast<::std::size_t>(res),base,len);
//	return fake_scatters_write(base,len);
}
#endif
int main()
{
	foo f;

#if 0	

constexpr bool test_char_put
{
#ifdef TEST_CHAR_PUT
true
#endif
};

constexpr bool use_fake
{
#ifdef USE_FAKE
true
#endif
};
	if constexpr(test_char_put)
	{
		if constexpr(use_fake)
		{
			char const ch{'4'};
			fakewrite(reinterpret_cast<::std::byte const*>(__builtin_addressof(ch)),
				reinterpret_cast<::std::byte const*>(__builtin_addressof(ch)+1));
		}
		else
		{
			::fast_io::operations::char_put(f,'4');
		}
	}
	else
	{
		char const buffer[]="asfasfsafasf";
	if constexpr(use_fake)
	{
		fakewrite(reinterpret_cast<::std::byte const*>(buffer),
			reinterpret_cast<::std::byte const*>(buffer+5));
	}
	else
	{
	::fast_io::operations::write_some(f,buffer,
		buffer+5);
	}
	}
	char buffer[20]="abcde\nf";
	char buffer1[20]="ppppp\nf";

	::fast_io::io_scatter_t scat[2]
	{{buffer,6},{buffer1,6}};

		::fast_io::operations::scatter_write_all_bytes(f,scat,2);
#endif
#if TEST_WRITE_ALL
	::fast_io::operations::write_all(f,"abc","abc"+3);
#else
#if 0
	::fast_io::operations::print_freestanding_decay<true>(f,
		::fast_io::basic_io_scatter_t<char>{"abc\n",4},
		::fast_io::basic_io_scatter_t<char>{"abcd\n",5},
		::fast_io::basic_io_scatter_t<char>{"abcde",5});
#endif
	::fast_io::operations::println_freestanding(f,"abc\n","abcsdfasdf\n","asfasf\n","sdgsg",421);
#endif
}
