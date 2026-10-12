#include <ranges>
#include <fast_io.h>
#include <fast_io_dsal/str_swiss_map.h>

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
	::fast_io::str_swiss_map<::std::size_t> mp1;
	::fast_io::io::println("default constructed, size=", mp1.size(), " is_empty=", mp1.is_empty());

	// 2. Initializer list construction
	::fast_io::str_swiss_map<::std::size_t> mp2{{"hello", 10}, {"world", 20}, {"foo", 30}};
	::fast_io::io::println("initializer_list construction, size=", mp2.size());
	for (auto &e : mp2)
	{
		::fast_io::io::println("  ", e.key(), " -> ", e.mapped());
	}

	// 3. Copy construction
	auto mp3(mp2);
	::fast_io::io::println("copy constructed, size=", mp3.size(), " equal=", (mp2 == mp3));

	// 4. Copy assignment
	::fast_io::str_swiss_map<::std::size_t> mp4;
	mp4 = mp2;
	::fast_io::io::println("copy assigned, size=", mp4.size(), " equal=", (mp2 == mp4));

	// 5. Move construction
	auto mp5(::std::move(mp3));
	::fast_io::io::println("move constructed, size=", mp5.size(), " moved-from size=", mp3.size());

	// 6. Move assignment
	::fast_io::str_swiss_map<::std::size_t> mp6;
	mp6 = ::std::move(mp4);
	::fast_io::io::println("move assigned, size=", mp6.size(), " moved-from size=", mp4.size());

	// 7. insert_key (new keys)
	::fast_io::io::print("\n--- insert_key ---\n");
	mp1.insert_key("apple", 1);
	mp1.insert_key("banana", 2);
	mp1.insert_key("cherry", 3);
	mp1.insert_key("date", 4);
	mp1.insert_key("elderberry", 5);
	::fast_io::io::println("after 5 insert_key, size=", mp1.size());
	for (auto &e : mp1)
	{
		::fast_io::io::println("  ", e.key(), " -> ", e.mapped());
	}

	// 8. insert_key (existing key - should not overwrite)
	mp1.insert_key("apple", 100);
	::fast_io::io::print("after insert_key existing, apple -> ", mp1.find_key("apple")->mapped(), " (should be 1)\n");

	// 9. insert_key_or_assign (existing key - should overwrite)
	mp1.insert_key_or_assign("apple", 1000);
	::fast_io::io::print("after insert_key_or_assign existing, apple -> ", mp1.find_key("apple")->mapped(), " (should be 1000)\n");

	// 10. insert_key_or_assign (new key)
	mp1.insert_key_or_assign("fig", 6);
	::fast_io::io::println("after insert_key_or_assign new, size=", mp1.size());

	// 11. insert_range
	::fast_io::io::print("\n--- insert_range ---\n");
	mp1.insert_range({{"grape", 7}, {"honeydew", 8}});
	::fast_io::io::println("after insert_range, size=", mp1.size());
	for (auto &e : mp1)
	{
		::fast_io::io::println("  ", e.key(), " -> ", e.mapped());
	}

	// 12. emplace_key
	::fast_io::io::print("\n--- emplace_key ---\n");
	mp1.emplace_key("kiwi", 9);
	::fast_io::io::println("after emplace_key, kiwi -> ", mp1.find_key("kiwi")->mapped());

	// 13. emplace_key_or_assign (new key)
	mp1.emplace_key_or_assign("lemon", 10);
	::fast_io::io::println("after emplace_key_or_assign new, lemon -> ", mp1.find_key("lemon")->mapped());

	// 14. emplace_key_or_assign (existing key)
	mp1.emplace_key_or_assign("kiwi", 99);
	::fast_io::io::print("after emplace_key_or_assign existing, kiwi -> ", mp1.find_key("kiwi")->mapped(), " (should be 99)\n");

	// 15. contains / find_key / count
	::fast_io::io::print("\n--- lookup ---\n");
	::fast_io::io::println("contains \"apple\": ", mp1.contains("apple"));
	::fast_io::io::println("contains \"notfound\": ", mp1.contains("notfound"));
	::fast_io::io::println("count \"banana\": ", mp1.count("banana"));
	::fast_io::io::println("count \"notfound\": ", mp1.count("notfound"));
	{
		auto it = mp1.find_key("cherry");
		if (it != mp1.end())
		{
			::fast_io::io::println("find_key \"cherry\" -> ", it->mapped());
		}
	}
	{
		auto it = mp1.find_key("notfound");
		::fast_io::io::println("find_key \"notfound\" is end: ", (it == mp1.end()));
	}

	// 16. erase_key (existing key)
	::fast_io::io::print("\n--- erase_key ---\n");
	auto erased = mp1.erase_key("banana");
	::fast_io::io::println("erase_key \"banana\": ", erased, ", size=", mp1.size(), ", contains=", mp1.contains("banana"));

	// 17. erase_key (non-existing key)
	erased = mp1.erase_key("notfound");
	::fast_io::io::println("erase_key \"notfound\": ", erased, ", size=", mp1.size());

	// 18. erase (iterator)
	::fast_io::io::print("\n--- erase(iterator) ---\n");
	{
		auto it = mp1.find_key("cherry");
		if (it != mp1.end())
		{
			auto next = mp1.erase(it);
			::fast_io::io::println("erased \"cherry\", size=", mp1.size(), ", contains=", mp1.contains("cherry"));
			::fast_io::io::println("next element key: ", next->key());
		}
	}

	// 19. erase (range)
	::fast_io::io::print("\n--- erase(range) ---\n");
	if (mp1.size() >= 2)
	{
		auto first = mp1.begin();
		auto last = mp1.end();
		::fast_io::io::println("size before range erase: ", mp1.size());
		mp1.erase(first, last);
		::fast_io::io::println("size after range erase: ", mp1.size(), " is_empty: ", mp1.is_empty());
	}

	// 20. clear and reuse
	::fast_io::io::print("\n--- clear ---\n");
	mp1.insert_key("reuse", 1);
	::fast_io::io::println("before clear, size=", mp1.size());
	mp1.clear();
	::fast_io::io::println("after clear, size=", mp1.size(), " is_empty=", mp1.is_empty(), " capacity=", mp1.capacity());
	mp1.insert_key("after_clear", 2);
	::fast_io::io::println("after insert after clear, size=", mp1.size(), " contains=", mp1.contains("after_clear"));

	// 21. clear_destroy
	mp1.insert_key("temp", 3);
	::fast_io::io::println("before clear_destroy, size=", mp1.size(), " capacity=", mp1.capacity());
	mp1.clear_destroy();
	::fast_io::io::println("after clear_destroy, size=", mp1.size(), " capacity=", mp1.capacity(), " is_empty=", mp1.is_empty());

	// 22. reserve / capacity / max_size / is_empty
	::fast_io::io::print("\n--- capacity ---\n");
	::fast_io::str_swiss_map<::std::size_t> mp7;
	::fast_io::io::println("default max_size: ", mp7.max_size());
	mp7.reserve(100);
	::fast_io::io::println("after reserve(100), capacity=", mp7.capacity(), " is_empty=", mp7.is_empty());
	mp7.insert_key("a", 1);
	mp7.insert_key("b", 2);
	::fast_io::io::println("after 2 inserts, capacity=", mp7.capacity(), " size=", mp7.size());

	// 23. Forward iteration
	::fast_io::io::print("\n--- forward iteration ---\n");
	::fast_io::str_swiss_map<::std::size_t> mp8{{"z", 26}, {"a", 1}, {"m", 13}, {"d", 4}, {"s", 19}};
	for (auto &e : mp8)
	{
		::fast_io::io::println("  ", e.key(), " -> ", e.mapped());
	}

	// 24. Reverse iteration
	::fast_io::io::print("\n--- reverse iteration ---\n");
	for (auto i{mp8.crbegin()}, ed{mp8.crend()}; i != ed; ++i)
	{
		::fast_io::io::println("  ", i->key(), " -> ", i->mapped());
	}

	// 25. swap
	::fast_io::io::print("\n--- swap ---\n");
	::fast_io::str_swiss_map<::std::size_t> mp9{{"x", 24}, {"y", 25}};
	::fast_io::io::println("before swap: mp8 size=", mp8.size(), " mp9 size=", mp9.size());
	swap(mp8, mp9);
	::fast_io::io::println("after swap: mp8 size=", mp8.size(), " mp9 size=", mp9.size());

	// 26. operator==
	::fast_io::io::print("\n--- operator== ---\n");
	::fast_io::str_swiss_map<::std::size_t> mp10{{"a", 1}, {"b", 2}};
	::fast_io::str_swiss_map<::std::size_t> mp11{{"a", 1}, {"b", 2}};
	::fast_io::str_swiss_map<::std::size_t> mp12{{"a", 1}, {"c", 3}};
	::fast_io::io::println("equal maps: ", (mp10 == mp11));
	::fast_io::io::println("different maps: ", (mp10 == mp12));

	// 27. hash_function
	::fast_io::io::print("\n--- hash_function ---\n");
	auto h = mp10.hash_function();
	::fast_io::io::println("hash_function valid: ", true);

	// 28. Reuse after move
	::fast_io::io::print("\n--- reuse after move ---\n");
	::fast_io::str_swiss_map<::std::size_t> mp13{{"original", 1}};
	auto mp14(::std::move(mp13));
	::fast_io::io::println("moved-from size: ", mp13.size());
	mp13.insert_key("new_life", 2);
	::fast_io::io::println("reused moved-from size: ", mp13.size(), " contains: ", mp13.contains("new_life"));

	// === RAII class tests ===
	::fast_io::io::print("\n========== RAII tests ==========\n");

	// 29. RAII: basic insert and destroy via clear_destroy
	::fast_io::io::print("\n--- RAII: clear_destroy ---\n");
	{
		::fast_io::str_swiss_map<raii_mapped> raii_map;
		raii_map.reserve(20);
		raii_map.insert_key("a", raii_mapped(1));
		raii_map.insert_key("b", raii_mapped(2));
		raii_map.insert_key("c", raii_mapped(3));
		::fast_io::io::print("after 3 inserts, raii_mapped::alive=", raii_mapped::alive, " (should be 3)\n");
		if (raii_mapped::alive != 3)
		{
			::fast_io::fast_terminate();
		}
		raii_map.clear_destroy();
		::fast_io::io::print("after clear_destroy, raii_mapped::alive=", raii_mapped::alive, " (should be 0)\n");
		if (raii_mapped::alive != 0)
		{
			::fast_io::fast_terminate();
		}
	}

	// 30. RAII: erase_key destroys one element
	::fast_io::io::print("\n--- RAII: erase_key ---\n");
	{
		::fast_io::str_swiss_map<raii_mapped> raii_map;
		raii_map.reserve(20);
		raii_map.insert_key("a", raii_mapped(1));
		raii_map.insert_key("b", raii_mapped(2));
		raii_map.insert_key("c", raii_mapped(3));
		::fast_io::io::println("before erase, alive=", raii_mapped::alive);
		raii_map.erase_key("b");
		::fast_io::io::print("after erase_key \"b\", alive=", raii_mapped::alive, " (should be 2)\n");
		if (raii_mapped::alive != 2)
		{
			::fast_io::fast_terminate();
		}
		raii_map.clear_destroy();
		if (raii_mapped::alive != 0)
		{
			::fast_io::fast_terminate();
		}
	}

	// 31. RAII: erase(iterator) destroys one element
	::fast_io::io::print("\n--- RAII: erase(iterator) ---\n");
	{
		::fast_io::str_swiss_map<raii_mapped> raii_map;
		raii_map.reserve(20);
		raii_map.insert_key("a", raii_mapped(1));
		raii_map.insert_key("b", raii_mapped(2));
		raii_map.insert_key("c", raii_mapped(3));
		{
			auto it = raii_map.find_key("b");
			if (it != raii_map.end())
			{
				raii_map.erase(it);
			}
		}
		::fast_io::io::print("after erase(iterator \"b\"), alive=", raii_mapped::alive, " (should be 2)\n");
		if (raii_mapped::alive != 2)
		{
			::fast_io::fast_terminate();
		}
		raii_map.clear_destroy();
		if (raii_mapped::alive != 0)
		{
			::fast_io::fast_terminate();
		}
	}

	// 32. RAII: clear() should not destroy (keeps memory), then insert and destroy
	::fast_io::io::print("\n--- RAII: clear() then destroy ---\n");
	{
		::fast_io::str_swiss_map<raii_mapped> raii_map;
		raii_map.reserve(20);
		raii_map.insert_key("a", raii_mapped(1));
		raii_map.insert_key("b", raii_mapped(2));
		::fast_io::io::print("before clear, alive=", raii_mapped::alive, " (should be 2)\n");
		if (raii_mapped::alive != 2)
		{
			::fast_io::fast_terminate();
		}
		raii_map.clear();
		::fast_io::io::print("after clear, alive=", raii_mapped::alive, " (should be 0)\n");
		if (raii_mapped::alive != 0)
		{
			::fast_io::fast_terminate();
		}
		raii_map.insert_key("c", raii_mapped(3));
		::fast_io::io::print("after insert after clear, alive=", raii_mapped::alive, " (should be 1)\n");
		if (raii_mapped::alive != 1)
		{
			::fast_io::fast_terminate();
		}
	}
	::fast_io::io::print("after scope end, alive=", raii_mapped::alive, " (should be 0)\n");
	if (raii_mapped::alive != 0)
	{
		::fast_io::fast_terminate();
	}

	// 33. RAII: copy construction
	::fast_io::io::print("\n--- RAII: copy construction ---\n");
	{
		::fast_io::str_swiss_map<raii_mapped> raii_map;
		raii_map.reserve(20);
		raii_map.insert_key("a", raii_mapped(1));
		raii_map.insert_key("b", raii_mapped(2));
		::fast_io::io::print("original alive=", raii_mapped::alive, " (should be 2)\n");
		if (raii_mapped::alive != 2)
		{
			::fast_io::fast_terminate();
		}
		{
			auto raii_copy(raii_map);
			::fast_io::io::print("after copy, alive=", raii_mapped::alive, " (should be 4)\n");
			if (raii_mapped::alive != 4)
			{
				::fast_io::fast_terminate();
			}
		}
		::fast_io::io::print("after copy destroyed, alive=", raii_mapped::alive, " (should be 2)\n");
		if (raii_mapped::alive != 2)
		{
			::fast_io::fast_terminate();
		}
	}
	::fast_io::io::print("after scope end, alive=", raii_mapped::alive, " (should be 0)\n");
	if (raii_mapped::alive != 0)
	{
		::fast_io::fast_terminate();
	}

	// 34. RAII: move construction
	::fast_io::io::print("\n--- RAII: move construction ---\n");
	{
		::fast_io::str_swiss_map<raii_mapped> raii_map;
		raii_map.reserve(20);
		raii_map.insert_key("a", raii_mapped(1));
		raii_map.insert_key("b", raii_mapped(2));
		::fast_io::io::print("original alive=", raii_mapped::alive, " (should be 2)\n");
		if (raii_mapped::alive != 2)
		{
			::fast_io::fast_terminate();
		}
		{
			auto raii_moved(::std::move(raii_map));
			::fast_io::io::print("after move, alive=", raii_mapped::alive, " (should be 2)\n");
			if (raii_mapped::alive != 2)
			{
				::fast_io::fast_terminate();
			}
		}
		::fast_io::io::print("after moved destroyed, alive=", raii_mapped::alive, " (should be 0)\n");
		if (raii_mapped::alive != 0)
		{
			::fast_io::fast_terminate();
		}
	}

	// 35. RAII: pre-reserved large insert (no rehash, tests bulk RAII safety)
	::fast_io::io::print("\n--- RAII: bulk insert with reserve ---\n");
	{
		::fast_io::str_swiss_map<raii_mapped> raii_map;
		raii_map.reserve(50);
		for (::std::size_t i = 0; i < 20; ++i)
		{
			char ch = static_cast<char>('a' + i);
			::fast_io::string_view key(&ch, 1);
			raii_map.insert_key(key, raii_mapped(i));
		}
		::fast_io::io::println("after bulk insert, size=", raii_map.size(), " alive=", raii_mapped::alive);
		if (raii_map.size() != 20)
		{
			::fast_io::fast_terminate();
		}
		if (raii_mapped::alive != 20)
		{
			::fast_io::fast_terminate();
		}
		for (::std::size_t i = 0; i < 20; ++i)
		{
			char ch = static_cast<char>('a' + i);
			::fast_io::string_view key(&ch, 1);
			if (!raii_map.contains(key))
			{
				::fast_io::fast_terminate();
			}
		}
	}
	::fast_io::io::print("after scope end, alive=", raii_mapped::alive, " (should be 0)\n");
	if (raii_mapped::alive != 0)
	{
		::fast_io::fast_terminate();
	}

	// 36. RAII: operation sequence (insert, erase interleaved with reserve)
	::fast_io::io::print("\n--- RAII: operation sequence ---\n");
	{
		::fast_io::str_swiss_map<raii_mapped> raii_map;
		raii_map.reserve(20);
		raii_map.insert_key("x", raii_mapped(10));
		raii_map.insert_key("y", raii_mapped(20));
		raii_map.insert_key("z", raii_mapped(30));
		if (raii_mapped::alive != 3)
		{
			::fast_io::fast_terminate();
		}
		raii_map.erase_key("y");
		if (raii_mapped::alive != 2)
		{
			::fast_io::fast_terminate();
		}
		raii_map.insert_key("w", raii_mapped(40));
		if (raii_mapped::alive != 3)
		{
			::fast_io::fast_terminate();
		}
		{
			auto it = raii_map.find_key("x");
			if (it != raii_map.end())
			{
				raii_map.erase(it);
			}
		}
		if (raii_mapped::alive != 2)
		{
			::fast_io::fast_terminate();
		}
		raii_map.clear();
		if (raii_mapped::alive != 0)
		{
			::fast_io::fast_terminate();
		}
	}
	if (raii_mapped::alive != 0)
	{
		::fast_io::fast_terminate();
	}

	::fast_io::io::print("\nAll tests passed.\n");
}
