#pragma once

//https://github.com/bminor/glibc/blob/5f72f9800b250410cad3abfeeb09469ef12b2438/sysdeps/unix/sysv/linux/bits/socket.h
//https://github.com/freebsd/freebsd/blob/230874c33e0e80cfe736ebb023cd26102b0572e2/sys/sys/socket.h

namespace fast_io
{

enum class protocol_family: std::uint_least32_t
{
	unspec = 0,
	local = 1,
	inet = 2,
	ax25 = 3,
	ipx = 4,
	appletalk = 5,
	netrom = 6,
	bridge = 7,
	atmpvc = 8,
	x25 = 9,
	inet6 = 10,
	rose = 11,
	decnet = 12,
	netbeui = 13,
	security = 14,
	key = 15,
	netlink = 16,
	route = 16,
	packet = 17,
	ash = 18,
	econet = 19,
	atmsvc = 20,
	rds = 21,
	sna = 22,
	irda = 23,
	pppox = 24,
	wanpipe = 25,
	llc = 26,
	ib = 27,
	mpls = 28,
	can = 29,
	tipc = 30,
	bluetooth = 31,
	iucv = 32,
	rxrpc = 33,
	isdn = 34,
	phonet = 35,
	ieee802154 = 36,
	caif = 37,
	alg = 38,
	nfc = 39,
	vsock = 40,
	kcm = 41,
	qipcrtr = 42,
	smc = 43,
	xdp = 44
};

using address_family = protocol_family;

}
