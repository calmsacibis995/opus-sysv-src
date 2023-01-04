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
 * _tcsetpgrp(fildes)
 *
 * Set the process group id of the foreground process group
 * associated with terminal
 * Directly maps to 88open and UNIX (TIOCSPGRP)
 */

int _tcsetpgrp(fildes, pgrp_id)
int fildes; 
pid_t pgrp_id;
{
	return(
		_ioctl(fildes, TC_PX_SETPGRP, (int)pgrp_id)
  	      );
}

