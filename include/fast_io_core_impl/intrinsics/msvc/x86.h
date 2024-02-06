#pragma once

namespace fast_io::intrinsics::msvc::x86
{

extern "C"
{
	unsigned __int64 __readgsqword(unsigned long);
	unsigned long __readfsdword(unsigned long);
	unsigned char _addcarry_u8(unsigned char, unsigned char, unsigned char, unsigned char *);
	unsigned char _addcarry_u16(unsigned char, unsigned short, unsigned short, unsigned short *);
	unsigned char _addcarry_u32(unsigned char, unsigned int, unsigned int, unsigned int *);
	unsigned char _addcarry_u64(unsigned char, unsigned __int64, unsigned __int64, unsigned __int64 *);

	unsigned char _subborrow_u8(unsigned char, unsigned char, unsigned char, unsigned char *);
	unsigned char _subborrow_u16(unsigned char, unsigned short, unsigned short, unsigned short *);
	unsigned char _subborrow_u32(unsigned char, unsigned int, unsigned int, unsigned int *);
	unsigned char _subborrow_u64(unsigned char, unsigned __int64, unsigned __int64, unsigned __int64 *);

	unsigned __int64 _udiv128(unsigned __int64, unsigned __int64, unsigned __int64, unsigned __int64 *);
	unsigned __int64 __shiftleft128(unsigned __int64, unsigned __int64, unsigned char);
	unsigned __int64 __shiftright128(unsigned __int64, unsigned __int64, unsigned char);
}

} // namespace fast_io::intrinsics::msvc::x86
