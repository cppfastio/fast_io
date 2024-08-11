#include <fast_io.h>
#include <fast_io_i18n.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	fast_io::vector<char> q;
	fast_io::native_l10n l10n{"en_US.UTF-8"};
	println(imbue(l10n, fast_io::c_stdout()), "Maximum size of a std::vector is ", q.max_size());
}