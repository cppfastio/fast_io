#include <fast_io_dsal/list.h>
#include <fast_io.h>
#include <memory>

int main()
{
	::fast_io::list<::std::shared_ptr<int>> lst{std::make_shared<int>(1), std::make_shared<int>(2), std::make_shared<int>(3)};
	lst.insert(lst.cbegin(), std::make_shared<int>(4));
	for (auto const &e : lst)
	{
		::fast_io::io::println(::fast_io::mnp::pointervw(e.get()), " use_count: ", e.use_count());
	}
}
