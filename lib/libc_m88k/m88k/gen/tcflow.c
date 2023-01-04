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
 * int _tcflow(int fildes, int action)
 * 
 * Suspend transmission or reception of data on 'fildes' based on
 * 'action'
 *		TCOOFF - suspend output (maps to UNIX and 88open)
 *		TCOON  - restart suspended output (...) 
 *		TCIOOF - Transmit an XOFF to fildes (...)
 *		TCIOON - Transmit an XON to fildes (..) 
 */

int
_tcflow(fildes, action)
int fildes, action;
{
	return (
		_ioctl(fildes, TC_PX_FLOW, action)
	       );
}
