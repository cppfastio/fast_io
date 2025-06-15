#include <fast_io_dsal/string.h>

extern "C" int LLVMFuzzerTestOneInput(char const *data, size_t size)
{
	// Convert fuzzer input data into three separate basic_string copies
	::fast_io::string str1(data, data + size);
	::fast_io::string str2(str1);
	::fast_io::string str3(str1);

	// Apply different trim orders to maximize variation in testing
	str1.trim(fast_io::char_category::c_space{})
		.trim_prefix(fast_io::char_category::c_space{})
		.trim_suffix(fast_io::char_category::c_space{});

	str2.trim_prefix(fast_io::char_category::c_space{})
		.trim_suffix(fast_io::char_category::c_space{})
		.trim(fast_io::char_category::c_space{});

	str3.trim_suffix(fast_io::char_category::c_space{})
		.trim(fast_io::char_category::c_space{})
		.trim_prefix(fast_io::char_category::c_space{});

	using char32_may_alias
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char32_t;
	::fast_io::u32string str4(reinterpret_cast<char32_may_alias const *>(data),
							  reinterpret_cast<char32_may_alias const *>(data) + (size / sizeof(char32_t)));
	::fast_io::u32string str5(str4);
	::fast_io::u32string str6(str4);

	// Apply different trim orders to maximize variation in testing
	str4.trim(fast_io::char_category::c_space{})
		.trim_prefix(fast_io::char_category::c_space{})
		.trim_suffix(fast_io::char_category::c_space{});

	str5.trim_prefix(fast_io::char_category::c_space{})
		.trim_suffix(fast_io::char_category::c_space{})
		.trim(fast_io::char_category::c_space{});

	str6.trim_suffix(fast_io::char_category::c_space{})
		.trim(fast_io::char_category::c_space{})
		.trim_prefix(fast_io::char_category::c_space{});

	return 0; // Indicate successful execution
}
