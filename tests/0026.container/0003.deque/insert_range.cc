#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <deque>
#include <vector>

namespace
{

inline void test_iterator_ops()
{
	::fast_io::io::perr("=== deque iterator test ===\n");

	using T = std::size_t;
	::fast_io::deque<T> dq;
	std::deque<T> ref;

	// Fill with known values
	for (std::size_t i{}; i != 300u; ++i)
	{
		dq.push_back(i * 10);
		ref.push_back(i * 10);
	}

	auto check_equal = [&](auto const &msg, ::std::source_location src = ::std::source_location::current()) {
		if (dq.size() != ref.size())
		{
			::fast_io::io::panicln(src, "\tsize mismatch: ", ::fast_io::mnp::os_c_str(msg));
		}

		for (std::size_t i{}; i != dq.size(); ++i)
		{
			if (*(dq.begin() + i) != ref[i])
			{
				::fast_io::io::panicln(src, "\tvalue mismatch at index ", i);
			}
		}
	};

	// 1. Forward iteration
	{
		auto it = dq.begin();
		for (std::size_t i{}; i != dq.size(); ++i, ++it)
		{
			if (*it != ref[i])
			{
				::fast_io::io::panicln("forward iteration mismatch at ", i);
			}
		}
	}

	// 2. Backward iteration
	{
		auto it = dq.end();
		for (std::size_t i{dq.size()}; i != 0;)
		{
			--i;
			--it;
			if (*it != ref[i])
			{
				::fast_io::io::panicln("backward iteration mismatch at ", i);
			}
		}
	}

	// 3. Random-access jumps
	{
		auto it = dq.begin();
		it += 100;
		if (*it != ref[100])
		{
			::fast_io::io::panic("+= jump mismatch\n");
		}

		it -= 50;
		if (*it != ref[50])
		{
			::fast_io::io::panic("-= jump mismatch\n");
		}

		auto it2 = it + 25;
		if (*it2 != ref[75])
		{
			::fast_io::io::panic("it + n mismatch\n");
		}

		auto it3 = it2 - 30;
		if (*it3 != ref[45])
		{
			::fast_io::io::panic("it - n mismatch\n");
		}

		std::ptrdiff_t diff = it2 - it3;
		if (diff != 30)
		{
			::fast_io::io::panic("iterator difference mismatch\n");
		}
	}

	// 4. Comparison operators (<=> and == only)
	{
		auto a = dq.begin() + 10;
		auto b = dq.begin() + 20;

		// Test <=> ordering
		auto cmp = (a <=> b);
		if (!(cmp < 0))
		{
			::fast_io::io::panic("a <=> b should be negative\n");
		}

		auto cmp2 = (b <=> a);
		if (!(cmp2 > 0))
		{
			::fast_io::io::panic("b <=> a should be positive\n");
		}

		auto cmp3 = (a <=> a);
		if (!(cmp3 == 0))
		{
			::fast_io::io::panic("a <=> a should be zero\n");
		}

		// Test equality
		if (!(a == a))
		{
			::fast_io::io::panic("a == a failed\n");
		}

		if (a == b)
		{
			::fast_io::io::panic("a == b incorrect\n");
		}

		if (!(a != b))
		{
			::fast_io::io::panic("a != b failed\n");
		}
	}


	// 5. Indexing operator it[n]
	{
		auto it = dq.begin() + 50;
		for (std::size_t k{}; k != 20; ++k)
		{
			if (it[k] != ref[50 + k])
			{
				::fast_io::io::panicln("iterator indexing mismatch at offset ", k);
			}
		}
	}

	// 6. const_iterator compatibility
	{
		auto const &cdq = dq;
		auto cit = cdq.begin();
		auto it = dq.begin();

		if (*cit != *it)
		{
			::fast_io::io::panic("const_iterator deref mismatch\n");
		}

		cit += 123;
		it += 123;

		if (*cit != *it)
		{
			::fast_io::io::panic("const_iterator += mismatch\n");
		}

		if ((cit - cdq.begin()) != 123)
		{
			::fast_io::io::panic("const_iterator difference mismatch\n");
		}
	}

	check_equal("iterator ops");

	::fast_io::io::print("deque iterator test finished\n");
}

inline void test_insert_range_index()
{
	::fast_io::io::perr("=== deque insert_range_index test ===\n");

	using T = ::std::size_t;
	::fast_io::deque<T> dq;
	::std::deque<T> ref;

	// Fill initial data
	for (::std::size_t i{}; i != 200u; ++i)
	{
		dq.push_back(i);
		ref.push_back(i);
	}

	// Helper to compare dq and ref
	auto check_equal = [&](auto const &msg, ::std::source_location src = ::std::source_location::current()) {
		if (dq.size() != ref.size())
		{
			::fast_io::io::panicln(src, "\tERROR: size mismatch: ", ::fast_io::mnp::os_c_str(msg));
		}
		for (::std::size_t i{}; i != dq.size(); ++i)
		{
			if (dq[i] != ref[i])
			{
				::fast_io::io::panicln(src, "\tERROR: value mismatch at index ", i, "\tdq[i]=", dq[i], "\tref[i]=", ref[i], " : ", ::fast_io::mnp::os_c_str(msg));
			}
		}
	};

	// Test ranges
	::std::vector<T> rg1{1000, 1001, 1002, 1003, 1004};
	::std::vector<T> rg2{2000, 2001, 2002};
	::std::vector<T> rg3{3000};

	// Insert at front
	{
		dq.insert_range_index(0, rg1);
		ref.insert(ref.begin(), rg1.begin(), rg1.end());
		check_equal("insert at front");
	}

	// Insert at middle
	{
		::std::size_t pos{dq.size() / 2};
		dq.insert_range_index(pos, rg2);
		ref.insert(ref.begin() + pos, rg2.begin(), rg2.end());
		check_equal("insert at middle");
	}

	// Insert at back
	{
		::std::size_t pos{dq.size()};
		dq.insert_range_index(pos, rg3);
		ref.insert(ref.end(), rg3.begin(), rg3.end());
		check_equal("insert at back");
	}

	// Randomized insertions
	for (::std::size_t iter{}; iter != 200u; ++iter)
	{
		::std::size_t pos = iter % (dq.size() + 1);
		::std::vector<T> rg{iter + 5000, iter + 5001};

		dq.insert_range_index(pos, rg);
		ref.insert(ref.begin() + pos, rg.begin(), rg.end());

		check_equal("randomized insert");
	}

	::fast_io::io::print("deque insert_range_index test finished\n");
}
} // namespace

int main()
{
	test_iterator_ops();
#if 1
	test_insert_range_index();
#endif
}