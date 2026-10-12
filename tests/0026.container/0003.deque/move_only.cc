#include <fast_io.h>
#include <fast_io_dsal/deque.h>

namespace
{

struct MoveOnly
{
	::std::size_t value;

	explicit MoveOnly(::std::size_t v) : value(v)
	{}

	MoveOnly(MoveOnly const &) = delete;
	MoveOnly &operator=(MoveOnly const &) = delete;

	MoveOnly(MoveOnly &&other) noexcept : value(other.value)
	{
		other.value = 0;
	}

	MoveOnly &operator=(MoveOnly &&other) noexcept
	{
		if (this != &other)
		{
			value = other.value;
			other.value = 0;
		}
		return *this;
	}
};

inline void test_move_only_push_back()
{
	::fast_io::io::perr("=== deque move-only push_back test ===\n");

	::fast_io::deque<MoveOnly> dq;
	for (::std::size_t i{}; i != 4096u; ++i)
	{
		dq.push_back(MoveOnly(i));
	}

	if (dq.size() != 4096u)
	{
		::fast_io::io::panic("size wrong after push_back move-only\n");
	}

	for (::std::size_t i{}; i != dq.size(); ++i)
	{
		if (dq[i].value != i)
		{
			::fast_io::io::panic("value mismatch at ", i, " after push_back move-only\n");
		}
	}

	::fast_io::io::print("deque move-only push_back test finished\n");
}

inline void test_move_only_push_front()
{
	::fast_io::io::perr("=== deque move-only push_front test ===\n");

	::fast_io::deque<MoveOnly> dq;
	for (::std::size_t i{}; i != 4096u; ++i)
	{
		dq.push_front(MoveOnly(i));
	}

	if (dq.size() != 4096u)
	{
		::fast_io::io::panic("size wrong after push_front move-only\n");
	}

	for (::std::size_t i{}; i != dq.size(); ++i)
	{
		if (dq[i].value != 4095u - i)
		{
			::fast_io::io::panic("value mismatch at ", i, " after push_front move-only\n");
		}
	}

	::fast_io::io::print("deque move-only push_front test finished\n");
}

inline void test_move_only_insert()
{
	::fast_io::io::perr("=== deque move-only insert test ===\n");

	::fast_io::deque<MoveOnly> dq;

	dq.push_back(MoveOnly(1u));
	dq.push_back(MoveOnly(2u));
	dq.push_back(MoveOnly(3u));

	// Insert by move at front
	dq.insert(dq.begin(), MoveOnly(0u));
	if (dq.front().value != 0u)
	{
		::fast_io::io::panic("insert move at front value wrong\n");
	}

	// Insert by move at end
	dq.insert(dq.end(), MoveOnly(4u));
	if (dq.back().value != 4u)
	{
		::fast_io::io::panic("insert move at end value wrong\n");
	}

	// Insert by move in middle
	dq.insert(dq.begin() + 3, MoveOnly(99u));
	if (dq[3u].value != 99u)
	{
		::fast_io::io::panic("insert move at middle value wrong\n");
	}

	if (dq.size() != 6u)
	{
		::fast_io::io::panic("size wrong after insert move\n");
	}

	::fast_io::io::print("deque move-only insert test finished\n");
}

inline void test_move_only_erase()
{
	::fast_io::io::perr("=== deque move-only erase test ===\n");

	::fast_io::deque<MoveOnly> dq;
	for (::std::size_t i{}; i != 100u; ++i)
	{
		dq.push_back(MoveOnly(i));
	}

	// Erase front
	dq.erase(dq.begin());
	if (dq.front().value != 1u)
	{
		::fast_io::io::panic("erase front of move-only deque wrong\n");
	}

	// Erase back
	dq.erase(dq.end() - 1);
	if (dq.back().value != 98u)
	{
		::fast_io::io::panic("erase back of move-only deque wrong\n");
	}

	// Erase middle
	dq.erase(dq.begin() + 10);
	if (dq[10u].value != 12u)
	{
		::fast_io::io::panic("erase middle of move-only deque wrong\n");
	}

	::fast_io::io::print("deque move-only erase test finished\n");
}

} // namespace

int main()
{
	test_move_only_push_back();
	test_move_only_push_front();
	test_move_only_insert();
	test_move_only_erase();
}
