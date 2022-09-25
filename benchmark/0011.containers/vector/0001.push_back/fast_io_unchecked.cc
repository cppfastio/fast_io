#include<fast_io.h>
#include<fast_io_driver/timer.h>
#include<fast_io_dsal/vector.h>

int main()
{
	fast_io::timer tm(u8"fast_io::vector::push_back_unchecked");
	fast_io::vector<std::size_t> vec;
	constexpr std::size_t n{100000000};
	vec.reserve(n);
	for(std::size_t i{};i!=n;++i)
	{
		vec.push_back_unchecked(i);
	}
}