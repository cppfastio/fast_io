#pragma once

namespace fast_io::freestanding
{

template<typename T>
inline constexpr bool is_trivially_relocatable_v =
#if defined(__has_extension)
#if __has_extension(trivially_relocatable)
__is_trivially_relocatable(T) ||
#endif
#endif
::std::is_trivially_copyable_v<T>
;
}