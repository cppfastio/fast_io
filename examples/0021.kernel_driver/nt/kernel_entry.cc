#include<fast_io.h>
#include<fast_io_driver/nt_kernel.h>

extern "C" std::uint32_t DriverEntry(::fast_io::win32::nt::driver_object* driverobject,::fast_io::win32::nt::utf16_string* registrypath) noexcept
{
	println(fast_io::u8nt_dbg(),u8"Hello World from fast_io:\ndriverobject:",driverobject,u8" registrypath:",registrypath);
	println(fast_io::u8nt_dbg(),u8"test floating point:",241.43643634);
	return 0;
}
