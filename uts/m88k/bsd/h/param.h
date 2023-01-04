#include "../../sys/types.h"
#include "../../sys/signal.h"
#include "../../sys/param.h"

#ifndef CLBYTES
#define	CLBYTES	2048
#define	CLSHIFT	11
#define	CLOFSET	(CLBYTES-1)
#endif


#ifndef BYTE_ORDER
#include "../../sys/endian.h"
#endif

#ifndef	INET
#define	INET	1
#endif

