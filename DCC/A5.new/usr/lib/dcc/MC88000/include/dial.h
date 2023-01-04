#ifndef __Idial
#define __Idial

#include <sys/termio.h>

#define LDEVS	"/usr/lib/uucp/L-devices"
#define SYSFILE "/usr/lib/uucp/L.sys"
#define DIALFILE "/usr/lib/uucp/L-dialcodes"
#define DEVDIR	"/dev/"
#define LOCK	"/usr/spool/uucp/LCK.."

#define DIAL
#define STANDALONE
#define DVC_LEN	80

#define	TRUE	1
#define FALSE	0
#define INTRPT	(-1)
#define D_HUNG	(-2)
#define NO_ANS	(-3)
#define ILL_BD	(-4)
#define A_PROB	(-5)
#define L_PROB	(-6)
#define NO_Ldv	(-7)
#define DV_NT_A	(-8)
#define DV_NT_K	(-9)
#define NO_BD_A	(-10)
#define NO_BD_K	(-11)
#define DV_NT_E (-12)
#define BAD_SYS (-13)

typedef struct {
	struct termio 	*attr;
	int		baud;
	int		speed;
	char		*line;
	char		*telno;
	int		modem;
	char		*device;
	int		dev_len;
} CALL;

#endif
