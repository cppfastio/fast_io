#pragma once

#include"sock_family.h"
#include"sock_protocol.h"
#include"sock_type.h"
#include"posix_sockaddr.h"
#include"ip.h"

#include"addrprt.h"
#if 0
#include"addrscn.h"
#endif

namespace fast_io
{
struct
#if __has_cpp_attribute(maybe_unused)
[[maybe_unused]]
#endif
posix_empty_network_service{};
//a dummy service to make serivce work with windows at a cross platform level

}
