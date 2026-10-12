#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <deque>
#include <vector>
#include <stdexcept>

namespace
{
inline constexpr ::std::size_t throw_threshold{50};

// Throwing type for exception safety testing - must be at namespace scope for static members
struct ThrowingType
{
	static ::std::size_t construct_count;

	::std::size_t value;

	ThrowingType() : value{0}
	{
		++construct_count;
		check_threshold();
	}

	ThrowingType(::std::size_t v) : value{v}
	{
		++construct_count;
		check_threshold();
	}

	ThrowingType(ThrowingType &&other) noexcept : value{other.value}
	{
		other.value = 0;
		// Move constructor is noexcept to avoid issues during relocation
	}

	ThrowingType(ThrowingType const &other) : value{other.value}
	{
		++construct_count;
		check_threshold();
	}

	ThrowingType &operator=(ThrowingType &&other) noexcept
	{
		value = other.value;
		other.value = 0;
		return *this;
	}

	ThrowingType &operator=(ThrowingType const &other)
	{
		value = other.value;
		return *this;
	}

	~ThrowingType() noexcept = default;

	static void check_threshold()
	{
		if (construct_count > throw_threshold)
		{
			throw ::std::runtime_error("construction threshold exceeded");
		}
	}

	static void reset() noexcept
	{
		construct_count = 0;
	}
};

::std::size_t ThrowingType::construct_count{0};

inline void test_insert_single()
{
	::fast_io::io::perr("=== deque insert single element test ===\n");

	ThrowingType::reset();

	using T = ::std::size_t;
	::fast_io::deque<T> dq;
	::std::deque<T> ref;

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

	// 1. Insert at front (empty deque)
	{
		dq.insert(dq.begin(), 1000);
		ref.insert(ref.begin(), 1000);
		check_equal("insert at front (empty)");
	}

	// 2. Insert at back
	{
		dq.insert(dq.end(), 1001);
		ref.insert(ref.end(), 1001);
		check_equal("insert at back");
	}

	// 3. Insert at front (non-empty)
	{
		dq.insert(dq.begin(), 999);
		ref.insert(ref.begin(), 999);
		check_equal("insert at front (non-empty)");
	}

	// 4. Insert in middle
	{
		::std::size_t pos{dq.size() / 2};
		dq.insert(dq.begin() + pos, 500);
		ref.insert(ref.begin() + pos, 500);
		check_equal("insert in middle");
	}

	// 5. Insert_index at front
	{
		dq.insert_index(0, 1);
		ref.insert(ref.begin(), 1);
		check_equal("insert_index at front");
	}

	// 6. Insert_index at back
	{
		::std::size_t pos{dq.size()};
		dq.insert_index(pos, 2);
		ref.insert(ref.end(), 2);
		check_equal("insert_index at back");
	}

	// 7. Insert_index in middle
	{
		::std::size_t pos{dq.size() / 2};
		dq.insert_index(pos, 3);
		ref.insert(ref.begin() + pos, 3);
		check_equal("insert_index in middle");
	}

	// 8. Emplace at front
	{
		dq.emplace(dq.begin(), 10);
		ref.emplace(ref.begin(), 10);
		check_equal("emplace at front");
	}

	// 9. Emplace at back
	{
		dq.emplace(dq.end(), 11);
		ref.emplace(ref.end(), 11);
		check_equal("emplace at back");
	}

	// 10. Emplace in middle
	{
		::std::size_t pos{dq.size() / 2};
		dq.emplace(dq.begin() + pos, 12);
		ref.emplace(ref.begin() + pos, 12);
		check_equal("emplace in middle");
	}

	// 11. Emplace_index at front
	{
		dq.emplace_index(0, 20);
		ref.emplace(ref.begin(), 20);
		check_equal("emplace_index at front");
	}

	// 12. Emplace_index at back
	{
		::std::size_t pos{dq.size()};
		dq.emplace_index(pos, 21);
		ref.emplace(ref.end(), 21);
		check_equal("emplace_index at back");
	}

	// 13. Emplace_index in middle
	{
		::std::size_t pos{dq.size() / 2};
		dq.emplace_index(pos, 22);
		ref.emplace(ref.begin() + pos, 22);
		check_equal("emplace_index in middle");
	}

	// 14. Insert with move
	{
		T val{300};
		dq.insert(dq.begin() + 1, ::std::move(val));
		ref.insert(ref.begin() + 1, 300);
		check_equal("insert with move");
	}

	// 15. Insert_index with move
	{
		T val{301};
		dq.insert_index(1, ::std::move(val));
		ref.insert(ref.begin() + 1, 301);
		check_equal("insert_index with move");
	}

	// 16. Randomized insertions
	for (::std::size_t iter{}; iter != 500u; ++iter)
	{
		::std::size_t pos{iter % (dq.size() + 1)};
		T val{iter + 10000};

		dq.insert_index(pos, val);
		ref.insert(ref.begin() + pos, val);

		check_equal("randomized insert_index");
	}

	// 17. Randomized insert with iterator
	for (::std::size_t iter{}; iter != 500u; ++iter)
	{
		::std::size_t pos{iter % (dq.size() + 1)};
		T val{iter + 20000};

		dq.insert(dq.begin() + pos, val);
		ref.insert(ref.begin() + pos, val);

		check_equal("randomized insert iterator");
	}

	::fast_io::io::print("deque insert single element test finished\n");
}

inline void test_insert_return_value()
{
	::fast_io::io::perr("=== deque insert return value test ===\n");

	using T = ::std::size_t;
	::fast_io::deque<T> dq;

	// Fill some initial data
	for (::std::size_t i{}; i != 100u; ++i)
	{
		dq.push_back(i);
	}

	// 1. Insert returns iterator pointing to inserted element
	{
		auto it = dq.insert(dq.begin(), 9999);
		if (*it != 9999)
		{
			::fast_io::io::panic("insert return value wrong\n");
		}
		if (it != dq.begin())
		{
			::fast_io::io::panic("insert iterator position wrong\n");
		}
	}

	// 2. Insert in middle returns correct iterator
	{
		::std::size_t pos{50};
		auto it = dq.insert(dq.begin() + pos, 8888);
		if (*it != 8888)
		{
			::fast_io::io::panic("insert middle return value wrong\n");
		}
		if (static_cast<::std::size_t>(it - dq.begin()) != pos)
		{
			::fast_io::io::panic("insert middle iterator position wrong\n");
		}
	}

	// 3. Insert at end returns correct iterator
	{
		auto old_end = dq.end();
		auto it = dq.insert(dq.end(), 7777);
		if (*it != 7777)
		{
			::fast_io::io::panic("insert end return value wrong\n");
		}
		if (it != old_end)
		{
			::fast_io::io::panic("insert end iterator position wrong\n");
		}
	}

	// 4. Insert_index returns reference to inserted element
	{
		::std::size_t pos{25};
		auto &ref = dq.insert_index(pos, 6666);
		if (ref != 6666)
		{
			::fast_io::io::panic("insert_index return value wrong\n");
		}
		if (dq[pos] != 6666)
		{
			::fast_io::io::panic("insert_index element position wrong\n");
		}
	}

	// 5. Emplace returns iterator
	{
		auto it = dq.emplace(dq.begin(), 5555);
		if (*it != 5555)
		{
			::fast_io::io::panic("emplace return value wrong\n");
		}
	}

	// 6. Emplace_index returns reference
	{
		auto &ref = dq.emplace_index(0, 4444);
		if (ref != 4444)
		{
			::fast_io::io::panic("emplace_index return value wrong\n");
		}
	}

	::fast_io::io::print("deque insert return value test finished\n");
}

inline void test_insert_exception_safety()
{
	::fast_io::io::perr("=== deque insert exception safety test ===\n");

	ThrowingType::reset();

	::fast_io::deque<ThrowingType> dq;

	// Fill below threshold
	for (::std::size_t i{}; i != 30u; ++i)
	{
		dq.emplace_back(i);
	}

	::std::size_t old_size{dq.size()};

	// Try to insert at front and back only (simple rollback)
	// Middle insertions have complex rollback that's harder to test
	bool threw{false};
	try
	{
		for (::std::size_t i{}; i != 100u; ++i)
		{
			// Alternate between front and back insertions
			if (i % 2 == 0)
			{
				dq.emplace_front(i + 100);
			}
			else
			{
				dq.emplace_back(i + 100);
			}
		}
	}
	catch (::std::runtime_error const &)
	{
		threw = true;
	}

	if (!threw)
	{
		::fast_io::io::panic("expected exception but none thrown\n");
	}

	// Check that size is at least old_size (elements weren't lost)
	if (dq.size() < old_size)
	{
		::fast_io::io::panic("exception safety failed: size decreased below original\n");
	}

	// Verify original elements are still present (values 0-29)
	// Note: due to front insertions, positions have shifted
	for (::std::size_t i{}; i != old_size; ++i)
	{
		// Find value i in the deque
		bool found{false};
		for (::std::size_t j{}; j != dq.size(); ++j)
		{
			if (dq[j].value == i)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			::fast_io::io::panic("exception safety failed: original element lost\n");
		}
	}

	::fast_io::io::print("deque insert exception safety test finished\n");
}

} // namespace

int main()
{
	test_insert_single();
	test_insert_return_value();
	test_insert_exception_safety();
}