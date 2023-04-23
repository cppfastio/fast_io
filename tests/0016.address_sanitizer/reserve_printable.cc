#include<fast_io.h>

struct foo
{

};

inline constexpr std::size_t print_reserve_size(::fast_io::io_reserve_type_t<char,foo>) noexcept
{
	return 5;
}

inline constexpr char* print_reserve_define(::fast_io::io_reserve_type_t<char,foo>,char* out,foo f)
{
	out[7]=2;
	return out+5;
}

static_assert(fast_io::reserve_printable<char,foo>);

using namespace fast_io::io;

int main()
{
	foo f;
	println("Hellowefqewjopfewjf");
	print(f);
}

/*
-fsanitize=address -DFAST_IO_SANITIZE_IO_BUFFER should detect bug here
*/