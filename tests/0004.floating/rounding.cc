#include<fast_io.h>
#include<charconv>

int main()
{
	println(4.8371439708550857e+306);
	char buffer[30];
	auto [p,ec]=std::to_chars(buffer,buffer+30,4.8371439708550857e+306);
	write(fast_io::c_stdout(),buffer,p);
}