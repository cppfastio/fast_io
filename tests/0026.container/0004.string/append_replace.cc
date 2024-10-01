/*
An example from C++ Primer 5th Edition
*/
#include <fast_io_dsal/string.h>
#include <fast_io.h>

int main()
{
	::fast_io::string s("C++ Primer"), s2 = s; // initialize s and s2 to "C++ Primer"
	s.insert_index(s.size(), " 4th Ed.");      // s == "C++ Primer 4th Ed."
	s2.append(" 4th Ed.");                     // equivalent: appends " 4th Ed." to s2; s == s2
	// equivalent way to replace "4th" by "5th"
	s.erase_index(11, 14);     // s == "C++ Primer Ed."
	s.insert_index(11, "5th"); // s == "C++ Primer 5th Ed."
	// starting at position 11, erase three characters and then insert "5th"
	s2.replace_index(11, 14, "5th");  // equivalent: s == s2
	::fast_io::io::println("equivalent: s <=> s2: ",s<=>s2);
	s.replace_index(11, 14, "Fifth"); // s == "C++ Primer Fifth Ed."
	::fast_io::io::println("s=", s, "\ns2=", s2);
}