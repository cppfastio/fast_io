#pragma once

#if (defined(_WIN32) && !defined(__WINE__)) && !defined(__CYGWIN__)
#ifdef _WIN32_WINDOWS
#include "win32.h"

namespace fast_io
{
using thread = ::fast_io::win32::win32_thread;

namespace this_thread
{

using ::fast_io::win32::this_thread::get_id;
using ::fast_io::win32::this_thread::sleep_for;
using ::fast_io::win32::this_thread::sleep_until;

} // namespace this_thread

} // namespace fast_io
#else
#include "nt.h"

namespace fast_io
{
template <bool zw = false>
using thread = ::fast_io::win32::nt::nt_thread<zw>;

namespace this_thread
{

using ::fast_io::win32::nt::this_thread::get_id;
using ::fast_io::win32::nt::this_thread::sleep_for;
using ::fast_io::win32::nt::this_thread::sleep_until;

} // namespace this_thread

} // namespace fast_io
#endif
#elif defined (__linux__)

#endif
