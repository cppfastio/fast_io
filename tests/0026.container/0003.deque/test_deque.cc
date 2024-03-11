#include<fast_io_dsal/deque.h>
#include<fast_io.h>

int main()
{
	::fast_io::deque<int> dq;
	dq.push_back(4);
	dq.push_back(6);
	for(auto const & e : dq)
	{
		::fast_io::io::println(e);
	}
}