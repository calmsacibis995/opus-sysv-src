#include <sys/types.h>
#include <sys/iorb.h>
#include <sys/sysconf.h>
#include <sys/errno.h>
#include "opconfig.h"

unixfilsys(driveno) {
	register i,j;
	register section;
	register changed = 0;
	int val;
	int oldstart;
	int swapdrive;
	int swapsection;
	int swapdev;
	int swplo;
	int nswap;
	char dbuf[32];
	char diskname[32];

	rdblock0(0,READONLY);		/* find swap information */
	swapdrive = blk0->swapdev / NUMLOGVOL;
	swapsection = blk0->swapdev % NUMLOGVOL;
	swplo = blk0->swplo;
	nswap = blk0->nswap;

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
	printdiv("UNIX File System Section Definitions\n");
	nl();
	brintf("\tSection definitions consist of a start block\n");
brintf("\tnumber and a block count.  You may specify \"end\" instead\n");
	brintf("\tof a block count if you wish for the section to span the\n");
	brintf("\trest of the disk.\n");
	nl();
brintf("The section definitions for disk drive %d (dsk/%d) are as follows:\n",
		driveno,driveno);
	nl();
	for (i=0;i<NUMLOGVOL-1;i++){
		brintf("\t(%d)  dsk/c1d%ds%d : [%d,",i,driveno,i,
			blk0->logvol[i].start);
		if (blk0->logvol[i].len == -1)
			brintf("end]\n");
		else
			brintf("%d]\n",blk0->logvol[i].len);
	}
	nl();
	brintf("\t(q)  Done with file system section definitions.\n");
	nl();
	brintf("To modify any of the above section ");
	brintf("definitions, specify the section number.\n");
	nl();
	choose();
	if ((*lineptr == 'q') || (*lineptr == 'Q')) {
		if (changed)
			updblock0(driveno,WITHPROMPT);
		return;
	}
	if ((*lineptr >= '0' && *lineptr <= '6') ||
		(*lineptr == '7' && lineptr[1] == '!')) {
		section = response[0] - '0';
		resp("Type [start block,block count]: ");
		if (*lineptr) {
			changed++;
			oldstart = blk0->logvol[section].start;
			blk0->logvol[section].start = getnum(10);
			getcomma();
			if (!strncmp("end",lineptr,3))
				blk0->logvol[section].len = -1;
			else {
				val = getnum(10);
				if (((swplo+nswap) > val)
					&& driveno == swapdrive 
					&& section == swapsection) {
 printf("Section contains swap area.  It may not be decreased in size.\n");
					bpause("\t");
					blk0->logvol[section].start = oldstart;
					continue;
				}
				blk0->logvol[section].len = val;
			}
		}
	} else {
		bpause("Invalid response.  ");
	}
    }
}

getdrive(drive) {

	if (drive >= 0)
		return(drive);
	resp("Please specify a logical disk drive number [0-15]: ");
	nl();
	if (!*lineptr)
		drive = 0;
	else
		drive = getnum(10);
	if (drive < 0 || drive > 15)
		drive = 0;
	return(drive);
}
