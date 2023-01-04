h33515
s 00003/00000/00016
d D 1.2 90/05/04 13:59:30 root 2 1
c made CLBYTES optional
e
s 00016/00000/00000
d D 1.1 90/03/06 12:32:35 root 1 0
c date and time created 90/03/06 12:32:35 by root
e
u
U
t
T
I 1
#include "../../sys/types.h"
#include "../../sys/signal.h"
#include "../../sys/param.h"

I 2
#ifndef CLBYTES
E 2
#define	CLBYTES	2048
#define	CLSHIFT	11
#define	CLOFSET	(CLBYTES-1)
I 2
#endif

E 2

#ifndef BYTE_ORDER
#include "../../sys/endian.h"
#endif

#ifndef	INET
#define	INET	1
#endif

E 1
