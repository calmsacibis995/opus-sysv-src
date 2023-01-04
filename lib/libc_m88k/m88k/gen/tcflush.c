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
 * int _tcflush(int fildes, int queue_selector)
 *
 * Flush the queue specified by 'queue_selector'
 * queue_selector
		TCIFLUSH  (0) Input queue
		TCOFLUSH  (1) Output queue
		TCIOFLUSH (2) Both input and output queues
 * This call maps to both 88open and UNIX directly

 SHOULD WE CHECK FOR THE VALUES OF QUEUE_SELECTOR??
 */

int
_tcflush(fildes, queue_selector)
int fildes, queue_selector;
{
	return(
		_ioctl(fildes, TC_PX_FLUSH, queue_selector)
	      );
}
