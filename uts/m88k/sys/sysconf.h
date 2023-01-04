#ifndef _SYS_SYSCONF_H_
#define _SYS_SYSCONF_H_


/*	Copyright (c) 1989 Opus Systems	*/
/*	      All Rights Reserved	*/

/*	SID @(#)sysconf.h	1.1	*/

#ifdef __88000_OCS_DEFINED
#include "Not OCS compliant."
#endif

/*
 * Configuration information
 */

#define NODENAMESIZE 12
#define BOOTNAMESIZE 16
#define UNIXNAMESIZE 32
#define PAD1SIZE 28
#define PAD2SIZE 32
#define NUMSPARE 63
#define NULLBLKPTR -1
#define BLKCHKSUM -1

#define CONFMAGIC0 'opus'

#define DEVMAPSIZE	64
#define PCTYPESIZE	8
#define TZSIZE		8

/*
 * Description of a logical volume
 */

struct logvol {
	unsigned long start;  	/* starting block number of volume */
	unsigned long len;	/* length of volume */
};

/*
 * Structure of a device map from dos to unix
 */

struct devmap {
	short unixdev;
	short dosdev;
};

/*
 * Dos information block
 */

struct dosinfo {
	struct devmap unixdos[DEVMAPSIZE];
	char segment;
	char intlev;
	char pad[2];
	char timezone[TZSIZE];
	char pctype[PCTYPESIZE];
};

/*
 * System configuration info
 */

struct confinfo {
	char nodename[NODENAMESIZE];  /* systems node name */
	short rootdev;  	/* unix root device */
	short pipedev;  	/* unix pipe device */
	short dumpdev;  	/* unix dump device */
	short swapdev;  	/* unix swap device */
	daddr_t swplo;  	/* first sector of swap area on swapdev */
	long  nswap;		/* number of sectors of swap area on swapdev */
	char bootname[BOOTNAMESIZE];  /* name of boot program */
	char unixname[UNIXNAMESIZE];  /* name of unix to boot */
	long sectors;		/* total number of sectors this partition */
	long spares;		/* total number spare sectors for bad blocks */
	short used;		/* spare sectors used for bad blocks */
	short entries;		/* entries in spare table */
	daddr_t sparetbl;	/* block number of first spare table block */
	long rootsects;		/* size of root file system */
	long fstype;		/* root file system type */
	char pad1[PAD1SIZE];  	/* padding to put logvol at 128 */
	struct logvol logvol[8];  /* logical volume info */
	struct dosinfo dosdata;	/* dos related information */
	char pad2[PAD2SIZE];	/* padding to force structure to 512 bytes */
	long nbuf;  		/* number of system disk buffers */
	long magic;		/* magic number for block 0 */
	long csum;  		/* checksum for confinfo struct */
};

/*
 * Description of a spared block
 */

struct spare {
	unsigned long badblk;  		/* address of bad block */
	unsigned long spareblk;		/* address of substitute block */
};

/*
 * Spare table block
 */

struct spareinfo {
	struct spare spare[NUMSPARE];  /* spares mapping */
	int link;  /* address of next spare table block */
	long csum;  /* checksum of spareinfo structure */
};

/*
 * User supplied overrides for configuration info.
 */

struct confoverride {
	int flags;  /* indicates which overrides were supplied */
	int nbuf;  /* number of buffers */
	short dumpdev;  /* unix dump device */
	short pipedev;  /* unix pipe device */
	short rootdev;  /* unix root device */
	short swapdev;  /* unix swap device */
	daddr_t swplo;  /* first block of swap area on swapdev */
	int  nswap;  /* number of blocks of swap area on swapdev */
	char nodename[NODENAMESIZE];  /* systems node name */
	int blk0dix;  /* dix which contains the initialization data */
	int drive;  /* drive for modified section data */
	int sect;  /* section number for modified section data */
	int sectstart;  /* start of modified section */
	int sectlen;  /* length of modified section */
};

/* confoverride.flags definitions */

#define OVNBUF		0x0001
#define OVPIPEDEV	0x0002
#define OVROOTDEV	0x0004
#define OVSWAPDEV	0x0008
#define OVSWPLO		0x0010
#define OVNSWAP		0x0020
#define OVNODENAME	0x0040
#define OVDUMPDEV	0x0080
#define OVBLK0DIX	0x0100
#define OVSECTDEF	0x0200

/* printflg catagories */

#define PRINT_STD		1	/* print standard salutation message */
#define PRINT_COPYRIGHT		2	/* print copyright notice */

#endif	/* ! _SYS_SYSCONF_H_ */
