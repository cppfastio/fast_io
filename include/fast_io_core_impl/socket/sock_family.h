#pragma once

namespace fast_io
{

enum class sock_family
{
unspec=-1,		/* Unspec */
alg=0,            	/* Algorithm sockets.  */
appletalk,      	/* Appletalk DDP.  */
arp,            	/* Address Resolution Protocol */
ash,            	/* Ash.  */
atm,            	/* ATM */
atmpvc,         	/* ATM PVCs.  */
atmsvc,         	/* ATM SVCs.  */
ax25,           	/* Amateur Radio AX.25.  */
ban,            	/* Banyan address. */
bluetooth,      	/* Bluetooth sockets.  */
bridge,         	/* Multiprotocol bridge.  */
caif,           	/* CAIF sockets.  */
can,            	/* Controller Area Network.  */
ccitt,          	/* CCITT protocols, X.25 etc */
chaos,          	/* mit CHAOS protocols */
cluster,        	/* Address for Microsoft cluster products. */
cnt,            	/* Computer Network Technology */
coip,           	/* connection-oriented IP, aka ST II */
datakit,        	/* datakit protocols */
decnet,         	/* Reserved for DECnet project.  */
dli,            	/* DEC Direct data link interface */
ecma,           	/* European computer manufacturers */
econet,         	/* Acorn Econet.  */
firefox,        	/* FireFox address. */
hylink,         	/* NSC Hyperchannel */
hyperv,         	/* HyperV sockets */
ib,             	/* Native InfiniBand address.  */
iclfxbm,
ieee12844,      	/* IEEE 1284.4 workgroup address. */
ieee80211,      	/* IEEE 802.11 protocol */
ieee802154,     	/* IEEE 802.15.4 sockets.  */
implink,        	/* arpanet imp addresses */
inet,           	/* IP protocol family.  */
inet6,          	/* IP version 6.  */
inet6_sdp,      	/* OFED Socket Direct Protocol ipv6 */
inet_sdp,       	/* OFED Socket Direct Protocol ipv4 */
ipx,            	/* Novell Internet Protocol.  */
irda,           	/* IRDA sockets.  */
isdn,           	/* mISDN sockets.  */
iso,            	/* ISO protocols */
iucv,           	/* IUCV sockets.  */
kcm,            	/* Kernel Connection Multiplexor.  */
key,            	/* PF_KEY key management API.  */
lat,            	/* LAT */
link,           	/* Link layer interface */
llc,            	/* Linux LLC.  */
local,          	/* Local to host (pipes and file-domain).  */
mpls,           	/* MPLS.  */
natm,           	/* native ATM access */
netbeui,        	/* Reserved for 802.2LLC project.  */
netbios,        	/* SMB protocols */
netdes,         	/* Address for Network Designers OSI gateway-enabled protocols. */
netgraph,       	/* Netgraph sockets */
netrom,         	/* Amateur radio NetROM.  */
nfc,            	/* NFC sockets.  */
packet,         	/* Packet family.  */
phonet,         	/* Phonet sockets.  */
pppox,          	/* PPPoX sockets.  */
pseudo_hdrcmplt,	/* Used by BPF to not rewrite headers * in interface output routine */
pseudo_key,     	/* Internal key-management function */
pseudo_pip,     	/* Help Identify PIP packets */
pseudo_rtip,    	/* Help Identify RTIP packets */
pseudo_xtp,     	/* eXpress Transfer Protocol (no AF) */
pup,            	/* pup protocols: e.g. BSP */
qipcrtr,        	/* Qualcomm IPC Router.  */
rds,            	/* RDS sockets.  */
rose,           	/* Amateur Radio X.25 PLP.  */
route,          	/* Internal Routing Protocol */
rxrpc,          	/* RxRPC sockets.  */
scluster,       	/* Sitara cluster protocol */
security,       	/* Security callback pseudo AF.  */
sip,            	/* Simple Internet Protocol */
slow,           	/* 802.3ad slow protocol */
smc,            	/* SMC sockets.  */
sna,            	/* IBM SNA */
tcnmessage,
tcnprocess,
tipc,           	/* TIPC sockets.  */
voiceview,      	/* VoiceView address. */
vsock,          	/* vSockets.  */
wanpipe,        	/* Wanpipe API sockets.  */
x25,            	/* Reserved for X.25 project.  */
xdp,            	/* XDP sockets.  */
};

}
