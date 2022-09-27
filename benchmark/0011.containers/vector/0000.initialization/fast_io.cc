#include<fast_io.h>
#include<fast_io_driver/timer.h>
#include<fast_io_dsal/vector.h>

int main()
{
	fast_io::timer t(u8"fast_io::vector");
	fast_io::vector<std::size_t> vec(100000000);
}