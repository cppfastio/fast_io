#include <fast_io.h>

int main() noexcept
{
	void *p = ::fast_io::c_malloc_allocator::allocate_aligned(16, 1024);
	::fast_io::c_malloc_allocator::deallocate_aligned(p, 16);
}
