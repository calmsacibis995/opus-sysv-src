

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libc:inc/lib_ctype.h"
#include <stdio.h>
#include <fcntl.h>

#define	_U	01	/* Upper case */
#define	_L	02	/* Lower case */
#define	_N	04	/* Numeral (digit) */
#define	_S	010	/* Spacing character */
#define	_P	020	/* Punctuation */
#define	_C	040	/* Control character */
#define	_B	0100	/* Blank */
#define	_X	0200	/* heXadecimal digit */

#define CCLASS_ROOT	"/lib/chrclass/"/* root for character classes */
#define LEN_CC_ROOT	14		/* length of CCLASS_ROOT */
#define DEFL_CCLASS	"ascii"		/* default character class */
#define SZ		514
