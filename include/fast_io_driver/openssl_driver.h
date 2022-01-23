#pragma once
#include<openssl/macros.h>
#ifdef OSSL_DEPRECATEDIN_3_0
#undef OSSL_DEPRECATEDIN_3_0
#define OSSL_DEPRECATEDIN_3_0
#endif
#include<openssl/bio.h>
#include<openssl/ssl.h>
#include<openssl/err.h>
#include<openssl/md5.h>
#include<openssl/sha.h>
#include<openssl/crypto.h>
#if __cpp_rtti
#include<typeinfo>
#endif

#include"../fast_io_hosted.h"
#include"../fast_io_legacy_impl/c/impl.h"
#include"openssl_driver/init.h"
#include"openssl_driver/observer.h"
#include"openssl_driver/error.h"
#include"openssl_driver/bio.h"
#if 0
#include"openssl_driver/ossl_lib_context.h"
#include"openssl_driver/context.h"
#include"openssl_driver/ssl.h"
#endif
#include"openssl_driver/hash.h"
#if 0
#include"openssl_driver/evp.h"
#endif
