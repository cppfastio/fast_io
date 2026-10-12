#pragma once

/*
TLS 1.3 protocol pieces: wire format, key schedule, handshake messages,
x509/DER validation, PEM trust-store decoding. Freestanding-capable --
the kTLS socket glue lives in <fast_io_tls.h> (hosted, Linux).
*/

#include "cipher_suite.h"
#include "defs.h"
#include "wire.h"
#include "key_schedule.h"
#include "handshake.h"
#include "record.h"
#include "x509.h"
#include "pem.h"
#include "pkey.h"
#include "crypto_backend.h"
#include "ossl_backend.h"
#include "gnutls_backend.h"

/*
default crypto backend for basic_tls::crypto: openssl's primitives
where the EVP headers are visible on non-windows platforms (-lcrypto
at link time), gnutls's where those are visible instead (-lgnutls
-lnettle), the fast_io algorithms on windows and under
FAST_IO_TLS_FORCE_FAST_IO.
*/
namespace fast_io::tls
{

#if FAST_IO_TLS_FORCE_FAST_IO || (defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__))
using tls_default_crypto = fast_io_crypto_backend;
#elif FAST_IO_TLS_HAS_OSSL_CRYPTO
using tls_default_crypto = ossl_crypto_backend;
#elif FAST_IO_TLS_HAS_GNUTLS_CRYPTO
using tls_default_crypto = gnutls_crypto_backend;
#else
using tls_default_crypto = fast_io_crypto_backend;
#endif

} // namespace fast_io::tls
