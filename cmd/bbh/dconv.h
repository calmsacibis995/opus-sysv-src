/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	Copyright (c) 1986 by National Semiconductor Corp.	*/
/*	All Rights Reserved					*/

#ident	"@(#)dconv.h	1.3	9/24/86 Copyright (c) 1986 by National Semiconductor Corp."

/* date and time conversion routines */

extern long to_day();
extern void to_date();
extern int weekday();
extern char *sweekday();
extern long tconv();
extern long tconvs();
