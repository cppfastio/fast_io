#include<fast_io.h>
#include<fast_io_driver/timer.h>

int main()
{
	fast_io::timer t(BENCH_VECTOR_COMMENT_STRING);
	for(std::size_t j{};j!=100000;++j)
	{
	test::vector<std::int_least32_t> vec1;
	test::vector<char32_t> vec2;
	test::vector<double> vec3;
	test::vector<float> vec4;
	test::vector<char> vec5;
	test::vector<unsigned char> vec6;
	for(std::size_t i{};i!=100;++i)
	{
		vec1.push_back(42);
		vec2.push_back(42);
		vec3.push_back(43);
		vec4.push_back(43);
		vec5.push_back(43);
		vec6.push_back(43);
		vec1.push_back(42);
		vec2.push_back(42);
		vec3.push_back(43);
		vec4.push_back(43);
		vec5.push_back(43);
		vec6.push_back(43);
		vec1.push_back(42);
		vec2.push_back(42);
		vec3.push_back(43);
		vec4.push_back(43);
		vec5.push_back(43);
		vec6.push_back(43);
		vec1.push_back(42);
		vec2.push_back(42);
		vec3.push_back(43);
		vec4.push_back(43);
		vec5.push_back(43);
		vec6.push_back(43);
		vec1.push_back(42);
		vec2.push_back(42);
		vec3.push_back(43);
		vec4.push_back(43);
		vec5.push_back(43);
		vec6.push_back(43);
		vec1.push_back(42);
		vec2.push_back(42);
		vec3.push_back(43);
		vec4.push_back(43);
		vec5.push_back(43);
		vec6.push_back(43);
		vec1.push_back(42);
		vec2.push_back(42);
		vec3.push_back(43);
		vec4.push_back(43);
		vec5.push_back(43);
		vec6.push_back(43);
		vec1.push_back(42);
		vec2.push_back(42);
		vec3.push_back(43);
		vec4.push_back(43);
		vec5.push_back(44);
		vec6.push_back(44);
	}
	}
}