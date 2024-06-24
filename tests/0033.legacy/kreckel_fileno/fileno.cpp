#include <fast_io_legacy.h>
#include "fileno.hpp"

/*
Test from
https://www.ginac.de/~kreckel/fileno/
*/

namespace
{
template <typename charT, typename traits>
inline int fileno_hack(std::basic_ios<charT, traits> const &stream) noexcept
{
	::fast_io::basic_streambuf_io_observer<charT, traits> bsiob{stream.rdbuf()};
	return static_cast<::fast_io::basic_posix_io_observer<charT>>(bsiob).fd;
}
} // namespace

//! 8-Bit character instantiation: fileno(ios).
template <>
int fileno<char>(std::ios const &stream) noexcept
{
	return fileno_hack(stream);
}

template <>
int fileno<wchar_t>(std::wios const &stream) noexcept
{
	return fileno_hack(stream);
}