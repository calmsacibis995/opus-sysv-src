#include <sys/types.h>
#include <sys/iorb.h>
#include <sys/sysconf.h>
#include <sys/errno.h>
#include "opconfig.h"

#define	Z_NODENAME	'1'
#define	Z_BUFS		'2'
#define	Z_PRIMARY	'3'
#define	Z_SECONDARY	'4'
#define	Z_SECTORS	'5'
#define Z_TABLESPARES	'6'
#define	Z_SPARES	'7'
#define	Z_FSTYPE	'8'
#define	Z_ROOTSIZ	'9'
#define	Z_ROOTDEV	'a'
#define	Z_PIPEDEV	'b'
#define	Z_DUMPDEV	'c'
#define	Z_SWAPDEV	'd'
#define	Z_SWPLO		'e'
#define	Z_NSWAP		'f'

struct confinfo dblock0;

unixsys(driveno) {
register changed;
register i;
register j;
static wizard = 0;

top:
if (wizard) {
printdiv("Opus5 UNIX System Parameters\n");
nl();
brintf("These parameters need not normally be modified, and should\n");
brintf("\tonly be modified by someone with a thorough understanding\n");
brintf("\tof the implications.  Otherwise, catastrophic results may\n");
brintf("\tresult, including damaging or destroying data and file systems.\n");
nl();
need(1);
brintf("Are you sure you want to modify the Opus5 UNIX System Parameters [y,n]? ");
	if (!yesno())
		return(0);;
}

	driveno = getdrive(driveno);

	if ((changed = rdblock0(driveno,READONLY))==BLKBAD) {
		printf("Uninitialized Parameter Block on drive %d.",driveno);
		printf(" Would you like to\n");
		printf("\tuse default values [y,n]? ");
		if (yesno()) {
			if ((changed = rdblock0(driveno,INITONLY))==BLKBAD) {
				printf("Unable to initialize Parameter Block");
				printf(" on drive %d with default values\n",
					driveno);
				bpause("\t");
				return(-1);
			}
		} else 
			return(-1);
	}

	while (1) {            
	printdiv("Opus5 UNIX System Parameters\n");
	nl();
	brintf("Disk Drive %d (dsk/%d) Parameter Block:\n",
		driveno,driveno);
	if (driveno != 0) {
		nl();
		brintf("WARNING: Only the parameter block on drive 0");
		brintf(" is used by the system.\n");
		brintf("\tThese parameters on drive %d will have no effect",
			driveno);
		brintf(" with your\n");
		brintf("\tcurrent configuration.\n");
	}
	nl();
	brintf("\t(%c)  System nodename     = %s\n",Z_NODENAME,blk0->nodename);
	brintf("\t(%c)  System buffers      = %d\n",Z_BUFS,blk0->nbuf);
	brintf("\t(%c)  Bootstrap name      = %s\n",Z_PRIMARY,blk0->bootname);
	brintf("\t(%c)  Opus5 boot name     = %s\n",Z_SECONDARY,blk0->unixname);
	brintf("\t(%c)  Blocks this drive   = %d\n",Z_SECTORS,blk0->sectors);
	brintf("\t(%c)  First spare block   = %d\n",Z_TABLESPARES,
		blk0->sparetbl);
	brintf("\t(%c)  Spare blocks        = %d\n",Z_SPARES,blk0->spares);
	brintf("\t(%c)  File system type    = %d\n",Z_FSTYPE,blk0->fstype);
	brintf("\t(%c)  Root fs size        = %d\n",Z_ROOTSIZ,blk0->rootsects);
	brintf("\t(%c)  Rootdev             = %d,%d\n",Z_ROOTDEV,
		blk0->rootdev>>8, blk0->rootdev&0xff);
	brintf("\t(%c)  Pipedev             = %d,%d\n",Z_PIPEDEV,
		blk0->pipedev>>8, blk0->pipedev&0xff);
	brintf("\t(%c)  Dumpdev             = %d,%d\n",Z_DUMPDEV,
		blk0->dumpdev>>8, blk0->dumpdev&0xff);
	brintf("\t(%c)  Swapdev             = %d,%d\n",Z_SWAPDEV,
		blk0->swapdev>>8, blk0->swapdev&0xff);
	brintf("\t(%c)  Swplo               = %d\n",Z_SWPLO,blk0->swplo);
	brintf("\t(%c)  Nswap               = %d\n",Z_NSWAP,blk0->nswap);
	nl();
	brintf("\t(q)  Return to top level\n");
	nl();

	choose();

	switch(response[0]) {
	    case Z_PRIMARY:
		resp("Initial bootstrap [%s]? ", blk0->bootname);
		changed++;
		for (i=0; i<BOOTNAMESIZE; i++) {
			blk0->bootname[i] = lineptr[i];
		}
		break;

	    case Z_SECONDARY:
		resp("Name of program to boot [%s]? ", blk0->unixname);
		changed++;
		for (i=0; i<UNIXNAMESIZE; i++) {
			blk0->unixname[i] = lineptr[i];
		}
		break;
		
	    case Z_TABLESPARES:
		resp("First block number for spare table [%d]? ",
			blk0->sparetbl);
		if (*response) {
			changed++;
			blk0->sparetbl = getnum(10);
		}
		break;

	    case Z_SPARES:
		resp("Spare blocks [%d]? ",blk0->spares);
		if (*response) {
			changed++;
			blk0->spares = getnum(10);
		}
		break;

	    case Z_SECTORS:
		resp("Number of blocks on drive [%d]? ",
			blk0->sectors);
		if (*response) {
			changed++;
			blk0->sectors = getnum(10);
		}
		break;

	    case Z_FSTYPE:
		resp("File system type [%d]? ", blk0->fstype);
		if (*response) {
			if (getnum(10) != 3) {
				bpause("File system type not allowed.  ");
			}
		}
		break;

	    case Z_ROOTSIZ:
		resp("Root file system size in blocks [%d]? ",
			blk0->rootsects);
		if (*response) {
			changed++;
			blk0->rootsects = getnum(10);
		}
		break;

	    case Z_NODENAME:
		resp("Opus5 system nodename (for uucp) [%s]? ",
			blk0->nodename);
		changed++;
		for (i=0; i<NODENAMESIZE; i++) {
			blk0->nodename[i] = response[i];
		}
		break;

	    case Z_ROOTDEV:
		resp("Rootdev  (specify major device, minor device) [%d,%d]? ",
			blk0->rootdev>>8,blk0->rootdev&0xff);
		if (*response) {
			changed++;
			blk0->rootdev = blk0->pipedev = getdev();
		}
		break;

	    case Z_PIPEDEV:
		resp("Pipedev  (specify major device, minor device) [%d,%d]? ",
			blk0->pipedev>>8,blk0->pipedev&0xff);
		if (*response) {
			changed++;
			blk0->pipedev = blk0->rootdev = getdev();
		}
		break;

	    case Z_DUMPDEV:
		resp("Dumpdev  (specify major device, minor device) [%d,%d]? ",
			blk0->dumpdev>>8,blk0->dumpdev&0xff);
		if (*response) {
			changed++;
			blk0->dumpdev = getdev();
		}
		break;

	    case Z_SWAPDEV:
		resp("Swapdev  (specify major device, minor device) [%d,%d]? ",
			blk0->swapdev>>8,blk0->swapdev&0xff);
		if (*response) {
			changed++;
			blk0->swapdev = getdev();
		}
		break;

	    case Z_SWPLO:
		resp("Swplo [%d]? ",blk0->swplo);
		if (*response) {
			changed++;
		    	blk0->swplo = getnum(10);
		}
		break;

	    case Z_NSWAP:
		resp("Nswap [%d]? ",blk0->nswap);
		if (*response) {
			changed++;
		    	blk0->nswap = getnum(10);
		}
		break;

	    case Z_BUFS:
		resp("System buffers [%d]? ",blk0->nbuf);
		if (*response) {
			changed++;
			blk0->nbuf = getnum(10);
		}
		break;

	    case 'q':
	    case 'Q':
		if (changed)
			updblock0(driveno,WITHPROMPT);
		return(0);

	    default:
		bpause("Illegal response.  ");
	    }
	}
}

updblock0(disknum,flag)		/* flag says to ask if its okay or not */
{
	char diskname[32];
	int pfile;

	if (flag == WITHPROMPT) {
		brintf("Would you like to update this information ");
		brintf("on disk [y/n]? ");
		if (!yesno())
			return(-1);
	}
	sprintf(diskname,"/dev/dsk/%ds7",disknum);
	pfile = open(diskname,2);
	setchksum(blk0);
        lseek(pfile, 0L, 0);
	if(write(pfile, blk0, sizeof block0) != sizeof block0) {
		brintf("Unable to write Parameter Block (block zero)");
		brintf(" of drive %d (dsk/%d)\n\t",disknum,disknum);
		if (_diskerrno == E_BADB) {
			brintf("\nThis block is designated as a bad block and");
			brintf(" may not be used\n");
			brintf("\tas the first block of of a UNIX partition.  ");
			bpause("");
			exit(6);
		}
		bpause("");
		close(pfile);
		return(-2);
	}
	globchanged++;
	if (flag == WITHPROMPT) {
		brintf("Parameter block (block zero) of drive %d (dsk/%d) ",
			disknum,disknum);
		brintf("updated.\n");
		nl();
	}
	close(pfile);
}

rdblock0(disknum,flag) {	/* Flag says to just read and initialize if */
				/*  necessary (READINIT), initialize only */
				/*  (INITONLY), or just read (READONLY) */
				/* Return values are BLKBAD if invalid blk 0, */
				/*  BLKOK if valid blk 0, or BLKMOD if blk 0 */
				/*  was modified and is now valid */
	register changed = BLKOK;
	char diskname[32];
	int pfile;

#ifdef STANDALONE
	partsize(disknum,&partinfo[disknum]);
	if (partinfo[disknum].status == -1)	/* not a legitimate drive */
		return(BLKBAD);
#endif
	sprintf(diskname,"/dev/dsk/%ds7",disknum);
	pfile = open(diskname,2);
	if (pfile < 0)
		return(BLKBAD);

	if (flag == READINIT || flag == INITONLY) {
		copy(&block0,&dblock0,sizeof(struct confinfo));
		if (flag == INITONLY) {
			close(pfile);
			return(BLKMOD);
		}
	}
	lseek(pfile, 0L, 0);
	if(read(pfile, blk0, sizeof(struct confinfo))
			!= sizeof(struct confinfo)) {
		if (flag == READONLY) {
			close(pfile);
			return(BLKBAD);
		}
		lseek(pfile, 0L, 0);
		write(pfile, &dblock0, sizeof(struct confinfo));
		lseek(pfile, 0L, 0);
		if(read(pfile, blk0, sizeof(struct confinfo))
			!= sizeof(struct confinfo)) {
		  brintf("\nCould not read block zero of drive %d (dsk/%d).  ",
			disknum,disknum);
		  bpause("");
		  close(pfile);
		  return(BLKBAD);
		}
	}
	close(pfile);

	if (blk0->magic != 'opus' || chksum(blk0) != BLKCHKSUM) {
		if (flag == READONLY)
			return(BLKBAD);
		brintf("Uninitialized Parameter Block on block 0 of drive %d.\n",
			disknum);
		pause("\tUsing default values.  ");
		if (*lineptr == 'q' || *lineptr == 'Q')
			return(BLKBAD);
		copy(&block0,&dblock0,sizeof(struct confinfo));
		changed = BLKMOD;
	}
	return(changed);
}

getdev()
{
	register major;
	register minor;

	major = getnum(10);
	getcomma();
	minor = getnum(10);
	return(((major<<8)&0xff00) | (minor&0xff));
}

chksum(buf)
struct confinfo *buf;
{
	int i;
	long *lptr;
	long sum = 0;

	lptr = (long *)buf;
	for (i=0; i<512; i+=4)
		sum += *lptr++;
	return(sum);
}

setchksum(buf)
struct confinfo *buf;
{
	int i;
	long *lptr;
	long sum = 0;

	buf->csum = 0;
	lptr = (long *)buf;
	for (i=0; i<sizeof(struct confinfo); i+=4)
		sum += *lptr++;
	buf->csum = BLKCHKSUM - sum;
	if (chksum(buf) != BLKCHKSUM) {
		brintf("Did not compute correct checksum %X\n",chksum(buf));
		exit(24);
	}
}

