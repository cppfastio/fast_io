#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <deque>

namespace
{

inline void test_size_empty()
{
	::fast_io::io::perr("=== deque size() / empty() test ===\n");

	// Empty deque
	{
		::fast_io::deque<::std::size_t> dq;
		if (dq.size() != 0u)
		{
			::fast_io::io::panic("default constructed deque size should be 0\n");
		}
		if (!dq.is_empty())
		{
			::fast_io::io::panic("default constructed deque should be empty\n");
		}
		if (!dq.is_empty())
		{
			::fast_io::io::panic("default constructed deque should be is_empty\n");
		}
	}

	// Non-empty deque
	{
		::fast_io::deque<::std::size_t> dq;
		for (::std::size_t i{}; i != 4096u; ++i)
		{
			dq.push_back(i);
		}

		if (dq.size() != 4096u)
		{
			::fast_io::io::panic("size should be 4096\n");
		}
		if (dq.is_empty())
		{
			::fast_io::io::panic("deque with elements should not be empty\n");
		}
	}

	// After clear
	{
		::fast_io::deque<::std::size_t> dq;
		for (::std::size_t i{}; i != 4096u; ++i)
		{
			dq.push_back(i);
		}
		dq.clear();
		if (dq.size() != 0u)
		{
			::fast_io::io::panic("size should be 0 after clear\n");
		}
		if (!dq.is_empty())
		{
			::fast_io::io::panic("deque should be empty after clear\n");
		}
	}

	::fast_io::io::print("deque size() / empty() test finished\n");
}

inline void test_max_size()
{
	::fast_io::io::perr("=== deque max_size() test ===\n");

	::fast_io::deque<::std::size_t> dq;
	::fast_io::deque<char> dq_char;

	// max_size should be very large (at least millions)
	if (dq.max_size() < 1000000u)
	{
		::fast_io::io::panicln("max_size for size_t too small: ", dq.max_size());
	}

	if (dq_char.max_size() < 1000000u)
	{
		::fast_io::io::panicln("max_size for char too small: ", dq_char.max_size());
	}

	// max_size and max_size_bytes should exist
	if (dq.max_size_bytes() < 1000000u)
	{
		::fast_io::io::panicln("max_size_bytes too small: ", dq.max_size_bytes());
	}

	::fast_io::io::print("deque max_size() test finished\n");
}

inline void test_shrink_to_fit()
{
	::fast_io::io::perr("=== deque shrink_to_fit() test ===\n");

	// Test sizes at and around block boundaries
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

		dq.shrink_to_fit();

		// After shrink_to_fit, elements must be preserved
		if (dq.size() != ref.size())
		{
			::fast_io::io::panicln("size mismatch after shrink_to_fit for n=", n);
		}

		for (::std::size_t i{}; i != dq.size(); ++i)
		{
			if (dq[i] != ref[i])
			{
				::fast_io::io::panicln("value mismatch at ", i, " after shrink_to_fit for n=", n);
			}
		}
	}

	// Shrink empty deque
	{
		::fast_io::deque<::std::size_t> dq;
		dq.shrink_to_fit();
		if (!dq.is_empty())
		{
			::fast_io::io::panic("empty deque should stay empty after shrink_to_fit\n");
		}
	}

	::fast_io::io::print("deque shrink_to_fit() test finished\n");
}

inline void test_size_bytes()
{
	::fast_io::io::perr("=== deque size_bytes() test ===\n");

	::fast_io::deque<::std::size_t> dq;
	for (::std::size_t i{}; i != 100u; ++i)
	{
		dq.push_back(i);
	}

	if (dq.size_bytes() != dq.size() * sizeof(::std::size_t))
	{
		::fast_io::io::panic("size_bytes mismatch\n");
	}

	::fast_io::io::print("deque size_bytes() test finished\n");
}

} // namespace

int main()
{
	test_size_empty();
	test_max_size();
	test_shrink_to_fit();
	test_size_bytes();
}
