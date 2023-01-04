/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 */
/*      @(#)netdb.h	3.5 LAI System V NFS Release 3.2/V3	source        */
/*
 * If using a 4.3 BSD derived transport (eg. LAI's Streams TCP/IP)
 * then just use <netdb.h>.
 * Otherwise, we are using the home grown udp/ip transport, so define the
 * 'ent structures here (The rpcent structure is defined here though).
 */

#ifdef __88000_OCS_DEFINED
#include "File not specified by 88000 OCS."
#endif

#define LAI_STREAMS_TCP
#ifdef LAI_STREAMS_TCP
#include <netdb.h>
#else
/*
 * Structures returned by network
 * data base library.  All addresses
 * are supplied in host order, and
 * returned in network order (suitable
 * for use in system calls).
 */
struct	hostent {
	char	*h_name;	/* official name of host */
	char	**h_aliases;	/* alias list */
	int	h_addrtype;	/* host address type */
	int	h_length;	/* length of address */
	char	*h_addr;	/* address */
};

/*
 * Assumption here is that a network number
 * fits in 32 bits -- probably a poor one.
 */
struct	netent {
	char	*n_name;	/* official name of net */
	char	**n_aliases;	/* alias list */
	int	n_addrtype;	/* net address type */
	int	n_net;		/* network # */
};

struct	servent {
	char	*s_name;	/* official service name */
	char	**s_aliases;	/* alias list */
	int	s_port;		/* port # */
	char	*s_proto;	/* protocol to use */
};

struct	protoent {
	char	*p_name;	/* official protocol name */
	char	**p_aliases;	/* alias list */
	int	p_proto;	/* protocol # */
};


struct hostent	*gethostbyname(), *gethostbyaddr(), *gethostent();
struct netent	*getnetbyname(), *getnetbyaddr(), *getnetent();
struct servent	*getservbyname(), *getservbyport(), *getservent();
struct protoent	*getprotobyname(), *getprotobynumber(), *getprotoent();
#endif

/*
 * Define the rpc stuff here, not in XPRT netdb.h.
 */

struct rpcent {
	char	*r_name;	/* name of server for this rpc program */
	char	**r_aliases;	/* alias list */
	int	r_number;	/* rpc program number */
};
struct rpcent	*getrpcbyname(), *getrpcbynumber(), *getrpcent();
