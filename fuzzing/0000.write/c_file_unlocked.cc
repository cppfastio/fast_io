#include<fast_io.h>

fast_io::c_file_unlocked obf("/dev/null",fast_io::open_mode::out);

extern "C" int LLVMFuzzerTestOneInput(std::uint8_t const* ptr,std::size_t n) noexcept
{
        write(obf,ptr,ptr+n);
        return 0;
}