/*
 * The functions in this file support/map POSIX terminal i/o
 * calls to 88open calls (and indirectly to UNIX calls)
 * Most of the calls have a direct counterpart under UNIX
 * TERMIO. However there are some calls that are not supported
 * directly by UNIX.
 *
 * Author: R.L.K. Dattatri
 * Date  : Mar 10, 89
 * Revision History:
 */

#include	<sys/types.h>
#include 	<sys/termios.h>
#include	<errno.h> 

/*
 * int _cfsetispeed(struct termios * termios_p, speed_t speed)
 *
 * Set the input baud rate component in the structure pointed by termios_p to
 * 'speed'. 
 * Returns -1 in case the 'speed' supplied is invalid.
 * Returns 0 otherwise.
 */

int
_cfsetispeed(termios_p, speed)
struct termios * termios_p;
register speed_t speed;
{
	speed >>= 16;
	if (speed < (B0 >> 16) || speed > (B38400 >> 16)) {
		errno = EINVAL;
		return (-1);
	}
	/* now set the speed */
	termios_p->c_cflag &= ~CIBAUD;
	if (speed == B0)	/* use output baud rate */
		speed = (termios_p->c_cflag & CBAUD) >> 16; 
	termios_p->c_cflag |= speed << 24;
	return (0);
}
