#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	fast_io::vector<int> a1{1, 2, 3}, a2{4, 5};

	auto it1 = std::next(a1.begin());
	auto it2 = std::next(a2.begin());

	int &ref1 = a1.front();
	int &ref2 = a2.front();

	perrln("{ ", rgvw(a1, " "), " } { ", rgvw(a2, " "), " } ", *it1, " ", *it2, " ", ref1, ' ', ref2);
	a1.swap(a2);
	perrln("{ ", rgvw(a1, " "), " } { ", rgvw(a2, " "), " } ", *it1, " ", *it2, " ", ref1, ' ', ref2);

	// Note that after swap the iterators and references stay associated with their
	// original elements, e.g. it1 that pointed to an element in 'a1' with value 2
	// still points to the same element, though this element was moved into 'a2'.
}