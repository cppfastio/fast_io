#include <fast_io.h>
#include <fast_io_dsal/vector.h>
#include <fast_io_dsal/deque.h>
#include <fast_io_dsal/list.h>
#include <fast_io_dsal/forward_list.h>
#include <fast_io_dsal/array.h>
#include <fast_io_dsal/stack.h>
#include <fast_io_dsal/queue.h>
#include <fast_io_dsal/priority_queue.h>

int main()
{
	::fast_io::zu_vector zv{1zu, 2zu, 3zu};
	static_assert(::std::same_as<::fast_io::zu_vector::value_type, ::std::size_t>);
	::fast_io::zu_deque zd;
	zd.push_back(1zu);
	::fast_io::zu_list zl{4zu};
	::fast_io::zu_forward_list zfl{5zu};
	::fast_io::zu_stack zs;
	zs.push(6zu);
	::fast_io::zu_queue zq;
	zq.push(7zu);
	::fast_io::zu_priority_queue zpq;
	zpq.push(8zu);
	::fast_io::zu_array<3> za{9zu, 0zu, 1zu};
	::fast_io::tlc::zu_vector tzv{1zu};
	if (zv[0] != 1 || zd[0] != 1 || zl.front() != 4 || zfl.front() != 5 || zs.top() != 6 ||
		zq.front() != 7 || zpq.top() != 8 || za[0] != 9 || tzv[0] != 1)
	{
		::fast_io::fast_terminate();
	}
}
