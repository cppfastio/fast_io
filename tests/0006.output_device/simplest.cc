#include<fast_io.h>

struct foo
{
	using char_type = char8_t;
};

inline constexpr foo io_value_handle(foo f) noexcept
{
	return f;
}

inline void write(foo,char8_t const* first,char8_t const* last)
{
	char8_t val{};
	auto diff{last-first};
	for(;first!=last;++first)
		val^=*first;
	println(fast_io::out(),diff," ",val);
}

struct new_type
{
};

inline constexpr std::size_t print_reserve_size(fast_io::io_reserve_type_t<char8_t,new_type>,new_type) noexcept
{
	return 30;
}

template<std::random_access_iterator Iter>
inline constexpr Iter print_reserve_define(fast_io::io_reserve_type_t<char8_t,new_type>,Iter iter,new_type) noexcept
{
	*iter=u8'o';
	++iter;
	return iter;
}

static_assert(fast_io::dynamic_reserve_printable<char8_t,new_type>);

int main()
{
	foo f;
	println(f,u8"Hello World\n",4,u8" ",20,u8" ",new_type{});
	println(f,u8"Hello World\n",4,u8" ",20,new_type{},u8" ",new_type{});
	println(f,u8"Hello World\n",4,u8" ",20,new_type{},u8" ",new_type{});
	println(f,u8"Hello World\n",4,u8" ",20,new_type{},u8" ",new_type{});
}