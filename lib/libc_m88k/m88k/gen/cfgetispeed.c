
#include	<sys/types.h>
#include 	<sys/termios.h>
#include	<errno.h> 

/*
 * speed_t _cfgetispeed(struct termios * termios_p)
 *
 * Extract the input baud rate from the structure pointed by
 * 'termios_p'.
 */

speed_t
_cfgetispeed(termios_p)
struct termios * termios_p;
{
	return (termios_p->c_cflag & CIBAUD) >> 8;
}
