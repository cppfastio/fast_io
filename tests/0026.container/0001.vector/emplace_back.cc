#include <cassert>
#include <string>
#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

struct President
{
	std::string name;
	std::string country;
	int year;

	President(std::string p_name, std::string p_country, int p_year)
		: name(std::move(p_name)), country(std::move(p_country)), year(p_year)
	{
		print("I am being constructed.\n");
	}

	President(President &&other)
		: name(std::move(other.name)), country(std::move(other.country)), year(other.year)
	{
		print("I am being moved.\n");
	}

	President &operator=(President const &other) = default;
};

int main()
{
	fast_io::vector<President> elections;
	print("emplace_back:\n");
	auto &ref = elections.emplace_back("Nelson Mandela", "South Africa", 1994);
	assert(ref.year == 1994 && "uses a reference to the created object (C++17)");

	fast_io::vector<President> reElections;
	print("\npush_back:\n");
	reElections.push_back(President("Franklin Delano Roosevelt", "the USA", 1936));

	print("\nContents:\n");
	for (President const &president : elections)
	{
		print(president.name, " was elected president of ", president.country, " in ", president.year, ".\n");
	}

	for (President const &president : reElections)
	{
		print(president.name, " was re-elected president of ", president.country, " in ", president.year, ".\n");
	}
}