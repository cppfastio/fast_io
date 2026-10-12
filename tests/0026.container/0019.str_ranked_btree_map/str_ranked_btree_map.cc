#include <cstdio>
#include <fast_io.h>
#include <fast_io_dsal/str_ranked_btree_map.h>

struct raii_mapped
{
	static inline ::std::size_t alive{};
	::std::size_t val{};

	raii_mapped() noexcept : val(0)
	{
		++alive;
	}
	explicit raii_mapped(::std::size_t v) noexcept : val(v)
	{
		++alive;
	}
	raii_mapped(raii_mapped const &o) noexcept : val(o.val)
	{
		++alive;
	}
	raii_mapped &operator=(raii_mapped const &o) noexcept
	{
		val = o.val;
		return *this;
	}
	raii_mapped(raii_mapped &&o) noexcept : val(o.val)
	{
		o.val = 0;
		++alive;
	}
	raii_mapped &operator=(raii_mapped &&o) noexcept
	{
		if (this != ::std::addressof(o))
		{
			val = o.val;
			o.val = 0;
		}
		return *this;
	}
	~raii_mapped() noexcept
	{
		--alive;
	}
};

int main()
{
	// 1. Default construction
	::fast_io::str_ranked_btree_map<::std::size_t> mp1;
	::fast_io::io::println("default constructed, is_empty=", mp1.is_empty(), " size=", mp1.size());
	if (!mp1.is_empty() || mp1.size() != 0)
	{
		::fast_io::fast_terminate();
	}

	// 2. Initializer list construction
	::fast_io::str_ranked_btree_map<::std::size_t> mp2{{"hello", 10}, {"world", 20}, {"foo", 30}};
	::fast_io::io::print("initializer_list construction:\n");
	for (auto e : mp2)
	{
		::fast_io::io::println("  ", e.key(), " -> ", e.mapped());
	}
	if (mp2.size() != 3)
	{
		::fast_io::fast_terminate();
	}

	// 3. Ordered iteration (keys must come out sorted)
	::fast_io::io::print("\n--- ordered iteration ---\n");
	::fast_io::str_ranked_btree_map<::std::size_t> mp3{{"z", 26}, {"a", 1}, {"m", 13}, {"d", 4}, {"s", 19}};
	::fast_io::cstring_view prev{};
	bool sorted{true};
	for (auto e : mp3)
	{
		::fast_io::io::println("  ", e.key(), " -> ", e.mapped());
		if (!prev.is_empty() && !(prev < e.key()))
		{
			sorted = false;
		}
		prev = e.key();
	}
	::fast_io::io::println("sorted order: ", sorted);
	if (!sorted)
	{
		::fast_io::fast_terminate();
	}

	// 4. Copy construction / assignment
	auto mp4(mp3);
	if (mp3 != mp4 || mp4.size() != mp3.size())
	{
		::fast_io::fast_terminate();
	}
	::fast_io::str_ranked_btree_map<::std::size_t> mp5;
	mp5 = mp3;
	if (mp3 != mp5)
	{
		::fast_io::fast_terminate();
	}

	// 5. Move construction / assignment
	auto mp6(::std::move(mp4));
	::fast_io::io::println("move constructed, is_empty source=", mp4.is_empty());
	::fast_io::str_ranked_btree_map<::std::size_t> mp7;
	mp7 = ::std::move(mp5);
	::fast_io::io::println("move assigned, is_empty source=", mp5.is_empty());

	// 6. insert_key + nth_element: nth smallest must match sorted order
	::fast_io::io::print("\n--- insert_key + nth_element ---\n");
	char buf[32];
	constexpr ::std::size_t N{5000};
	for (::std::size_t i{}; i != N; ++i)
	{
		auto len{static_cast<::std::size_t>(::std::snprintf(buf, sizeof(buf), "k%08zu", i))};
		if (!mp1.insert_key(::fast_io::string_view{buf, len}, i * 3))
		{
			::fast_io::fast_terminate();
		}
	}
	if (mp1.size() != N)
	{
		::fast_io::fast_terminate();
	}
	for (::std::size_t i{}; i != N; ++i)
	{
		auto len{static_cast<::std::size_t>(::std::snprintf(buf, sizeof(buf), "k%08zu", i))};
		auto it{mp1.nth_element(i)};
		if (it == mp1.end() || it->key() != ::fast_io::string_view{buf, len} || it->mapped() != i * 3)
		{
			::fast_io::fast_terminate();
		}
	}
	::fast_io::io::print("nth_element 0..N matches sorted order\n");

	// 7. insert_key (existing key - should not overwrite)
	mp1.insert_key("k00000042", 424242);
	if (mp1.size() != N || mp1.find("k00000042")->mapped() != 126)
	{
		::fast_io::fast_terminate();
	}

	// 8. insert_key_or_assign (existing key - should overwrite)
	mp1.insert_key_or_assign("k00000042", 999);
	if (mp1.find("k00000042")->mapped() != 999 || mp1.size() != N)
	{
		::fast_io::fast_terminate();
	}

	// 9. operator[]
	mp1["fig"] = 6;
	mp1["k00000042"] = 7;
	if (mp1["k00000042"] != 7 || mp1["fig"] != 6)
	{
		::fast_io::fast_terminate();
	}
	if (mp1.size() != N + 1)
	{
		::fast_io::fast_terminate();
	}

	// 10. emplace_key / emplace_key_or_assign
	mp1.emplace_key("kiwi", 9);
	mp1.emplace_key_or_assign("lemon", 10);
	mp1.emplace_key_or_assign("kiwi", 99);
	if (mp1.find("kiwi")->mapped() != 99)
	{
		::fast_io::fast_terminate();
	}

	// 11. insert_range
	mp1.insert_range({{"grape", 7}, {"honeydew", 8}});
	if (mp1.find("grape")->mapped() != 7 || mp1.find("honeydew")->mapped() != 8)
	{
		::fast_io::fast_terminate();
	}

	// 12. contains / find / count
	::fast_io::io::print("\n--- lookup ---\n");
	if (!mp1.contains("fig") || mp1.contains("notfound"))
	{
		::fast_io::fast_terminate();
	}
	{
		auto it = mp1.find("notfound");
		if (it != mp1.end())
		{
			::fast_io::fast_terminate();
		}
	}

	// 13. lower_bound / upper_bound / equal_range
	{
		auto lb{mp1.lower_bound("fig")};
		if (lb == mp1.end() || lb->key() != ::fast_io::string_view{"fig", 3})
		{
			::fast_io::fast_terminate();
		}
		auto ub{mp1.upper_bound("fig")};
		if (ub == mp1.end() || !(::fast_io::string_view{"fig", 3} < ub->key()))
		{
			::fast_io::fast_terminate();
		}
		auto er{mp1.equal_range("fig")};
		if (er.first == mp1.end() || er.second == mp1.end())
		{
			::fast_io::fast_terminate();
		}
		auto miss{mp1.lower_bound("zzzzzzz")};
		if (miss != mp1.end())
		{
			::fast_io::fast_terminate();
		}
	}

	// 14. front / back
	::fast_io::io::println("front: ", mp1.front().key(), " -> ", mp1.front().mapped());
	::fast_io::io::println("back: ", mp1.back().key(), " -> ", mp1.back().mapped());

	// 15. erase + nth_element on survivors
	::fast_io::io::print("\n--- erase + nth_element ---\n");
	mp1.erase_key("fig");
	mp1.erase_key("grape");
	mp1.erase_key("honeydew");
	mp1.erase_key("kiwi");
	mp1.erase_key("lemon");
	if (mp1.size() != N)
	{
		::fast_io::fast_terminate();
	}
	for (::std::size_t i{}; i != N; i += 2)
	{
		auto len{static_cast<::std::size_t>(::std::snprintf(buf, sizeof(buf), "k%08zu", i))};
		if (mp1.erase_key(::fast_io::string_view{buf, len}) != 1)
		{
			::fast_io::fast_terminate();
		}
	}
	if (mp1.size() != N / 2)
	{
		::fast_io::fast_terminate();
	}
	for (::std::size_t i{}; i != mp1.size(); ++i)
	{
		auto len{static_cast<::std::size_t>(::std::snprintf(buf, sizeof(buf), "k%08zu", i * 2 + 1))};
		auto it{mp1.nth_element(i)};
		if (it == mp1.end() || it->key() != ::fast_io::string_view{buf, len})
		{
			::fast_io::fast_terminate();
		}
	}
	::fast_io::io::print("nth_element after erasing evens matches\n");

	// 16. erase (iterator) and erase (range)
	{
		auto it{mp1.find("k00000003")};
		if (it != mp1.end())
		{
			mp1.erase(it);
		}
		if (mp1.contains("k00000003"))
		{
			::fast_io::fast_terminate();
		}
		auto first{mp1.begin()};
		auto last{mp1.end()};
		mp1.erase(first, last);
		if (!mp1.is_empty() || mp1.size() != 0)
		{
			::fast_io::fast_terminate();
		}
	}

	// 17. reverse iteration
	::fast_io::io::print("\n--- reverse iteration ---\n");
	for (auto i{mp3.crbegin()}, ed{mp3.crend()}; i != ed; ++i)
	{
		::fast_io::io::println("  ", i->key(), " -> ", i->mapped());
	}

	// 18. swap / comparison
	::fast_io::str_ranked_btree_map<::std::size_t> mp8{{"x", 24}, {"y", 25}};
	swap(mp3, mp8);
	::fast_io::io::println("after swap, mp3 front=", mp3.front().key(), " mp8 front=", mp8.front().key());
	::fast_io::str_ranked_btree_map<::std::size_t> mp9{{"x", 24}, {"y", 25}};
	if (mp3 != mp9 || (mp3 <=> mp9) != 0)
	{
		::fast_io::fast_terminate();
	}

	// 19. clear / clear_destroy / reuse
	mp1.insert_key("reuse", 1);
	mp1.clear();
	if (!mp1.is_empty() || mp1.size() != 0)
	{
		::fast_io::fast_terminate();
	}
	mp1.insert_key("after_clear", 2);
	mp1.clear_destroy();
	if (!mp1.is_empty())
	{
		::fast_io::fast_terminate();
	}

	// === RAII tests: every placement-new must pair with a destroy ===
	::fast_io::io::print("\n========== RAII tests ==========\n");

	// 20. bulk insert + erase half + copy: alive count must track exactly
	{
		::fast_io::str_ranked_btree_map<raii_mapped> rm;
		constexpr ::std::size_t RN{3000};
		for (::std::size_t i{}; i != RN; ++i)
		{
			auto len{static_cast<::std::size_t>(::std::snprintf(buf, sizeof(buf), "rk%08zu", i))};
			rm.insert_key(::fast_io::string_view{buf, len}, raii_mapped{i});
		}
		if (raii_mapped::alive != RN || rm.size() != RN)
		{
			::fast_io::fast_terminate();
		}
		for (::std::size_t i{}; i != RN; i += 2)
		{
			auto len{static_cast<::std::size_t>(::std::snprintf(buf, sizeof(buf), "rk%08zu", i))};
			if (rm.erase_key(::fast_io::string_view{buf, len}) != 1)
			{
				::fast_io::fast_terminate();
			}
		}
		if (raii_mapped::alive != RN / 2 || rm.size() != RN / 2)
		{
			::fast_io::fast_terminate();
		}
		for (::std::size_t i{}; i != rm.size(); ++i)
		{
			auto it{rm.nth_element(i)};
			if (it == rm.end() || it->mapped().val != i * 2 + 1)
			{
				::fast_io::fast_terminate();
			}
		}
		{
			auto cp{rm};
			if (raii_mapped::alive != RN || cp.size() != rm.size())
			{
				::fast_io::fast_terminate();
			}
		}
		if (raii_mapped::alive != RN / 2)
		{
			::fast_io::fast_terminate();
		}
		rm.clear();
		if (raii_mapped::alive != 0)
		{
			::fast_io::fast_terminate();
		}
	}
	::fast_io::io::print("after scope end, alive=", raii_mapped::alive, " (should be 0)\n");
	if (raii_mapped::alive != 0)
	{
		::fast_io::fast_terminate();
	}

	// 21. or_assign replaces the value object (destroy + construct, no leak)
	{
		::fast_io::str_ranked_btree_map<raii_mapped> rm;
		rm.insert_key("a", raii_mapped{1});
		rm.insert_key("b", raii_mapped{2});
		if (raii_mapped::alive != 2)
		{
			::fast_io::fast_terminate();
		}
		rm.insert_key_or_assign("a", raii_mapped{10});
		rm.emplace_key_or_assign("b", 20);
		if (raii_mapped::alive != 2 || rm.size() != 2)
		{
			::fast_io::fast_terminate();
		}
		if (rm.find("a")->mapped().val != 10 || rm.find("b")->mapped().val != 20)
		{
			::fast_io::fast_terminate();
		}
	}
	if (raii_mapped::alive != 0)
	{
		::fast_io::fast_terminate();
	}

	// 22. churn: interleaved insert/erase keeps alive and size exact
	{
		::fast_io::str_ranked_btree_map<raii_mapped> rm;
		::std::size_t expected{};
		for (::std::size_t round{}; round != 6000; ++round)
		{
			auto k{(round * 7919) % 700};
			auto len{static_cast<::std::size_t>(::std::snprintf(buf, sizeof(buf), "ck%06zu", k))};
			::fast_io::string_view fk{buf, len};
			if (round % 3 == 2)
			{
				if (rm.erase_key(fk))
				{
					--expected;
				}
			}
			else if (rm.insert_key(fk, raii_mapped{k}))
			{
				++expected;
			}
			if (rm.size() != expected || raii_mapped::alive != expected)
			{
				::fast_io::fast_terminate();
			}
		}
		::fast_io::io::println("after churn, alive=", raii_mapped::alive, " size=", rm.size());
	}
	if (raii_mapped::alive != 0)
	{
		::fast_io::fast_terminate();
	}

	::fast_io::io::print("\nAll tests passed.\n");
}
