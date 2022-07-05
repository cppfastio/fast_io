#pragma once

//fast_io_crypto.h defines what you could use for cryptography
//It is likely usable in a freestanding environment
#if !defined(__cplusplus)
#error "You are not using a C++ compiler"
#endif

#if !defined(__cpp_concepts)
#error "fast_io requires at least C++20 standard compiler."
#else

#include"fast_io_core.h"

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(push)
#pragma warning( disable : 4514 )
#pragma warning( disable : 4623 )
#pragma warning( disable : 4710 )
#pragma warning( disable : 4668 )
#endif

//#include"fast_io_crypto/symmetric_crypto.h"
//#include"fast_io_crypto/hash/intrin_include.h"
#include"fast_io_crypto/hash/impl.h"

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#endif


#endif
