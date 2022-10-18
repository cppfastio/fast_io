#include<string>
#include<fast_io.h>

int main()
{
	std::string str=fast_io::concat("The "
		"fart is ",3," years old.\n"
		"Leeroy!!!!","Jenkins!!!\n");
	print(str);
}

