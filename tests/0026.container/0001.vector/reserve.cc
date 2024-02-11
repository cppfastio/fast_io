#include <cstddef>
#include <new>
#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

// minimal allocator with debug output
struct NAlloc
{
	static void *allocate(std::size_t n)
	{
		void *p = ::operator new(n);
		println("allocating ", n, " bytes @ ", pointervw(p));
		return p;
	}
	static void deallocate_n(void *p, std::size_t n)
	{
		print("deallocating ", n, " bytes @ ", pointervw(p), "\n\n");
		::operator delete(p);
	}
	constexpr bool operator==(NAlloc const &) const noexcept = default;
};

int main()
{
	constexpr int max_elements = 32;

	print("using reserve: \n");
	{
		fast_io::vector<int, fast_io::generic_allocator_adapter<NAlloc>> v1;
		v1.reserve(max_elements); // reserves at least max_elements * sizeof(int) bytes

		for (int n = 0; n < max_elements; ++n)
		{
			v1.push_back(n);
		}
	}

	print("not using reserve: \n");
	{
		fast_io::vector<int, fast_io::generic_allocator_adapter<NAlloc>> v1;

		for (int n = 0; n < max_elements; ++n)
		{
			if (v1.size() == v1.capacity())
			{
				println("size() == capacity() == ", v1.size());
			}
			v1.push_back(n);
		}
	}
}