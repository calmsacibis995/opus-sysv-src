#include <sys/types.h>
#include <sys/iorb.h>
#include <sys/sysconf.h>
#include <sys/errno.h>
#include "opconfig.h"

#define BADBLK		1
#define SPAREBLK	2
#define NEWBADBLK	3

#define EOTABLE		-2
#define REPEAT		-3
#define SERROR		-4

#define SCREENLEN	24

#define MAXBADBLKS 8192
int badblklist[MAXBADBLKS];

char *hscr_badblk[] = {
	"",
	"Bad blocks are handled by reading the disk and creating a",
	"table at the end of the disk that contains the mapping as",
	"well as the alternate sectors.  It is necessary to allocate",
	"some space at the end of the each partition (note that sparing",
	"is not necessary for DOS file based logical drives) in order",
	"for enable sparing.  By default during the standard initialization",
	"procedure, 1% is allocated.  This is usually sufficient for all",
	"but the worst of drives.  Normally, sparing is also done during",
	"the standard initialization.  It shouldn't be necessary to redo",
	"the alternate table unless it is inadvertantly wiped out.  It is",
	"possible to add single sectors to the alternate table by choosing",
	"the \"Designate specific bad blocks\" option.",
	"",
	0
};

badblock(driveno) {
	register changed = 0;
	register bn;
	register sn;
	int val;
	register gsize;
	int i;
	char diskname[32];

top:
	sparedrive = driveno = getdrive(driveno);
	sprintf(diskname,"/dev/dsk/%ds7",driveno);
	sparedisk = open(diskname,2);
	clearcache();
	while (1) {
		if (sparedisk < 0) {
			printf("Unable to access drive %d for sparing.  ",
				driveno);
			bpause("");
			close(sparedisk);
			return(-2);
		}
		if (rdblock0(driveno,READONLY)==BLKBAD) {
			printf("Parameter block not initialized on drive %d.",
				driveno);
			printf("  Do you wish to\n");
			printf("\tset up this drive for bad block sparing [y,n]? ");
			close(sparedisk);
			if (!yesno()) {
				return(0);
			}
			setupdisk(0,driveno);
			setupblk0(0,driveno);
			if (rdblock0(driveno,READINIT)==BLKBAD) {
				printf("Unable to access drive %d for sparing.  ",
					driveno);
				bpause("");
				return(-1);
			}
			goto top;
	        }
		printdiv("Opus5 Bad Block Handling\n");
		nl();
		brintf(" Current drive being examined: dsk/%d\n",
			driveno);
		brintf("       Total spares available: %d\n",blk0->spares);
		brintf("            First spare block: %d\n",blk0->sparetbl);
		brintf("                Table entries: %d\n",blk0->entries);
		brintf("                Spares in use: %d\n",blk0->used);
		nl();
		brintf("The following options are available for handling ");
		brintf("bad blocks:\n");
		nl();
		brintf("\t(?)  More information\n");
		brintf("\t(1)  Test drive and automatically designate ");
		brintf("bad blocks\n");
		brintf("\t(2)  Use a file to designate bad blocks\n"); 
		brintf("\t(3)  Designate specific bad blocks\n");
		brintf("\t(4)  Examine bad blocks list\n");
		brintf("\t(5)  Clear bad block table\n");
		brintf("\t(6)  Choose a different disk drive\n");
		brintf("\n");
		brintf("\t(q)  Return to top level\n");
		nl();
		choose();
		switch(response[0]) {
		case '?':
			scrn(hscr_badblk);
			pause("\t");
			break;
		case '1':
			lineptr++;
			gsize = getnum(10);
			if (gsize <= 0)
				gsize = 1;
			srchbadblk(driveno,gsize,0,1);
			break;

		case '2':
			need(SCREENLEN);
			printdiv("Bad Block Designation Using a File\n");
			nl();
			need(5);
			brintf("Type the name of the file containing the ");
			brintf("bad block list: ");
			getresponse();
			if (*lineptr == 'q' || *lineptr == 'Q')
				break;
			nl();
			srchbadblk(driveno,1,lineptr,1);
			break;
		case '3':
			need(SCREENLEN);
			printdiv("Bad Block Designation\n");
			nl();
			need(5);
			brintf("Type bad block numbers for ");
			brintf("disk drive %d (dsk/%d).\n",driveno,
				driveno);
			brintf("\tBad block numbers should be specified by ");
			brintf("the disk \"section\" number\n");
			brintf("\tand \"bn\" (as reported in ");
			brintf("the system console error messsage).\n");
			brintf("\tType \"q\" to terminate sequence.\n");
			nl();
			while (1) {
				printf("Bad block [<section>,<bn> or q]? ");
				getresponse();
				if (*lineptr == 'q' || *lineptr == 'Q')
					break;
				if ((sn = getnum(10)) == -1) {
					nl();
					printf("\t\tError: Invalid ");
					printf("response.\n");
					continue;
				}
			tryagain:
				getcomma();
				if ((bn = getnum(10)) == -1) {
					nl();
					printf("\t\tError: Invalid ");
					printf("response.\n");
					continue;
				}
				if (newbad(sn,bn) == -1)
					lineptr++;
				if ((sn = getnum(10)) != -1)
					goto tryagain;
			}
			need(SCREENLEN);
			nl();
			break;
		case '4':
			dumpbadblks(driveno);
			break;
		case '5':
			clrspares(driveno);
			break;
		case '6':
			driveno = -1;
			close(sparedisk);
			goto top;
		case 'q':
		case 'Q':
			close(sparedisk);
			return;
		}
	}
}

dumpbadblks(driveno) {
	register i = 0;
	register curbn = 0;
	register bn;
	register result;
	struct spare sparetmp;

	divider();
	brintf("Disk Drive %d (dsk/%d) Bad Blocks:\n",driveno,driveno);
	while (1) {
		result = getentry(i++,&sparetmp);
		if (result < 0)
			break;
		bn = sparetmp.badblk;
		if (bn == -1)
			break;
		if (bn) {
			if (!(curbn++ % 4))
				brintf("\n\t %d",bn);
			else
				brintf("\t %d",bn);
			brintf("->%d",sparetmp.spareblk);
			if (!(curbn % 80)) {
				nl();
				need(2);
				nl();
				bpause("More bad blocks.  ");
				if (*lineptr == '!' || *lineptr == 'q' || 
						*lineptr == 'Q')
					break;
			}
		}
	}
	nl();
	nl();
	if (curbn == 0) {
		brintf("No bad blocks currently designated\n");
		nl();
	}
	bpause("");
	if (*lineptr++ == '!') {
	    while ((bn = getnum(10)) != -1) {
		if (rmblk(bn),0) {
			printf("Could not release block %d on drive %d (dsk/%d).\n",bn,driveno,driveno);
			rmblk(bn,1);
			bpause("");
		}
	    }
	}
}

rmblk(sectno,force) {
	register i;
	register altblk;
	register result;
	struct spare sparetmp;

	for (i=0; ; i++) {
		if (getentry(i,&sparetmp) != i)
			if (!force) 
				return(SERROR);
		if (sparetmp.badblk == sectno)
			break;
	}
	altblk = sparetmp.spareblk;
	if (rdblk(sparetmp.spareblk,sectbuf) != 512)
		if (!force)
			return(SERROR);
	if (wrtblk(sparetmp.badblk,sectbuf) != 512)
		if (!force)
			return(SERROR);
	for (; ; i++) {
		result = getentry(i+1,&sparetmp);
		if (result == EOTABLE) {
			sparetmp.badblk = -1;
			sparetmp.spareblk = altblk;
			if (putentry(i,&sparetmp) != i)
				if (!force)
					return(SERROR);
			blk0->entries--;
			updblock0(sparedrive,NOPROMPT);
			return(0);
		}
		if (putentry(i,&sparetmp) != i)
			if (!force)
				return(SERROR);
	}
}

newbad(section,bn) {
	struct spareinfo sparebuf;
	long spareblk;
	struct spare spare;
	register i;
	register sectno;
	register newblk;
	register alt;
	register result;

	if (section >= NUMLOGVOL) {
		printf("\t\tError: invalid section number (%d)\n",
			section);
		return(-1);
	}
	if (bn < 0 || bn >= blk0->logvol[section].len) {
		printf("\t\tError: bn (%d) invalid for section (%d)\n",
			bn,section);
		return(-1);
	}
	sectno = bn + blk0->logvol[section].start;
	printf("\t\tReading block %d (section %d, bn %d)\n",sectno,
		section,bn);
	if (sectno == 0) {
		printf("\t\tError: cannot designate block 0 as bad.\n",
			sectno);
		return(-1);
	}
	if (sectno > blk0->sectors) {
		printf("\t\tError: invalid block (%d)\n",sectno);
		return(-1);
	}
	result = isbadblk(sectno);
	if (result == 0)
		result = spareit(sectno);
	if (result <= 0) {
		if (result == REPEAT) {
			printf("\t\tError: block %d already marked bad\n",
				sectno);
		}
		return(-1);
	}
	printf("\t\tBlock %d spared to alternate block %d.\n",
		sectno,result);
	return(sectno);
}

islink(badblk) {
	register link;
	struct spareinfo sparebuf;

	if (blk0->used == 0)
		return(0);
	link =  blk0->sparetbl;
	while (link != 0) {
		if (link == badblk)
			return(badblk);
		if (rdblk(link,&sparebuf) == 0)
			return(0);
		if (chksum(&sparebuf) != BLKCHKSUM)
			return(SERROR);
		link = sparebuf.link;
	}
	return(0);
}

fixbadlink(badblk,alt) {
	register link;
	struct spareinfo sparebuf;

	if (blk0->used == 0)
		return(0);
	link = blk0->sparetbl;
	if (link == badblk)
		return(SERROR);

	while (link != 0) {
		if (rdblk(link,&sparebuf) == 0)
			return(SERROR);
		if (chksum(&sparebuf) != BLKCHKSUM)
			return(SERROR);
		if (sparebuf.link == badblk) {
			if (rdblk(badblk,&sparebuf) != 512)
				return(SERROR);
			if (wrtblk(alt,&sparebuf) != 512)
				return(SERROR);
			if (rdblk(link,&sparebuf) != 512)
				return(SERROR);
			sparebuf.link = alt;
			setchksum(&sparebuf);
			if (wrtblk(link,&sparebuf) != 512)
				return(SERROR);
			return(badblk);
		}
		link = sparebuf.link;
	}
	return(0);
}

isbadblk(badblk) {
	register i;
	register result;
	struct spare tmpspare;

	for (i=0; ; i++) {
		result = getentry(i,&tmpspare);
		if (result == EOTABLE)
			return(0);
		if (result == SERROR)
			return(0);
		if (tmpspare.badblk == badblk)
			return(tmpspare.spareblk);
	}
}

isaltblk(altblk) {
	register i;
	register result;
	struct spare tmpspare;

	for (i=0; ; i++) {
		result = getentry(i,&tmpspare);
		if (result == EOTABLE)
			return(EOTABLE);
		if (result == SERROR)
			return(SERROR);
		if (tmpspare.spareblk == altblk)
			return(i);
	}
}

addbadblk(badblk,alt) {
	register i;
	register result;
	register bmax;
	register newval;
	struct spare aspare;
	struct spare bspare;

	bspare.badblk = badblk;
	bspare.spareblk = alt;
	bmax = blk0->entries;
	if (bmax > 0) {
				/* check to see if it goes at the end */
	 	result = getentry(bmax-1,&aspare);	
		if (aspare.badblk < badblk) {
			if (getbadslot(bmax) != bmax)
				return(SERROR);
			if (putentry(bmax,&bspare) != bmax)
				return(SERROR);
			blk0->entries++;
			updblock0(sparedrive,NOPROMPT);
			return(0);
		}
	}
	for (i=0; ; i++) {
		result = getentry(i,&aspare);
		if (result == EOTABLE) {
			if (getbadslot(i) != i)
				return(SERROR);
			if (putentry(i,&bspare) != i)
				return(SERROR);
			blk0->entries++;
			updblock0(sparedrive,NOPROMPT);
			return(0);
		}
		if (aspare.badblk < badblk)
			continue;
		if (aspare.badblk == badblk)
			return(REPEAT);
		if (putentry(i,&bspare) != i)
			return(SERROR);
		i++;
		result = getentry(i,&bspare);
		break;
	}
	while (result != EOTABLE) {
		if (putentry(i,&aspare) != i)
			return(SERROR);
		i++;
		aspare = bspare;
		result = getentry(i,&bspare);
	}
	if (getbadslot(i) != i)
		return(SERROR);
	if (putentry(i,&aspare) != i)
		return(SERROR);
	blk0->entries++;
	updblock0(sparedrive,NOPROMPT);
	return(0);
}

getnextspare() {
	register i;
	register result;
	register spareno;
	register entry;
	struct spare aspare;
	struct spare bspare;

	aspare.badblk = 0;
	for (i=0; ; i++) {
		bspare = aspare;
		result = getentry(i,&aspare);
		if (result == EOTABLE) {
			if (bspare.badblk == -1 && bspare.spareblk != 0) {
				spareno = bspare.spareblk;
				bspare.badblk = 0;
				bspare.spareblk = 0;
				if (putentry(i-1,&bspare) == i-1)
					return(spareno);
			}
			entry = getbadslot(i);
			if (entry != i)
				return(entry);
			return(getspare());
		}
	}
}

getentry(entry,sparetmp)
struct spare *sparetmp;
{
	register tentry;
	register link;
	struct spareinfo sparetable;

	if (blk0->used == 0)
		return(EOTABLE);
	tentry = entry;
	link = blk0->sparetbl;

	if (rdblk(link,&sparetable) == 0)
		return(SERROR);
	if (chksum(&sparetable) != BLKCHKSUM)
		return(SERROR);

	while (tentry >= NUMSPARE) {
		tentry -= NUMSPARE;
		link = sparetable.link;
		if (link == 0) {
			if (tentry == 0)
				return(EOTABLE);
			else
				return(SERROR);
		}
		if (rdblk(link,&sparetable) == 0)
			return(SERROR);
		if (chksum(&sparetable) != BLKCHKSUM)
			return(SERROR);
	}

	sparetmp->badblk = sparetable.spare[tentry].badblk;
	sparetmp->spareblk = sparetable.spare[tentry].spareblk;

	if (sparetmp->badblk == 0)
		return(EOTABLE);
	return(entry);
}

putentry(entry,sparetmp)
struct spare *sparetmp;
{
	register tentry;
	register link;
	struct spareinfo sparetable;

	tentry = entry;
	link = blk0->sparetbl;

	if (rdblk(link,&sparetable) == 0)
		return(SERROR);
	if (chksum(&sparetable) != BLKCHKSUM)
		return(SERROR);

	while (tentry >= NUMSPARE) {
		link = sparetable.link;
		if (link == 0)
			return(SERROR);
		if (rdblk(link,&sparetable) == 0)
			return(SERROR);
		if (chksum(&sparetable) != BLKCHKSUM)
			return(SERROR);
		tentry -= NUMSPARE;
	}

	sparetable.spare[tentry].badblk = sparetmp->badblk;
	sparetable.spare[tentry].spareblk = sparetmp->spareblk;

	setchksum(&sparetable);
	if (wrtblk(link,&sparetable) == 0)
		return(SERROR);

	return(entry);
}

getbadslot(entry)
{
	register newblk;
	register link;
	register tentry;
	struct spareinfo sparetable;

	tentry = entry;
	link = blk0->sparetbl;

	if (rdblk(link,&sparetable) <= 0)
		return(SERROR);

	if (blk0->used == 0) {
		blk0->used++;
		updblock0(sparedrive,NOPROMPT);
		clrblk(&sparetable);
		setchksum(&sparetable);
		if (wrtblk(link,&sparetable) == 0)
			return(SERROR);
	}

	if (chksum(&sparetable) != BLKCHKSUM)
		return(SERROR);
	
	while (tentry >= NUMSPARE) {
		if (sparetable.link == 0) {
			if ((newblk = getspare()) == -1)
				return(SERROR);
			sparetable.link = newblk;
			setchksum(&sparetable);
			if (wrtblk(link,&sparetable) == 0)
				return(SERROR);
			break;
		}
		link = sparetable.link;
		if (rdblk(link,&sparetable) == 0)
			return(SERROR);
		if (chksum(&sparetable) != BLKCHKSUM)
			return(SERROR);
		tentry -= NUMSPARE;
	}

	return(entry);
}

getspare() {
	register blkno;
	struct spareinfo sparebuf;
	
	while (1) {
		if (blk0->used >= blk0->spares)
			return(-1);
		blkno = blk0->sparetbl + blk0->used++;
		updblock0(sparedrive,NOPROMPT);
		if (rdblk(blkno,&sparebuf) != 512)
			continue;
		clrblk(&sparebuf);
		setchksum(&sparebuf);
		if (wrtblk(blkno,&sparebuf) != 512)
			continue;
		if (isbadblk(blkno))
			continue;
		if (isaltblk(blkno) >= 0)
			continue;
		if (islink(blkno) == blkno)
			continue;
		return(blkno);
	}
}

badcomp(a,b) 
int *a, *b;
{
	return(*a - *b);
}

srchbadblk(driveno,groupsize,sparelist,flag) {	/* flag says to check block 0 */
	register i;
	register j;
	int spares = 0;
	int used = 0;
	int badblks = 0;
	int newbadblks = 0;
	int onepercent = 0;
	int badstate = 0;
	int firstspare = 0;
	int firstbadblk = 0;
	int lastbadblk = 0;
	int alt;
	int link;
	int badblk;
	int cyl, head, sect, lsect, fsect;
	int sectspercyl;
	int sectspertrack;
	int firstsect;
	int lastsect;
	int eof;
	struct partinfo partbuf;
	char tmpbuf[512];
	char *buf;
	FILE *fdesc;
	FILE *savestream;
	int okblks;
	struct spare sparetmp;
	struct spareinfo sparebuf;

	sparedrive = driveno;
	if (flag) {
	    if ((rdblock0(driveno,READONLY)==BLKBAD) ||
			(getentry(0,&sparetmp) < EOTABLE) ||
			(blk0->spares == 0) ||
			(rdblk(blk0->sparetbl,tmpbuf) != 512)) {
		if (rdblock0(driveno,READINIT)==BLKBAD) {
			printf("Unable to access parameter block on drive %d.",
				driveno);
			printf("  Unable to spare\n");
			bpause("\tbad blocks.");
			return(-1);
		}
		printf("Parameter block on drive %d not set up properly",
			driveno);
		printf(" for sparing bad blocks.\n");
		printf("\tWould you like to set up the parameter block [y,n]? ");
		if (!yesno())
			return(-1);
		setupdisk(0,driveno);
		setupblk0(0,driveno);
		return(0);
	    }
	}
	if (sparelist) {
	    	if ((fdesc = fopen(sparelist,"r")) <= 0) {
		    	printf("Unable to open %s for bad block list.  ",
				sparelist);
			bpause("");
		    	return(-1);
		}
		partsize(driveno,&partbuf);
		sectspercyl = partbuf.sectspercyl;
		sectspertrack = partbuf.sectspercyl / partbuf.headcnt;
		firstsect = firstpartblk(driveno);
		lastsect = numpartblk(driveno);
		eof = 0;
		while(fgets(tmpbuf,sizeof(tmpbuf),fdesc) != NULL) {
			buf = tmpbuf;
			if (*buf == '#')
				continue;
			while (*buf != 0 && *buf != '\032') {
				cyl = head = fsect = lsect = sect = -1;
				if (sscanf(buf,"%d %d %d - %d",&cyl,&head,
					&fsect,&lsect) < 4) {
				    fsect = lsect = -1;
			    	    if (sscanf(buf,"%d %d %d",&cyl,&head,
					&sect) < 3) {
					sect = -1;
					if (sscanf(buf,"%d %d",&cyl,
						&head) < 2) {
			printf("Illegal bad block specification: %s",buf);
						break;
					}
				    }
				}
#ifdef DEBUG
printf("cyl(%d),head(%d),fsect(%d),lsect(%d),sect(%d)\n",
cyl,head,fsect,lsect,sect);
printf("sectspercyl(%d),sectspertrack(%d),firstsect(%d),lastsect(%d),",
sectspercyl,sectspertrack,firstsect,lastsect);
#endif
				badblk = cyl * sectspercyl +
					head * sectspertrack - firstsect;
#ifdef DEBUG
printf("badblk(%d)\n",badblk);
#endif
				if ((badblk >= 0) && (badblk < lastsect)) {
				    if (sect > 0) {
					badblklist[badblks++] = badblk + sect-1;
#ifdef DEBUG
printf("{a-%d} ",badblk+sect);
#endif
				    } else if (fsect <= lsect && lsect > 0 &&
					fsect > 0 && lsect <= sectspertrack) {
					for (i=fsect; i<lsect; i++) {
#ifdef DEBUG
printf("{b-%d} ",badblk+i);
#endif
						badblklist[badblks++] =
							badblk + i - 1;
					}
				    } else if (lsect == -1 && fsect == -1) {
					for (i=0; i<sectspertrack; i++) {
#ifdef DEBUG
printf("{c-%d} ",badblk+i);
#endif
						badblklist[badblks++] =
							badblk + i;
					}
				    }
				}
				while (*buf != 0 && *buf != ';')
					buf++;
				while ((*buf < '0' || *buf > '9') && *buf != 0)
					buf++;
			}
		}
		fclose(fdesc);
	} else {
	    printf("Testing disk and automatically designating bad blocks:    ");
	    for (i=1; i<blk0->sparetbl; i++) {
		if ((onepercent -= 1) < 0) {
			onepercent += blk0->sectors / 100;
			printf("\b\b\b%2d%%",(i*100)/blk0->sparetbl);
			if (Dflag) {
				savestream = instream;
				instream = stdin;
				printf(".");
				instream = savestream;
			}
		}
#if 1
		if (badstate == 0) {
			if (rdbblk(i,sectbuf,BIGBLK*512) == BIGBLK*512) {
				onepercent -= BIGBLK - 1;
				i += BIGBLK - 1;
				continue;
			}
			badstate = 1;
		}
		if (rdblk(i,sectbuf) != 512) {
			if (_diskerrno == E_DA) {
				nl();
				printf("Attempt to read past end of disk.  ");
				printf("Terminating search\n");
				bpause("");
				return(1);
			}
			for (j=0; j<groupsize; j++) {
				if (badblks % 8 == 0)
					printf("\b\b\b   \n\t   ");
				printf("\b\b\b%d,\t   ",i + j);
				badblklist[badblks++] = i + j;
			}
			i += groupsize - 1;
			badstate = 2;
			continue;
		}
		if (badstate == 2)
			badstate = 0;
#endif
	    }
	    printf("\b\b\bDONE\n");
	}
	qsort(&badblklist[0],badblks,sizeof(badblklist[0]),badcomp);
	newbadblks = 0;
	for (i=0; i<badblks; i++)	/* block zero cannot be bad */
		if (badblklist[i] > 0)
			break;
#ifdef DEBUG
printf("i(%d),newbadblks(%d)\n",i,newbadblks);
#endif
	if (badblks)
		badblklist[newbadblks++] = badblklist[i++];
	for (; i<badblks; i++) {
		if (badblklist[newbadblks-1] != badblklist[i])
			badblklist[newbadblks++] = badblklist[i];
	}
	badblks = newbadblks;
#ifdef DEBUG
for (i=0; i<badblks; i++)
printf("{%d},",badblklist[i]);
#endif
	firstspare = 0;
	lastbadblk = blk0->sparetbl - 1;
	clrspares(driveno);
	for (i=0; i<badblks; i++) {
		if (i % NUMSPARE == 0) {
			alt = getnewspare(lastbadblk,badblks);
			if (alt < 0)
				break;
			lastbadblk = alt;
			if (firstspare == 0)
				firstspare = alt;
			else {
				sparebuf.link = alt;
				setchksum(&sparebuf);
				if (wrtblk(link,&sparebuf) == 0) {
					alt = -1;
					break;
				}
			}
			clrblk(&sparebuf);
			link = lastbadblk;
		}
		if ((alt = getnewspare(lastbadblk,badblks)) <= 0)
			break;
		lastbadblk = alt;
		sparebuf.spare[i%NUMSPARE].badblk = badblklist[i];
		sparebuf.spare[i%NUMSPARE].spareblk = alt;
	}
	if (badblks && alt <= 0) {
		printf("More bad blocks were found than allocated for the");
		printf(" spare area.  You should\n");
		printf("\tincrease the size of the spare area.  At least");
		printf(" %d blocks\n",badblks + badblks / NUMSPARE + 1);
		bpause("\tshould be specified.  ");
		return(3);
	}
	if (badblks) {
		setchksum(&sparebuf);
		if (wrtblk(link,&sparebuf) == 0) {
			bpause("Unable to write spare list.  ");
			return(2);
		}
		blk0->spares -= firstspare - blk0->sparetbl;
		blk0->sparetbl = firstspare;
		blk0->used = lastbadblk - firstspare + 1;
		blk0->entries = badblks;
		updblock0(sparedrive,NOPROMPT);
	}
	if (sparelist)
		printf("\nFile \"%s\" specified %d bad blocks on disk drive %d (dsk/%d).\n",sparelist,badblks,driveno,driveno);
	else
		printf("\nSearch found %d bad blocks on disk drive %d (dsk/%d).\n",badblks,driveno,driveno);
	if (badblks)
		printf("\tAll %d bad blocks were spared.\n",badblks);
	nl();
	bpause("");
	return(0);
}

getnewspare(lastbadblk,badlistsize) {
	int i;
	char tmpbuf[512];

again:
	while (rdblk(++lastbadblk,tmpbuf) != 512)
		if (_diskerrno == E_DA)
			return(-1);
	for (i=badlistsize; i>0; i--) {
		if (badblklist[i-1] > lastbadblk)
			continue;
		if (badblklist[i-1] == lastbadblk)
			goto again;
		break;
	}
	return(lastbadblk);
}

spareit(sectno) {
	register i;
	register entry;
	register alt;
	register result;
	struct spare tmpspare;
	int sectsread;
	char savebuf[512];

	if (isbadblk(sectno)) {
		return(REPEAT);
	}
	printf("\t\tSparing block number %d  ",sectno);
	if (frdblk(sectno,savebuf) < 512) {
		printf("(unreadable)\n");
		clrblk(savebuf);
	} else
		nl();
	alt = getnextspare();
	if (alt < 0) {
		printf("\t\tError: out of room in spare table\n");
		return(SERROR);
	}
	entry = isaltblk(sectno);
	if (entry >= 0) {
		result = getentry(entry,&tmpspare);	
		if (result != entry)
			return(SERROR);
		tmpspare.spareblk = alt;
		if (putentry(entry,&tmpspare) != entry)
			return(SERROR);
		return(alt);
	}
	result = fixbadlink(sectno,alt);
	if (result) {
		if (result == sectno) {
			return(sectno);
		}
		if (sectno == blk0->sparetbl) {
			nl();
			printf("\t\tError: First spare table block ");
			printf("may not be spared\n");
			return(SERROR);
		}
		nl();
		printf("\t\tError: bad spare table\n");
		return(SERROR);
	}
	if (wrtblk(alt,savebuf) != 512) {
		nl();
		printf("\t\tError: could not write alternate ");
		printf("block (%d)\n",alt);
		return(SERROR);
	}
	if (addbadblk(sectno,alt) != 0) {
		nl();
		printf("\t\tError: could not designate bad block for %d\n",
			sectno);
		return(SERROR);
	}
	return(alt);
}

clrspares(driveno) {
	blk0->entries = 0;
	blk0->used = 0;
	updblock0(driveno,NOPROMPT);
	clrblk(sectbuf);
	wrtblk(blk0->sparetbl);
}
