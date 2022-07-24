#if defined(USE_STD)
#include<vector>
#include<cstdint>
#else
#include<fast_io_dsal/vector.h>
#endif

/*
Unfortunately fast_io internally sometimes uses vector despite i try to avoid them. Well then better expose the APIs
*/

int main()
{
#if defined(USE_STD)
	std::vector<std::int_least32_t> vec1;
	std::vector<char32_t> vec2;
#else
	fast_io::vector<std::int_least32_t> vec1;
	fast_io::vector<char32_t> vec2;
#endif
	vec1.push_back(42);
	vec2.push_back(42);
	vec1.push_back(42);
	vec2.push_back(42);
	vec1.push_back(42);
	vec2.push_back(42);
	vec1.push_back(42);
	vec2.push_back(42);
}