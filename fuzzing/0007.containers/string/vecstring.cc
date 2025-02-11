/**
 * This code was generated using Microsoft Copilot.
 */

#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <fast_io_dsal/string_view.h>
#include <fast_io_dsal/string.h>
#include <fast_io_dsal/vector.h>

template <typename CharT>
inline void FuzzVectorOfString(uint8_t const *data, size_t size)
{
	// Adjust size according to the character type
	size /= sizeof(CharT);

	// Convert input data to a string view
	fast_io::basic_string_view<CharT> sv(reinterpret_cast<CharT const *>(data), size);

	// Initialize your vector of strings
	fast_io::vector<fast_io::basic_string<CharT>> vec;

	// Perform various operations on the vector
	vec.emplace_back(sv); // Emplace back a new string with the string view

	// Test emplace_back with a new string
	fast_io::basic_string<CharT> new_string(sv);
	vec.emplace_back(new_string);

	// Test pop_back method
	if (!vec.is_empty())
	{
		vec.pop_back();
	}

	// Test insert_index method
	if (!vec.is_empty())
	{
		vec.insert_index(0, new_string);
	}

	// Test element access using bounds-checked indexing
	if (!vec.is_empty())
	{
		vec[0] = new_string;
	}

	// Test resize method
	vec.resize(10);
	vec.resize(0);
	vec.resize(5);

	// Test clear method
	vec.clear();

	// Final clear_destroy to test memory deallocation
	vec.clear_destroy();

	// Perform some final operations
	for (size_t i = 0; i < 10; ++i)
	{
		vec.emplace_back(sv);
	}

	vec.clear();
	vec.clear_destroy();
}

extern "C" int LLVMFuzzerTestOneInput(uint8_t const *data, size_t size)
{
	FuzzVectorOfString<char>(data, size);
	FuzzVectorOfString<wchar_t>(data, size);
	FuzzVectorOfString<char8_t>(data, size);
	FuzzVectorOfString<char16_t>(data, size);
	FuzzVectorOfString<char32_t>(data, size);
	return 0;
}
