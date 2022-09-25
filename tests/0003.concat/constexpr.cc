#include<string>
#include<fast_io.h>
#include<cassert>

constexpr bool foo()
{
	std::string str{fast_io::concat("abcwe")};
	std::string str2{"abcwe"};
	return str==str2;
}

static_assert(foo());

int main()
{
	assert(foo());
}