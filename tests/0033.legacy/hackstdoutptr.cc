#include <fast_io.h>

int main()
{
	using namespace fast_io::mnp;
	using namespace fast_io::io;
	fast_io::c_io_observer_unlocked ciob{stdout};
	perrln(pointervw(obuffer_begin(ciob)), " ", pointervw(obuffer_curr(ciob)), " ", pointervw(obuffer_end(ciob)));
	println(40);
	perrln(pointervw(obuffer_begin(ciob)), " ", pointervw(obuffer_curr(ciob)), " ", pointervw(obuffer_end(ciob)));
	println(40);
	perrln(pointervw(obuffer_begin(ciob)), " ", pointervw(obuffer_curr(ciob)), " ", pointervw(obuffer_end(ciob)));
}