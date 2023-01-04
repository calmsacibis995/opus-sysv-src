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
 * int _tcsendbreak(int fildes, int duration)
 * 
 * Send zero valued bits on the asynchrnous comm line depending on
 * the value of 'duration'. 
 * If duration is zero, send 0 bits for atleast 0.25 sec but less than 0.5 sec.
 * Otherwise the transmission time is implementation dependent.
 * This calls maps to 88open and UNIX (TCSBRK) calls directly.
 * We return the value of 'ioctl'
 */

int
_tcsendbreak(fildes, duration)
int fildes, duration;
{
	return (
		_ioctl(fildes, TC_PX_BREAK, duration)
 	       );
}
