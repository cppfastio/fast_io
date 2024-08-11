#include <fast_io_dsal/queue.h>
#include <cstddef>
#include <fast_io.h>

int main()
{
	::fast_io::queue<::std::size_t> queue;
	queue.push(40);
	queue.push(60);
	::fast_io::io::println(queue.pop_element());
}
