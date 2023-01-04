/******************************************************************************\
**									      **
**	Copyright (c) 1986 by National Semiconductor Corp.		      **
**	All Rights Reserved						      **
**									      **
\******************************************************************************/

#ident	"@(#)hdeupdate.c	1.6	10/21/86 Copyright (c) 1986 by National Semiconductor Corp."

#define	HDELOG			/* don't depend on the makefile	*/

#include <stdio.h>
#include <fcntl.h>
#include "sys/types.h"
#include "sys/sysmacros.h"
#include "sys/disk.h"
#include "sys/hdelog.h"
#include "sys/hdeioctl.h"
#include "sys/stat.h"
#include "sys/dsd5217.h"
#include "hdecmds.h"
#include "edio.h"
#include "Ga.out.h"

#define	ASIZE	1024		/* size of a.out header buffer	 */
#define	CPSIZE	4096		/* size of copy buffer */
#define DIDB	1
#define DIDb	2
#define DIDD	4

char	*cmdname;

dev_t	devarg = -1;
daddr_t	bnoarg;

int	cylarg, trkarg, secarg;
int	didflgs, linecnt;

main(argc, argv)
int	argc;
char	*argv[];
{
	if (argc < 1)
		cmdname = "hdeupdate";
	else
		cmdname = argv[0];
	if (argc < 2)
	{
		cmdusage();
		exit(INVEXIT);
	}
	if (argv[1][0] != '-')
		badusage();
	switch (argv[1][1])
	{
		case 'p':
			dophys(argc-2, argv+2);
			break;

		case 'm':
			dobadmap(argc-2, argv+2);
			break;

		case 'c':
			docpboot(argc-2, argv+2);
			break;

		default:
			badusage();
	}
	exit(NORMEXIT);
}

cmdusage()
{
	fprintf(stderr,
"usage:\t%s -p -D maj min [ -u ] [ -b blockno | -B cyl trk sec ]\n",
		cmdname);
	fprintf(stderr,
"\t%s -m -D maj min [ -u ] [ -b blockno | -B cyl trk sec ]\n",
		cmdname);
	fprintf(stderr,
"\t%s -c -D maj min [ -b blockno | -B cyl trk sec ]\n",
		cmdname);
}

badusage()
{
	if (linecnt)
		badline();
	fprintf(stderr, "bad command usage\n");
	cmdusage();
	exit(INVEXIT);
}

getdev(ac, av)
char *av[];
{
int	maj, min, rcnt;
long	maxmaj, maxmin;
dev_t	maxdev = -1;

	rcnt = 2;
	if (ac < 2)
		badusage();
	if (av[0][0] == '-')
	{
		if (av[0][1] != 'D' || ac < 3)
			badusage();
		rcnt++;
		av++;
	}
	if (sscanf(av[0], "%d", &maj) != 1 || sscanf(av[1], "%d", &min) != 1)
		badusage();
	maxmaj = major(maxdev);
	maxmin = minor(maxdev);
	if (maj < 0 || maj > maxmaj)
	{
		fprintf(stderr,
			"major device number %d out of range (0 to %d)\n",
			maj, maxmaj);
		exit(INVEXIT);
	}
	if (min < 0 || min > maxmin)
	{
		fprintf(stderr,
			"minor device number %d out of range (0 to %d)\n",
			min, maxmin);
		exit(INVEXIT);
	}
	devarg = makedev(maj, min);
	return(rcnt);
}

badline()
{

	if(!linecnt)
		badusage();
	fprintf(stderr, "bad usage on line %d\n", linecnt);
	exit(INVEXIT);
}

dophys(ac, av)
char **av;
{
int	i, update = 0;

	didflgs = 0;
	bnoarg = 0;
	while (ac > 0)
	{
		if (av[0][0] != '-')
			badline();
		switch(av[0][1])
		{
			case 'D':
				if (DIDD & didflgs)
					badline();
				didflgs |= DIDD;
				i = getdev(ac, av);
				ac -= i;
				av += i;
				break;

			case 'b':
				if ((DIDb|DIDB) & didflgs)
					badline();
				didflgs |= DIDb;
				if (ac < 2)
					badline();
				if (sscanf(av[1], "%d", &bnoarg) != 1)
					badline();
				/* check it later when equipped disk is open */
				ac -= 2;
				av += 2;
				break;

			case 'B':
				if ((DIDb|DIDB) & didflgs)
					badline();
				didflgs |= DIDB;
				if (ac < 4)
					badline();
				if (sscanf(av[1], "%d", &cylarg) != 1)
					badline();
				if (sscanf(av[2], "%d", &trkarg) != 1)
					badline();
				if (sscanf(av[3], "%d", &secarg) != 1)
					badline();
				/* check it later when equipped disk is open */
				ac -= 4;
				av += 4;
				break;

			case 'u':	/* update phys block 0 */
				update = 1;
				ac--;
				av++;
				break;

			default:
				badline();
		}
	}
	if (!(didflgs & DIDD))
		badline();
	if (edopen(devarg))
		exit(ERREXIT);
#ifdef	notdef
	if (didflgs & DIDb)
	{
		if (dadcheck(bnoarg))
			exit(ERREXIT);
	}
	else
	{
		if (didflgs & DIDB)
		{
			if ((bnoarg = ctstodad(cylarg, trkarg, secarg)) == -1)
				exit(ERREXIT);
		}
		else
		{
			bnoarg = 0;
		}
	}
#endif	/* notdef */
	dophysdisplay();
	if (update)
		dophysupdate();
	edclose();
}

dophysupdate()
{
int	i;
int	nmag;
int	nsan;
int	inum;
short	snum;
long	lnum;
unsigned int uinum;
unsigned short usnum;
unsigned long ulnum;
char	c, s[80];
char	nbuf[DNSIZE+PDNSIZE];

	fprintf(stderr,
	"\nDo you want to update disk physical description [n] ? ");
	if (scanf("%c", &c) != 1)
		return(0);
	if ((c != 'y') && (c != 'Y'))
		return(0);
doagain:
	nsan = nmag = 0;
	if (edpdp->dt_magic != DMAGIC)
	{
		fprintf(stderr,"Enter   dt_magic [%#lx] : ",DMAGIC);
		fflush(stdin);
		if (scanf("%lx", &lnum) != 1)
			edpdp->dt_magic = DMAGIC;
		else
			edpdp->dt_magic = lnum;
		nmag = 1;
		fprintf(stderr,"Updated dt_magic: %#lx\n",edpdp->dt_magic);
	}
	else
	{
		nmag = 0;
		fprintf(stderr,"Default dt_magic: %#lx\n",edpdp->dt_magic);
	}
	fprintf(stderr,"Enter   dt_ploz  [%#x] : ",
		((nmag == 1) ? 0 : edpdp->dt_ploz));
	fflush(stdin);
	if (scanf("%x", &inum) != 1)
	{
		if (nmag);
			edpdp->dt_ploz = 0;
		fprintf(stderr,"Default dt_ploz: %#x\n",edpdp->dt_ploz);
	}
	else
	{
		edpdp->dt_ploz = inum;
		fprintf(stderr,"Updated dt_ploz: %#x\n",edpdp->dt_ploz);
	}
	fprintf(stderr,"Enter   dt_plonz [%#x] : ",
		((nmag == 1) ? 0 : edpdp->dt_plonz));
	fflush(stdin);
	if (scanf("%x", &inum) != 1)
	{
		if (nmag);
			edpdp->dt_plonz = 0;
		fprintf(stderr,"Default dt_plonz: %#x\n",edpdp->dt_plonz);
	}
	else
	{
		edpdp->dt_plonz = inum;
		fprintf(stderr,"Updated dt_plonz: %#x\n",edpdp->dt_plonz);
	}
	strncpy(nbuf,edpdp->dt_name,DNSIZE);
	nbuf[DNSIZE] = '\0';
	fprintf(stderr,"Enter   dt_name  [%s] : ",
		((nmag == 1) ? "STANDARD" : nbuf));
	fflush(stdin);
	if (scanf("%s", s) != 1)
	{
		if (nmag)
			strcpy(edpdp->dt_name,"STANDARD");
		strncpy(nbuf,edpdp->dt_name,DNSIZE);
		nbuf[DNSIZE] = '\0';
		fprintf(stderr,"Default dt_name: '%s'\n",nbuf);
	}
	else
	{
		strncpy(edpdp->dt_name,s,DNSIZE);
		strncpy(nbuf,edpdp->dt_name,DNSIZE);
		nbuf[DNSIZE] = '\0';
		fprintf(stderr,"Updated dt_name: '%s'\n",nbuf);
	}
	fprintf(stderr,"Enter   dt_maxbad [%u] : ",
		((nmag == 1) ? DEFMAXBAD : edpdp->dt_maxbad));
	fflush(stdin);
	if (scanf("%u", &uinum) != 1)
	{
		uinum = edpdp->dt_maxbad;
		if (nmag)
			edpdp->dt_maxbad = DEFMAXBAD;
		else
			if ((uinum < MAXBAD) || (uinum > MAXBADTOT))
				edpdp->dt_maxbad = DEFMAXBAD;
			else
				edpdp->dt_maxbad =
					((uinum + MAXBAD) / (MAXBAD + 1)) *
						(MAXBAD + 1);
		fprintf(stderr,"Default dt_maxbad: %u\n",
			edpdp->dt_maxbad);
	}
	else
	{
		if ((uinum < MAXBAD) || (uinum > MAXBADTOT))
			edpdp->dt_maxbad = DEFMAXBAD;
		else
			edpdp->dt_maxbad = ((uinum + MAXBAD) / (MAXBAD + 1)) *
						(MAXBAD + 1);
		fprintf(stderr,"Updated dt_maxbad: %u\n",
			edpdp->dt_maxbad);
	}
	inum = ((edpdp->dt_maxbad + MAXBAD) / (MAXBAD + 1)) + 1;
	fprintf(stderr,"Enter   dt_bootblkno [%u] : ",
		((nmag == 1) ? (unsigned)inum : edpdp->dt_bootblkno));
	fflush(stdin);
	if (scanf("%u", &uinum) != 1)
	{
		if (nmag || (edpdp->dt_bootblkno < (unsigned)inum))
			edpdp->dt_bootblkno = (unsigned)inum;
		fprintf(stderr,"Default dt_bootblkno: %u\n",
			edpdp->dt_bootblkno);
	}
	else
	{
		if (uinum < (unsigned)inum)
			edpdp->dt_bootblkno = (unsigned)inum;
		else
			edpdp->dt_bootblkno = uinum;
		fprintf(stderr,"Updated dt_bootblkno: %u\n",
			edpdp->dt_bootblkno);
	}
	fprintf(stderr,"Enter   dt_nsectors [%u] : ",
		((nmag == 1) ? 17 : edpdp->dt_nsectors));
	fflush(stdin);
	if (scanf("%u", &uinum) != 1)
	{
		if (nmag)
			edpdp->dt_nsectors = 17;
		fprintf(stderr,"Default dt_nsectors: %u\n",edpdp->dt_nsectors);
	}
	else
	{
		edpdp->dt_nsectors = uinum;
		fprintf(stderr,"Updated dt_nsectors: %u\n",edpdp->dt_nsectors);
	}
	fprintf(stderr,"Enter   dt_ntracks [%u] : ",
		((nmag == 1) ? 8 : edpdp->dt_ntracks));
	fflush(stdin);
	if (scanf("%u", &uinum) != 1)
	{
		if (nmag)
			edpdp->dt_ntracks = 8;
		fprintf(stderr,"Default dt_ntracks: %u\n",edpdp->dt_ntracks);
	}
	else
	{
		edpdp->dt_ntracks = uinum;
		fprintf(stderr,"Updated dt_ntracks: %u\n",edpdp->dt_ntracks);
	}
	fprintf(stderr,"Enter   dt_ncylinders [%u] : ",
		((nmag == 1) ? 512 : edpdp->dt_ncylinders));
	fflush(stdin);
	if (scanf("%u", &uinum) != 1)
	{
		if (nmag)
			edpdp->dt_ncylinders = 512;
		fprintf(stderr,"Default dt_ncylinders: %u\n",
			edpdp->dt_ncylinders);
	}
	else
	{
		edpdp->dt_ncylinders = uinum;
		fprintf(stderr,"Updated dt_ncylinders: %u\n",
			edpdp->dt_ncylinders);
	}
	if (edpdp->dt_secpercyl == (edpdp->dt_nsectors * edpdp->dt_ntracks))
	{
		fprintf(stderr,"Default dt_secpercyl: %u\n",
			edpdp->dt_secpercyl);
	}
	else
	{
		edpdp->dt_secpercyl = edpdp->dt_nsectors * edpdp->dt_ntracks;
		fprintf(stderr,"Updated dt_secpercyl: %u\n",
			edpdp->dt_secpercyl);
	}
	if (edpdp->dt_secperunit ==(edpdp->dt_secpercyl * edpdp->dt_ncylinders))
	{
		fprintf(stderr,"Default dt_secperunit: %u\n",
			edpdp->dt_secperunit);
	}
	else
	{
		edpdp->dt_secperunit = edpdp->dt_secpercyl *
					edpdp->dt_ncylinders;
		fprintf(stderr,"Updated dt_secperunit: %u\n",
			edpdp->dt_secperunit);
	}
	for (i = 0; i < NPART; i++)
	{
		fprintf(stderr,"Enter   dt_nsec(%d) [%u] : ",
			i, ((nmag == 1) ? 0 : edpdp->dt_nsec(i)));
		fflush(stdin);
		if (scanf("%u", &uinum) != 1)
		{
			if (nmag)
				edpdp->dt_nsec(i) = 0;
			fprintf(stderr,"Default dt_nsec(%d): %u\n",
				i, edpdp->dt_nsec(i));
		}
		else
		{
			edpdp->dt_nsec(i) = uinum;
			fprintf(stderr,"Updated dt_nsec(%d): %u\n",
				i, edpdp->dt_nsec(i));
		}
		fprintf(stderr,"Enter   dt_cyloff(%d) [%u] : ",
			i, ((nmag == 1) ? 1 : edpdp->dt_cyloff(i)));
		fflush(stdin);
		if (scanf("%u", &uinum) != 1)
		{
			if (nmag)
				edpdp->dt_cyloff(i) = 1;
			fprintf(stderr,"Default dt_cyloff(%d): %u\n",
				i, edpdp->dt_cyloff(i));
		}
		else
		{
			edpdp->dt_cyloff(i) = uinum;
			fprintf(stderr,"Updated dt_cyloff(%d): %u\n",
				i, edpdp->dt_cyloff(i));
		}
	}
	if (edpdp->dt_sanity != VALID_PD)
	{
		fprintf(stderr,"Enter   dt_sanity [%#lx] : ",VALID_PD);
		fflush(stdin);
		if (scanf("%lx", &lnum) != 1)
			edpdp->dt_sanity = VALID_PD;
		else
			edpdp->dt_sanity = lnum;
		nsan = 1;
		fprintf(stderr,"Updated dt_sanity: %#lx\n",edpdp->dt_sanity);
	}
	else
	{
		nsan = 0;
		fprintf(stderr,"Default dt_sanity: %#lx\n",edpdp->dt_sanity);
	}
	fprintf(stderr,"Enter   dt_driveid [%lu] : ",
		((nsan == 1) ? 0 : edpdp->dt_driveid));
	fflush(stdin);
	if (scanf("%lu", &ulnum) != 1)
	{
		if (nsan)
			edpdp->dt_driveid = 0;
		fprintf(stderr,"Default dt_driveid: %lu\n",edpdp->dt_driveid);
	}
	else
	{
		edpdp->dt_driveid = ulnum;
		fprintf(stderr,"Updated dt_driveid: %lu\n",edpdp->dt_driveid);
	}
	fprintf(stderr,"Enter   dt_version [%lu] : ",
		((nsan == 1) ? 0 : edpdp->dt_version));
	fflush(stdin);
	if (scanf("%lu", &ulnum) != 1)
	{
		if (nsan)
			edpdp->dt_version = 0;
		fprintf(stderr,"Default dt_version: %lu\n",edpdp->dt_version);
	}
	else
	{
		edpdp->dt_version = ulnum;
		fprintf(stderr,"Updated dt_version: %lu\n",edpdp->dt_version);
	}
	strncpy(nbuf,edpdp->dt_serial,PDNSIZE);
	nbuf[PDNSIZE] = '\0';
	fprintf(stderr,"Enter   dt_serial  [%s] : ",
		((nsan == 1) ? edpdp->dt_name : nbuf));
	fflush(stdin);
	if (scanf("%s", s) != 1)
	{
		if (nsan)
			strncpy(edpdp->dt_serial,edpdp->dt_name,PDNSIZE);
		strncpy(nbuf,edpdp->dt_serial,PDNSIZE);
		nbuf[PDNSIZE] = '\0';
		fprintf(stderr,"Default dt_serial: '%s'\n",nbuf);
	}
	else
	{
		strncpy(edpdp->dt_serial,s,PDNSIZE);
		strncpy(nbuf,edpdp->dt_serial,PDNSIZE);
		nbuf[PDNSIZE] = '\0';
		fprintf(stderr,"Updated dt_serial: '%s'\n",nbuf);
	}
	fprintf(stderr,"Enter   dt_sectors [%lu] : ",
		((nsan == 1) ? edpdp->dt_nsectors : edpdp->dt_sectors));
	fflush(stdin);
	if (scanf("%lu", &ulnum) != 1)
	{
		if (nsan)
			edpdp->dt_sectors = edpdp->dt_nsectors;
		fprintf(stderr,"Default dt_sectors: %lu\n",edpdp->dt_sectors);
	}
	else
	{
		edpdp->dt_sectors = ulnum;
		fprintf(stderr,"Updated dt_sectors: %lu\n",edpdp->dt_sectors);
	}
	fprintf(stderr,"Enter   dt_tracks [%lu] : ",
		((nsan == 1) ? edpdp->dt_ntracks : edpdp->dt_tracks));
	fflush(stdin);
	if (scanf("%lu", &ulnum) != 1)
	{
		if (nsan)
			edpdp->dt_tracks = edpdp->dt_ntracks;
		fprintf(stderr,"Default dt_tracks: %lu\n",edpdp->dt_tracks);
	}
	else
	{
		edpdp->dt_tracks = ulnum;
		fprintf(stderr,"Updated dt_tracks: %lu\n",edpdp->dt_tracks);
	}
	fprintf(stderr,"Enter   dt_cyls [%lu] : ",
		((nsan == 1) ? edpdp->dt_ncylinders : edpdp->dt_cyls));
	fflush(stdin);
	if (scanf("%lu", &ulnum) != 1)
	{
		if (nsan)
			edpdp->dt_cyls = edpdp->dt_ncylinders;
		fprintf(stderr,"Default dt_cyls: %lu\n",edpdp->dt_cyls);
	}
	else
	{
		edpdp->dt_cyls = ulnum;
		fprintf(stderr,"Updated dt_cyls: %lu\n",edpdp->dt_cyls);
	}
	fprintf(stderr,"Enter   dt_bytes [%lu] : ",
		((nsan == 1) ? 512 : edpdp->dt_bytes));
	fflush(stdin);
	if (scanf("%lu", &ulnum) != 1)
	{
		if (nsan)
			edpdp->dt_bytes = 512;
		fprintf(stderr,"Default dt_bytes: %lu\n",edpdp->dt_bytes);
	}
	else
	{
		edpdp->dt_bytes = ulnum;
		fprintf(stderr,"Updated dt_bytes: %lu\n",edpdp->dt_bytes);
	}
	fprintf(stderr,"Enter   dt_logicalst [%ld] : ",
		((nsan == 1) ? -1 : edpdp->dt_logicalst));
	fflush(stdin);
	if (scanf("%ld", &lnum) != 1)
	{
		if (nsan)
			edpdp->dt_logicalst = -1;
		fprintf(stderr,"Default dt_logicalst: %ld\n",
			edpdp->dt_logicalst);
	}
	else
	{
		edpdp->dt_logicalst = lnum;
		fprintf(stderr,"Updated dt_logicalst: %ld\n",
			edpdp->dt_logicalst);
	}
	fprintf(stderr,"Enter   dt_errlogst [%ld] : ",
		((nsan == 1) ? -1 : edpdp->dt_errlogst));
	fflush(stdin);
	if (scanf("%ld", &lnum) != 1)
	{
		if (nsan)
			edpdp->dt_errlogst = -1;
		fprintf(stderr,"Default dt_errlogst: %ld\n",edpdp->dt_errlogst);
	}
	else
	{
		edpdp->dt_errlogst = lnum;
		fprintf(stderr,"Updated dt_errlogst: %ld\n",edpdp->dt_errlogst);
	}
	fprintf(stderr,"Enter   dt_errlogsz [%lu] : ",
		((nsan == 1) ? 0 : edpdp->dt_errlogsz));
	fflush(stdin);
	if (scanf("%lu", &ulnum) != 1)
	{
		if (nsan)
			edpdp->dt_errlogsz = 0;
		fprintf(stderr,"Default dt_errlogsz: %lu\n",edpdp->dt_errlogsz);
	}
	else
	{
		edpdp->dt_errlogsz = ulnum;
		fprintf(stderr,"Updated dt_errlogsz: %lu\n",edpdp->dt_errlogsz);
	}
	fprintf(stderr,"Enter   dt_mfgst [%ld] : ",
		((nsan == 1) ? -1 : edpdp->dt_mfgst));
	fflush(stdin);
	if (scanf("%ld", &lnum) != 1)
	{
		if (nsan)
			edpdp->dt_mfgst = -1;
		fprintf(stderr,"Default dt_mfgst: %ld\n",edpdp->dt_mfgst);
	}
	else
	{
		edpdp->dt_mfgst = lnum;
		fprintf(stderr,"Updated dt_mfgst: %ld\n",edpdp->dt_mfgst);
	}
	fprintf(stderr,"Enter   dt_mfgsz [%lu] : ",
		((nsan == 1) ? 0 : edpdp->dt_mfgsz));
	fflush(stdin);
	if (scanf("%lu", &ulnum) != 1)
	{
		if (nsan)
			edpdp->dt_mfgsz = 0;
		fprintf(stderr,"Default dt_mfgsz: %lu\n",edpdp->dt_mfgsz);
	}
	else
	{
		edpdp->dt_mfgsz = ulnum;
		fprintf(stderr,"Updated dt_mfgsz: %lu\n",edpdp->dt_mfgsz);
	}
	fprintf(stderr,"Enter   dt_defectst [%ld] : ",
		((nsan == 1) ? -1 : edpdp->dt_defectst));
	fflush(stdin);
	if (scanf("%ld", &lnum) != 1)
	{
		if (nsan)
			edpdp->dt_defectst = -1;
		fprintf(stderr,"Default dt_defectst: %ld\n",edpdp->dt_defectst);
	}
	else
	{
		edpdp->dt_defectst = lnum;
		fprintf(stderr,"Updated dt_defectst: %ld\n",edpdp->dt_defectst);
	}
	fprintf(stderr,"Enter   dt_defectsz [%lu] : ",
		((nsan == 1) ? 0 : edpdp->dt_defectsz));
	fflush(stdin);
	if (scanf("%lu", &ulnum) != 1)
	{
		if (nsan)
			edpdp->dt_defectsz = 0;
		fprintf(stderr,"Default dt_defectsz: %lu\n",edpdp->dt_defectsz);
	}
	else
	{
		edpdp->dt_defectsz = ulnum;
		fprintf(stderr,"Updated dt_defectsz: %lu\n",edpdp->dt_defectsz);
	}
	fprintf(stderr,"Enter   dt_relst [%ld] : ",
		((nsan == 1) ? -1 : edpdp->dt_relst));
	fflush(stdin);
	if (scanf("%ld", &lnum) != 1)
	{
		if (nsan)
			edpdp->dt_relst = -1;
		fprintf(stderr,"Default dt_relst: %ld\n",edpdp->dt_relst);
	}
	else
	{
		edpdp->dt_relst = lnum;
		fprintf(stderr,"Updated dt_relst: %ld\n",edpdp->dt_relst);
	}
	fprintf(stderr,"Enter   dt_relsz [%lu] : ",
		((nsan == 1) ? 0 : edpdp->dt_relsz));
	fflush(stdin);
	if (scanf("%lu", &ulnum) != 1)
	{
		if (nsan)
			edpdp->dt_relsz = 0;
		fprintf(stderr,"Default dt_relsz: %lu\n",edpdp->dt_relsz);
	}
	else
	{
		edpdp->dt_relsz = ulnum;
		fprintf(stderr,"Updated dt_relsz: %lu\n",edpdp->dt_relsz);
	}
	fprintf(stderr,"Enter   dt_relnext [%ld] : ",
		((nsan == 1) ? -1 : edpdp->dt_relnext));
	fflush(stdin);
	if (scanf("%ld", &lnum) != 1)
	{
		if (nsan)
			edpdp->dt_relnext = -1;
		fprintf(stderr,"Default dt_relnext: %ld\n",edpdp->dt_relnext);
	}
	else
	{
		edpdp->dt_relnext = lnum;
		fprintf(stderr,"Updated dt_relnext: %ld\n",edpdp->dt_relnext);
	}
	fprintf(stderr,"Enter   dt_relno [%lu] : ",
		((nsan == 1) ? 0 : edpdp->dt_relno));
	fflush(stdin);
	if (scanf("%lu", &ulnum) != 1)
	{
		if (nsan)
			edpdp->dt_relno = 0;
		fprintf(stderr,"Default dt_relno: %lu\n",edpdp->dt_relno);
	}
	else
	{
		edpdp->dt_relno = ulnum;
		fprintf(stderr,"Updated dt_relno: %lu\n",edpdp->dt_relno);
	}
	fprintf(stderr,"\n");
	fflush(stdin);
	dophysdisplay();
	fprintf(stderr,
	"\nDo you want to change disk physical description AGAIN [n] ? ");
	fflush(stdin);
	if (scanf("%c", &c) == 1)
		if ((c == 'y') || (c == 'Y'))
			goto doagain;
	fprintf(stderr,
"\nAre you sure that you want to WRITE new disk physical description [n] ? ");
	fflush(stdin);
	if (scanf("%c", &c) != 1)
		return (0);
	if ((c != 'y') && (c != 'Y'))
		return (0);
	edputpd();
	return (0);
}

dophysdisplay()
{
int	i;
char	nbuf[DNSIZE+PDNSIZE];

	if (edpdp->dt_magic == DMAGIC)
	{
		fprintf(stderr,"dt_magic = %#lx\n",edpdp->dt_magic);
		fprintf(stderr,"dt_ploz = %#x  dt_plonz = %#x\n",
			edpdp->dt_ploz, edpdp->dt_plonz);
		strncpy(nbuf,edpdp->dt_name,DNSIZE);
		nbuf[DNSIZE] = '\0';
		fprintf(stderr,
			"dt_name = '%s'  dt_maxbad = %u  dt_bootblkno = %u\n",
			nbuf, edpdp->dt_maxbad, edpdp->dt_bootblkno);
		fprintf(stderr,
			"dt_nsectors = %u dt_ntracks = %u dt_ncylinders = %u\n",
			edpdp->dt_nsectors,
			edpdp->dt_ntracks,
			edpdp->dt_ncylinders);
		fprintf(stderr,"dt_secpercyl = %u  dt_secperunit = %lu\n",
			edpdp->dt_secpercyl, edpdp->dt_secperunit);
		for (i = 0; i < NPART; i++)
		{
			fprintf(stderr,
				"dt_nsec[%d] = %8u  dt_cyloff[%d] = %8u\n",
				i, edpdp->dt_nsec(i), i, edpdp->dt_cyloff(i));
		}
		if (edpdp->dt_sanity == VALID_PD)
		{
			fprintf(stderr,"dt_sanity = %#lx\n",edpdp->dt_sanity);
			fprintf(stderr,"dt_driveid = %lu  dt_version = %lu",
				edpdp->dt_driveid,
				edpdp->dt_version);
			strncpy(nbuf,edpdp->dt_serial,PDNSIZE);
			nbuf[PDNSIZE] = '\0';
			fprintf(stderr,"  dt_serial = '%s'\n",nbuf);
	fprintf(stderr,	
	"dt_cyls = %lu  dt_tracks = %lu  dt_sectors = %lu  dt_bytes = %lu\n",
				edpdp->dt_cyls,
				edpdp->dt_tracks,
				edpdp->dt_sectors,
				edpdp->dt_bytes);
			if ((long)edpdp->dt_logicalst < (long)0)
			{
				fprintf(stderr,"dt_logicalst = %#lx",
					edpdp->dt_logicalst);
			}
			else
			{
				fprintf(stderr,"dt_logicalst = %lu",
					edpdp->dt_logicalst);
			}
			if ((long)edpdp->dt_errlogst < (long)0)
			{
				fprintf(stderr,"  dt_errlogst = %#lx",
					edpdp->dt_errlogst);
			}
			else
			{
				fprintf(stderr,"  dt_errlogst = %lu",
					edpdp->dt_errlogst);
			}
			fprintf(stderr,"  dt_errlogsz = %lu\n",	
				edpdp->dt_errlogsz);
			if ((long)edpdp->dt_mfgst < (long)0)
			{
				fprintf(stderr,"dt_mfgst = %#lx",
					edpdp->dt_mfgst);
			}
			else
			{
				fprintf(stderr,"dt_mfgst = %lu",
					edpdp->dt_mfgst);
			}
			fprintf(stderr,"  dt_mfgsz = %lu\n",edpdp->dt_mfgsz);
			if ((long)edpdp->dt_defectst < (long)0)
			{
				fprintf(stderr,"dt_defectst = %#lx",
					edpdp->dt_defectst);
			}
			else
			{
				fprintf(stderr,"dt_defectst = %lu",
					edpdp->dt_defectst);
			}
			fprintf(stderr,"  dt_defectsz = %lu\n",
				edpdp->dt_defectsz);
			if ((long)edpdp->dt_relst < (long)0)
			{
				fprintf(stderr,"dt_relst = %#lx",
					edpdp->dt_relst);
			}
			else
			{
				fprintf(stderr,"dt_relst = %lu",
					edpdp->dt_relst);
			}
			fprintf(stderr,"  dt_relsz = %lu\n",edpdp->dt_relsz);
			if ((long)edpdp->dt_relnext < (long)0)
			{
				fprintf(stderr,"dt_relnext = %#lx",
					edpdp->dt_relnext);
			}
			else
			{
				fprintf(stderr,"dt_relnext = %lu",
					edpdp->dt_relnext);
			}
			fprintf(stderr,"  dt_relno = %lu\n",edpdp->dt_relno);
		}
		else
			fprintf(stderr,"invalid dt_sanity = %#lx\n",
				edpdp->dt_sanity);
	}
	else
		fprintf(stderr,"invalid dt_magic = %#lx\n",edpdp->dt_magic);
}

dobadmap(ac, av)
char **av;
{
int	i, update = 0;

	didflgs = 0;
	bnoarg = 0;
	while (ac > 0)
	{
		if (av[0][0] != '-')
			badline();
		switch(av[0][1])
		{
			case 'D':
				if (DIDD & didflgs)
					badline();
				didflgs |= DIDD;
				i = getdev(ac, av);
				ac -= i;
				av += i;
				break;

			case 'b':
				if ((DIDb|DIDB) & didflgs)
					badline();
				didflgs |= DIDb;
				if (ac < 2)
					badline();
				if (sscanf(av[1], "%d", &bnoarg) != 1)
					badline();
				/* check it later when equipped disk is open */
				ac -= 2;
				av += 2;
				break;

			case 'B':
				if ((DIDb|DIDB) & didflgs)
					badline();
				didflgs |= DIDB;
				if (ac < 4)
					badline();
				if (sscanf(av[1], "%d", &cylarg) != 1)
					badline();
				if (sscanf(av[2], "%d", &trkarg) != 1)
					badline();
				if (sscanf(av[3], "%d", &secarg) != 1)
					badline();
				/* check it later when equipped disk is open */
				ac -= 4;
				av += 4;
				break;

			case 'u':	/* update bad block map */
				update = 1;
				ac--;
				av++;
				break;

			default:
				badline();
		}
	}
	if (!(didflgs & DIDD))
		badline();
	if (edopen(devarg))
		exit(ERREXIT);
#ifdef	notdef
	if (didflgs & DIDb)
	{
		if (dadcheck(bnoarg))
			exit(ERREXIT);
	}
	else
	{
		if (didflgs & DIDB)
		{
			if ((bnoarg = ctstodad(cylarg, trkarg, secarg)) == -1)
				exit(ERREXIT);
		}
		else
		{
			bnoarg = 0;
		}
	}
#endif	/* notdef */
	domapdisplay();
	if (update)
		domapupdate();
	edclose();
	return (0);
}

domapupdate()
{
	return (0);
}

domapdisplay()
{
int	i, cnt, blkno;
struct	sect_forw *bmap;
extern	struct eddata badmap;
extern	dev_t edisopen;

	if (edgetmap())
		return (-1);
	bmap = (struct sect_forw *) badmap.badr;
	if (bmap->sf_magic != BSTMAGIC)
	{
		fprintf(stderr,"%s: invalid badmap magic number: %#x\n",
			cmdname, bmap->sf_magic);
		return (-1);
	}
	if (bmap->sf_count <= 0)
	{
		fprintf(stderr,"%s: there are no badmap entries on disk\n",
			cmdname);
		return (0);
	}
	cnt = bmap->sf_count;
	fprintf(stderr,"Bad Block Map for disk maj/min: %d/%d\n",
			major(edisopen), minor(edisopen));
	fprintf(stderr,"Block         Disk  Address         Replacement\n");
	fprintf(stderr,"Number        cyl  trk  sec         cyl  trk  sec\n");
	for (i = 0; i < cnt; i++)
	{
		blkno  = bmap->sf_forw[i].sf_from.da_cyl * edpdp->dt_secpercyl;
		blkno += bmap->sf_forw[i].sf_from.da_track * edpdp->dt_nsectors;
		blkno += bmap->sf_forw[i].sf_from.da_sector;
		fprintf(stderr,"%6d      %5d%5d%5d       %5d%5d%5d\n",blkno,
			bmap->sf_forw[i].sf_from.da_cyl,
			bmap->sf_forw[i].sf_from.da_track,
			bmap->sf_forw[i].sf_from.da_sector,
			bmap->sf_forw[i].sf_to.da_cyl,
			bmap->sf_forw[i].sf_to.da_track,
			bmap->sf_forw[i].sf_to.da_sector);
	}
	return (0);
}

docpboot(ac, av)
char **av;
{
struct	stat	statbuf;	/* stat buffer			 */
struct	exec	*ahead;		/* a.out header	pointer		 */
int		i, infd, cnt, blkcnt;
int		aouth, minboot, maxboot, modboot, maxbad, modmax;
int		skip = 1;
int		tape = 0;
unsigned	long asize, acnt;
unsigned	int bootblkno;
ushort		mode;
char		*bigbuf, *bgp, *bcopy();
char		dbuf[CPSIZE];
char		abuf[ASIZE];
char		inname[128];
char		c;

	didflgs = 0;
	bnoarg = 0;
	while (ac > 0)
	{
		if (av[0][0] != '-')
			badline();
		switch(av[0][1])
		{
			case 'D':
				if (DIDD & didflgs)
					badline();
				didflgs |= DIDD;
				i = getdev(ac, av);
				ac -= i;
				av += i;
				break;

			case 'b':
				if ((DIDb|DIDB) & didflgs)
					badline();
				didflgs |= DIDb;
				if (ac < 2)
					badline();
				if (sscanf(av[1], "%d", &bnoarg) != 1)
					badline();
				/* check it later when equipped disk is open */
				ac -= 2;
				av += 2;
				break;

			case 'B':
				if ((DIDb|DIDB) & didflgs)
					badline();
				didflgs |= DIDB;
				if (ac < 4)
					badline();
				if (sscanf(av[1], "%d", &cylarg) != 1)
					badline();
				if (sscanf(av[2], "%d", &trkarg) != 1)
					badline();
				if (sscanf(av[3], "%d", &secarg) != 1)
					badline();
				/* check it later when equipped disk is open */
				ac -= 4;
				av += 4;
				break;

			default:
				badline();
		}
	}
	if (!(didflgs & DIDD))
		badline();
	if (edopen(devarg))
		exit(ERREXIT);
#ifdef	notdef
	if (didflgs & DIDb)
	{
		if (dadcheck(bnoarg))
			exit(ERREXIT);
	}
	else
	{
		if (didflgs & DIDB)
		{
			if ((bnoarg = ctstodad(cylarg, trkarg, secarg)) == -1)
				exit(ERREXIT);
		}
		else
		{
			bnoarg = 0;
		}
	}
#endif	/* notdef */

getinname:
	skip = 1;
	tape = 0;
	fprintf(stderr,
"\nEnter input device/file name that has boot program [/stand/sash] : ");
	if (scanf("%s",inname) != 1)
		strcpy(inname,"/stand/sash");
	if (strcmp("/dev/mt/0mn",inname)==0 || strcmp("/dev/rmt/0mn",inname)==0)
	{
		fprintf(stderr,"Enter the file number on the tape [1] : ");
		if (scanf("%d",&skip) != 1)
			skip = 1;
		tape++;
	} 
	if ((infd = open(inname,0)) < 0)
	{
		fprintf(stderr,"ERROR: can't open '%s' for input\n",inname);
		perror("docpboot-open");
		goto getinname;
	}
	for (i = 1; i < skip; i++)
	{
		fprintf(stderr,"Skiping file %d on tape '%s'\n",i,inname);
		close(infd);
		if ((infd = open(inname,0)) < 0)
		{
			fprintf(stderr,"ERROR: can't reopen '%s' for skip\n",
				inname);
			perror("docpboot-open-skip");
			edclose();
			return (1);
		}
	}
	maxbad = edpdp->dt_maxbad;
	if ((maxbad < MAXBAD) || (maxbad > MAXBADTOT))
	{
		edpdp->dt_maxbad = maxbad = DEFMAXBAD;
		modmax = 1;
	}
	else
		modmax = 0;
	modboot = 0;
	maxbad = (maxbad + MAXBAD) / (MAXBAD + 1);
	minboot = maxbad + 1;
	maxboot = (edpdp->dt_secpercyl - 1) / 2;
	bootblkno = edpdp->dt_bootblkno;
	fprintf(stderr,"\n");
reboot:
	if ((bootblkno < minboot) || (bootblkno > maxboot))
	{
		bootblkno = minboot;	/* change to logical start sector */
		modboot = 1;
	}
	fprintf(stderr,"Boot program will start on sector # %d\n",bootblkno);
	fprintf(stderr,
		"Do you want to change boot program start sector (y/n) ? ");
	fflush(stdin);
	if (scanf("%c",&c) != 1)
		goto reboot;
	if ((c == 'y') || (c == 'Y'))	/* no defaults are allowed */
	{
	fprintf(stderr,"Enter new boot copy start sector (%d to %d) [%d] : ",
			minboot,maxboot,edpdp->dt_bootblkno);
		fflush(stdin);
		if (scanf("%d",&bootblkno) != 1)
			bootblkno = edpdp->dt_bootblkno;
		modboot = 1;
		goto reboot;
	}
	else
		if ((c != 'n') && (c != 'N')) /* make sure it was not a typo */
		{
			bootblkno = edpdp->dt_bootblkno;
			modboot = 0;
			goto reboot;
		}
		else
			fprintf(stderr,"\n");
	i = 0;
	if (fstat(infd,&statbuf) < 0)
	{
		fprintf(stderr,"ERROR: can't stat input file '%s'\n",inname);
		perror("docpboot-stat");
		close (infd);
		edclose();
		return (1);
	}
	mode = statbuf.st_mode & S_IFMT;
	aouth = 0;
	if ((mode == S_IFCHR) || (mode == S_IFBLK))
	{
		acnt = read(infd, abuf, ASIZE);
		if (acnt < 0)
		{
			fprintf(stderr,
			"ERROR: can't read a.out header of input file '%s'\n",
				inname);
			perror("docpboot-read a.out header");
			close (infd);
			edclose();
			return (1);
		}
		ahead = (struct exec *) abuf;
		if ((ahead->a_magic == XMAGIC) || (ahead->a_magic == ZMAGIC))
		{
			aouth = 1;
			asize = (unsigned long) 1024 +
				ahead->a_text   + ahead->a_data +
				ahead->a_syms   + ahead->a_trsize +
				ahead->a_drsize + ahead->a_strings;
		fprintf(stderr,
		"Copying a.out format file directly from physical device.\n");
		fprintf(stderr,
		"File size = %lu bytes or %lu%s 512-byte blocks.\n",
				asize,
				(asize / edsecsz),
				(((asize % edsecsz) > 0) ? "+1" : ""));
		}
		else
		{
			aouth = 0;
			if (ahead->a_magic == OMAGIC)
				fprintf(stderr,
					"Found OMAGIC type executable.\n");
			else
				if (ahead->a_magic == NMAGIC)
					fprintf(stderr,
					"Found NMAGIC type executable.\n");
				else
					fprintf(stderr,
					"No a.out header format found.\n");
			fprintf(stderr,
				"Will not write to disk from raw device!\n");
			close(infd);
			if (tape)
				close(open("/dev/mt/0m",0));
			edclose();
			return (1);
		}
	}
	else
	{
		fprintf(stderr,"Input file is a regular file system file.\n");
		aouth = 0;
		asize = statbuf.st_size;
	}
	blkcnt = (asize / edsecsz) + (((asize % edsecsz) > 0) ? 1 : 0);
	if ((bootblkno + blkcnt) >= (edpdp->dt_cyloff(0) * edpdp->dt_secpercyl))
	{
		fprintf(stderr,
		"ERROR: new boot program would overwrite a partition\n");
		close(infd);
		if (tape)
			close(open("/dev/mt/0m",0));
		edclose();
		return (1);
	}
	bigbuf = (char *) malloc(blkcnt * edsecsz);
	bzero(bigbuf,(blkcnt * edsecsz));
	if (aouth)
	{
		bgp = bcopy(abuf,bigbuf,acnt);
		acnt = asize - acnt;
	}
	else
	{
		bgp = bigbuf;
		acnt = asize;
	}
	fprintf(stderr,
		"Transferring %d bytes of input file using %d disk blocks\n",
			asize,blkcnt);
	if ((cnt = read(infd, bgp, acnt)) != acnt)
	{
		fprintf(stderr,"ERROR: bad read of input file '%s'\n",inname);
		fprintf(stderr,"       expected %d, got %d\n",acnt,cnt);
		perror("docpboot-read file");
		close(infd);
		if (tape)
			close(open("/dev/mt/0m",0));
		edclose();
		return (1);
	}
	if (edwrite(bootblkno, blkcnt, bigbuf))
	{
		fprintf(stderr,"ERROR: can't write to physical disk!\n");
		perror("docpboot-write file");
		close(infd);
		if (tape)
			close(open("/dev/mt/0m",0));
		edclose();
		return (1);
	}
	close(infd);
	/*
	 * If no-rewind tape selected, force it to rewind.
	 */
	if (tape)
		close(open("/dev/mt/0m",0));
	if (modboot || modmax)	/* if boot copy start has been modified */
	{
		edpdp->dt_bootblkno = bootblkno;
		edputpd();
	}
	edclose();
	return (0);
}

bzero(buf,cnt)
char	*buf;
int	cnt;
{
int	i;

	for (i = 0; i < cnt; i++)
		*buf++ = 0;
	return (cnt);
}

char *
bcopy(from,to,cnt)
char	*from, *to;
int	cnt;
{
int	i;

	for (i = 0; i < cnt; i++, to++, from++)
		*to = *from;
	return (to);
}
