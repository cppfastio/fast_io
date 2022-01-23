#pragma once

namespace fast_io
{

/*
https://github.com/mirror/mingw-w64/blob/99b840e52d6e46f689b9cd086d925fad88db2b17/mingw-w64-headers/include/winsock2.h
*/

enum class sock_protocol
{
ah,          	/* authentication header.  */
beetph,      	/* IP option pseudo header for BEET.  */
carp,
cbt,
comp,        	/* Compression Header Protocol.  */
dccp,        	/* Datagram Congestion Control Protocol.  */
dstopts,     	/* IPv6 destination options */
egp,         	/* Exterior Gateway Protocol.  */
eigrp,       	/* Cisco/GXS IGRP */
encap,       	/* Encapsulation Header.  */
esp,         	/* encapsulating security payload.  */
ethernet,    	/* Ethernet-within-IPv6 Encapsulation.  */
fragment,    	/* IPv6 fragmentation header */
ggp,
gre,         	/* General Routing Encapsulation.  */
hopopts,     	/* IPv6 hop-by-hop options */
iclfxbm,
icmp,        	/* Internet Control Message Protocol.  */
icmpv6,      	/* ICMPv6 */
idp,         	/* XNS IDP protocol.  */
igmp,        	/* Internet Group Management Protocol. */
igp,
ip,          	/* Dummy protocol for TCP.  */
ipcomp,
ipip,        	/* IPIP tunnels (older KA9Q tunnels use 94).  */
ipv4,
ipv6,        	/* IPv6 header.  */
l2tp,
mobile,
mobility,
mobility_old,
mpls,        	/* MPLS in IP.  */
mptcp,       	/* Multipath TCP connection.  */
mtp,         	/* Multicast Transport Protocol.  */
nd,          	/* Sun net disk proto (temp.) */
none,        	/* IPv6 no next header */
ospf,
pgm,
pigp,
pim,         	/* Protocol Independent Multicast.  */
pup,         	/* PUP protocol.  */
raw,         	/* Raw IP packets.  */
rdp,
routing,     	/* IPv6 routing header */
rsvp,        	/* Reservation Protocol.  */
sctp,        	/* Stream Control Transmission Protocol.  */
st,
tcp,         	/* Transmission Control Protocol.  */
tp,          	/* SO Transport Protocol Class 4.  */
udp,         	/* User Datagram Protocol.  */
udplite,     	/* UDP-Lite protocol.  */
vrrp
};

}
