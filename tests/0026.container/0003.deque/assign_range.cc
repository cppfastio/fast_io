#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <fast_io_dsal/vector.h>
#include <deque>
#include <vector>

namespace
{

inline void test_assign_count_value()
{
	::fast_io::io::perr("=== deque assign(count, value) test ===\n");

	{
		::fast_io::deque<::std::size_t> dq;
		::std::deque<::std::size_t> ref;

		for (::std::size_t i{}; i != 100u; ++i)
		{
			dq.push_back(i);
			ref.push_back(i);
		}

		dq.assign(50u, 777u);
		ref.assign(50, 777);

		if (dq.size() != ref.size())
		{
			::fast_io::io::panic("size mismatch after assign(count, value)\n");
		}

		for (::std::size_t i{}; i != dq.size(); ++i)
		{
			if (dq[i] != ref[i])
			{
				::fast_io::io::panic("value mismatch at ", i, " after assign(count, value)\n");
			}
		}
	}

	// Assign zero count
	{
		::fast_io::deque<::std::size_t> dq;
		for (::std::size_t i{}; i != 100u; ++i)
		{
			dq.push_back(i);
		}

		dq.assign(0u, 999u);
		if (!dq.is_empty())
		{
			::fast_io::io::panic("deque should be empty after assign(0, val)\n");
		}
	}

	// Test sizes at and around block boundaries
	{
		::std::size_t const sizes[] = {0, 1, 2, 3, 1023, 1024, 1025, 2047, 2048, 2049};

		for (::std::size_t n : sizes)
		{
			::fast_io::deque<::std::size_t> dq;
			::std::deque<::std::size_t> ref;

			for (::std::size_t i{}; i != n; ++i)
			{
				dq.push_back(i);
				ref.push_back(i);
			}

			dq.assign(0u, 0u);
			ref.assign(0, 0);

			if (dq.size() != ref.size())
			{
				::fast_io::io::panicln("size mismatch after assign to empty for n=", n);
			}
		}
	}

	::fast_io::io::print("deque assign(count, value) test finished\n");
}

inline void test_assign_range()
{
	::fast_io::io::perr("=== deque assign_range test ===\n");

	{
		::fast_io::deque<::std::size_t> dq;
		::std::deque<::std::size_t> ref;

		for (::std::size_t i{}; i != 100u; ++i)
		{
			dq.push_back(i);
			ref.push_back(i);
		}

		::fast_io::vector<::std::size_t> src;
		for (::std::size_t i{}; i != 50u; ++i)
		{
			src.push_back(i + 2000u);
		}

		dq.assign_range(src);
		::std::vector<::std::size_t> std_src(src.begin(), src.end());
		ref.assign(std_src.begin(), std_src.end());

		if (dq.size() != ref.size())
		{
			::fast_io::io::panic("size mismatch after assign_range\n");
		}

		for (::std::size_t i{}; i != dq.size(); ++i)
		{
			if (dq[i] != ref[i])
			{
				::fast_io::io::panic("value mismatch at ", i, " after assign_range\n");
			}
		}
	}

	::fast_io::io::print("deque assign_range test finished\n");
}

} // namespace

int main()
{
	test_assign_count_value();
	test_assign_range();
}
