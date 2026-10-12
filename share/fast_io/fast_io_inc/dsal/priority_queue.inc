export namespace fast_io
{
namespace containers
{
using ::fast_io::containers::priority_queue;
using ::fast_io::containers::swap;
} // namespace containers

#ifndef FAST_IO_FREESTANDING
using ::fast_io::priority_queue;
using ::fast_io::zu_priority_queue;

namespace tlc
{
using ::fast_io::tlc::priority_queue;
using ::fast_io::tlc::zu_priority_queue;
} // namespace tlc
#endif
} // namespace fast_io
