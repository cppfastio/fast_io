#include<fast_io.h>

class dummy_class
{
public:
	void dummy_method() noexcept{}
};

int main()
{
	using namespace fast_io::mnp;
	int fd{3};
	int *ptr{&fd};

	char const *ptr2="hello";

	println("funcvw(main):",funcvw(main),"\n"
		"pointervw(ptr):",pointervw(ptr),"\n"
		"handlevw(fd):",handlevw(fd),"\n"
		"handlevw(ptr):",handlevw(ptr),"\n"
		"pointervw(ptr2):",pointervw(ptr2),"\n"
		"os_c_str(ptr2):",os_c_str(ptr2),"\n"
		"methodvw(&dummy_class::dummy_method):",methodvw(&dummy_class::dummy_method));
}