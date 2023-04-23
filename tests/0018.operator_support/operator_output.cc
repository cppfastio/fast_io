#include<fast_io_legacy.h>

struct my_custom_type
{};

inline std::ostream& operator<<(std::ostream& out,my_custom_type)
{
	return out<<"custom type";
}

using namespace fast_io::io;

int main()
{
	my_custom_type m;
	println(fast_io::mnp::operator_output(m));
}

