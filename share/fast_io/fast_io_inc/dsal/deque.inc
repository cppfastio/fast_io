export namespace fast_io
{
namespace containers
{
using ::fast_io::containers::deque;
using ::fast_io::containers::operator==;
using ::fast_io::containers::operator<=>;
using ::fast_io::containers::swap;
using ::fast_io::containers::erase;
using ::fast_io::containers::erase_if;
} // namespace containers

#ifndef FAST_IO_FREESTANDING
using ::fast_io::deque;
using ::fast_io::zu_deque;

namespace tlc
{
using ::fast_io::tlc::deque;
using ::fast_io::tlc::zu_deque;
} // namespace tlc
#endif
} // namespace fast_io
