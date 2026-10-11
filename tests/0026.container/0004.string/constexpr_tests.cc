#include <fast_io_dsal/string.h>

// Tests that default construction (reset_imp) works in constexpr context.
constexpr bool default_construct()
{
	::fast_io::u8string s{};
	return s.is_empty();
}
static_assert(default_construct());

#if !(defined _MSC_VER && !defined __clang__)

// reserve at compile time (exercises string_heap_dilate_uncheck constexpr path)
constexpr bool reserve_constexpr()
{
	::fast_io::u8string s{};
	s.reserve(100);
	return s.capacity() >= 100;
}
static_assert(reserve_constexpr());

#endif

#if defined __GNUC__ && !defined __clang__
// following tests compile with clang -fexperimental-new-constant-interpreter flag can pass
// However, the default clang constant interpreter sucks

// push_back at compile time (exercises push_back + grow_twice constexpr path)
constexpr bool push_back_constexpr()
{
	::fast_io::u8string s{};
	s.push_back('a');
	s.push_back('b');
	return s.size() == 2 && s[0] == 'a' && s[1] == 'b';
}
static_assert(push_back_constexpr());

// Multiple push_back with growth at compile time
constexpr bool push_back_many_constexpr()
{
	::fast_io::u8string s{};
	for (char c = 'a'; c <= 'z'; ++c)
	{
		s.push_back(c);
	}
	return s.size() == 26;
}
static_assert(push_back_many_constexpr());

// clear at compile time
constexpr bool clear_constexpr()
{
	::fast_io::u8string s{};
	s.push_back('a');
	s.clear();
	return s.is_empty();
}
static_assert(clear_constexpr());

#endif

int main()
{
	return 0;
}
