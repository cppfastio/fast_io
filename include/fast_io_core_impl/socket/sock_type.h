#pragma once

namespace fast_io
{

enum class sock_type:char8_t
{
stream,
dgram,
seqpacket,
raw,
rdm,
packet
};

}
