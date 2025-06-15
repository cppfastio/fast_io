#include <fast_io.h>
#include <fast_io_dsal/string.h>

int main()
{
	::fast_io::u8string ustr(u8"abcdefwegeABCDwgwasfsa 325235 fhasdfhsdh"),
		ustr_upper(ustr),
		ustr_lower(ustr),
		numbers(u8"1234567890"),
		symbols(u8"!@#$%^&*()"),
		empty;

	// Apply transformations
	::fast_io::char_category::ranges::to_c_upper(ustr_upper);
	::fast_io::char_category::ranges::to_c_lower(ustr_lower);
	::fast_io::char_category::ranges::to_c_upper(numbers);
	::fast_io::char_category::ranges::to_c_upper(symbols);
	::fast_io::char_category::ranges::to_c_upper(empty);

	// Print all results in one println
	::fast_io::io::println(::fast_io::u8c_stdout(),
						   u8"Original: ", ustr,
						   u8"\nUppercase: ", ustr_upper,
						   u8"\nLowercase: ", ustr_lower,
						   u8"\nNumbers (unchanged): ", numbers,
						   u8"\nSymbols (unchanged): ", symbols,
						   u8"\nEmpty string: ", empty);

	return 0;
}
