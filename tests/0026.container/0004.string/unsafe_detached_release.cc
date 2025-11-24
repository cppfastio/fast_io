#include <cstdlib>
#include <fast_io_dsal/string.h>

int main()
{
	fast_io::basic_string<char, ::fast_io::generic_allocator_adapter<::fast_io::c_malloc_allocator>> str("hello world");
	auto ptr = str.unsafe_detached_release();
    ::std::free(ptr);
    str.append("hello world");
    if (str != "hello world") [[unlikely]] {
        ::fast_io::fast_terminate();
    }
}
