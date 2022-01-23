#include<string>
#include<atlfile.h>
#include<fast_io_driver/mfc.h>
#include<fast_io.h>

int main()
{
	fast_io::atl_file file(u"atl.txt",fast_io::open_mode::out);
	print(file,"Hello World\n");
}
