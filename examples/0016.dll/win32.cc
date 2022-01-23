#include<fast_io.h>
#include<fast_io_device.h>

int main()
{
	fast_io::native_dll_file dll(L"a.dll",fast_io::dll_mode::posix_rtld_lazy);
	[[maybe_unused]] auto proc{dll_load_symbol(dll,u8"GetProc")};
}
