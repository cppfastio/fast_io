#include <fast_io.h>
#include <fast_io_dsal/str_swiss_set.h>
#include <fast_io_dsal/string_view.h>

int main()
{
	static constexpr ::fast_io::cstring_view to_insert[]{
		::fast_io::cstring_view("hello"),
		::fast_io::cstring_view("world"),
		::fast_io::cstring_view("foo"),
		::fast_io::cstring_view("bar"),
		::fast_io::cstring_view("baz"),
		::fast_io::cstring_view("quux"),
		::fast_io::cstring_view("corge"),
		::fast_io::cstring_view("grault"),
#if 1

		::fast_io::cstring_view("garply"),
		::fast_io::cstring_view("waldo"),
		::fast_io::cstring_view("fred"),
		::fast_io::cstring_view("plugh"),
		::fast_io::cstring_view("xyzzy"),
		::fast_io::cstring_view("thud"),
		::fast_io::cstring_view("abcdefghijklmnop"),
		::fast_io::cstring_view("the quick brown fox jumps over the lazy dog"),
		::fast_io::cstring_view("lorem ipsum dolor sit amet consectetur adipiscing elit"),
		::fast_io::cstring_view("a"),
		::fast_io::cstring_view("ab"),
		::fast_io::cstring_view("abc"),
		::fast_io::cstring_view("abcd"),
		::fast_io::cstring_view("abcde"),
		::fast_io::cstring_view("abcdef"),
		::fast_io::cstring_view("abcdefg"),
		::fast_io::cstring_view("abcdefgh"),
		::fast_io::cstring_view("abcdefghi"),
		::fast_io::cstring_view("abcdefghij"),
		::fast_io::cstring_view("abcdefghijk"),
		::fast_io::cstring_view("abcdefghijkl"),
		::fast_io::cstring_view("abcdefghijklm"),
		::fast_io::cstring_view("abcdefghijklmn"),
		::fast_io::cstring_view("abcdefghijklmno"),
		::fast_io::cstring_view("1234567890"),
		::fast_io::cstring_view("0987654321"),
		::fast_io::cstring_view("test"),
		::fast_io::cstring_view("testing"),
		::fast_io::cstring_view("tester"),
		::fast_io::cstring_view("tested"),
		::fast_io::cstring_view("testes")
#endif
	};
	// These should NOT be found
	static constexpr ::fast_io::cstring_view not_to_insert[]{
		::fast_io::cstring_view("notinserted"),
		::fast_io::cstring_view(""),
		::fast_io::cstring_view("hell"),
		::fast_io::cstring_view("helloo"),
		::fast_io::cstring_view("hello world"),
		::fast_io::cstring_view("foobar"),
		::fast_io::cstring_view("tes"),
		::fast_io::cstring_view("testin"),
		::fast_io::cstring_view("xyz"),
		::fast_io::cstring_view("THUD")};
	::fast_io::str_swiss_set sset;

	for (auto e : to_insert)
	{
		sset.insert_key(e);
	}
	::fast_io::io::print("::fast_io::str_swiss_set size: ", sset.size(),
						 "\nto_insert:\n");
	for (auto e : to_insert)
	{
		::fast_io::io::println(e, " ", sset.contains(e));
	}
	::fast_io::io::print("not_to_insert:\n");
	for (auto e : not_to_insert)
	{
		::fast_io::io::println(e, " ", sset.contains(e));
	}

	::fast_io::io::print("::fast_io::str_swiss_set iteration:\n");
	for (auto e : sset)
	{
		::fast_io::io::println(e);
	}

	::fast_io::io::print("::fast_io::str_swiss_set reverse iteration:\n");
	for (auto i{sset.crbegin()}, ed{sset.crend()}; i != ed; ++i)
	{
		::fast_io::io::println(*i);
	}

	sset.erase_key("grault");
	::fast_io::io::print("::fast_io::str_swiss_set iteration after erase_key, size=", sset.size(), ":\n");
	for (auto e : sset)
	{
		::fast_io::io::println(e);
	}

	auto s2(sset);
	::fast_io::io::print("::fast_io::str_swiss_set clone, size=", sset.size(), ":\n");
	for (auto e : s2)
	{
		::fast_io::io::println(e);
	}
}
