#include <fast_io_dsal/list.h>
#include <fast_io.h>

struct X
{
	X()
	{
		::fast_io::print("default\n");
	}

	X(X const &)
	{
		::fast_io::print("copy\n");
	}

	X(X &&)
	{
		::fast_io::print("move\n");
	}

	~X()
	{
		::fast_io::print("destruct\n");
	}

	X &operator=(X const &)
	{
		::fast_io::print("copy assign\n");
		return *this;
	}

	X &operator=(X &&)
	{
		::fast_io::print("move assign\n");
		return *this;
	}
};

int main()
{
	// TODO copy X here, can it be fixed? (caused by std::initilizer_list)
	::fast_io::list<X> const l1{X{}};

	return 0;
}
