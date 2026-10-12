#include <fast_io.h>
// #include <fast_io_i18n.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
#if 0
	fast_io::vector<char> q;
	auto const *l10n{::fast_io::l10n::load_l10n(u8"en_US.UTF-8")};
	println(imbue(l10n, fast_io::c_stdout()), "Maximum size of a std::vector is ", q.max_size());
#endif
}