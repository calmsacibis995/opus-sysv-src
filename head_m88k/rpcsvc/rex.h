/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 */
/*      @(#)rex.h	3.5 LAI System V NFS Release 3.2/V3	source        */
/* @(#)rex.h	1.3 87/03/16 NFSSRC */
/* static char sccsid[] = "@(#)rex.h 1.1 86/09/25 Copyr 1985 Sun Micro"; */

/*
 * rex - remote execution server definitions
 *
 * Copyright (c) 1985 Sun Microsystems, Inc.
 */

#ifdef __88000_OCS_DEFINED
#include "File not specified by 88000 OCS."
#endif

/*
 * Structures for BSD_to_SysV tty conversion and representation.
 */

struct B_sgttyb {
	char	bsd_array[4];
#define	bsg_ispeed	bsd_array[0]
#define	bsg_ospeed	bsd_array[1]
#define	bsg_erase	bsd_array[2]
#define	bsg_kill	bsd_array[3]
/*	char	sg_ispeed;		/* input speed     */
/*	char	sg_ospeed;		/* output speed    */
/*	char	sg_erase;		/* erase character */
/*	char	sg_kill;		/* kill character  */
	short	bsg_flags;
};

struct B_tchars {
	char	t_array[6];
#define	t_intrc	t_array[0]
#define	t_quitc	t_array[1]
#define	t_startc t_array[2]
#define	t_stopc t_array[3]
#define	t_eofc	t_array[4]
#define	t_brkc	t_array[5]
/*	char	t_intrc;	/* interrupt    */
/*	char	t_quitc;	/* quit         */
/*	char	t_startc;	/* start output */
/*	char	t_stopc;	/* stop output  */
/*	char	t_eofc;		/* end-of-file  */
/*	char	t_brkc;		/* input delimiter (like nl) */
};

struct B_ltchars {
	char	t_array[6];
#define	t_suspc	t_array[0]
#define	t_dsuspc t_array[1]
#define	t_rprntc t_array[2]
#define	t_flushc t_array[3]
#define	t_werasc t_array[4]
#define	t_lnextc t_array[5]
/*	char	t_suspc;	/* stop process signal         */
/*	char	t_dsuspc;	/* delayed stop process signal */
/*	char	t_rprntc;	/* reprint line                */
/*	char	t_flushc;	/* flush output (toggles)      */
/*	char	t_werasc;	/* word erase                  */
/*	char	t_lnextc;	/* literal next character      */
};

#define	tchars	B_tchars
#define	ltchars	B_ltchars

#define	REXPROG		100017
#define	REXPROC_NULL	0	/* no operation                   */
#define	REXPROC_START	1	/* start a command                */
#define	REXPROC_WAIT	2	/* wait for a command to complete */
#define	REXPROC_MODES	3	/* send the tty modes             */
#define REXPROC_WINCH	4	/* signal a window change         */
#define REXPROC_SIGNAL	5	/* other signals                  */

#define	REXVERS	1

/* flags for rst_flags field */
#define REX_INTERACTIVE		1	/* Interative mode */

struct rex_start {
  /*
   * Structure passed as parameter to start function
   */
	char	**rst_cmd;	/* list of command and args             */
	char	*rst_host;	/* working directory host name          */
	char	*rst_fsname;	/* working directory file system name   */
	char	*rst_dirwithin;	/* working directory within file system */
	char	**rst_env;	/* list of environment                  */
	ushort	rst_port0;	/* port for stdin                       */
	ushort	rst_port1;	/* port for stdin                       */
	ushort	rst_port2;	/* port for stdin                       */
	ulong	rst_flags;	/* options - see #defines above         */
};

bool_t xdr_rex_start();

struct rex_result {
	/*
	 * Structure returned from the start function
	 */
   	int	rlt_stat;	/* integer status code                  */
	char	*rlt_message;	/* string message for human consumption */
};

bool_t xdr_rex_result();

struct rex_ttymode {
	/*
	 * Structure sent to set-up the tty modes
	 */
	struct B_sgttyb basic;	/* standard unix tty flags          */
	struct tchars more;	/* interrupt, kill characters, etc. */
	struct ltchars yetmore;	/* special Berkeley characters    */
	ulong andmore;		/* and Berkeley modes               */
};

bool_t xdr_rex_ttymode();
bool_t xdr_rex_ttysize();
bool_t xdr_sig ();

struct ttysize {
	int     ts_lines;       /* number of lines on terminal   */
	int     ts_cols;        /* number of columns on terminal */
};

/*
 * For bsd compatability
 */

#ifndef	MAXPATHLEN
#define	MAXPATHLEN	1024	/* From param.h in bsd4.2 */
#endif

/*
 * For debugging purposes. Tells us kind of rexd process we are.
 */
#define	REXD__NULL	00	/* Not defined */
#define	REXD__MASTER	01	/* Master Rexd process   */
#define	REXD__RPC	02	/* Rpc side of rexd proc */
#define	REXD__OUTPUT	03	/* Master->socket proc   */
#define	REXD__EXEC	04	/* Execution child itself*/
