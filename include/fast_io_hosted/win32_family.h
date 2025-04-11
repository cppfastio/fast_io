#pragma once

namespace fast_io
{

enum class win32_family
{
	ansi_9x,
	wide_nt,
#ifdef _WIN32_WINDOWS
	native = ansi_9x
#else
	native = wide_nt
#endif
};

} // namespace fast_io