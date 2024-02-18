#include <fast_io_dsal/list.h>
#include <fast_io.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	::fast_io::list<::std::size_t> lst{40,60,214,64,30};

	lst.sort();

	for(auto const & e : lst)
	{
		println(e);
	}
}