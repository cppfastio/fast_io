#include <string>
#include <fast_io.h>

int main()
{
	::std::string str("asfasfjaspofjaspofajsfpoasfsaf");
	::fast_io::details::string_hack::set_end_ptr(str, str.data() + 5); // resize to 5
}