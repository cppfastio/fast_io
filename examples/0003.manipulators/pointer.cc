#include<fast_io.h>
#include<memory>

class dummy_class
{
public:
	void dummy_method() noexcept{}
};

int main()
{
	using namespace fast_io::mnp;
	int fd{3};
	int *ptr{::std::addressof(fd)};

	char const *ptr2="hello";

	println("funcvw(main):",funcvw(main),"\n"
		"pointervw(ptr):",pointervw(ptr),"\n"
		"handlevw(fd):",handlevw(fd),"\n"
		"handlevw(ptr):",handlevw(ptr),"\n"
		"pointervw(ptr2):",pointervw(ptr2),"\n"
		"os_c_str(ptr2):",os_c_str(ptr2),"\n"
		"methodvw(&dummy_class::dummy_method):",methodvw(&dummy_class::dummy_method));
}

/*
funcvw(main):0x000056456b0ffdf0
pointervw(ptr):0x00007fff97f226cc
handlevw(fd):3
handlevw(ptr):0x00007fff97f226cc
pointervw(ptr2):0x000056456b0fe914
os_c_str(ptr2):hello
methodvw(&dummy_class::dummy_method):0x000056456b100640+0x0
*/
