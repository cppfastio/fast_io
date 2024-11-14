#pragma once

namespace fast_io::intrinsics::msvc::arm
{

extern "C"
{
	unsigned __int64 __getReg(int);

	unsigned int _MoveFromCoprocessor(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);

}

} // namespace fast_io::intrinsics::msvc::arm
