#include <fast_io.h>
#include <fast_io_dsal/string.h>
#include <fast_io_dsal/string_view.h>

int main()
{
	::fast_io::u8string str(u8"   \n\n\t   sdgsdg sdgsds    \n\t \n"),
		str1(str), str2(str), str3(str);

	// Apply different trim methods
	str.trim_c_space();                             // Trim both prefix and suffix spaces
	str1.trim_prefix_c_space();                     // Trim only leading spaces
	str2.trim_suffix_c_space();                     // Trim only trailing spaces
	str3.trim(::fast_io::char_category::c_blank{}); // Trim blank characters

	// Print results
	::fast_io::io::println(::fast_io::u8c_stdout(), u8"Original: ", str3);
	::fast_io::io::println(::fast_io::u8c_stdout(), u8"Trimmed (Full): ", str, u8" | Size: ", str.size());
	::fast_io::io::println(::fast_io::u8c_stdout(), u8"Trimmed (Prefix): ", str1, u8" | Size: ", str1.size());
	::fast_io::io::println(::fast_io::u8c_stdout(), u8"Trimmed (Suffix): ", str2, u8" | Size: ", str2.size());
	::fast_io::io::println(::fast_io::u8c_stdout(), u8"Trimmed (Blank Only): ", str3, u8" | Size: ", str3.size());

	::fast_io::u8string_view u8strvw(u8" testing trim view    \n\t\n");
	::fast_io::io::println(::fast_io::u8c_stdout(), u8strvw.trim_prefix_subview_c_space());
	return 0;
}
