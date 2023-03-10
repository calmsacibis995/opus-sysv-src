/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)ld:m32/params.h	1.12"
/*
 */
/***********************************************************************
*   This file contains system dependent parameters for the link editor.
*   There must be a different params.h file for each version of the
*   link editor (e.g. b16, n3b, mac80, etc.)
*
*   THIS COPY IS FOR M80
*********************************************************************/


/*
 * Maximum size of a section
 */
#define MAXSCNSIZE	0x20000000L
#define MAXSCNSZ	MAXSCNSIZE


/*
 * Default size of configured memory
 */
#define MEMORG		0x20000L
#define MEMSIZE		0xffffffffL
#define NONULLORG	0x400000L

/*
 * Size of a region. If USEREGIONS is zero, the link editor will NOT
 * permit the use of REGIONS, nor partition the address space
 * USEREGIONS is defined in system.h
 */
#define REGSIZE 	0

#define COM_ALIGN	0x7L	/* Align factor for .comm's	*//*was 3 DONS*/

/*
 * define boundary for use by paging
 */

#define BOUNDARY	0x400000L		/* 4 MEGS */

/*
 * define special symbol names
 */

#define _CSTART	"__start"
#define _MAIN	"_main"
