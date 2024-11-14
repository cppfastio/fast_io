#include <cstdio>
#include <fast_io_core.h>

int main()
{
	printf("nt_current_teb():%p\n", ::fast_io::win32::nt::nt_current_teb());
	printf("nt_get_current_peb():%p\nRtlGetCurrentPeb():%p\n",
		   ::fast_io::win32::nt::nt_get_current_peb(),
		   ::fast_io::win32::nt::RtlGetCurrentPeb());
}
