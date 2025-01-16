#include <fast_io.h>

int main()
{
	using namespace fast_io::io;
	::std::string output;
	::fast_io::ostring_ref obf{__builtin_addressof(output)};
	print(obf, "CREATE TABLE t1(a INTEGER, b INTEGER, c VARCHAR(100));\n");
	for (::std::size_t i{1}; i <= 1000; ++i)
	{
		auto val{i};
		print(obf, "INSERT INTO t1 VALUES(", i, ",", val, ",\'", val,
			  "\');\n");
	}
	println(output);
}
