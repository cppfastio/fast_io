#include <fast_io.h>
#include <fast_io_dsal/str_swiss_set.h>

thread_local ::fast_io::str_swiss_set sset;
thread_local ::std::size_t n{};

extern "C" int LLVMFuzzerTestOneInput(::std::uint8_t *data, ::std::size_t size)
{
	if (n == 100000)
	{
		sset.clear_destroy();
		n = 0;
	}
	sset.insert_key(::fast_io::string_view(reinterpret_cast<char const *>(data), size));
	++n;
	return 0;
}
