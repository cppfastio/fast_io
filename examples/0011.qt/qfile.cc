#include<fast_io.h>
#include<fast_io_driver/qt.h>

using namespace fast_io::io;

int main()
{
	fast_io::qt_file file("a.txt",fast_io::open_mode::out);
	print(file,"Hello World",20," die\n");
}
