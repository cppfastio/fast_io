#include <fast_io_dsal/priority_queue.h>
#include <cstddef>
#include <fast_io.h>

int main()
{
	::fast_io::priority_queue<::std::size_t> pqueue;
	pqueue.push(40);
	pqueue.push(60);
	pqueue.push(70);
	pqueue.push(20);
	pqueue.push(15);
	::fast_io::io::println(pqueue.pop_element());
}

