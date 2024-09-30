﻿#include <fast_io_dsal/string.h>
#include <fast_io.h>

int main()
{
	constexpr ::fast_io::containers::basic_string_view<char8_t> u8vw(u8"hello world");
	::fast_io::u8string str(u8"abcdd");
	::fast_io::u8string str3(str);
	str.assign(u8vw);
	::fast_io::u8string str2(str);
	::fast_io::u8string str4(str3);
	str4 = str2;
	::fast_io::u8string str5;
	::fast_io::u8string str6, str7;
	str6 = str5;
	str5 = str4;
	str7.assign(u8"hello you");
	::fast_io::u8string str8;
	str8.assign_characters(20, u8'b');
	str8.append(str7);
	str8.append(str8); // self append
	::fast_io::u8string str9;
	str9.append(str9); // self append for empty string
	str9.append(str7);
	str9.append(::fast_io::u8concat_fast_io(u8"concat:", str9.size(), u8"\tok"));
	constexpr ::fast_io::containers::basic_cstring_view<char8_t> u8cstrvw(u8"hello c_str");
	::fast_io::u8string str10(u8cstrvw);
	constexpr char8_t const *oldstylecstr{u8"old style char8_t coonst* ptr"};
	::fast_io::u8string str11(::fast_io::mnp::os_c_str(oldstylecstr));
	str11.insert_index(4, u8"stupid ");
	str11.insert_index(4, u8"shit ");
	auto str12(str11.substr_front(3));
	auto str13(str11.substr_back(3));
	auto str14(str11.substr(4,11));
	::fast_io::io::println(
		::fast_io::u8c_stdout(),
		u8"u8vw=", u8vw,
		u8"\nstr=", str,
		u8"\nstr2=", str2,
		u8"\nstr3=", str3,
		u8"\nstr4=", str4,
		u8"\nstr5=", str5,
		u8"\nstr6=", str6,
		u8"\nstr7=", str7,
		u8"\nstr8=", str8,
		u8"\nstr9=", str9,
		u8"\nstr10=", str10,
		u8"\nstr11=", str11,
		u8"\nstr12=", str12,
		u8"\nstr13=", str13,
		u8"\nstr14=", str14,
		u8"\nstr4==str5:",str4==str5,
		u8"\nstr10<=>str14:",str10<=>str14,
		u8"\nstr4==u8\"hello world\":",str4==u8"hello world",
		u8"\nstr11<=>u8\"hello world\":",str11==u8"hello world",
		u8"\nstr4==u8cstrvw:",str4==u8cstrvw,
		u8"\nstr10<=>u8cstrvw:",str10<=>u8cstrvw);
}