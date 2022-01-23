#include<fast_io.h>
#include<fast_io_unit/gb18030.h>
#include<fast_io_device.h>

fast_io::ogb18030_file obf("/dev/null");

extern "C" int LLVMFuzzerTestOneInput(std::uint8_t const* ptr,std::size_t n) noexcept
{
        write(obf,ptr,ptr+n);
        return 0;
}