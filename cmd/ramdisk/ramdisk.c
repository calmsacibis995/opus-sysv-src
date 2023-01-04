/*	Copyright (c) 1984 Fairchild Semiconductor Corp.	*/
/*	Copyright (c) 1987 Fairchild Semiconductor Corporation	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF 		*/
/*	AND FAIRCHILD SEMICONDUCTOR CORPORATION		   	*/
/*	FAIRCHILD SEMICONDUCTOR CORPORATION.			*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)ramdsk:ramdisk.c	1.3 (Fairchild) 1/22/87"

#include <stdio.h>
#include <sys/rgd.h>

long strtol();

#define CBASE "/dev/rdsk/"	/* Disk character devices */
#define BBASE "/dev/dsk/"	/* Disk block devices */

#define ptob(x) ((x)<<3)	/* 4Kb pages to 512b blocks */
#define btop(x) ((x)>>3)	/*   and back again */

void usage()
{
    fprintf(stderr,"usage: ramdsk devname cmd [args]\n");
    fprintf(stderr,"       cmd = free getsize mkfs mount resize or umount\n");
    exit(-1);
    /*NOTREACHED*/
}

/*
 * Readsize returns the number of pages representd by the string s.  The string
 * s is assumed to represent a number of 512 byte blocks (so the user interface
 * agrees with other file system utilities) unless suffixed with p or P to mean
 * 4Kb pages.
 */

int
readsize(s)
    char *s;			/* String to convert */
{
    char *ptr;			/* Addr of rest of s after conversion */
    int size;			/* Desired size */

    size = strtol(s,&ptr,10);	/* Convert the string */
    if (ptr == s) {
	fprintf(stderr,"size must be an integer optionally followed by p or P\n");
	exit(-1);
	/*NOTREACHED*/
    }
    if (*ptr != 'p' && *ptr != 'P') {
	size = btop(size + (ptob(1) - 1));	/* Round up to even pages */
    }
    return(size);
}

int
getsize(fd)
    int fd;			/* File descriptor of raw ramdisk device */
{
    int retval;			/* Return value from ioctl */
    int size;			/* Current ramdisk size (in CLIPPER pages) */

    retval = ioctl(fd, RGDGETSIZE, &size);
    if (retval) {
	fprintf(stderr,"Error - could not get current ramdisk size\n");
	exit(-1);
	/*NOTREACHED*/
    }
    printf("current ramdisk size = %d blocks, %d pages\n", ptob(size), size);
    return(size);
}

int
resize(fd,newsize)
    int newsize;		/* New ramdisk size (in CLIPPER pages) */
{
    int retval;			/* Return value from ioctl */
    int size;			/* Current ramdisk size */

    size = getsize(fd);
    if (size == newsize) {
	printf("no change in ramdisk size\n");
    }
    else {
	retval = ioctl(fd, RGDRESIZE, newsize);
	if (retval) {
	    fprintf(stderr,"Error - could not resize ramdisk\n");
	    exit(-1);
	    /*NOTREACHED*/
	}
	printf("new ramdisk size = %d blocks, %d pages\n",
	    ptob(newsize), newsize);
    }
    return(newsize);
}

main(argc,argv)
int argc;
char *argv[];
{
    int fd;		/* Raw ramdisk file descriptor */
    int cmd;		/* Ramdisk command reduced from command line */
    int retval;		/* Return value from various ramdisk ioctls */
    int size;		/* Current ramdisk size (in CLIPPER pages) */
    char rname[256];	/* Raw disk device name */

    if (argc < 3)
	usage();
    strcpy(rname,CBASE);			/* Build raw disk dev name */
    strcat(rname,argv[1]);
    fd = open(rname, 2);			/* Open it */
    if (fd < 2) {
	fprintf(stderr,"cannot open ramdisk %s\n",rname);
	exit(-1);
	/*NOTREACHED*/
    }

    if(!strcmp(argv[2], "free")) {		/* Free the ramdisk */
	retval = ioctl(fd, RGDFREE, 0);
	if (retval) {
	    fprintf(stderr,"Error - could not free ramdisk\n");
	    exit(-1);
	    /*NOTREACHED*/
	}
    }
    else if (!strcmp(argv[2], "getsize")) {	/* Print the size */
	size = getsize(fd);
    }
    else if (!strcmp(argv[2], "resize")) {	/* Create or change the size */
	if (argc <= 3) {
	    fprintf(stderr,"resize command requires new size argument\n");
	    exit(-1);
	    /*NOTREACHED*/
	}
	size = resize(fd,readsize(argv[3]));
    }
    else {
	usage();
	/*NOTREACHED*/
    }

    close(fd);
    return(0);
}
