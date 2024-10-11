#include <fast_io_dsal/forward_list.h>
#include <fast_io.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	::fast_io::forward_list<::std::size_t> lst;
	for (::std::size_t i{}; i != 100; ++i)
	{
		lst.push_front(i);
	}
	for (auto const &e : lst)
	{
		println(e);
	}
	lst.pop_front();
	println("front:", lst.front());
	::fast_io::forward_list<::std::size_t> lst2(lst);
	lst.erase_after(lst.cbefore_begin(), lst.cend());
	// unfinished
	println("lst is_empty? ", lst.is_empty(), "\n"
											  "lst2 is empty? ",
			lst2.is_empty(),"\n"
		"lst<=>lst2:",lst<=>lst2);
}
