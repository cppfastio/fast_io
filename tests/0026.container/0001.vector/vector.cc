#include <string>
#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	// C++11 initializer list syntax:
	fast_io::vector<std::string> words1{"the", "frogurt", "is", "also", "cursed"};
	print("1: {", rgvw(words1, ", "), "}\n");

	// words2 == words1
	fast_io::vector<std::string> words2(words1.begin(), words1.end());
	print("2: {", rgvw(words2, ", "), "}\n");

	// words3 == words1
	fast_io::vector<std::string> words3(words1);
	print("3: {", rgvw(words3, ", "), "}\n");

	// words4 is {"Mo", "Mo", "Mo", "Mo", "Mo"}
	fast_io::vector<std::string> words4(5, "Mo");
	print("4: {", rgvw(words4, ", "), "}\n");

	auto const rg = {"cat", "cow", "crow"};
	fast_io::vector<std::string> words5(std::from_range, rg); // overload (11)
	print("5: {", rgvw(words5, ", "), "}\n");
}