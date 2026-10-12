#include <fast_io.h>
#include <fast_io_dsal/sized_string.h>

int main()
{
	using S = ::fast_io::sized_string_zu32;
	static_assert(sizeof(S) == 16);
	static_assert(::std::same_as<S::iterator, char *>);
	static_assert(::std::same_as<S::const_iterator, char const *>);
	static_assert(noexcept(::std::declval<S &>().push_back('x')));
	static_assert(noexcept(::std::declval<S &>().operator[](0u)));
	static_assert(noexcept(::std::declval<S &>().append("abc")));
	S s;
	for (int i = 0; i != 100; ++i)
	{
		s.push_back(static_cast<char>('a' + i % 26));
	}
	if (s.size() != 100 || s[50] != 'a' + 50 % 26)
	{
		::fast_io::fast_terminate();
	}
	s.append("xyz");
	if (s.size() != 103 || s[102] != 'z')
	{
		::fast_io::fast_terminate();
	}
	S t{s};
	if (t != s)
	{
		::fast_io::fast_terminate();
	}
	auto sub = t.substr(0, 3);
	if (sub != "abc")
	{
		::fast_io::fast_terminate();
	}
	s.insert_index(0, ">>");
	if (s[0] != '>' || s.size() != 105)
	{
		::fast_io::fast_terminate();
	}
	s.erase_index(0, 2);
	if (s.size() != 103 || s[0] != 'a')
	{
		::fast_io::fast_terminate();
	}
	s.pop_back();
	if (s.size() != 102 || s.back() != 'y')
	{
		::fast_io::fast_terminate();
	}
	s.clear();
	if (!s.is_empty() || s.c_str()[0] != 0)
	{
		::fast_io::fast_terminate();
	}

	::fast_io::u8sized_string_zu32 u8{u8"abc"};
	if (u8.size() != 3)
	{
		::fast_io::fast_terminate();
	}
	::fast_io::wsized_string_zu32 ws{L"xy"};
	if (ws.size() != 2)
	{
		::fast_io::fast_terminate();
	}
	::fast_io::u32sized_string_zu32 u32{U"p"};
	if (u32.size() != 1)
	{
		::fast_io::fast_terminate();
	}
	::fast_io::sized_string_zu32 ss{"hello world"};
	if (ss.find_character('o') != 4)
	{
		::fast_io::fast_terminate();
	}
	if (ss.subview_front(5) != "hello")
	{
		::fast_io::fast_terminate();
	}
	::fast_io::sized_string_zu32 big;
	big.reserve(1000);
	for (int i = 0; i != 500; ++i)
	{
		big.push_back('z');
	}
	if (big.size() != 500 || big.capacity() < 500)
	{
		::fast_io::fast_terminate();
	}
	big.shrink_to_fit();
	// sized_string works as a strlike target for concat
	::fast_io::sized_string_zu32 cs = ::fast_io::basic_general_concat<false, char, ::fast_io::sized_string_zu32>(1, "x");
	if (cs != "1x")
	{
		::fast_io::fast_terminate();
	}
}
