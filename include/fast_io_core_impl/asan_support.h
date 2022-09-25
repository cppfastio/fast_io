#pragma once

namespace fast_io::details
{

enum class asan_state
{
none,
activate,
current=
#if (defined(_GLIBCXX_DEBUG) || defined(_DEBUG) || _ITERATOR_DEBUG_LEVEL>=2 || _LIBCPP_DEBUG >= 2 || defined(FAST_IO_SANITIZE_IO_BUFFER))
#if defined(__SANITIZE_ADDRESS__)
activate
#elif defined(__has_feature)
#if __has_feature(address_sanitizer)
activate
#else
none
#endif
#else
none
#endif
#else
none
#endif
};

}
