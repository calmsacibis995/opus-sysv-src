h25074
s 00073/00000/00000
d D 1.1 90/03/06 12:28:41 root 1 0
c date and time created 90/03/06 12:28:41 by root
e
u
U
t
T
I 1
/*		Copyright (c) 1985 AT&T		*/
/*		All Rights Reserved		*/
/* 		@(#)err.c	10.1			*/
#include "sys/param.h"
#include "sys/types.h"
#include "sys/buf.h"

#ifdef REL2
#include "sys/dir.h"
#else
#include "sys/fs/s5dir.h"
#endif

#include "sys/signal.h"
#include "sys/user.h"
#include "sys/errno.h"
#include "sys/file.h"
#include "sys/utsname.h"
#ifndef m68k
#ifndef m88k
#include "sys/mba.h"
#endif
#endif
#include "sys/elog.h"
#include "sys/erec.h"

#ifndef REL2
#include "sys/sysmacros.h"
#endif

static	short	logging;

erropen(dev,flg)
{
	if(logging) {
		u.u_error = EBUSY;
		return;
	}
#ifdef REL2
	if((flg&FWRITE) || dev != 0) {
#else
	if((flg&FWRITE) || minor(dev) != 0) {
#endif
		u.u_error = ENXIO;
		return;
	}
	if(suser()) {
		logstart();
		logging++;
	}
}

errclose(dev,flg)
{
	logging = 0;
}

errread(dev)
{
	register struct errhdr *eup;
	register n;
	struct errhdr	*geterec();

	if(logging == 0)
		return;
	eup = geterec();
	n = min(eup->e_len, u.u_count);
	if (copyout(eup, u.u_base, n))
		u.u_error = EFAULT;
	else
		u.u_count -= n;
	freeslot(eup);
}
E 1
