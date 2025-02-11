/**
 * This code was generated using Microsoft Copilot.
 */

#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <fast_io_dsal/string_view.h>
#include <fast_io_dsal/string.h>

template <typename CharT>
inline void FuzzString(uint8_t const *data, size_t size)
{
	// Adjust size according to the character type
	size /= sizeof(CharT);

	// Convert input data to a string view
	fast_io::basic_string_view<CharT> sv(reinterpret_cast<CharT const *>(data), size);

	// Initialize your string with the string view
	fast_io::basic_string<CharT> s(sv);

	// Perform various operations on the string
	s.append(sv);

	// Test insert method with a string view
	if (s.size() >= 5)
	{
		s.insert(s.cbegin() + 3, sv);
		s.insert_index(3, sv);
	}

	// Test replace_index method within valid range
	if (s.size() >= 10)
	{
		s.replace_index(2, 8, sv);
	}

	// Access elements using bounds-checked indexing
	if (s.size() > 0)
	{
		s[0] = 'x'; // Test element modification with bounds checking
	}

	// Perform unchecked element access if size is valid
	if (s.size() > 0)
	{
		s.index_unchecked(0) = 'y'; // Test unchecked element access
	}

	// Test push_back and pop_back methods
	if (s.size() > 0)
	{
		CharT last_char = s[s.size() - 1]; // Store the last character
		s.push_back(last_char);            // Push back the last character
		s.pop_back();                      // Pop back the last character

		CharT last_char_back = s.back(); // Store the last character using back()
		s.push_back(last_char_back);     // Push back the last character using back()
		s.pop_back();                    // Pop back the last character
	}

	// Test string comparison
	fast_io::basic_string<CharT> other_string(s);
	if ((s <=> other_string) != 0)
	{
		std::abort();
	}

	// Test clear method
	s.clear();

	// Test clear_destroy method
	s.clear_destroy();

	// Continue operations after clear_destroy to ensure correctness
	if constexpr (std::is_same_v<CharT, char>)
	{
		s.append("additional data after clear_destroy");
	}
	else if constexpr (std::is_same_v<CharT, wchar_t>)
	{
		s.append(L"additional data after clear_destroy");
	}
	else if constexpr (std::is_same_v<CharT, char8_t>)
	{
		s.append(u8"additional data after clear_destroy");
	}
	else if constexpr (std::is_same_v<CharT, char16_t>)
	{
		s.append(u"additional data after clear_destroy");
	}
	else if constexpr (std::is_same_v<CharT, char32_t>)
	{
		s.append(U"additional data after clear_destroy");
	}

	s.insert(s.cbegin(), sv);
	if (s.size() >= 15)
	{
		s.replace_index(5, 12, sv);
	}

	// Test resize method
	s.resize(50);
	s.resize(0);
	s.resize(100);

	// Test empty string operations
	fast_io::basic_string<CharT> empty_string;
	empty_string.append(sv);
	empty_string.insert(empty_string.cbegin(), sv);
	empty_string.insert_index(0, sv);
	if (empty_string.size() >= 5)
	{
		empty_string.replace_index(0, 5, sv);
	}
	empty_string.clear();
	empty_string.clear_destroy();

	// Final clear_destroy to test memory deallocation
	s.clear_destroy();

	// Perform some final operations
	if constexpr (std::is_same_v<CharT, char>)
	{
		s.append("final test");
	}
	else if constexpr (std::is_same_v<CharT, wchar_t>)
	{
		s.append(L"final test");
	}
	else if constexpr (std::is_same_v<CharT, char8_t>)
	{
		s.append(u8"final test");
	}
	else if constexpr (std::is_same_v<CharT, char16_t>)
	{
		s.append(u"final test");
	}
	else if constexpr (std::is_same_v<CharT, char32_t>)
	{
		s.append(U"final test");
	}

	s.clear();
	s.clear_destroy();
}

extern "C" int LLVMFuzzerTestOneInput(uint8_t const *data, size_t size)
{
	FuzzString<char>(data, size);
	FuzzString<wchar_t>(data, size);
	FuzzString<char8_t>(data, size);
	FuzzString<char16_t>(data, size);
	FuzzString<char32_t>(data, size);
	return 0;
}
