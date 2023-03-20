#include<string>
#include<fast_io_dsal/vector.h>

struct metaindex
{
	::std::size_t modulepos{SIZE_MAX},moduleroutinepos{SIZE_MAX};
};

struct edge_info
{
	metaindex modpos;
	::std::size_t counts{};
};

int main()
{
	::fast_io::vector<::fast_io::vector<int>> vec;
//	vec.push_back(foo{});
//	println(vec.size());
	vec.emplace_back(30);
}
