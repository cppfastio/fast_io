#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <fast_io_dsal/string.h>
#include <fast_io_dsal/string_view.h>
#include <deque>
#include <string>

namespace
{

inline void test_n_val()
{
	::fast_io::io::perr("=== deque(n, val) constructor test ===\n");

	// Test sizes at and around block boundaries
	// For std::size_t (8 bytes), block_size = 4096/8 = 512
	// Test with int (4 bytes): block_size = 1024
	::std::size_t const sizes[] = {0, 1, 2, 3, 1023, 1024, 1025, 2047, 2048, 2049};

	for (::std::size_t n : sizes)
	{
		::fast_io::deque<::std::size_t> dq(n, 42u);
		::std::deque<::std::size_t> ref(n, 42u);

		if (dq.size() != ref.size())
		{
			::fast_io::io::panicln("size mismatch for n=", n);
		}

		for (::std::size_t i{}; i != dq.size(); ++i)
		{
			if (dq[i] != ref[i])
			{
				::fast_io::io::panicln("value mismatch at ", i, " for n=", n);
			}
		}
	}

	::fast_io::io::print("deque(n, val) constructor test finished\n");
}

inline void test_n_val_string()
{
	::fast_io::io::perr("=== deque(n, val) string test ===\n");

	::fast_io::string const hello("hello");
	::fast_io::deque<::fast_io::string> dq(100u, hello);

	if (dq.size() != 100u)
	{
		::fast_io::io::panic("size mismatch for string type\n");
	}

	for (::std::size_t i{}; i != dq.size(); ++i)
	{
		if (dq[i] != hello)
		{
			::fast_io::io::panic("value mismatch at ", i, " for string type\n");
		}
	}

	::fast_io::io::print("deque(n, val) string test finished\n");
}

} // namespace

int main()
{
	test_n_val();
	test_n_val_string();
}
