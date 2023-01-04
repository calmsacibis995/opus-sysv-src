#include <sys/types.h>
#include <sys/iorb.h>
#include <sys/dkcomm.h>
#include <sys/sysconf.h>
#include <sys/errno.h>
#include "opconfig.h"

#define NUMPARTS	4
#define PARTOFFSET 	0x1be

#define D_DSKBLK	1
#define D_PARTYPE	2
#define D_CYLCNT	3
#define D_HEADCNT	4
#define D_SECTTRK	5
#define D_DRNAME	6
#define D_RDPART	7
#define D_WRTPART	8
#define D_DSKSIZE	9
#define D_TRUNCATE	10
#define D_TOTSIZE	11

setupdisk(tflag,driveno) {
	int i;
	int beenhere;
	int j;
	int fid;

	printdiv("Disk partitioning and setup\n");
	nl();
	beenhere = 0;
	for (i=0; i<DKMAX; i++) {
		if (driveno > -1) {
			if (beenhere++)
				return(0);
			i = driveno;
		}
		partsize(i,&partinfo[i]);
		if (partinfo[i].status < 0) 	/* does not exist or old DOS */
			continue;
		if (partinfo[i].parttype >= 10)	/* no partition yet */
			setuppart(i);
		else {				/* new DOS partition w/size */
			chkdsksize(i);	
		}
		if (partinfo[i].parttype >= 10) {
			readpartdata(i,&partblk[0]);
			for (j=0; j<4; j++) {
				if (partinfo[i].parttype == partblk[j].systind)
					partinfo[i].sectsperpart =
						partblk[j].numsects;
			}
		}
	}
	return(setupall(tflag)); 
}

firstpartblk(driveno) {
	int i;
	int j;

	i = driveno;
	partsize(i,&partinfo[i]);
	if (partinfo[i].status < 10) 	/* not a partitioned drive */
		return(-2);
	readpartdata(i,&partblk[0]);
	for (j=0; j<4; j++) {
		if (partinfo[i].parttype == partblk[j].systind)
			return(partblk[j].relsect);
	}
	return(-1);
}

numpartblk(driveno) {
	int i;
	int j;

	i = driveno;
	partsize(i,&partinfo[i]);
	if (partinfo[i].status < 10) 	/* not a partitioned drive */
		return(-2);
	readpartdata(i,&partblk[0]);
	for (j=0; j<4; j++) {
		if (partinfo[i].parttype == partblk[j].systind)
			return(partblk[j].numsects);
	}
	return(-1);
}

setupall(flag) {
	int i;
	int j;
	int driveno;
	int pfile;
	char diskname[32];

    	while (1) {
		printdiv("Current available logical disk drives\n");
		nl();
		printf("Drive   Drive    Partition    Sector      Opmon\n");
		printf("number  name       type        count      name\n");
		nl();
		for (i=0; i<DKMAX; i++) {
			partsize(i,&partinfo[i]);
 					/* not a partitioned drive */
			if (partinfo[i].status == -1)
				continue;
			if (partinfo[i].status < 0) /* not there or old DOS */
				partinfo[i].sectsperpart = 0;
			if (partinfo[i].parttype >= 10) {
				readpartdata(i,&partblk[0]);
				for (j=0; j<4; j++) {
					if (partinfo[i].parttype == 
							partblk[j].systind)
						partinfo[i].sectsperpart =
							partblk[j].numsects;
				}
			}
			printf(" (%d)\t",i);
			switch(i) {
			case 11:
				printf("flpB\t  ");
				break;
			case 12:
				printf("flpA\t  ");
				break;
			case 13:
				printf("flpb\t  ");
				break;
			case 14:
				printf("flpa\t  ");
				break;
			case 15:
				printf("swap\t  ");
				break;
			default:
				printf("dsk/%d\t  ",i);
				break;
			}
			if (partinfo[i].parttype < 0)
				if ((partinfo[i].status == -2) &&
					(i != 11) && (i != 12))
					printf("DOS FILE");
				else
					printf("        ");
			else if (partinfo[i].parttype == 4 || partinfo[i].parttype == 6) 
				printf("DOS FILE");
			else if (partinfo[i].parttype == 10)
				printf("  UNIX  ");
			else
				printf("   %2d   ",partinfo[i].parttype);
			if (partinfo[i].parttype >= 0) {
				sprintf(diskname,"/dev/dsk/%ds7",i);
				pfile = open(diskname,2);
				if (pfile < 0)
					partinfo[i].sectsperpart = 0;
				printf("  %8d",partinfo[i].sectsperpart);
			} else
				printf("          ");
			printf("      %s",partinfo[i].drivername);
			nl();
		}
		nl();
		if (partinfo[0].status == -1 || partinfo[0].nblk == 0 || (partinfo[0].parttype >=0 && partinfo[0].sectsperpart <= 0)) {
			printf("The disk for the root file system (dsk/0)");
			printf(" has not been set up properly.\n");
			printf("\tYou must have a unique entry in the");
			printf(" configuration file opus.cfg\n");
			printf("\tfor dsk/0.  If dsk/0 is a DOS file,");
			printf(" you must create the directories\n");
			printf("\ton each drive specified (e.g.,");
			printf(" \\opus\\opfs).  Would you like to\n");
			printf("\tcontinue anyway [y,n]? ");
			if (!yesno())
				exit(10);
		}
		printf("To change any of the partition information");
		printf(" for the above drives,\n");
		printf("\ttype the drive number;");
		printf(" otherwise type \"y\" if okay: ");
		getresponse();
		nl();
		if (*lineptr == 'y' || *lineptr == 'Y') {
			return(0);
		} else if (*lineptr == 'q' || *lineptr == 'Q') {
			return(-1);
		} else if (*lineptr) {
			driveno = getnum(10);
			if (setuppart(driveno) < 0) {
				printf("Drive %d (dsk/%d) is a DOS file and cannot be partitioned.\n",
					driveno,driveno);
				bpause("\t");
			}
	if (rdblock0(driveno,READONLY)==BLKBAD) {
		printf("Uninitialized Parameter Block on drive %d.",driveno);
		printf(" Would you like to\n");
		printf("\tuse default values [y,n]? ");
		if (yesno()) {
			if (rdblock0(driveno,INITONLY)==BLKBAD) {
				printf("Unable to initialize Parameter Block");
				printf(" on drive %d with default values\n",
					driveno);
				bpause("\t");
			}
		}
	}
		}
    	}
}

partsize(driveno,partbuf)
int driveno;
struct partinfo *partbuf;
{
	int status;

	partbuf->status = -1;
#ifdef STANDALONE
	partbuf->status = _dkioctl(driveno,D_DRNAME,partbuf->drivername);
	if (partbuf->status >= 0) {
		_dkioctl(driveno,D_DSKBLK,&partbuf->nblk);
		_dkioctl(driveno,D_PARTYPE,&partbuf->parttype);
		_dkioctl(driveno,D_CYLCNT,&partbuf->cylcnt);
		_dkioctl(driveno,D_HEADCNT,&partbuf->headcnt);
		_dkioctl(driveno,D_SECTTRK,&partbuf->secttrk);
		_dkioctl(driveno,D_DSKSIZE,&partbuf->sectsperpart);
		_dkioctl(driveno,D_TOTSIZE,&partbuf->totsects);
		partbuf->sectspercyl = partbuf->headcnt * partbuf->secttrk;
		partbuf->status = partbuf->parttype;
	}
#endif
}

chkdsksize(disknum) {
	int i;
	int fid;
	int ioctlrval;
	char drivename[16];

#ifdef STANDALONE
	partsize(disknum,&partinfo[disknum]);
	if (partinfo[disknum].status < 0)
		return(-1);
	if (partinfo[disknum].parttype < 0 || partinfo[disknum].parttype >= 10)
		return(-1);
	if (partinfo[disknum].nblk != partinfo[disknum].totsects) {
		printf("There already exists a DOS file with a different size associated\n");
		printf("\twith this device (%s).  Do you wish to resize\n",partinfo[disknum].drivername);
		printf("\tit according to the information in opus.cfg [y,n]? ");
		if (yesno()) {
			ioctlrval = _dkioctl(disknum,D_TRUNCATE);
			partsize(disknum,&partinfo[disknum]);
		} else {
			printf("\nThis file will not be accessible by UNIX.\n");
			seeman();
			return(-1);
		}
	}
	if (ioctlrval < 0 || partinfo[disknum].nblk != partinfo[disknum].totsects) {
		printf("\nThe requested sized file cannot be created on your system.  Either you don't\n");
		printf("\thave enough space on your disk(s) or your DOS software is not a\n");
		printf("\tsupported version.\n");
		seeman();
		return(-1);
	} 
	return(0);
#endif /* STANDALONE */
}

setuppart(disknum) {
	int i;
	int fid;
	char drivename[16];

	partsize(disknum,&partinfo[disknum]);
	if (partinfo[disknum].status < 0)
		return(-1);
	if (partinfo[disknum].parttype < 10)
		return(-1);
	if (readpartdata(disknum,&partblk[0]) < 0)
		return(-1); 
	for (i=0; i<4; i++) {
		if (partblk[i].systind == partinfo[disknum].parttype)
			if (partblk[i].relsect != 0 || partblk[i].numsects > 0)
				break;
	}
	if (i == 4) {
		printf("Do you wish to use the available disk space as a");
		if (partinfo[disknum].parttype == 10)
			printf(" UNIX partition\n\t");
		else
			printf(" UNIX (type %d)\n\tpartition ",
				partinfo[disknum].parttype);
		printf("for logical drive %d (dsk/%d) [y,n]? ",disknum,disknum);
		if (yesno()) {
			printf("Allocating the largest contiguous disk");
			if (partinfo[disknum].parttype == 10)
				printf(" area as a UNIX partition\n");
			else
				printf(" area as a UNIX (type %d) partition\n",
					partinfo[disknum].parttype);
			nl();
			addunixpart(disknum);
		}
	}
	printf("The following table shows the usage of disk space on");
	printf(" drive %d\n",disknum);
	nl();
	if (displaypart(disknum,0) < 1)
		return(0);
}

addunixpart(disknum) {
	int i;
	int bmax;
	int firstblk;
	int lastdiskblk;
	int cyl;
	int start;
	
	bmax = 0;
	partsize(disknum,&partinfo[disknum]);
	if ((partinfo[disknum].status < 0) || (partinfo[disknum].parttype < 0)
		|| (readpartdata(disknum,&partblk[0]) < 0)) {
		bpause("Unable to create a UNIX partition.  ");
		return(-1);
	}
	lastdiskblk = (partinfo[disknum].cylcnt) * 
			partinfo[disknum].sectspercyl;
	start = partinfo[disknum].secttrk;
	for (i=(start ? start : 1); i < lastdiskblk;
			i += partinfo[disknum].sectspercyl) {
		if (maxallowed(disknum,4,i) > bmax) {
			bmax = maxallowed(disknum,4,i);
			firstblk  = i;
		}
	}
	if (disknum==0 && partinfo[disknum].parttype==10 && bmax<MINUNIXSIZE) {
		printf("Insufficient space available for root file system.\n");
#if pm100
		bpause("\tPlease see the Opus 100PM User Manual.  ");
#endif
#if pm200
		bpause("\tPlease see the Opus 200PM User Manual.  ");
#endif
#if pm300
		bpause("\tPlease see the Opus 300PM User Manual.  ");
#endif
#if pm400
		bpause("\tPlease see the Opus 400PM User Manual.  ");
#endif
		return(-1);
	}
	if (bmax < 100) {
		printf("Insufficient space available for a UNIX file system.\n");
#if pm100
		bpause("\tPlease see the Opus 100PM User Manual.  ");
#endif
#if pm200
		bpause("\tPlease see the Opus 200PM User Manual.  ");
#endif
#if pm300
		bpause("\tPlease see the Opus 300PM User Manual.  ");
#endif
		return(-1);
	}
	for (i=0; i<4; i++) {
		if (partblk[i].systind == 0 || 
		    (partblk[i].systind == partinfo[disknum].parttype)) {
    			heads = partinfo[disknum].headcnt;
    			sects = partinfo[disknum].secttrk;
			partblk[i].bootind = 0;
			partblk[i].systind = partinfo[disknum].parttype;
			partblk[i].relsect = firstblk;
			bmax = (bmax + firstblk) / partinfo[disknum].sectspercyl
				* partinfo[disknum].sectspercyl - firstblk;
			partblk[i].numsects = bmax;
			cyl = partblk[i].relsect/partinfo[disknum].sectspercyl;
			if (cyl >= 1024)
				cyl = 1023;
			partblk[i].hibegcyl = cyl >> 8;
			partblk[i].beghead = (partblk[i].relsect/sects) % heads;
			partblk[i].begsect = 1;
			partblk[i].begcyl = cyl & 0xff;
			cyl = (partblk[i].relsect + partblk[i].numsects - 1)
				/ partinfo[disknum].sectspercyl;
			if (cyl >= 1024)
				cyl = 1023;
			partblk[i].hiendcyl = cyl >> 8;
			partblk[i].endhead = heads - 1;
			partblk[i].endsect = sects;
			partblk[i].endcyl = cyl & 0xff;
			writepartdata(disknum,partblk);
			rdblock0(disknum,INITONLY);
			updblock0(disknum,NOPROMPT);
			return(0);
		}
	}
	printf("Unable to create a UNIX partition.  All partitions have\n");
	bpause("\talready been allocated.  ");
	return(-1);
}

maxallowed(disknum,part,bn) {
	int i;
	int bmax;
	int newmax;
	int sect;
	int end;
	int lastdiskblk;

	lastdiskblk = (partinfo[disknum].cylcnt) * 
			partinfo[disknum].sectspercyl;
	bmax = lastdiskblk - bn;
	for (i=0; i<4; i++) {
		if (part == i)
			continue;
		if (partblk[i].systind == 0)
			continue;
		sect = partblk[i].relsect;
		end = sect + partblk[i].numsects;
		if (bn >= end)
			newmax = lastdiskblk - bn;
		else if (bn < sect && bn < end)
			newmax = sect - bn;
		else
			return(-1);
		if (newmax < bmax)
			bmax = newmax;
	}
	return(bmax);
}
			
displaypart(driveno,changed) {
    int i;
    int firstsect;
    int numsects;
    int systype;
    int newval;
    int oldval;
    int head, sector, cyl;
    int select;
    int sectspercyl;
    int lastdiskblk;

    partsize(driveno,&partinfo[driveno]);
    sectspercyl = partinfo[driveno].sectspercyl;
    lastdiskblk = (partinfo[driveno].cylcnt) * partinfo[driveno].sectspercyl;
    heads = partinfo[driveno].headcnt;
    sects = partinfo[driveno].secttrk;
    select = -1;
    for (i=0; i<NUMPARTS; i++)
	if (partinfo[driveno].parttype == partblk[i].systind)
		select = i;
    while (1) {
    	partsize(driveno,&partinfo[driveno]);
    	sectspercyl = partinfo[driveno].sectspercyl;
    	lastdiskblk = (partinfo[driveno].cylcnt)*partinfo[driveno].sectspercyl;
    	heads = partinfo[driveno].headcnt;
    	sects = partinfo[driveno].secttrk;
	divider();
	printf("Current Fixed Disk Logical Drive \"dsk/%d\"\n",driveno);
	nl();
	printf("Partition   Status   Type    Start      End     Cyls");
	printf("  Sectors\n");
#ifdef DEBUG
	for (i=0; i<NUMPARTS; i++) {
		printf("boot ind = %d, beg head = %d\n",partblk[i].bootind,
			partblk[i].beghead);
		printf("beg cyl = %d, beg sect = %d\n",partblk[i].begcyl
			| (partblk[i].hibegcyl<<8),partblk[i].begsect);
		printf("syst ind = %d, end head = %d\n",partblk[i].systind,
			partblk[i].endhead);
		printf("end sect = %d, end cyl = %d\n",partblk[i].endsect,
			partblk[i].endcyl | (partblk[i].hiendcyl<<8));
		printf("rel sect = %d, num sects = %d\n",partblk[i].relsect,
			partblk[i].numsects);
	}
#endif
	for (i=0; i<NUMPARTS; i++) {
		firstsect = partblk[i].relsect;
		numsects = partblk[i].numsects;
		printf("%c   %d",(select == i) ? '>' : ' ',i);
		printf("          %c     ",
			partblk[i].bootind == 0x80 ? 'A' : 'N');
		systype = partblk[i].systind;
		if (systype == 1 || systype == 4)
			printf(" DOS");
		else if (systype == 10)
			printf("UNIX");
		else 
			printf("%4d",systype);
		printf("   %6d",firstsect/sectspercyl);
		printf("   %6d",
			numsects ? (firstsect + numsects - 1) / sectspercyl:0);
		printf("   %6d",numsects ? ((numsects-1)/sectspercyl) + 1 : 0);
		printf("   %6d",numsects);
		printf("  (%d.%d MB)\n",MEGSX10(numsects)/10,
			MEGSX10(numsects)%10);
	}
	nl();
	printf("Total Disk                  %6d   %6d   %6d   %6d  (%d.%d MB)",
		0,(lastdiskblk-1)/sectspercyl,(lastdiskblk-1)/sectspercyl+1,
		lastdiskblk,MEGSX10(lastdiskblk)/10,MEGSX10(lastdiskblk)%10);
	nl();
	nl();
	while (select == -1) {
	    printf("If this is not acceptable, type the");
	    printf(" partition number [0-3] of the partition\n");
	    printf("\tyou wish to modify;");
	    printf(" type \"u\" if you would like to reread\n");
	    printf("\tthe table from the disk;");
	    printf(" otherwise type \"y\" if okay: ");
	    getresponse();
	    nl();
	    if (*lineptr == 'y' || *lineptr == 'Y') {
done:
		for (i=0; i<4; i++) {
		    	if (maxallowed(driveno,i,partblk[i].relsect) < 0) {
				printf("Illegal start cylinder of");
				printf(" partition %d conflicts with",i);
				printf(" another partition.\n");
				printf("\tType <cr> if you would like to");
				printf(" continue anyway; type \"y\" if you\n");
				printf("\twould like to fix this problem: ");
				if (yesno())
					break;
				else
					continue;
		    	} else if (maxallowed(driveno,i,partblk[i].relsect) <
					partblk[i].numsects) {
				printf("Illegal end cylinder of");
				printf(" partition %d conflicts with",i);
				printf(" another partition.\n");
				printf("\tType <cr> if you would like to");
				printf(" continue anyway; type \"y\" if you\n");
				printf("\twould like to fix this problem: ");
				if (yesno())
					break;
				else
					continue;
			} else if (partblk[i].numsects >
					(lastdiskblk - partblk[i].relsect)) {
				printf("Illegal end cylinder of partition");
				printf(" %d is past end of disk.\n",i);
				printf("\tType <cr> if you would like to");
				printf(" continue anyway; type \"y\" if you\n");
				printf("\twould like to fix this problem: ");
				if (yesno())
					break;
				else
					continue;
			}
		}
		if (i == 4) {
			if (changed) {
			    if (writepartdata(driveno,partblk)) {
				rdblock0(driveno,INITONLY);
				updblock0(driveno,NOPROMPT);
				printf("Modified partition information");
				printf(" written to drive \"%d\".\n",driveno);
				nl();
				return(0);
			    } else
				return(-1);
			} else
			    return(0);
		} else
			break;
	    } else if (*lineptr == 'u' || *lineptr == 'U') {
		readpartdata(driveno,&partblk[0]);
	    } else if (*lineptr >= '0' && *lineptr <= '3') {
	    	select = i = *lineptr - '0';
	    } else if (*lineptr == '!')
		parmagic = 0;
	    else
	    	continue;
	    break;
	}	
	i = select;
	while (i >= 0 && i <= 3) {
		firstsect = partblk[i].relsect;
		numsects = partblk[i].numsects;
		printf("Modifying Logical Drive \"%d\", Partition %d\n",
			driveno,i);
		nl();
		printf("\t(0)  Kill this partition\n");
		printf("\t(1)  Modify another partition\n");
		nl();
		printf("\t(2)    Type: ");
		systype = partblk[i].systind;
		if (systype == 1 || systype == 4)
			printf("DOS");
		else if (systype == 10)
			printf("UNIX");
		else 
			printf("%d",partblk[i].systind);
		printf("   \t\t[DOS, UNIX, or type number]\n");
		printf("\t(3)   Start: %d    \t\t[Begin cylinder number]\n",
			firstsect/sectspercyl);
		printf("\t(4)     End: %d    \t\t[End cylinder number]\n",
			numsects ? (firstsect + numsects - 1) / sectspercyl:0);
		printf("\t(5)    Cyls: %d    \t\t[Total number of cylinders]\n",
			numsects ? ((numsects - 1)/sectspercyl) + 1 : 0);
		printf("\t(6) Sectors: %d  (%d.%d MB)",
			numsects,MEGSX10(numsects)/10,MEGSX10(numsects)%10);
		printf("\t[Total number of 512 byte blocks]\n");
		nl();
		printf("Choose an item from the list above [0-6]");
		printf(" or type \"y\" if okay: ");
		getresponse();
		nl();
		if ((*lineptr) >= '2' && (systype == 1 || systype == 4)) {
			printf("You must change the DOS partition with");
			printf(" the DOS fdisk program.\n");
			seeman();
			nl();
			select = -1;
			break;
		}
		if (!*lineptr)
			break;
		switch(*lineptr) {
		case '0':
			partblk[i].bootind = 0;
			partblk[i].beghead = 0;
			partblk[i].hibegcyl = 0;
			partblk[i].begsect = 0;
			partblk[i].begcyl = 0;
			partblk[i].systind = 0;
			partblk[i].endhead = 0;
			partblk[i].hiendcyl = 0;
			partblk[i].endsect = 0;
			partblk[i].endcyl = 0;
			partblk[i].relsect = 0;
			partblk[i].numsects = 0;
			select = -1;
			changed++;
			break;
		case '1':
			i = select = -1;
			continue;
		case '2':
			systype = partblk[i].systind;
			printf("\tType ");
			if (systype == 1 || systype == 4)
				printf("[DOS]: ");
			else if (systype == 10)
				printf("[UNIX]: ");
			else 
				printf("[%d]: ",systype);
			getresponse();
			nl();
			if (!*lineptr)
				continue;
			if (!strcmp(response,"dos") || !strcmp(response,"DOS"))
			{
				printf("Use the DOS fdisk program to make");
				bpause(" DOS partitions.  ");
			}
			else if (!strcmp(response,"unix") ||
			 		!strcmp(response,"UNIX")) {
				partblk[i].systind = 10;
				changed++;
			} else 
				partblk[i].systind = atoi(response);
				changed++;
			break;
		case '3':
			printf("\tStart [%d]: ",firstsect/sectspercyl);
			getresponse();
			nl();
			if (*lineptr) {
				oldval = partblk[i].relsect;
				newval = getnum(10) * sectspercyl;
				if (newval == 0)
					newval = sects;
				if (newval == 0)
					newval = 1;
				partblk[i].relsect = newval;
				partblk[i].numsects = partblk[i].numsects -
					newval + oldval;
				if (partblk[i].numsects < 0)
					partblk[i].numsects = 0;
				changed++;
			}
			break;
		case '4':
			printf("\tEnd [%d]: ",numsects ?
				(firstsect+numsects-1)/sectspercyl : 0);
			getresponse();
			nl();
			if (*lineptr) {
				newval = (getnum(10) + 1) * sectspercyl 
					- partblk[i].relsect;
				if (newval > 0) {
					partblk[i].numsects = newval;
					changed++;
				}
			}
			break;
		case '5':
			printf("\tCyls [%d]: ",numsects ? 
				((numsects-1)/sectspercyl)+1 : 0);
			getresponse();
			nl();
			if (*lineptr) {
				newval = getnum(10) * sectspercyl -
					partblk[i].relsect % sectspercyl;
				partblk[i].numsects = newval;
				changed++;
			}
			break;
		case '6':
			printf("\tSectors [%d] (%d.%d MB) : ",numsects,
				MEGSX10(numsects)/10,MEGSX10(numsects)%10);
			getresponse();
			nl();
			if (*lineptr) {
				newval = getnum(10);
				if ((newval+partblk[i].relsect) % sectspercyl) {
					printf("(Sector rounded up to");
					printf(" cylinder boundary.)\n");
					newval = (((newval+partblk[i].relsect) /
						sectspercyl)+1) * sectspercyl -
						partblk[i].relsect;
					nl();
				}
				partblk[i].numsects = newval;
				changed++;
			}
			break;
		case 'y':
		case 'Y':
			select = -1;
			cyl = partblk[i].relsect / sectspercyl;
			if (cyl >= 1024)
				cyl = 1023;
			partblk[i].hibegcyl = cyl >> 8;
			if (sects == 0)		/* just in case it's bogus */
				sects = 1;
			partblk[i].beghead = (partblk[i].relsect/sects) % heads;
			partblk[i].begsect = 1;
			partblk[i].begcyl = cyl & 0xff;
			cyl = (partblk[i].relsect + partblk[i].numsects - 1)
				/ sectspercyl;
			if (cyl >= 1024)
				cyl = 1023;
			partblk[i].hiendcyl = cyl >> 8;
			partblk[i].endhead = heads - 1;
			partblk[i].endsect = sects;
			partblk[i].endcyl = cyl & 0xff;
			if (!changed)
				goto done;
		}
		break;
	}
    }
}

readpartdata(diskno,partptr)
struct partable *partptr;
{
	register fid;
	register i;

#ifdef STANDALONE
	/* Setup for reading partition data */
	if (_dkioctl(diskno,D_RDPART,partbuffer) != 0) {
		printf("Could not read partition data from disk.\n");
	}
	parmagic = *((short *)&partbuffer[510]);
#if pm400
	parmagic = swaps(parmagic);
#endif
	if ((parmagic & 0xffff) != 0xaa55 && (parmagic & 0xffff) != 0xaa16) {
		for (i=0; i<512; i++)
			partbuffer[i] = 0;
		parmagic = 0xaa16;
	}
	memcpy(partptr,&partbuffer[PARTOFFSET],512-PARTOFFSET-2);
#if pm400
	for (i=0; i<4; i++) {
		swaplp(&(partptr[i].relsect));
		swaplp(&(partptr[i].numsects));
	}
#endif
#endif
}

writepartdata(diskno,partptr)
struct partable *partptr;
{
	register i;
	short magic;

#ifdef STANDALONE
	/* Setup for reading partition data */
	if (_dkioctl(diskno,D_RDPART,partbuffer) != 0) {
		printf("Could not read partition data from disk.\n");
	}
	magic = *((short *)&partbuffer[510]);
#if pm400
	magic = swaps(magic);
#endif
	if (((magic & 0xffff) != 0xaa55) || parmagic == 0) {
		for (i=0; i<512; i++)
			partbuffer[i] = 0;
		magic = 0xaa16;
	}
#if pm400
	magic = swaps(magic);
	for (i=0; i<4; i++) {
		swaplp(&partptr[i].relsect);
		swaplp(&partptr[i].numsects);
	}
#endif
	if (parmagic != 0) {
		memcpy(&partbuffer[PARTOFFSET],partptr,512-PARTOFFSET-2);
		memcpy(&partbuffer[510],&magic,2);
	}
	/* Setup for writing partition data */
	if (_dkioctl(diskno,D_WRTPART,partbuffer) != 0) {
		printf("Could not write partition data to disk.\n");
		seeman();
	}
	
#if pm400
	for (i=0; i<4; i++) {
		swaplp(&partptr[i].relsect);
		swaplp(&partptr[i].numsects);
	}
#endif
#endif
}
