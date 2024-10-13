#include <fast_io.h>
#include <fast_io_dsal/list.h>
#include <iterator>

int main()
{
	fast_io::list<int> list1{1, 2, 3, 4, 5};
	fast_io::list<int> list2{10, 20, 30, 40, 50};

	auto it = list1.begin();
	std::advance(it, 2);

	list1.splice(it, std::move(list2));

	::fast_io::println("list1:", ::fast_io::mnp::rgvw(list1, " "), "\nlist2:",
			::fast_io::mnp::rgvw(list2, " "));

	list2.splice(list2.begin(), it, list1.end());

	::fast_io::println("list1:", ::fast_io::mnp::rgvw(list1, " "), "\nlist2:",
			::fast_io::mnp::rgvw(list2, " "));

	list2.splice(list2.cbegin(), list1.cbegin(), list1.cbegin());

	::fast_io::println("list1:", ::fast_io::mnp::rgvw(list1, " "), "\nlist2:",
			::fast_io::mnp::rgvw(list2, " "));
}

