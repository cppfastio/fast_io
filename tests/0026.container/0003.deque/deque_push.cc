#include <fast_io_dsal/deque.h>
#include <fast_io.h>

int main()
{
	::fast_io::deque<int> deq;

	for (int i{}; i != 1000000; ++i)
	{
		deq.push_front(i);
		deq.push_back(i);
	}
	::fast_io::io::println(deq.size());
}
