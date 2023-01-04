/*	@(#)creat.c	1.1	*/

# include <fcntl.h>

creat(str)
char *str; {

	return (open(str, O_RDWR | O_CREAT));
}
