export namespace fast_io
{

inline namespace io
{
using ::fast_io::io::print;
using ::fast_io::io::println;
using ::fast_io::io::perr;
using ::fast_io::io::perrln;
using ::fast_io::io::panic;
using ::fast_io::io::panicln;
using ::fast_io::io::scan;

#ifndef FAST_IO_DISABLE_DEBUG_PRINT
using ::fast_io::io::debug_print;
using ::fast_io::io::debug_println;
using ::fast_io::io::debug_perr;
using ::fast_io::io::debug_perrln;
#endif
} // namespace io

} // namespace fast_io