
/*
 * ioctl processing that converts system v ioctls to bcs/posix ioctls
 */

#define	INKERNEL	1	/* almost there !! */

#include <sys/termios.h>
#include <sys/termio.h>

_ioctl(fd, cmd, arg)
register cmd;
register struct termio *arg;
{
	struct termios local;
	register int i;

	switch (cmd) {
		case TCGETA:
			cmd = TC_PX_GETATTR;
			break;
		case TCSETA:
			cmd = TC_PX_SETATTR;
			break;
		case TCSETAW:
			cmd = TC_PX_SETATTRD;
			break;
		case TCSETAF:
			cmd = TC_PX_SETATTRF;
			break;
		case TCSBRK:
			cmd = TC_PX_BREAK;
			goto normal;
		case TCXONC:
			cmd = TC_PX_FLOW;
			goto normal;
		case TCFLSH:
			cmd = TC_PX_FLUSH;
			/* fall thru */
		default:
		normal:
			return(__ioctl(fd, cmd, (int) arg));
	}

	if ((i = __ioctl(fd, TC_PX_GETATTR, (int) &local)) < 0)
		return (i);
	if (cmd == TC_PX_GETATTR) {
		arg->c_iflag = local.c_iflag;
		arg->c_oflag = local.c_oflag;
		arg->c_cflag = local.c_cflag;
		arg->c_cflag &= ~CBAUD;
		arg->c_cflag |= (local.c_cflag >> 16) & CBAUD;
		arg->c_lflag = local.c_lflag;
		arg->c_line = local.c_line;
		_memcpy(arg->c_cc, local.c_cc, sizeof(arg->c_cc));
	}
	else {
		local.c_iflag = arg->c_iflag;
		local.c_oflag = arg->c_oflag;
		i = arg->c_cflag & CBAUD;
		local.c_cflag = (i<<16) | (i<<24) | (arg->c_cflag& ~CBAUD);
		local.c_lflag = arg->c_lflag;
		local.c_line = arg->c_line;
		_memcpy(local.c_cc, arg->c_cc, sizeof(arg->c_cc));
		i = __ioctl(fd, cmd, (int) &local);
	}
	return(i);
}
