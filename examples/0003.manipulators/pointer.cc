#include<fast_io.h>
#include<memory>

class dummy_class
{
public:
	void dummy_method() noexcept{}
};

struct A
{
	virtual void f() noexcept {}
};
struct B
{
	virtual void g() noexcept {}
};

struct C : A, B {};

using namespace fast_io::io;

int main()
{
	using namespace fast_io::mnp;
	int fd{3};
	int *ptr{::std::addressof(fd)};

	char const *ptr2="hello";

	void (C::*downcptr)() noexcept = &B::g;

	println("funcvw(main):",funcvw(main),"\n"
		"pointervw(ptr):",pointervw(ptr),"\n"
		"handlevw(fd):",handlevw(fd),"\n"
		"handlevw(ptr):",handlevw(ptr),"\n"
		"pointervw(ptr2):",pointervw(ptr2),"\n"
		"dec(pointervw(ptr2)):",dec(pointervw(ptr2)),"\n"
		"os_c_str(ptr2):",os_c_str(ptr2),"\n"
		"methodvw(&dummy_class::dummy_method):",methodvw(&dummy_class::dummy_method),"\n"
		"dec(methodvw(&dummy_class::dummy_method)):",dec(methodvw(&dummy_class::dummy_method)),"\n"
		"methodvw(downcptr):",methodvw(downcptr));
}

/*
funcvw(main):0x00007ff75abf27c0
pointervw(ptr):0x000000c232d0fa9c
handlevw(fd):3
handlevw(ptr):0x000000c232d0fa9c
pointervw(ptr2):0x00007ff75abf4540
dec(pointervw(ptr2)):140700356134208
os_c_str(ptr2):hello
methodvw(&dummy_class::dummy_method):0x00007ff75abf34b0+0x0
dec(methodvw(&dummy_class::dummy_method)):140700356129968+0
methodvw(downcptr):0x0000000000000001+0x8
*/
