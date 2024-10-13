#include <fast_io_dsal/forward_list.h>
#include <fast_io.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	::fast_io::forward_list<::std::size_t> lst;
	for (::std::size_t i{}; i != 10; ++i)
	{
		lst.push_front(i);
	}
	print("lst:\n");
	for (auto const &e : lst)
	{
		println(e);
	}
	println("front:", lst.front());
	::fast_io::forward_list<::std::size_t> lst2(lst);
	lst2.splice_before_after(lst2.cbefore_begin(), lst.cbefore_begin(), lst.cbegin());
	lst2.splice_before_after(lst2.cbefore_begin(), lst.cbefore_begin(), lst.cbefore_begin());
	lst.erase_after(lst.cbefore_begin(), lst.cbegin());
	lst.pop_front();
	print("lst2:\n");
	for (auto const &e : lst2)
	{
		println(e);
	}
	print("lst after operations:\n");
	for (auto const &e : lst)
	{
		println(e);
	}
	// unfinished
	println("lst is empty? ", lst.empty(), "\n"
										   "lst2 is empty? ",
			lst2.is_empty(), "\n"
							 "lst<=>lst2:",
			lst <=> lst2);
}
