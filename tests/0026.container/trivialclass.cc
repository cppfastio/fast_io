#include<fast_io_dsal/vector.h>

#include<fast_io.h>

struct metaindex
{
	::std::size_t modulepos{SIZE_MAX},moduleroutinepos{SIZE_MAX};
};

int main()
{
	::fast_io::vector<::fast_io::vector<metaindex>> vec;
	vec.emplace_back(30);

	auto& vec20{vec[0]};
	vec20.push_back(metaindex{20,30});
}
