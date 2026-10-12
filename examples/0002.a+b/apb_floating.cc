import fast_io;

using namespace fast_io::iomnp;

int main()
try
{
	double a, b;
	scan(a, b);
	println(a + b);
}
catch throws(std::error e)
{
	perrln(e);
	return 1;
}
