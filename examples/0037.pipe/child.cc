#include <fast_io.h>

int main()
{
	::fast_io::native_pipe pipe_out{};
	::fast_io::native_process_io npi{.out{pipe_out}};
	::fast_io::native_process child{
#ifdef _WIN32
		u8"parent.exe"
#else
		u8"parent"
#endif
		,{},{},{}
	};
}
