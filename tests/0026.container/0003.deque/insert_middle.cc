#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <deque>

namespace
{

inline void test_insert_middle()
{
	::fast_io::io::perr("=== deque insert middle test ===\n");

	::fast_io::deque<::std::size_t> dq;
	::std::deque<::std::size_t> ref;

	// Add some elements
	for (::std::size_t i = 0; i < 100; ++i)
	{
		dq.push_back(i);
		ref.push_back(i);
	}

	auto check_equal = [&](auto const &msg,
						   ::std::source_location src = ::std::source_location::current()) {
		if (dq.size() != ref.size())
		{
			::fast_io::io::panicln(src, "\tERROR: size mismatch: ", ::fast_io::mnp::os_c_str(msg));
		}
		for (::std::size_t i = 0; i < dq.size(); ++i)
		{
			if (dq[i] != ref[i])
			{
				::fast_io::io::panicln(src, "\tERROR: value mismatch at ", i, " dq=", dq[i], " ref=", ref[i], " : ", ::fast_io::mnp::os_c_str(msg));
			}
		}
	};

	// Insert at position 50 (middle)
	dq.insert_index(50, 999);
	ref.insert(ref.begin() + 50, 999);
	check_equal("insert at middle");

	// Insert at position 0 (front)
	dq.insert_index(0, 888);
	ref.insert(ref.begin(), 888);
	check_equal("insert at front");

	// Insert at end
	dq.insert_index(dq.size(), 777);
	ref.insert(ref.end(), 777);
	check_equal("insert at end");

	// Randomized insertions
	for (::std::size_t iter = 0; iter < 200; ++iter)
	{
		::std::size_t pos = iter % (dq.size() + 1);
		dq.insert_index(pos, iter + 1000);
		ref.insert(ref.begin() + pos, iter + 1000);
		check_equal("randomized insert");
	}

	::fast_io::io::print("deque insert middle test finished\n");
}

inline void test_insert_count_value()
{
	::fast_io::io::perr("=== deque insert(count, value) test ===\n");

	using T = std::size_t;
	::fast_io::deque<T> dq;
	std::deque<T> ref;

	// Fill initial data
	for (std::size_t i{}; i != 200u; ++i)
	{
		dq.push_back(i);
		ref.push_back(i);
	}

	auto check_equal = [&](auto const &msg,
						   std::source_location src = std::source_location::current()) {
		if (dq.size() != ref.size())
		{
			::fast_io::io::panicln(src, "\tERROR: size mismatch: ", ::fast_io::mnp::os_c_str(msg));
		}
		for (std::size_t i{}; i != dq.size(); ++i)
		{
			if (dq[i] != ref[i])
			{
				::fast_io::io::panicln(src,
									   "\tERROR: value mismatch at index ", i,
									   "\tdq[i]=", dq[i], "\tref[i]=", ref[i],
									   " : ", ::fast_io::mnp::os_c_str(msg));
			}
		}
	};

	// 1. Insert at front
	{
		dq.insert(dq.begin(), 5, 9999);
		ref.insert(ref.begin(), 5, 9999);
		check_equal("insert(count,val) at front");
	}

	// 2. Insert at middle
	{
		std::size_t pos = dq.size() / 2;
		dq.insert(dq.begin() + pos, 3, 7777);
		ref.insert(ref.begin() + pos, 3, 7777);
		check_equal("insert(count,val) at middle");
	}

	// 3. Insert at back
	{
		dq.insert(dq.end(), 4, 5555);
		ref.insert(ref.end(), 4, 5555);
		check_equal("insert(count,val) at back");
	}

	// 4. Randomized insertions
	for (std::size_t iter{}; iter != 200u; ++iter)
	{
		std::size_t pos = iter % (dq.size() + 1);
		std::size_t count = (iter % 4) + 1;
		std::size_t val = iter + 3000;

		dq.insert(dq.begin() + pos, count, val);
		ref.insert(ref.begin() + pos, count, val);

		check_equal("randomized insert(count,val)");
	}

	::fast_io::io::print("deque insert(count,val) test finished\n");
}

inline void test_insert_index_count_value()
{
	::fast_io::io::perr("=== deque insert_index(count, value) test ===\n");

	using T = std::size_t;
	::fast_io::deque<T> dq;
	std::deque<T> ref;

	// Fill initial data
	for (std::size_t i{}; i != 200u; ++i)
	{
		dq.push_back(i);
		ref.push_back(i);
	}

	auto check_equal = [&](auto const &msg,
						   std::source_location src = std::source_location::current()) {
		if (dq.size() != ref.size())
		{
			::fast_io::io::panicln(src, "\tERROR: size mismatch: ", ::fast_io::mnp::os_c_str(msg));
		}
		for (std::size_t i{}; i != dq.size(); ++i)
		{
			if (dq[i] != ref[i])
			{
				::fast_io::io::panicln(src,
									   "\tERROR: value mismatch at index ", i,
									   "\tdq[i]=", dq[i], "\tref[i]=", ref[i],
									   " : ", ::fast_io::mnp::os_c_str(msg));
			}
		}
	};

	// 1. Insert at front
	{
		dq.insert_index(0, 5, 9999);
		ref.insert(ref.begin(), 5, 9999);
		check_equal("insert_index(count,val) at front");
	}

	// 2. Insert at middle
	{
		std::size_t pos = dq.size() / 2;
		dq.insert_index(pos, 3, 7777);
		ref.insert(ref.begin() + pos, 3, 7777);
		check_equal("insert_index(count,val) at middle");
	}

	// 3. Insert at back
	{
		std::size_t pos = dq.size();
		dq.insert_index(pos, 4, 5555);
		ref.insert(ref.end(), 4, 5555);
		check_equal("insert_index(count,val) at back");
	}

	// 4. Randomized insertions
	for (std::size_t iter{}; iter != 200u; ++iter)
	{
		std::size_t pos = iter % (dq.size() + 1);
		std::size_t count = (iter % 4) + 1;
		std::size_t val = iter + 4000;

		dq.insert_index(pos, count, val);
		ref.insert(ref.begin() + pos, count, val);

		check_equal("randomized insert_index(count,val)");
	}

	::fast_io::io::print("deque insert_index(count,val) test finished\n");
}

} // namespace

int main()
{
	test_insert_middle();
	test_insert_count_value();
	test_insert_index_count_value();
}