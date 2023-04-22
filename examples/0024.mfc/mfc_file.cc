#include<string>
#include<afx.h>
#include<fast_io_driver/mfc.h>
#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	fast_io::mfc_file file(u"mfc.txt",fast_io::open_mode::out);
	print(file,"Hello World\n");
}
