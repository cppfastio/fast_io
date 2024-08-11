#include<fast_io_legacy.h>
#ifdef __GLIBCXX__
#include<ext/stdio_sync_filebuf.h>
#include<ext/stdio_filebuf.h>
#endif
#include<iostream>
#include<cstring>

int main()
{
	::fast_io::wfilebuf_file fb(u8"testgetfd.txt",::fast_io::open_mode::out);

	::fast_io::wstreambuf_io_observer smiob{fb.fb};
	println(::fast_io::mnp::handlevw(static_cast<::fast_io::wposix_io_observer>(smiob).fd));

	::fast_io::streambuf_io_observer siob{::std::cout.rdbuf()};
	println(::fast_io::mnp::handlevw(static_cast<::fast_io::posix_io_observer>(siob).fd));

	::std::ios::sync_with_stdio(false);
	siob.fb=::std::cerr.rdbuf();
	println(::fast_io::mnp::handlevw(static_cast<::fast_io::posix_io_observer>(siob).fd));

	::std::ios::sync_with_stdio(true);
	siob.fb=::std::cin.rdbuf();
	println(::fast_io::mnp::handlevw(static_cast<::fast_io::posix_io_observer>(siob).fd));

	using namespace ::fast_io::mnp;
	auto& coutrdbuf{*std::cout.rdbuf()};
	auto& wcoutrdbuf{*std::wcout.rdbuf()};
	auto& cinrdbuf{*std::cin.rdbuf()};
	auto& wcinrdbuf{*std::wcin.rdbuf()};
	println(
		"char: ",os_c_str(typeid(coutrdbuf).name()),"\n"
		"wchar_t: ",os_c_str(typeid(wcoutrdbuf).name()),"\n"
		"char: ",os_c_str(typeid(cinrdbuf).name()),"\n"
		"wchar_t: ",os_c_str(typeid(wcinrdbuf).name()),"\n\n"

#ifdef __GLIBCXX__
		"char: ",os_c_str(typeid(__gnu_cxx::stdio_sync_filebuf<char>).name()),"\n"
		"wchar_t: ",os_c_str(typeid(__gnu_cxx::stdio_sync_filebuf<wchar_t>).name()),"\n"
		"char8_t: ",os_c_str(typeid(__gnu_cxx::stdio_sync_filebuf<char8_t>).name()),"\n"
		"char16_t: ",os_c_str(typeid(__gnu_cxx::stdio_sync_filebuf<char16_t>).name()),"\n"
		"char32_t: ",os_c_str(typeid(__gnu_cxx::stdio_sync_filebuf<char32_t>).name()),"\n\n"

		"char: ",os_c_str(typeid(__gnu_cxx::stdio_filebuf<char>).name()),"\n"
		"wchar_t: ",os_c_str(typeid(__gnu_cxx::stdio_filebuf<wchar_t>).name()),"\n"
		"char8_t: ",os_c_str(typeid(__gnu_cxx::stdio_filebuf<char8_t>).name()),"\n"
		"char16_t: ",os_c_str(typeid(__gnu_cxx::stdio_filebuf<char16_t>).name()),"\n"
		"char32_t: ",os_c_str(typeid(__gnu_cxx::stdio_filebuf<char32_t>).name()),"\n\n"
#endif

		"char: ",os_c_str(typeid(std::basic_filebuf<char>).name()),"\n"
		"wchar_t: ",os_c_str(typeid(std::basic_filebuf<wchar_t>).name()),"\n"
		"char8_t: ",os_c_str(typeid(std::basic_filebuf<char8_t>).name()),"\n"
		"char16_t: ",os_c_str(typeid(std::basic_filebuf<char16_t>).name()),"\n"
		"char32_t: ",os_c_str(typeid(std::basic_filebuf<char32_t>).name())
		);
}