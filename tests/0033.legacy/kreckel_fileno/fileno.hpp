#include <iosfwd>

template <typename charT, typename traits>
int fileno(std::basic_ios<charT, traits> const &stream) noexcept;
