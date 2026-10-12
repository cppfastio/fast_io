#pragma once

/*
TLS 1.3 client: userspace handshake + record layer, optional Linux kTLS
offload. The client, observer and file composites are transport-generic
-- anywhere fast_io's native socket types exist. ktls.h carries the
Linux SOL_TLS plumbing; async.h carries the io_uring/pthread-pool ops --
both stay linux-gated (windows async would come from an iocp backend).
*/

#include "client.h"
#include "observer.h"
#include "server.h"
#include "file.h"
#include "roots.h"
#include "swasync.h"
#if (!defined(_WIN32) || defined(__WINE__)) && !defined(__MSDOS__) && !defined(__wasi__)
#include "async.h"
#endif
/* the OpenSSL backend is a driver living outside this tree:
   <fast_io_driver/openssl_driver.h> provides basic_ossl_tls for
   explicit opt-in use. It never participates in native_tls
   selection. */
#include "schannel.h"
#include "schannel_async.h"
#include "native_backend.h"
