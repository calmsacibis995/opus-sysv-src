/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	Copyright (c) by National Semiconductor Corp.		*/
/*	All Rights Reserved					*/

/* sex01pckd is combination of sex01up and packup */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <filehdr.h>
#include <scnhdr.h>
#include <ldfcn.h>
#include <sys/stropts.h>
#include <sys/sexuser.h>


#define isodev	"/dev/sex0"	/* dev to talk npack	*/
#define muxdriver	"/dev/npack"	/* mux driver for npack network layer*/

#define TIMEOUT	240		/* I_STR timeout value		*/


struct strioctl ioctlst;	/* STREAMS I_STR control str	*/



extern int errno, optind;
extern char *optarg;


main(argc, argv)
int argc;
char *argv[];
{
	register int i, sect;
	int fd_sex0, fd_npack; 


	switch (fork()) {
	case 0:		/* child */
		break;
	case -1:
		perror("npackup: fork failed");
	default:	/* parent */
		exit(0);
	}

	setpgrp();

	/* get access to the driver by calling its open routine. This will
	   setup the driver module and its stream head, calls drivers open
	   routine, and returns a file descriptor for the newly created stream
	*/
	if ((fd_sex0 = open(isodev, O_RDWR)) < 0 ) {
		perror("pckd: cannot open driver (isodev)");
		exit(1);
	}

	/* reset the board. This resets the board and configures it as well. All	   of this is done by calling sex_init routine of the driver.
	*/
	ioctlst.ic_cmd    = EI_RESET;
	ioctlst.ic_timout = TIMEOUT;
	ioctlst.ic_len    = 0;
	ioctlst.ic_dp     = NULL;

	if (ioctl(fd_sex0, I_STR, &ioctlst) < 0 ) {
		perror("pckd: unable to reset driver");
		exit(1);
	}


	/* enable the board for receiving packets */
	ioctlst.ic_cmd    = EI_TURNON;
	ioctlst.ic_timout = TIMEOUT;
	ioctlst.ic_len    = 0;
	ioctlst.ic_dp     = NULL;

	if (ioctl(fd_sex0, I_STR, &ioctlst) < 0 ) {
		perror("pckd: EI_TURNON ioctl failed");
		exit(1);
	}

	/* allocate receive buffers. These buffers are of q message type */
	/* should it be done before or after EI_TURNON???? */
	ioctlst.ic_cmd    = EI_ALLOC;
	ioctlst.ic_timout = TIMEOUT;
	ioctlst.ic_len    = 0;
	ioctlst.ic_dp     = NULL;

	if (ioctl(fd_sex0, I_STR, &ioctlst) < 0 ) {
		perror("pckd: EI_ALLOC ioctl failed");
		exit(1);
	}

	/* open the npack driver. It setup npack's module and stream head, and
	   calls npack's open routine, and returns a file descriptor for the
	   driver.
	*/
	if ((fd_npack = open(muxdriver, O_RDWR)) == -1) {
		perror("pckd: open npack failed");
		exit(1);
	}

	/* connect npack and sex driver together in such a way that npack
           will become a multiplexor so, several streams can connect to sex
	   driver.
	*/
	if (ioctl(fd_npack, I_LINK, fd_sex0) == -1) {
		perror("pckd: I_LINK ioctl failed");
		exit(1);
	}

	/** close(fd_sex0); **/
	fclose(stdin);
	fclose(stderr);
	fclose(stdout);

	/* hold npack open forever */
	pause();
}
