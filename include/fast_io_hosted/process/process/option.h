#pragma once

namespace fast_io
{
enum process_mode : ::std::uint_least64_t
{
	none = 0,
	// *indicates that the process mode has not been evaluated yet
	new_session = static_cast<::std::uint_least64_t>(1) << 0,
	// [POSIX] setsid(), [WINDOWS, WINNT] CREATE_NEW_PROCESS_GROUP | DETACHED_PROCESS
	nt_path = static_cast<::std::uint_least64_t>(1) << 1
	// [WINNT] Set the process path in the PEB to nt path
};
}
