#include <cstdio>
#include <fast_io.h>
#include <fast_io_dsal/str_btree_map.h>

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
	::fast_io::str_btree_map<::std::size_t> mp1;
	::fast_io::io::println("default constructed, is_empty=", mp1.is_empty());

	// 2. Initializer list construction
	::fast_io::str_btree_map<::std::size_t> mp2{{"hello", 10}, {"world", 20}, {"foo", 30}};
	::fast_io::io::print("initializer_list construction:\n");
	for (auto e : mp2)
	{
		::fast_io::io::println("  ", e.key(), " -> ", e.mapped());
	}

	// 3. Ordered iteration (keys must come out sorted)
	::fast_io::io::print("\n--- ordered iteration ---\n");
	::fast_io::str_btree_map<::std::size_t> mp3{{"z", 26}, {"a", 1}, {"m", 13}, {"d", 4}, {"s", 19}};
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

	// 4. Copy construction
	auto mp4(mp3);
	::fast_io::io::println("copy constructed, equal=", (mp3 == mp4));
	if (mp3 != mp4)
	{
		::fast_io::fast_terminate();
	}

	// 5. Copy assignment
	::fast_io::str_btree_map<::std::size_t> mp5;
	mp5 = mp3;
	::fast_io::io::println("copy assigned, equal=", (mp3 == mp5));
	if (mp3 != mp5)
	{
		::fast_io::fast_terminate();
	}

	// 6. Move construction / assignment
	auto mp6(::std::move(mp4));
	::fast_io::io::println("move constructed, is_empty source=", mp4.is_empty());
	::fast_io::str_btree_map<::std::size_t> mp7;
	mp7 = ::std::move(mp5);
	::fast_io::io::println("move assigned, is_empty source=", mp5.is_empty());

	// 7. insert_key (new keys)
	::fast_io::io::print("\n--- insert_key ---\n");
	mp1.insert_key("apple", 1);
	mp1.insert_key("banana", 2);
	mp1.insert_key("cherry", 3);
	mp1.insert_key("date", 4);
	mp1.insert_key("elderberry", 5);
	for (auto e : mp1)
	{
		::fast_io::io::println("  ", e.key(), " -> ", e.mapped());
	}

	// 8. insert_key (existing key - should not overwrite)
	mp1.insert_key("apple", 100);
	::fast_io::io::print("after insert_key existing, apple -> ", mp1.find("apple")->mapped(), " (should be 1)\n");
	if (mp1.find("apple")->mapped() != 1)
	{
		::fast_io::fast_terminate();
	}

	// 9. insert_key_or_assign (existing key - should overwrite)
	mp1.insert_key_or_assign("apple", 1000);
	::fast_io::io::print("after insert_key_or_assign existing, apple -> ", mp1.find("apple")->mapped(), " (should be 1000)\n");
	if (mp1.find("apple")->mapped() != 1000)
	{
		::fast_io::fast_terminate();
	}

	// 10. operator[]
	::fast_io::io::print("\n--- operator[] ---\n");
	mp1["fig"] = 6;
	mp1["apple"] = 7;
	::fast_io::io::println("fig -> ", mp1["fig"], " apple -> ", mp1["apple"]);
	if (mp1["apple"] != 7)
	{
		::fast_io::fast_terminate();
	}

	// 11. emplace_key / emplace_key_or_assign
	mp1.emplace_key("kiwi", 9);
	mp1.emplace_key_or_assign("lemon", 10);
	mp1.emplace_key_or_assign("kiwi", 99);
	::fast_io::io::print("kiwi -> ", mp1.find("kiwi")->mapped(), " (should be 99)\n");
	if (mp1.find("kiwi")->mapped() != 99)
	{
		::fast_io::fast_terminate();
	}

	// 12. insert_key_hint with end() hint (sequential bulk load)
	::fast_io::io::print("\n--- insert_key_hint ---\n");
	{
		::fast_io::str_btree_map<::std::size_t> hinted;
		char buf[32];
		for (::std::size_t i{}; i != 200; ++i)
		{
			auto len{static_cast<::std::size_t>(::std::snprintf(buf, sizeof(buf), "hk%08zu", i))};
			hinted.insert_key_hint(hinted.cend(), ::fast_io::string_view{buf, len}, i);
		}
		bool ok{true};
		::std::size_t expect{};
		for (auto e : hinted)
		{
			if (e.mapped() != expect)
			{
				ok = false;
			}
			++expect;
		}
		::fast_io::io::println("hinted sequential insert ok: ", ok, " count=", expect);
		if (!ok || expect != 200)
		{
			::fast_io::fast_terminate();
		}
		// wrong hint falls back to full search
		hinted.insert_key_hint(hinted.cbegin(), "aa_first", 999);
		if (!hinted.contains("aa_first"))
		{
			::fast_io::fast_terminate();
		}
	}

	// 13. insert_range
	::fast_io::io::print("\n--- insert_range ---\n");
	mp1.insert_range({{"grape", 7}, {"honeydew", 8}});
	::fast_io::io::println("grape -> ", mp1.find("grape")->mapped(), " honeydew -> ", mp1.find("honeydew")->mapped());

	// 14. contains / find / count
	::fast_io::io::print("\n--- lookup ---\n");
	::fast_io::io::println("contains \"apple\": ", mp1.contains("apple"));
	::fast_io::io::println("contains \"notfound\": ", mp1.contains("notfound"));
	::fast_io::io::println("count \"banana\": ", mp1.count("banana"));
	{
		auto it = mp1.find("notfound");
		::fast_io::io::println("find \"notfound\" is end: ", (it == mp1.end()));
		if (it != mp1.end())
		{
			::fast_io::fast_terminate();
		}
	}

	// 15. lower_bound / upper_bound / equal_range
	::fast_io::io::print("\n--- bounds ---\n");
	{
		auto lb{mp1.lower_bound("date")};
		::fast_io::io::println("lower_bound \"date\" -> ", lb->key());
		if (lb == mp1.end() || lb->key() != ::fast_io::string_view{"date", 4})
		{
			::fast_io::fast_terminate();
		}
		auto ub{mp1.upper_bound("date")};
		::fast_io::io::println("upper_bound \"date\" -> ", ub->key());
		auto er{mp1.equal_range("date")};
		if (er.first == mp1.end() || er.second == mp1.end())
		{
			::fast_io::fast_terminate();
		}
		::fast_io::io::print("equal_range [", er.first->key(), ", ", er.second->key(), ")\n");
		auto miss{mp1.lower_bound("zzz")};
		::fast_io::io::println("lower_bound \"zzz\" is end: ", (miss == mp1.end()));
		if (miss != mp1.end())
		{
			::fast_io::fast_terminate();
		}
	}

	// 16. front / back
	::fast_io::io::print("\n--- front/back ---\n");
	::fast_io::io::println("front: ", mp1.front().key(), " -> ", mp1.front().mapped());
	::fast_io::io::println("back: ", mp1.back().key(), " -> ", mp1.back().mapped());

	// 17. erase_key
	::fast_io::io::print("\n--- erase_key ---\n");
	auto erased = mp1.erase_key("banana");
	::fast_io::io::println("erase_key \"banana\": ", erased, ", contains=", mp1.contains("banana"));
	if (mp1.contains("banana"))
	{
		::fast_io::fast_terminate();
	}
	erased = mp1.erase_key("notfound");
	::fast_io::io::println("erase_key \"notfound\": ", erased);

	// 18. erase (iterator)
	{
		auto it = mp1.find("cherry");
		if (it != mp1.end())
		{
			auto next = mp1.erase(it);
			::fast_io::io::println("erased \"cherry\", contains=", mp1.contains("cherry"));
			if (next != mp1.end())
			{
				::fast_io::io::println("next element key: ", next->key());
			}
		}
	}

	// 19. erase (range)
	{
		auto first = mp1.begin();
		auto last = mp1.end();
		mp1.erase(first, last);
		::fast_io::io::println("after range erase, is_empty=", mp1.is_empty());
		if (!mp1.is_empty())
		{
			::fast_io::fast_terminate();
		}
	}

	// 20. reverse iteration
	::fast_io::io::print("\n--- reverse iteration ---\n");
	for (auto i{mp3.crbegin()}, ed{mp3.crend()}; i != ed; ++i)
	{
		::fast_io::io::println("  ", i->key(), " -> ", i->mapped());
	}

	// 21. swap / comparison
	::fast_io::io::print("\n--- swap/compare ---\n");
	::fast_io::str_btree_map<::std::size_t> mp8{{"x", 24}, {"y", 25}};
	swap(mp3, mp8);
	::fast_io::io::println("after swap, mp3 front=", mp3.front().key(), " mp8 front=", mp8.front().key());
	::fast_io::str_btree_map<::std::size_t> mp9{{"x", 24}, {"y", 25}};
	::fast_io::io::println("mp3 == mp9: ", (mp3 == mp9));
	::fast_io::io::println("mp3 <=> mp9 ordering consistent: ", ((mp3 <=> mp9) == 0));
	if (mp3 != mp9)
	{
		::fast_io::fast_terminate();
	}

	// 22. clear / clear_destroy / reuse
	mp1.insert_key("reuse", 1);
	mp1.clear();
	::fast_io::io::println("after clear, is_empty=", mp1.is_empty());
	mp1.insert_key("after_clear", 2);
	mp1.clear_destroy();
	::fast_io::io::println("after clear_destroy, is_empty=", mp1.is_empty());
	if (!mp1.is_empty())
	{
		::fast_io::fast_terminate();
	}

	// === RAII tests: every placement-new must pair with a destroy ===
	::fast_io::io::print("\n========== RAII tests ==========\n");

	// 23. bulk insert + erase half + copy: alive count must track exactly
	{
		::fast_io::str_btree_map<raii_mapped> rm;
		char buf[32];
		constexpr ::std::size_t N{3000};
		for (::std::size_t i{}; i != N; ++i)
		{
			auto len{static_cast<::std::size_t>(::std::snprintf(buf, sizeof(buf), "k%08zu", i))};
			rm.insert_key(::fast_io::string_view{buf, len}, raii_mapped{i});
		}
		::fast_io::io::print("after bulk insert, alive=", raii_mapped::alive, " (should be ", N, ")\n");
		if (raii_mapped::alive != N)
		{
			::fast_io::fast_terminate();
		}
		for (::std::size_t i{}; i != N; i += 2)
		{
			auto len{static_cast<::std::size_t>(::std::snprintf(buf, sizeof(buf), "k%08zu", i))};
			if (rm.erase_key(::fast_io::string_view{buf, len}) != 1)
			{
				::fast_io::fast_terminate();
			}
		}
		::fast_io::io::print("after erase half, alive=", raii_mapped::alive, " (should be ", N / 2, ")\n");
		if (raii_mapped::alive != N / 2)
		{
			::fast_io::fast_terminate();
		}
		{
			auto cp{rm};
			if (raii_mapped::alive != N)
			{
				::fast_io::fast_terminate();
			}
			for (::std::size_t i{1}; i < N; i += 2)
			{
				auto len{static_cast<::std::size_t>(::std::snprintf(buf, sizeof(buf), "k%08zu", i))};
				auto it{cp.find(::fast_io::string_view{buf, len})};
				if (it == cp.end() || it->mapped().val != i)
				{
					::fast_io::fast_terminate();
				}
			}
		}
		::fast_io::io::print("after copy scope, alive=", raii_mapped::alive, " (should be ", N / 2, ")\n");
		if (raii_mapped::alive != N / 2)
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

	// 24. or_assign replaces the value object (destroy + construct, no leak)
	{
		::fast_io::str_btree_map<raii_mapped> rm;
		rm.insert_key("a", raii_mapped{1});
		rm.insert_key("b", raii_mapped{2});
		if (raii_mapped::alive != 2)
		{
			::fast_io::fast_terminate();
		}
		rm.insert_key_or_assign("a", raii_mapped{10});
		rm.emplace_key_or_assign("b", 20);
		if (raii_mapped::alive != 2)
		{
			::fast_io::fast_terminate();
		}
		if (rm.find("a")->mapped().val != 10 || rm.find("b")->mapped().val != 20)
		{
			::fast_io::fast_terminate();
		}
	}
	::fast_io::io::print("after or_assign scope, alive=", raii_mapped::alive, " (should be 0)\n");
	if (raii_mapped::alive != 0)
	{
		::fast_io::fast_terminate();
	}

	// 25. churn: interleaved insert/erase keeps alive count exact
	{
		::fast_io::str_btree_map<raii_mapped> rm;
		char buf[32];
		::std::size_t expected{};
		for (::std::size_t round{}; round != 4000; ++round)
		{
			auto k{(round * 7919) % 500};
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
		}
		::fast_io::io::println("after churn, alive=", raii_mapped::alive, " expected=", expected);
		if (raii_mapped::alive != expected)
		{
			::fast_io::fast_terminate();
		}
	}
	::fast_io::io::print("final alive=", raii_mapped::alive, " (should be 0)\n");
	if (raii_mapped::alive != 0)
	{
		::fast_io::fast_terminate();
	}

	::fast_io::io::print("\nAll tests passed.\n");
}
