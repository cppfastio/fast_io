#include<fast_io.h>
#include<fast_io_driver/qt.h>
#include<QtCore/QVariant>

using namespace fast_io::io;

int main()
{
	QVariant var=4.503;
	println("var:",var);
}