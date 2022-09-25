#if defined(USE_STD)
#include<vector>
#include<cstdint>
namespace test
{
template<typename T>
using vector = ::std::vector<T>;
}
#else
#include<fast_io_dsal/vector.h>

namespace test
{
template<typename T>
using vector = ::fast_io::vector<T>;
}
#endif

/*
Unfortunately fast_io internally sometimes uses vector despite i try to avoid them. Well then better expose the APIs
*/

int main()
{
	test::vector<test::vector<std::int_least32_t>> vec1(1000);
	test::vector<test::vector<char32_t>> vec2;

}