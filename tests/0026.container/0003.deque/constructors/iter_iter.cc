#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <vector>

namespace
{

// fast_io::deque does not have an iterator-pair constructor.
// This test verifies that from_range construction works for vector ranges,
// which is the closest equivalent.
inline void test_from_range_with_vector()
{
	::fast_io::io::perr("=== deque from_range with vector range test ===\n");

	// Test from_range with vector
	{
		::std::vector<::std::size_t> vec;
		for (::std::size_t i{}; i != 4096u; ++i)
		{
			vec.push_back(i * 3u);
		}

		::fast_io::deque<::std::size_t> dq(::fast_io::freestanding::from_range, vec);

		if (dq.size() != vec.size())
		{
			::fast_io::io::panic("size mismatch with from_range vector\n");
		}

		for (::std::size_t i{}; i != dq.size(); ++i)
		{
			if (dq[i] != vec[i])
			{
				::fast_io::io::panic("value mismatch at ", i, " with from_range vector\n");
			}
		}
	}

	// Test from_range empty vector
	{
		::std::vector<::std::size_t> empty_vec;
		::fast_io::deque<::std::size_t> dq(::fast_io::freestanding::from_range, empty_vec);
		if (!dq.is_empty())
		{
			::fast_io::io::panic("deque from empty range should be empty\n");
		}
	}

	// Test from_range with initializer_list
	{
		::fast_io::deque<::std::size_t> dq(::fast_io::freestanding::from_range, ::std::initializer_list<::std::size_t>{1, 2, 3, 4, 5});

		if (dq.size() != 5u)
		{
			::fast_io::io::panic("size mismatch with from_range initializer_list\n");
		}

		::std::size_t expected{1u};
		for (auto const &e : dq)
		{
			if (e != expected)
			{
				::fast_io::io::panicln("value mismatch: expected ", expected, " got ", e);
			}
			++expected;
		}
	}

	::fast_io::io::print("deque from_range with vector range test finished\n");
}

} // namespace

int main()
{
	test_from_range_with_vector();
}
