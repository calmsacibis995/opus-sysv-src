/*	fs_size.c
**
**	figure out how big the file system is
**	use swaplo
*/
/*
**	1.00  10/10/88	dcl  first version
**
*/

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/sysconf.h>

/*
 * functions
 */
void xread();

/*
 * global variables
 */
int fs;

/*
 *  system calls
 */
void exit();
long lseek();
void sync();

/*
 *  library calls
 */
perror();

/*
 * CONSTANTS
 */
#define BLK0 0L

main(argc, argv)
int argc;
char *argv[];
{
	char *arg0 = argv[0];
	char *disk;
	int openflag;
	struct confinfo confinfo;

	sync();

	openflag = O_RDWR;
	disk ="/dev/dsk/c1d0s0";	/* default location of file system */

	/*
	 *  open the file system
	 */
	if ((fs = open(disk, openflag)) == -1) {
		fprintf(stderr, "%s: error opening file system: %s\n", arg0, disk);	/* error */
		perror(arg0);
		exit(1);
	}

	/*
	 * read block 0
	 */

	xread(&confinfo, sizeof(confinfo), BLK0);
	if (confinfo.swapdev != 0) {
		printf("swapdev is %d\n", confinfo.swapdev);
		exit(1);
	} else { 
		printf("%d\n", confinfo.swplo);
		close(fs);
		exit(0);
	}
}

#define RDWRSZ	1024
static unsigned char rwbuf[RDWRSZ];

void
xread(addr, count, seekval)
long addr;
long count;
long seekval;
{
	register seekaddr;

	if (((seekval & (RDWRSZ-1)) + count) > RDWRSZ) {
		printf("xread: read count too large - %d\n", count);
		exit(1);
	}
	seekaddr = seekval & ~(RDWRSZ-1);
	if (lseek(fs, seekaddr, 0) != seekaddr) {
		perror("xread: lseek");
		exit(1);
	}
	if (read(fs, rwbuf, RDWRSZ) == -1) {
		perror("xread: read");
		exit(1);
	}
	memcpy(addr, &rwbuf[seekval&(RDWRSZ-1)], count);
}
