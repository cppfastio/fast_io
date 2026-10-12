#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <deque>
#include <vector>
#include <array>

namespace
{

// fast_io::deque does not have insert(const_iterator, InputIterator, InputIterator).
// This test verifies insert_range with various range types, which is the equivalent.

inline void test_insert_range_from_array()
{
	::fast_io::io::perr("=== deque insert_range test ===\n");

	// Insert from std::vector range
	{
		::fast_io::deque<::std::size_t> dq;
		::std::deque<::std::size_t> ref;

		for (::std::size_t i{}; i != 100u; ++i)
		{
			dq.push_back(i);
			ref.push_back(i);
		}

		::std::vector<::std::size_t> src{200u, 201u, 202u, 203u, 204u};

		dq.insert_range(dq.cbegin() + 50, src);
		ref.insert(ref.cbegin() + 50, src.begin(), src.end());

		if (dq.size() != ref.size())
		{
			::fast_io::io::panic("size mismatch after insert_range from vector\n");
		}

		for (::std::size_t i{}; i != dq.size(); ++i)
		{
			if (dq[i] != ref[i])
			{
				::fast_io::io::panic("value mismatch at ", i, " after insert_range from vector\n");
			}
		}
	}

	// Insert at front
	{
		::fast_io::deque<::std::size_t> dq;
		::std::deque<::std::size_t> ref;

		for (::std::size_t i{}; i != 100u; ++i)
		{
			dq.push_back(i);
			ref.push_back(i);
		}

		::std::vector<::std::size_t> src{1000u, 1001u, 1002u};

		dq.insert_range(dq.cbegin(), src);
		ref.insert(ref.cbegin(), src.begin(), src.end());

		if (dq.size() != ref.size())
		{
			::fast_io::io::panic("size mismatch after insert_range at front\n");
		}

		for (::std::size_t i{}; i != dq.size(); ++i)
		{
			if (dq[i] != ref[i])
			{
				::fast_io::io::panic("value mismatch at ", i, " after insert_range at front\n");
			}
		}
	}

	// Insert at end
	{
		::fast_io::deque<::std::size_t> dq;
		::std::deque<::std::size_t> ref;

		for (::std::size_t i{}; i != 100u; ++i)
		{
			dq.push_back(i);
			ref.push_back(i);
		}

		::std::vector<::std::size_t> src{2000u, 2001u, 2002u, 2003u};

		dq.insert_range(dq.cend(), src);
		ref.insert(ref.cend(), src.begin(), src.end());

		if (dq.size() != ref.size())
		{
			::fast_io::io::panic("size mismatch after insert_range at end\n");
		}

		for (::std::size_t i{}; i != dq.size(); ++i)
		{
			if (dq[i] != ref[i])
			{
				::fast_io::io::panic("value mismatch at ", i, " after insert_range at end\n");
			}
		}
	}

	// Insert with various sizes around block boundaries
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

			::std::vector<::std::size_t> src{777u, 888u, 999u};
			::std::size_t pos = n / 2;

			dq.insert_range(dq.cbegin() + pos, src);
			ref.insert(ref.cbegin() + pos, src.begin(), src.end());

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
	}

	// Insert empty range
	{
		::fast_io::deque<::std::size_t> dq;
		::std::deque<::std::size_t> ref;

		for (::std::size_t i{}; i != 50u; ++i)
		{
			dq.push_back(i);
			ref.push_back(i);
		}

		::std::vector<::std::size_t> empty_src;
		dq.insert_range(dq.cbegin() + 25, empty_src);
		ref.insert(ref.cbegin() + 25, empty_src.begin(), empty_src.end());

		if (dq.size() != ref.size())
		{
			::fast_io::io::panic("size mismatch after empty range insert\n");
		}
	}

	::fast_io::io::print("deque insert_range test finished\n");
}

} // namespace

int main()
{
	test_insert_range_from_array();
}
