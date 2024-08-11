#include <fast_io_dsal/deque.h>
#include <fast_io.h>

int main()
{
	::fast_io::deque<int> dq;
	dq.push_back(4);
	dq.push_back(6);
	static_assert(::std::random_access_iterator<decltype(dq.begin())>);
	::fast_io::io::println("diff:", dq.end() - dq.begin());
	for (auto const &e : dq)
	{
		::fast_io::io::println(e);
	}
}
