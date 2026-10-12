#include <cstdio>
#include <fast_io.h>
#include <fast_io_dsal/str_ranked_btree_set.h>

int main()
{
	// 1. Default construction
	::fast_io::str_ranked_btree_set s1;
	::fast_io::io::println("default constructed, is_empty=", s1.is_empty(), " size=", s1.size());
	if (!s1.is_empty() || s1.size() != 0)
	{
		::fast_io::fast_terminate();
	}

	// 2. Initializer list construction
	::fast_io::str_ranked_btree_set s2{"hello", "world", "foo"};
	::fast_io::io::print("initializer_list construction:\n");
	for (auto e : s2)
	{
		::fast_io::io::println("  ", e);
	}
	if (s2.size() != 3)
	{
		::fast_io::fast_terminate();
	}

	// 3. Ordered iteration (keys must come out sorted)
	::fast_io::io::print("\n--- ordered iteration ---\n");
	::fast_io::str_ranked_btree_set s3{"z", "a", "m", "d", "s"};
	::fast_io::cstring_view prev{};
	bool sorted{true};
	for (auto e : s3)
	{
		::fast_io::io::println("  ", e);
		if (!prev.is_empty() && !(prev < e))
		{
			sorted = false;
		}
		prev = e;
	}
	::fast_io::io::println("sorted order: ", sorted);
	if (!sorted)
	{
		::fast_io::fast_terminate();
	}

	// 4. Copy construction / assignment
	auto s4(s3);
	if (s3 != s4 || s4.size() != s3.size())
	{
		::fast_io::fast_terminate();
	}
	::fast_io::str_ranked_btree_set s5;
	s5 = s3;
	if (s3 != s5)
	{
		::fast_io::fast_terminate();
	}
	auto s6(::std::move(s4));
	::fast_io::io::println("move constructed, is_empty source=", s4.is_empty());
	::fast_io::str_ranked_btree_set s7;
	s7 = ::std::move(s5);
	::fast_io::io::println("move assigned, is_empty source=", s5.is_empty());

	// 5. insert_key + nth_element: the order statistic must match sorted order
	::fast_io::io::print("\n--- insert_key + nth_element ---\n");
	::fast_io::str_ranked_btree_set s8;
	char buf[32];
	constexpr ::std::size_t N{5000};
	for (::std::size_t i{}; i != N; ++i)
	{
		auto len{static_cast<::std::size_t>(::std::snprintf(buf, sizeof(buf), "k%08zu", i))};
		if (!s8.insert_key(::fast_io::string_view{buf, len}))
		{
			::fast_io::fast_terminate();
		}
	}
	::fast_io::io::println("inserted ", N, " keys, size=", s8.size());
	if (s8.size() != N)
	{
		::fast_io::fast_terminate();
	}
	for (::std::size_t i{}; i != N; ++i)
	{
		auto len{static_cast<::std::size_t>(::std::snprintf(buf, sizeof(buf), "k%08zu", i))};
		auto it{s8.nth_element(i)};
		if (it == s8.end() || ::fast_io::string_view{*it} != ::fast_io::string_view{buf, len})
		{
			::fast_io::fast_terminate();
		}
	}
	::fast_io::io::print("nth_element 0..N matches sorted order\n");

	// 6. duplicate insert returns false and keeps size
	if (s8.insert_key("k00000042"))
	{
		::fast_io::fast_terminate();
	}
	if (s8.size() != N)
	{
		::fast_io::fast_terminate();
	}

	// 7. erase half (evens) then nth_element on survivors
	::fast_io::io::print("\n--- erase + nth_element ---\n");
	for (::std::size_t i{}; i != N; i += 2)
	{
		auto len{static_cast<::std::size_t>(::std::snprintf(buf, sizeof(buf), "k%08zu", i))};
		if (s8.erase_key(::fast_io::string_view{buf, len}) != 1)
		{
			::fast_io::fast_terminate();
		}
	}
	if (s8.size() != N / 2)
	{
		::fast_io::fast_terminate();
	}
	for (::std::size_t i{}; i != s8.size(); ++i)
	{
		auto len{static_cast<::std::size_t>(::std::snprintf(buf, sizeof(buf), "k%08zu", i * 2 + 1))};
		auto it{s8.nth_element(i)};
		if (it == s8.end() || ::fast_io::string_view{*it} != ::fast_io::string_view{buf, len})
		{
			::fast_io::fast_terminate();
		}
	}
	::fast_io::io::print("nth_element after erasing evens matches\n");

	// 8. lookup
	::fast_io::io::print("\n--- lookup ---\n");
	::fast_io::io::println("contains \"k00001001\": ", s8.contains("k00001001"));
	::fast_io::io::print("contains \"k00001000\": ", s8.contains("k00001000"), " (erased, should be false)\n");
	if (s8.contains("k00001000") || !s8.contains("k00001001"))
	{
		::fast_io::fast_terminate();
	}

	// 9. bounds
	{
		auto lb{s8.lower_bound("k00001001")};
		if (lb == s8.end() || ::fast_io::string_view{*lb} != ::fast_io::string_view{"k00001001", 9})
		{
			::fast_io::fast_terminate();
		}
		auto er{s8.equal_range("k00001001")};
		if (er.first == s8.end() || er.second == s8.end())
		{
			::fast_io::fast_terminate();
		}
		auto miss{s8.lower_bound("zzzzzzz")};
		if (miss != s8.end())
		{
			::fast_io::fast_terminate();
		}
	}

	// 10. erase remaining via iterators
	{
		auto first{s8.begin()};
		auto last{s8.end()};
		s8.erase(first, last);
		if (!s8.is_empty() || s8.size() != 0)
		{
			::fast_io::fast_terminate();
		}
	}

	// 11. random churn: insert/erase mix with nth_element cross-checks
	::fast_io::io::print("\n--- churn ---\n");
	{
		::fast_io::str_ranked_btree_set cs;
		::std::size_t expected{};
		for (::std::size_t round{}; round != 6000; ++round)
		{
			auto k{(round * 7919) % 700};
			auto len{static_cast<::std::size_t>(::std::snprintf(buf, sizeof(buf), "ck%06zu", k))};
			::fast_io::string_view fk{buf, len};
			if (round % 3 == 2)
			{
				if (cs.erase_key(fk))
				{
					--expected;
				}
			}
			else if (cs.insert_key(fk))
			{
				++expected;
			}
			if (cs.size() != expected)
			{
				::fast_io::fast_terminate();
			}
		}
		::fast_io::io::println("after churn, size=", cs.size(), " expected=", expected);
		// iteration order must be sorted and nth_element consistent
		::fast_io::cstring_view cprev{};
		::std::size_t idx{};
		for (auto e : cs)
		{
			if (!cprev.is_empty() && !(cprev < e))
			{
				::fast_io::fast_terminate();
			}
			cprev = e;
			auto nth{cs.nth_element(idx)};
			if (nth == cs.end() || ::fast_io::string_view{*nth} != ::fast_io::string_view{e})
			{
				::fast_io::fast_terminate();
			}
			++idx;
		}
		if (idx != expected)
		{
			::fast_io::fast_terminate();
		}
	}

	// 12. swap / comparison
	::fast_io::io::print("\n--- swap/compare ---\n");
	::fast_io::str_ranked_btree_set sa{"x", "y"};
	swap(s3, sa);
	::fast_io::io::println("after swap, s3 front=", s3.front(), " sa front=", sa.front());
	::fast_io::str_ranked_btree_set sb{"x", "y"};
	if (s3 != sb || (s3 <=> sb) != 0)
	{
		::fast_io::fast_terminate();
	}

	// 13. clear / clear_destroy / reuse
	s8.insert_key("reuse");
	s8.clear();
	if (!s8.is_empty() || s8.size() != 0)
	{
		::fast_io::fast_terminate();
	}
	s8.insert_key("after_clear");
	s8.clear_destroy();
	if (!s8.is_empty())
	{
		::fast_io::fast_terminate();
	}
	::fast_io::io::println("after clear_destroy, is_empty=", s8.is_empty());

	::fast_io::io::print("\nAll tests passed.\n");
}
