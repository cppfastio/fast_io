#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <deque>
#include <vector>

namespace
{

inline void test_erase()
{
	::fast_io::io::perr("=== deque erase test ===\n");

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
	auto check_equal = [&](auto const &msg,
						   ::std::source_location src = ::std::source_location::current()) {
		if (dq.size() != ref.size())
		{
			::fast_io::io::panicln(src, "\tERROR: size mismatch: ", ::fast_io::mnp::os_c_str(msg));
		}
		for (::std::size_t i{}; i != dq.size(); ++i)
		{
			if (dq[i] != ref[i])
			{
				::fast_io::io::panicln(src,
									   "\tERROR: value mismatch at index ",
									   i,
									   "\tdq[i]=",
									   dq[i],
									   "\tref[i]=",
									   ref[i],
									   " : ",
									   ::fast_io::mnp::os_c_str(msg));
			}
		}
	};

	// 1. Erase single element at front
	{
		dq.erase(dq.begin());
		ref.erase(ref.begin());
		check_equal("erase single at front");
	}

	// 2. Erase single element in the middle
	{
		if (!dq.is_empty())
		{
			::std::size_t pos{dq.size() / 2};
			dq.erase(dq.begin() + pos);
			ref.erase(ref.begin() + pos);
			check_equal("erase single in middle");
		}
	}

	// 3. Erase single element at back (last element)
	{
		if (!dq.is_empty())
		{
			::std::size_t pos{dq.size() - 1};
			dq.erase(dq.begin() + pos);
			ref.erase(ref.begin() + pos);
			check_equal("erase single at back");
		}
	}

	// 4. Erase a range at the front
	{
		if (dq.size() >= 10u)
		{
			dq.erase(dq.begin(), dq.begin() + 10);
			ref.erase(ref.begin(), ref.begin() + 10);
			check_equal("erase range at front");
		}
	}

	// 5. Erase a range in the middle
	{
		if (dq.size() >= 30u)
		{
			::std::size_t start{dq.size() / 3};
			::std::size_t len{20u};
			if (start + len > dq.size())
			{
				len = dq.size() - start;
			}

			dq.erase(dq.begin() + start, dq.begin() + start + len);
			ref.erase(ref.begin() + start, ref.begin() + start + len);
			check_equal("erase range in middle");
		}
	}

	// 6. Erase a range at the back
	{
		if (dq.size() >= 15u)
		{
			::std::size_t len{15u};
			::std::size_t start{dq.size() - len};

			dq.erase(dq.begin() + start, dq.end());
			ref.erase(ref.begin() + start, ref.end());
			check_equal("erase range at back");
		}
	}

	// 7. Randomized erases: erase random ranges until we have done enough iterations
	for (::std::size_t iter{}; iter != 200u; ++iter)
	{
		if (dq.is_empty())
		{
			break;
		}

		::std::size_t current_size{dq.size()};
		::std::size_t pos{iter % current_size};

		// Choose a small length depending on iter, clamped to the remaining size
		::std::size_t max_len{5u};
		::std::size_t len{(iter * 7u) % max_len + 1u};
		if (pos + len > current_size)
		{
			len = current_size - pos;
		}

		auto dq_first = dq.begin() + pos;
		auto dq_last = dq.begin() + pos + len;

		auto ref_first = ref.begin() + pos;
		auto ref_last = ref.begin() + pos + len;

		dq.erase(dq_first, dq_last);
		ref.erase(ref_first, ref_last);

		check_equal("randomized erase");
	}

	::fast_io::io::print("deque erase test finished\n");
}

inline void test_erase_index()
{
	::fast_io::io::perr("=== deque erase_index test ===\n");

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
	auto check_equal = [&](auto const &msg,
						   ::std::source_location src = ::std::source_location::current()) {
		if (dq.size() != ref.size())
		{
			::fast_io::io::panicln(src, "\tERROR: size mismatch: ", ::fast_io::mnp::os_c_str(msg));
		}
		for (::std::size_t i{}; i != dq.size(); ++i)
		{
			if (dq[i] != ref[i])
			{
				::fast_io::io::panicln(src,
									   "\tERROR: value mismatch at index ",
									   i,
									   "\tdq[i]=", dq[i],
									   "\tref[i]=", ref[i],
									   " : ", ::fast_io::mnp::os_c_str(msg));
			}
		}
	};

	//
	// === 1. Single‑index erase ===
	//

	// Erase at front
	{
		dq.erase_index(0);
		ref.erase(ref.begin());
		check_equal("erase_index(pos) front");
	}

	// Erase in middle
	{
		if (!dq.is_empty())
		{
			::std::size_t pos{dq.size() / 2};
			dq.erase_index(pos);
			ref.erase(ref.begin() + pos);
			check_equal("erase_index(pos) middle");
		}
	}

	// Erase at back
	{
		if (!dq.is_empty())
		{
			::std::size_t pos{dq.size() - 1};
			dq.erase_index(pos);
			ref.erase(ref.begin() + pos);
			check_equal("erase_index(pos) back");
		}
	}

	//
	// === 2. Range erase ===
	//

	// Erase range at front
	{
		if (dq.size() >= 10u)
		{
			dq.erase_index(0, 10);
			ref.erase(ref.begin(), ref.begin() + 10);
			check_equal("erase_index(first,last) front");
		}
	}

	// Erase range in middle
	{
		if (dq.size() >= 30u)
		{
			::std::size_t start{dq.size() / 3};
			::std::size_t len{20u};
			if (start + len > dq.size())
			{
				len = dq.size() - start;
			}

			dq.erase_index(start, start + len);
			ref.erase(ref.begin() + start, ref.begin() + start + len);
			check_equal("erase_index(first,last) middle");
		}
	}

	// Erase range at back
	{
		if (dq.size() >= 15u)
		{
			::std::size_t len{15u};
			::std::size_t start{dq.size() - len};

			dq.erase_index(start, dq.size());
			ref.erase(ref.begin() + start, ref.end());
			check_equal("erase_index(first,last) back");
		}
	}

	//
	// === 3. Randomized single‑index erase ===
	//
	for (::std::size_t iter{}; iter != 200u; ++iter)
	{
		if (dq.is_empty())
		{
			break;
		}

		::std::size_t pos = iter % dq.size();

		dq.erase_index(pos);
		ref.erase(ref.begin() + pos);

		check_equal("randomized erase_index(pos)");
	}

	//
	// === 4. Randomized range erase ===
	//
	for (::std::size_t iter{}; iter != 200u; ++iter)
	{
		if (dq.is_empty())
		{
			break;
		}

		::std::size_t n = dq.size();
		::std::size_t first = (iter * 7) % n;
		::std::size_t last = first + (iter % 5 + 1);

		if (last > n)
		{
			last = n;
		}

		dq.erase_index(first, last);
		ref.erase(ref.begin() + first, ref.begin() + last);

		check_equal("randomized erase_index(first,last)");
	}

	::fast_io::io::print("deque erase_index test finished\n");
}

inline void test_erase_value()
{
	::fast_io::io::perr("=== deque erase(value) test ===\n");

	using T = ::std::size_t;
	::fast_io::deque<T> dq;
	::std::deque<T> ref;

	// Fill initial data with repeated patterns
	for (::std::size_t i{}; i != 300u; ++i)
	{
		dq.push_back(i % 10);
		ref.push_back(i % 10);
	}

	auto check_equal = [&](auto const &msg,
						   ::std::source_location src = ::std::source_location::current()) {
		if (dq.size() != ref.size())
		{
			::fast_io::io::panicln(src, "size mismatch: ", ::fast_io::mnp::os_c_str(msg));
		}

		for (::std::size_t i{}; i != dq.size(); ++i)
		{
			if (dq[i] != ref[i])
			{
				::fast_io::io::panicln(src, "value mismatch at ", i, " : ", ::fast_io::mnp::os_c_str(msg));
			}
		}
	};

	// 1. Erase a value that appears many times
	{
		auto r1 = erase(dq, static_cast<T>(3));
		auto r2 = std::erase(ref, static_cast<T>(3));
		if (r1 != r2)
		{
			::fast_io::io::panic("erase(value) count mismatch\n");
		}
		check_equal("erase(value) many occurrences");
	}

	// 2. Erase a value that appears once
	{
		dq.push_back(12345);
		ref.push_back(12345);

		auto r1 = erase(dq, static_cast<T>(12345));
		auto r2 = std::erase(ref, static_cast<T>(12345));
		if (r1 != r2)
		{
			::fast_io::io::panic("erase(value) count mismatch (single)\n");
		}
		check_equal("erase(value) single occurrence");
	}

	// 3. Erase a value that does not exist
	{
		auto r1 = erase(dq, static_cast<T>(99999));
		auto r2 = std::erase(ref, static_cast<T>(99999));
		if (r1 != r2)
		{
			::fast_io::io::panic("erase(value) count mismatch (none)\n");
		}
		check_equal("erase(value) none");
	}

	// 4. Randomized erase(value)
	for (std::size_t iter{}; iter != 200u; ++iter)
	{
		if (dq.is_empty())
		{
			break;
		}

		std::size_t val = iter % 15;

		auto r1 = erase(dq, static_cast<T>(val));
		auto r2 = std::erase(ref, static_cast<T>(val));

		if (r1 != r2)
		{
			::fast_io::io::panic("erase(value) randomized count mismatch\n");
		}

		check_equal("erase(value) randomized");
	}

	::fast_io::io::print("deque erase(value) test finished\n");
}

inline void test_erase_if()
{
	::fast_io::io::perr("=== deque erase_if(pred) test ===\n");

	using T = ::std::size_t;
	::fast_io::deque<T> dq;
	::std::deque<T> ref;

	// Fill initial data
	for (::std::size_t i{}; i != 300u; ++i)
	{
		dq.push_back(i);
		ref.push_back(i);
	}

	auto check_equal = [&](auto const &msg,
						   ::std::source_location src = ::std::source_location::current()) {
		if (dq.size() != ref.size())
		{
			::fast_io::io::panicln(src, "size mismatch: ", ::fast_io::mnp::os_c_str(msg));
		}

		for (::std::size_t i{}; i != dq.size(); ++i)
		{
			if (dq[i] != ref[i])
			{
				::fast_io::io::panicln(src, "value mismatch at ", i, " : ", ::fast_io::mnp::os_c_str(msg));
			}
		}
	};

	// 1. Remove even numbers
	{
		auto pred = [](T x) { return (x % 2) == 0; };

		auto r1 = erase_if(dq, pred);
		auto r2 = std::erase_if(ref, pred);

		if (r1 != r2)
		{
			::fast_io::io::panic("erase_if(pred) count mismatch (even)\n");
		}

		check_equal("erase_if even");
	}

	// 2. Remove numbers divisible by 3
	{
		auto pred = [](T x) { return (x % 3) == 0; };

		auto r1 = erase_if(dq, pred);
		auto r2 = std::erase_if(ref, pred);

		if (r1 != r2)
		{
			::fast_io::io::panic("erase_if(pred) count mismatch (div3)\n");
		}

		check_equal("erase_if divisible by 3");
	}

	// 3. Remove nothing
	{
		auto pred = [](T x) { return x == static_cast<T>(999999); };

		auto r1 = erase_if(dq, pred);
		auto r2 = std::erase_if(ref, pred);

		if (r1 != r2)
		{
			::fast_io::io::panic("erase_if(pred) count mismatch (none)\n");
		}

		check_equal("erase_if none");
	}

	// 4. Randomized erase_if
	for (std::size_t iter{}; iter != 200u; ++iter)
	{
		if (dq.is_empty())
		{
			break;
		}

		std::size_t mod = (iter % 7) + 2;

		auto pred = [&](T x) { return (x % mod) == 1; };

		auto r1 = erase_if(dq, pred);
		auto r2 = std::erase_if(ref, pred);

		if (r1 != r2)
		{
			::fast_io::io::panic("erase_if(pred) randomized count mismatch\n");
		}

		check_equal("erase_if randomized");
	}

	::fast_io::io::print("deque erase_if(pred) test finished\n");
}

} // namespace

int main()
{
	test_erase();
	test_erase_index();
	test_erase_value();
	test_erase_if();
}
