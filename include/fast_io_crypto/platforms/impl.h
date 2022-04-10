#pragma once

#if __has_include(<openssl/evp.h>)
#include<openssl/evp.h>
#include"ossl_evp.h"
#endif

namespace fast_io
{

#if __has_include(<openssl/evp.h>)
using native_hash_file = ossl_evp_hash_file;
#endif

}
