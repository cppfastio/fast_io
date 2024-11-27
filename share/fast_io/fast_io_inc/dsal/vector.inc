export namespace fast_io
{
namespace containers
{
using ::fast_io::containers::vector;
using ::fast_io::containers::operator==;
using ::fast_io::containers::operator<=>;
using ::fast_io::containers::swap;
using ::fast_io::containers::erase;
using ::fast_io::containers::erase_if;
}

#ifndef FAST_IO_FREESTANDING
namespace tlc
{
using ::fast_io::tlc::vector;
}

using ::fast_io::vector;
#endif
}
