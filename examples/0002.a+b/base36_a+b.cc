#include<fast_io.h>

int main()
{
	using namespace fast_io::mnp;
	std::size_t a,b;
	scan(base_get<36>(a),base_get<36>(b));
	println(base<36>(a+b));
}
