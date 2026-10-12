export namespace fast_io
{
using ::fast_io::size32_t;

namespace containers
{
using ::fast_io::containers::sized_vector;
using ::fast_io::containers::operator==;
using ::fast_io::containers::operator<=>;
using ::fast_io::containers::swap;
using ::fast_io::containers::erase;
using ::fast_io::containers::erase_if;
} // namespace containers

#ifndef FAST_IO_FREESTANDING
using ::fast_io::sized_vector;
using ::fast_io::sized_vector_zu32;
using ::fast_io::zu32_sized_vector_zu32;

namespace tlc
{
using ::fast_io::tlc::sized_vector;
using ::fast_io::tlc::sized_vector_zu32;
using ::fast_io::tlc::zu32_sized_vector_zu32;
} // namespace tlc
#endif
} // namespace fast_io
