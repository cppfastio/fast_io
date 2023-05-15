#include<fast_io_core.h>

struct foo
{
	using input_char_type = char;
	using output_char_type = char;
};

extern "C" ::std::byte const* fakewrite(
	::std::byte const* first,
	::std::byte const* last) noexcept;

inline constexpr foo output_stream_ref_define(foo f) noexcept
{
	return f;
}

extern "C" void fake_scatters_write(
	::fast_io::io_scatter_t const* base,::std::size_t len) noexcept;

inline constexpr void scatter_write_all_bytes_overflow_define(
	foo,
	::fast_io::io_scatter_t const* base,::std::size_t len)
{
	return fake_scatters_write(base,len);
}

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
	::fast_io::operations::print_freestanding_decay<true>(f,
		::fast_io::basic_io_scatter_t<char>{"abc",3},
		::fast_io::basic_io_scatter_t<char>{"abcd",4},
		::fast_io::basic_io_scatter_t<char>{"abcde",5});
#endif
}
