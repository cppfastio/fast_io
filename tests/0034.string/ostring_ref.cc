#include <string>
#include <fast_io.h>
#include <fast_io_dsal/string.h>
#include <fast_io_dsal/string_view.h>

int main()
{
	using namespace fast_io::io;
	::std::string output1;
	{
	::fast_io::ostring_ref_std obf{__builtin_addressof(output1)};
	print(obf, "CREATE TABLE t1(a INTEGER, b INTEGER, c VARCHAR(100));\n");
	for (::std::size_t i{1}; i <= 1000; ++i)
	{
		auto val{i};
		print(obf, "INSERT INTO t1 VALUES(", i, ",", val, ",\'", val,
			  "\');\n");
	}
	}
	::fast_io::string output2;
	{
	::fast_io::ostring_ref_fast_io obf{__builtin_addressof(output2)};
	print(obf, "CREATE TABLE t1(a INTEGER, b INTEGER, c VARCHAR(100));\n");
	for (::std::size_t i{1}; i <= 1000; ++i)
	{
		auto val{i};
		print(obf, "INSERT INTO t1 VALUES(", i, ",", val, ",\'", val,
			  "\');\n");
	}
	}
	println(output1,output2,
		"output1<=>output2:",
		::fast_io::string_view(::fast_io::freestanding::from_range, output1)<=>
		::fast_io::string_view(::fast_io::freestanding::from_range, output2));
}
