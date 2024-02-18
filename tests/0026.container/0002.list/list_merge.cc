#include <fast_io_dsal/list.h>
#include <fast_io.h>

int main()
{
	fast_io::list<int> list1{5, 9, 1, 3, 3};
	fast_io::list<int> list2{8, 7, 2, 3, 4, 4};

	list1.sort();
	list2.sort();
	println("list1:", ::fast_io::mnp::rgvw(list1, " "), "\nlist2:",
			::fast_io::mnp::rgvw(list2, " "));

	list1.merge(::std::move(list2));
	println("merged:", ::fast_io::mnp::rgvw(list1, " "));
}