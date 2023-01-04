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
 * int _tcdrain(int fildes)
 *
 * This call will wait until all output written to 'fildes' has been
 * transmitted. The 88open supports this call but not UNIX.
 * NOTE: Our kernel supprots this 'ioctl' call.
 */

int
_tcdrain(fildes)
int fildes;
{
	return (
		_ioctl(fildes, TC_PX_DRAIN, 0)
	       );
}
