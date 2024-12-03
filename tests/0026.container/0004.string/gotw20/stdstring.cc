/*
 * herb sutter's http://www.gotw.ca/gotw/020.htm
 * */
#include <string>
#include <cstdint>
#include <iostream>

using String = ::std::string;
using ::std::endl;
using ::std::cout;

struct Employee
{
	String title;
	::std::int64_t salary;
	String firstname;
	String lastname;
	auto const &Title() const noexcept
	{
		return title;
	}
	auto const &Salary() const noexcept
	{
		return salary;
	}
	auto const &First() const noexcept
	{
		return firstname;
	}
	auto const &Last() const noexcept
	{
		return lastname;
	}
};

String EvaluateSalaryAndReturnName(Employee e)
{
	if (e.Title() == "CEO" || e.Salary() > 100000)
	{
		cout << e.First() <<  " "
		       	<< e.Last() <<  " is overpaid" << endl;
	}
	return e.First() + " " + e.Last();
}

int main()
{}
