#include<fast_io.h>
#include<fast_io_driver/qt.h>

using namespace fast_io::io;

int main()
{
	QString qstr;
	scan(fast_io::mnp::strlike_get(qstr));
	println(qstr);
}
