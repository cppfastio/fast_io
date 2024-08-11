#include <fast_io_dsal/stack.h>
#include <cstddef>
#include <fast_io.h>

int main()
{
	::fast_io::stack<::std::size_t> stack;
	stack.push(40);
	stack.push(60);
	::fast_io::io::println(stack.pop_element());
}
