#include <string>
#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	fast_io::vector<std::string> letters;

	letters.push_back("abc");
	std::string s{"def"};
	letters.push_back(std::move(s));

	print("std::vector letters holds: ");
	for (auto &&e : letters)
	{
		print("\"", e, "\" ");
	}

	print("\nMoved-from string s holds: \"", s, "\"\n");
}