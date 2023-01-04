
/* SID @(#)disklib.c	1.2 */

/*
 * Library of special disk driver functions.
 */

#include <sys/dkcomm.h>
#include <fcntl.h>

#define DISKNAME "/dev/rdsk/c1d0s0"  /* dev used for disk ioctl calls */

struct diskcommand comm;

extern int errno;

/*
 * Do a disk ioctl command to DISKNAME.
 */
static
diskcmd(cmd, comm)
struct diskcommand *comm;
{
	int fd;
	int err;

	if ((fd = open(DISKNAME, O_RDONLY)) == -1) {
		err = errno;
	} else if (ioctl(fd, cmd, comm) < 0) {
		err = errno;
	} else {
		err = 0;
	}
	close(fd);
	return(err);
}

/*
 * Get the file name associated with a disk drive.
 */
getdiskname(drive, name)
int drive;
char *name;
{
	int err;

	comm.drive = drive;
	if ((err = diskcmd(DKGETFNAME, &comm)) == 0) {
		strcpy(name, comm.name);
	}
	return(err);
}

/*
 * Set the file name associated with a disk drive.
 */
setdiskname(drive, name)
int drive;
char *name;
{

	comm.drive = drive;
	strncpy(comm.name, name, NAMESIZE);
	comm.name[NAMESIZE-1] = 0;  /* make sure string is terminated */
	return(diskcmd(DKSETFNAME, &comm));
}

/*
 * Get the parm associated with a disk drive.
 */
getdiskparm(drive, parm)
int drive;
int *parm;
{
	int err;

	comm.drive = drive;
	if ((err = diskcmd(DKGETFPARM, &comm)) == 0) {
		*parm = comm.parm;
	}
	return(err);
}

/*
 * Wait for all disk queues to drain.
 */
draindisk()
{

	return(diskcmd(DKDRAINDISK, &comm));
}
