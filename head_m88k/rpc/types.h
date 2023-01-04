/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 */
/*      @(#)types.h	3.3 LAI System V NFS Release 3.2/V3	source        */
/*
 * Rpc additions to <sys/types.h>
 */

#ifdef __88000_OCS_DEFINED
#include "File not specified by 88000 OCS."
#endif

#define	bool_t	int
#define	enum_t	int
#define	FALSE	(0)
#define	TRUE	(1)
#define __dontcare__	-1

#define mem_alloc(bsize)	malloc(bsize)
#define mem_free(ptr, bsize)	free(ptr)
#ifndef _TYPES_
#include <sys/types.h>
#endif

#ifndef NULL
#	define NULL	0
#endif
