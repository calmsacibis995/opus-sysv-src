h46310
s 00115/00000/00000
d D 1.1 90/03/06 12:31:27 root 1 0
c date and time created 90/03/06 12:31:27 by root
e
u
U
t
T
I 1
#ifndef _NFS_KLM_PROT_H_
#define _NFS_KLM_PROT_H_

/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 */


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*      @(#)klm_prot.h	4.5 LAI System V NFS Release 3.2/V3	source        */

/* Copyright (c) 1986 by Sun Microsystems, Inc.  */
/* @(#)klm_prot.h	1.2 86/11/17 NFSSRC */
/*      @(#)klm_prot.h 1.1 86/09/25 SMI */

#define KLM_PROG ((ulong)100020)
#define KLM_VERS ((ulong)1)
#define KLM_NULL 0
#define KLM_TEST 1
#define KLM_LOCK 2
#define KLM_CANCEL 3
#define KLM_UNLOCK 4
#define KLM_PROCS_CNT 5

#define LM_MAXSTRLEN	1024

enum klm_stats {
	/* NLM generated */
	klm_granted = 0,
	klm_denied = 1,
	klm_denied_nolocks = 2,
	klm_working = 3,
	/* KLM generated */
	klm_interrupted = 4,
	klm_denied_nolink = 5,
	klm_timedout = 6,
};
typedef enum klm_stats klm_stats;

#define KLM_STATS_CNT 7

struct klm_lock {
	char *server_name;
	netobj fh;
	int pid;
	uint l_offset;
	uint l_len;
};
typedef struct klm_lock klm_lock;


struct klm_holder {
	bool_t exclusive;
	int svid;
	uint l_offset;
	uint l_len;
};
typedef struct klm_holder klm_holder;


struct klm_stat {
	klm_stats stat;
};
typedef struct klm_stat klm_stat;


struct klm_testrply {
	klm_stats stat;
	union {
		struct klm_holder holder;
	} klm_testrply;
};
typedef struct klm_testrply klm_testrply;


struct klm_lockargs {
	bool_t block;
	bool_t exclusive;
	struct klm_lock lock;
};
typedef struct klm_lockargs klm_lockargs;


struct klm_testargs {
	bool_t exclusive;
	struct klm_lock lock;
};
typedef struct klm_testargs klm_testargs;


struct klm_unlockargs {
	struct klm_lock lock;
};
typedef struct klm_unlockargs klm_unlockargs;

bool_t xdr_klm_stats();
bool_t xdr_klm_lock();
bool_t xdr_klm_holder();
bool_t xdr_klm_stat();
bool_t xdr_klm_testrply();
bool_t xdr_klm_lockargs();
bool_t xdr_klm_testargs();
bool_t xdr_klm_unlockargs();

#endif	/* ! _NFS_KLM_PROT_H_ */
E 1
