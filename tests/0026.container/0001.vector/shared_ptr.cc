#include <fast_io_dsal/vector.h>
#include <fast_io.h>
#include <memory>

int main()
{
	::fast_io::vector<::std::shared_ptr<int>> vec{std::make_shared<int>(1), std::make_shared<int>(2), std::make_shared<int>(3)};
	vec.insert(vec.begin() + 1, std::make_shared<int>(4));
	for (auto const &e : vec)
	{
		::fast_io::io::println(::fast_io::mnp::pointervw(e.get()), " use_count: ", e.use_count());
	}
}
