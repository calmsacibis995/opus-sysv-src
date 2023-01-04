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
 * int _tcsetattr(int fildes, int optional_actions, struct termios * termios_p)
 *
 * This call will set the attributes of the device controlled by 'fildes'
 * to the values specified in the structure pointed by 'termios_p'.
 * The 'optional_actions' argument indicates the nature of action.
 * It can be 
		TCSANOW   - act immediately
		TCSADRAIN - wait till all output is processed
		TCSAFLUSH - wait till both i/p and o/p are flushed
 * The corresponding ioctl call is supported both in 88open and UNIX.
 * Return (-1) if the 'optional_actions' argument is invalid.
 * Otherwise return the value from 'ioctl()'
 */

int
_tcsetattr(fildes, optional_actions, termios_p)
int fildes;
register int optional_actions; 
register struct termios * termios_p;
{
	/*
	 * We need a case statement because the 'ioctl'
	 * needs different arguments depending on the value
	 * of 'optional_actions'. 
	 */
	switch (optional_actions) {
		case TCSANOW:
			return(
				_ioctl(fildes, TC_PX_SETATTR, termios_p)
			      );

		case TCSADRAIN:
			return(
				_ioctl(fildes, TC_PX_SETATTRD, termios_p)
			      );

		case TCSAFLUSH:
			return(
				_ioctl(fildes, TC_PX_SETATTRF, termios_p)
			      );

		default:
			/* invalid argument for set attribute */
			errno = EINVAL;
			return (-1);
	}
}
