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
 * int _tcgetattr(int fd, reg struct termios * termios_p)
 *
 * Get the attributes of the terminal device referred by 'fildes'
 * in the structure pointed by 'termios_p'.
 * The necessary 'ioctl' call is directly supported in 88open and
 * UNIX.
 */

int
_tcgetattr(fildes, termios_p)
int fildes; 
struct termios * termios_p;
{
	return (_ioctl(fildes, TC_PX_GETATTR, termios_p));
}
