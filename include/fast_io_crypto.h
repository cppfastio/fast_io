#pragma once

// fast_io_crypto.h defines what you could use for cryptography
// It is likely usable in a freestanding environment
#if !defined(__cplusplus)
#error "You are not using a C++ compiler"
#endif

#if !defined(__cpp_concepts)
#error "fast_io requires at least a C++26 standard compiler."
#else

#include "fast_io_core.h"
#include "fast_io_dsal/impl/common.h"
#include "fast_io_dsal/impl/span.h"
#include "fast_io_dsal/impl/index_span.h"

#include "fast_io_dsal/impl/misc/push_warnings.h"
#include "fast_io_dsal/impl/misc/push_macros.h"

// #include"fast_io_crypto/symmetric_crypto.h"
// #include"fast_io_crypto/hash/intrin_include.h"
#include "fast_io_crypto/hash/impl.h"
#include "fast_io_crypto/hash/hmac.h"
#include "fast_io_crypto/cipher/aes.h"
#include "fast_io_crypto/curve25519/impl.h"
#include "fast_io_crypto/rsa/impl.h"
#include "fast_io_crypto/ecc/impl.h"
#include "fast_io_crypto/tls/impl.h"
#include "fast_io_crypto/websocket/impl.h"

#include "fast_io_dsal/impl/misc/pop_macros.h"
#include "fast_io_dsal/impl/misc/pop_warnings.h"

#endif
