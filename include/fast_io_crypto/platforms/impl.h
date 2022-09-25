#pragma once

#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
#include"win32/impl.h"
#include"nt_bcrypt.h"
#endif

#if __has_include(<openssl/evp.h>)
#include<openssl/evp.h>
#include"ossl_evp.h"
#endif

namespace fast_io
{

#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
using native_hash_file = nt_bcrypt_hash_file;
#elif __has_include(<openssl/evp.h>)
using native_hash_file = ossl_evp_hash_file;
#endif

}
