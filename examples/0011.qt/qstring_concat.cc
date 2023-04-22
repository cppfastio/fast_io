#include<fast_io.h>
#include<fast_io_driver/qt.h>

using namespace fast_io::io;

int main()
{
	QString qstr=fast_io::u16concat_qt_qstring(u"Hello QString\n",24.43624362634,u"124124");
	print(qstr);
}