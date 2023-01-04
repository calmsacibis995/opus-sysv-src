h47814
s 00112/00000/00000
d D 1.1 90/03/06 12:28:03 root 1 0
c date and time created 90/03/06 12:28:03 by root
e
u
U
t
T
I 1
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)kern-port:os/move.c	10.3"
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/param.h"
#include "sys/inode.h"
#include "sys/fs/s5dir.h"
#include "sys/signal.h"
#include "sys/errno.h"
#include "sys/user.h"
#include "sys/buf.h"

/*
 * Move n bytes at byte location
 * &bp->b_un.b_addr[o] to/from (flag) the
 * user/kernel (u.segflg) area starting at u.base.
 * Update all the arguments by the number
 * of bytes moved.
 */
iomove(cp, n, flag)
register caddr_t cp;
register n;
{
	register t;

	if (n==0)
		return;
	if (u.u_segflg != 1)  {
		if (flag==B_WRITE)
			t = copyin(u.u_base, (caddr_t)cp, n);
		else
			t = copyout((caddr_t)cp, u.u_base, n);
		if (t) {
			u.u_error = EFAULT;
			return;
		}
	}
	else
		if (flag == B_WRITE)
			bcopy(u.u_base,(caddr_t)cp,n);
		else
			bcopy((caddr_t)cp,u.u_base,n);
	u.u_base += n;
	u.u_offset += n;
	u.u_count -= n;
	return;
}

/*
 * Pass back  c  to the user at his location u_base;
 * update u_base, u_count, and u_offset.  Return -1
 * on the last character of the user's read.
 * u_base is in the user data space.
 */
passc(c)
register c;
{
	if (subyte(u.u_base, c) < 0) {
		u.u_error = EFAULT;
		return(-1);
	}
	u.u_count--;
	u.u_offset++;
	u.u_base++;
	return(u.u_count == 0? -1: 0);
}

/*
 * Pick up and return the next character from the user's
 * write call at location u_base;
 * update u_base, u_count, and u_offset.  Return -1
 * when u_count is exhausted.
 * u_base is in the user data space.
 */
cpass()
{
	register c;

	if (u.u_count == 0)
		return(-1);
	if ((c = fubyte(u.u_base)) < 0) {
		u.u_error = EFAULT;
		return(-1);
	}
	u.u_count--;
	u.u_offset++;
	u.u_base++;
	return(c);
}

/*
 *	Copy 8 or fewer characters from string *b to string *a
 *	terminated by null character.	String *a must be at 
 *	least 9 characters long.  Used in file machdep.c.
 */

str8cpy(a, b)
register char	*a, *b;
{
	register int	i = 0;

	while (*a++ = *b++)
		if (++i >= 8)
			break;
	*a = '\0';
}
E 1
