#include<fast_io.h>

void test_nt_stdout()
{
	print(::fast_io::nt_stdout(),"Hello World from nt\n");
}

void test_win32_stdout()
{
	print(::fast_io::win32_stdout(),"Hello World from win32\n");
}

void test_posix_stdout()
{
	print(::fast_io::posix_stdout(),"Hello World from posix\n");
}

int main()
{
	test_nt_stdout();
	test_win32_stdout();
	test_posix_stdout();
}