#include<fast_io.h>
#include<fast_io_dsal/vector.h>

using throwing_allocator = ::fast_io::generic_allocator_adapter<
	::fast_io::c_malloc_allocator,
	::fast_io::allocator_adapter_flags::throws_on_violations |
		::fast_io::allocator_adapter_flags::throws_on_allocation_failure>;

int main()
try
{
	::fast_io::vector<::std::size_t, throwing_allocator> v(40);
	v[40]=50;
}
catch throws(::std::error e)
{
	using namespace ::fast_io::iomnp;
	perrln(e);
	return 1;
}
