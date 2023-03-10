/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 */
/* "@(#)sm_inter.h	3.3 LAI System V NFS Release 3.2/V3	source */
/*	Copyright (c) 1986 by Sun Microsystems, Inc.
 * @(#)sm_inter.h	1.2 86/11/17 NFSSRC
 * @(#)sm_inter.h 1.1 86/09/25
 */

#ifdef __88000_OCS_DEFINED
#include "File not specified by 88000 OCS."
#endif

#define SM_PROG 100024
#define SM_VERS 1
#define SM_STAT 1
#define SM_MON 2
#define SM_UNMON 3
#define SM_UNMON_ALL 4
#define SM_SIMU_CRASH 5

#define SM_MAXSTRLEN 	1024

struct sm_name {
	char *mon_name;
};
typedef struct sm_name sm_name;


struct my_id {
	char *my_name;
	int my_prog;
	int my_vers;
	int my_proc;
};
typedef struct my_id my_id;


struct mon_id {
	char *mon_name;
	struct my_id my_id;
};
typedef struct mon_id mon_id;


struct mon {
	struct mon_id mon_id;
	char priv[16];
};
typedef struct mon mon;


struct sm_stat {
	int state;
};
typedef struct sm_stat sm_stat;


enum res {
	stat_succ = 0,
	stat_fail = 1,
};
typedef enum res res;


struct sm_stat_res {
	res res_stat;
	int state;
};
typedef struct sm_stat_res sm_stat_res;


struct status {
	char *mon_name;
	int state;
	char priv[16];
};
typedef struct status status;

bool_t xdr_sm_name();
bool_t xdr_my_id();
bool_t xdr_mon_id();
bool_t xdr_mon();
bool_t xdr_sm_stat();
bool_t xdr_res();
bool_t xdr_sm_stat_res();
bool_t xdr_status();
