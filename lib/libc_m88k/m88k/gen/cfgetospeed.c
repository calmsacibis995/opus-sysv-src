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
 * speed_t _tcgetospeed(termios_p)
 * 
 * Get the output speed stored in the structure pointed to by 'termios_p'
 */

speed_t
_cfgetospeed(termios_p)
struct termios * termios_p;
{
	return (termios_p->c_cflag & CBAUD);
}
