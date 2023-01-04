#include <sys/types.h>
#include <sys/iorb.h>
#include <sys/sysconf.h>
#include <sys/errno.h>
#include "opconfig.h"

struct cache cache[CACHECNT];

struct confinfo dblock0 = {
	"opus",
	0,0,0,0,
	-1,-1,
	":/opus/opsash",
	"/unix",
	0,0,0,0,0,0,3,{0},
	{ {0,-1},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,-1} },
	{ {{0}}, '?', '7', {0}, "PST8PDT", "at" },
	{ 0 },
	256,
	'opus', 0
};

char *scr_init[] = {
#if pm100
	"(Please read the \"Opus 100PM User Manual\" before attempting",
#endif
#if pm200
	"(Please read the \"Opus 200PM User Manual\" before attempting",
#endif
#if pm300
	"(Please read the \"Opus 300PM User Manual\" before attempting",
#endif
#if pm400
	"(Please read the \"Opus 400PM User Manual\" before attempting",
#endif
	"\tto use this program.)",
	"",
	"The following options are available",
	"",
	"\t(?)  More information",
	"\t(1)  Standard System Initialization",
	"\t(2)  Opus5 Device Configuration (opus.cfg)",
	"\t(3)  DOS/Opus5 Partition Information",
	"\t(4)  Disk Bad Block Handling",
	"\t(5)  UNIX File System Layout",
	"\t(6)  Opus5 UNIX System Parameters",
	"",
	"\t(q)  Quit this program",
	"",
	0
};

char *hscr_init[] = {
	"Opconfig sets up system parameters and configuration information.",
	"Opus5 software uses this information during installation and",
	"booting.  Opconfig maps UNIX devices to DOS drivers and makes",
 	"allowances for hardware differences among various PC manufacturers.",
	"It defines how disk areas are to be used by DOS and Opus5, handles",
	"bad sectors, and defines file system boundaries.",
	"",
	"Opconfig is intended to be self-explanatory for most installations.",
	"It is menu driven and has help information.",
	"",  
	"Unless you have unusual system requirements, you should respond", 
	"in the affirmative to all opconfig's queries, making as few",
	"changes as possible to the standard definitions.",
	"",
#if pm100
	"See the \"Opus 100PM User Manual\" for more information",
#endif
#if pm200
	"See the \"Opus 200PM User Manual\" for more information",
#endif
#if pm300
	"See the \"Opus 300PM User Manual\" for more information",
#endif
#if pm400
	"See the \"Opus 400PM User Manual\" for more information",
#endif
	"",
	0
};

main(argc,argv)
int argc;
char **argv;
{
	register int i ,j;
	int ofid;
	int driveno;

#ifdef STANDALONE
#if pm200 || pm300 || pm400
	extern argwait;

	argwait = 0;
	if ( argv[0] == 0 )
		argc = getargv("opconfig ", &argv, 0);
#endif
#endif
	instream = stdin;
	blocksize = 1024;
	flpsects = FLPSECTS;
	swapsize = SWAPSIZE;
	rootsize = ROOTSIZE;
	configfile = CONFIGFILE;
	gap = DEFAULTGAP;
	drive = -1;
	blk0 = &block0;
	if (sizeof(struct confinfo) != 512) {
		brintf("CONFIGURATION INFORMATION NOT 512 BYTES (%d)\n",
			sizeof(struct confinfo));
		brintf("\tdosinfo starts at %d\n",(char *)&block0.dosdata -
			(char *)&block0);
		brintf("\tbootname starts at %d\n",(char *)block0.bootname -
			(char *)&block0);
	}
	if (sizeof(struct spareinfo) != 512) {
		brintf("SPARE INFORMATION NOT 512 BYTES (%d)\n",
			sizeof(struct spareinfo));
	}

	argv++;
	argc--;
	if (argc > 0) {
		if (argv[0][0] >= '0' && argv[0][0] <= '7') {
			drive = atoi(argv[0]);
			argv++;
			argc--;
		} else if (argv[0][0] != '-') {
			drive = -1;
			argv++;
			argc--;
		}

		if (argc > 0) {
		    while (argc > 0) {
			if (argv[0][0] == '-') {
				switch(argv[0][1]) {
				case 'A':
					Aflag++;
					break;
				case 'a':
					altcopydev = atoi(argv[1]);
					argc--; argv++;
					break;
				case 'b':
					blocksize = atoi(argv[1]);
					argc--; argv++;
					switch (blocksize) {
						case 1024:
						case 2048:
						case 4096:
						case 8192:
							break;
						default:
							brintf("opconfig: bad -b argument\n", argv[0]);
							brintf("opconfig: valid block sizes: [1024|2048|4096|8192]\n");
							exit(1);
					}
					break;
				case 'd':
						/* just default, don't read */
					rdblock0(drive,INITONLY);
					updblock0(drive,NOPROMPT);
					dflag++;
					break;
				case 'g':
				case 'G':
					gap = atoi(argv[1]);
					argc--; argv++;
					break;
				case 'C':
					Cflag++;
					break;
				case 'E':
					Eflag++;
					break;
				case 'I':
					if (argc > 1) {
						if ((instream = fopen(argv[1],
							"r")) == NULL) {
	brintf("opconfig: %s: cannot open\n",argv[1]);
	exit(10);
						}
						argc--; argv++;
					}
					break;
				case 'O':
					if (argc > 1) {
						if ((outfilid = open(argv[1],O_RDWR | O_CREAT)) < 0) {
	brintf("opconfig: %s: cannot create\n",argv[1]);
	exit(11);
						}
						argc--; argv++;
					}
					break;
				case 'F':
					if (argc > 1) {
						flpsects = strtol(argv[1],NULL,0);
						argc--; argv++;
					}
					break;
				case 'P':
					Pflag++;
					break;
				case 'D':
					Dflag++;
					break;
				case 'M':
					if (argc > 1) {
						msgfilename = argv[1];
						argc--; argv++;
					}
					break;
				case 'L':
					Lflag++;
					break;
				case 'R':
					Rflag++;
					break;
				case 'S':
					Sflag++;
					break;
				case 't':
				case 'T':
					if (argc > 1) {
						Tflag = chktype(argv[1]);
						if (!Tflag) {
					   		printf("opconfig: %s: bad type\n",
								argv[1]);
					    		break;
						}
						argc--;
						argv++;
					}
					break;
				case 'X':
					if (argc > 1) {
						configfile = argv[1];
						argc--;
						argv++;
					}
					break;
				case 'V':
					Vflag++;
					break;
				case 'Y':
					Yflag++;
					break;
				case 'u':
				case 'U':
					rootsize = strtol(argv[1],NULL,0);
					Uflag++;
					break;
				case 'w':
				case 'W':
					swapsize = strtol(argv[1],NULL,0);
					Wflag++;
					break;
				case 'Z':
					/* just default, don't read block 0 */
					drive = 0;
					rdblock0(drive,INITONLY);
					updblock0(drive,NOPROMPT);
					dflag++;
				case 'q':
		        		updblock0(drive,NOPROMPT);
					exit(0);
				default:
					brintf("opconfig: %s: bad parameter\n",
						argv[0]);
					break;
				}
			} else if (argv[0][0] != 0) {
				;
				/*if (strcmp(argv[0],"install"))
					brintf("opconfig: %s: bad parameter\n",
						argv[0]);*/
			}
			argc--; argv++;
		    }
		}
	} else {
		drive = -1;
	}

	nl();
	while (1) {
		printdiv("Opus5 System Configuration Program (opconfig)\n");
		nl();
		if (Sflag) {
			exit(standard());
		}
		scrn(scr_init);
		choose();
		if (strlen(response) > 2) {
			invresp();
			continue;
		}
		switch (response[0]) {
		case '?':
			scrn(hscr_init);
			pause("");
			break;
		case '1':
			exit(standard());
			break;
		case '2':
			setupcfg(Tflag,1);
			break;
		case '3':
			setupall(Tflag);
			break;
		case '4':
			badblock(drive);
			break;
		case '5':
			unixfilsys(drive);
			break;
		case '6':
			unixsys(drive);
			break;
		case 'q':
		case 'Q':
			alldone();
			exit(0);
			break;
		}
	}
}

badzero(driveno) {
	printf("Parameter block for drive %d not initialized.",driveno);
	printf("  Use -d option\n");
	printf("\tto initialize with default values.\n");
	nl();
}

sizeswap(blkno) {
	char device[16];
	
	if (blk0->swapdev == NUMLOGVOL * (DKMAX-1)) {
		if (touchblk("/dev/swap",blk0->nswap + blk0->swplo))
			return(0);
	} else {
		sprintf(device,"/dev/rdsk/%ds%d",(blk0->swapdev&0xff)/NUMLOGVOL,
			(blk0->swapdev&0xff)%NUMLOGVOL);
		if (touchblk(device,blk0->nswap + blk0->swplo))
			return(0);
	}
	brintf("opconfig: %d blocks not available for swap area\n",
		blk0->nswap);
	return(1);
}

touchblk(device,blkno)
char *device;
{
	int fid;
	char buf[512];

	if ((fid = open(device,2)) < 0)
		return(0);
	if (lseek(fid,(blkno-1)*512,0) != (blkno-1)*512)
		return(0);
	if (read(fid,buf,512) != 512)
		return(0);
	if (lseek(fid,(blkno-1)*512,0) != (blkno-1)*512)
		return(0);
	if (write(fid,buf,512) != 512) 
		return(0);
	close(fid);
	return(512);
}

choose()
{
	resp("Choose an item from the list above: ");
}

scrn(scr)
char *scr[];
{
	while (*scr) {
		brintf("%s\n",*scr++);
	}
}

invresp() {
	nl();
	bpause("Invalid response.  ");
}

bpause(str) {
	resp("%s%s",str,"Type <cr> to continue: ");
}

pause(str) {
	resp("%s%s",str,"Type <cr> to continue: ");
}

alldone() {
	if (globchanged) {
		brintf("System Parameter Block Modified\n");
	}
	else
		brintf("System Parameter Block Unmodified\n");
	if (outfilid > 0) {
		write(outfilid,"\032",1);
		close(outfilid);
	}
}

updatesys() {
	brintf("Updating system information\n");
}
		
yesno() {
	nlresponse();
	if (response[0] == 'y' || response[0] == 'Y')
		return(1);
	else
		return(0);
}

printdiv(str)
char *str;
{
	divider();
	brintf("%s",str);
}

divider() {	
	brintf("----------------------------------------\n");
}

nl() {
	brintf("\n");
}

need(cnt)
{
/*
	while (eolcnt-- > cnt)
		printf("\n");
*/
	eolcnt = SCREENLEN;
}

seeman() {
#if pm100
	bpause("\tSee the \"Opus 100PM User Manual\".  ");
#endif
#if pm200
	bpause("\tSee the \"Opus 200PM User Manual\".  ");
#endif
#if pm300
	bpause("\tSee the \"Opus 300PM User Manual\".  ");
#endif
#if pm400
	bpause("\tSee the \"Opus 400PM User Manual\".  ");
#endif
}

brintf(arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7) {
	register i;
	register char *cptr;
	
/* CCF
	cptr = (char *)arg0;
	while (*cptr)
		if (*cptr++ == '\n')
			if (eolcnt-- < 1) {
				bpause("Screen full.  ");
			}
*/
	printf(arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7);
}

PRINTF(arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7) {
	register i;
	register char *cptr;
	
	if ((instream == stdin) || Pflag)
		fprintf(stdout,arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7);
}

resp(arg0,arg1,arg2,arg3,arg4) {
	need(1);
	printf(arg0,arg1,arg2,arg3,arg4);
	nlresponse();
}

nlresponse() {
	getresponse();
	nl();
}

getresponse() {
	register i;
	char xresp[RESPLEN];
	FILE *savestream;

	for (i=0; i<RESPLEN; i++)
		response[i] = 0;
	if (feof(instream)) {
		instream = stdin;
		nl();
		printf("Invalid specification file, unable to continue.\n");
		exit(13);
	}
	fgets(response,RESPLEN,instream);
	while ((response[strlen(response)-1] == '\n') ||
	        ((response[strlen(response)-1] == '\r')))
			response[strlen(response)-1] = 0;
	if (outfilid > 0) {
		write(outfilid,response,strlen(response));
		write(outfilid,"\r\n",2);
	}
	if (Pflag) {
		printf("{%s}",response);
		fgets(xresp,RESPLEN,stdin);
	}
	if (Dflag) {
		savestream = instream;
		instream = stdin;
		printf(".");
		instream = savestream;
	}
	lineptr = response;
	skipblanks();
	if (!strcmp(lineptr,"exit") || !strcmp(lineptr,"EXIT"))
		exit(99);
}

getcomma() {
	skipblanks();
	if (*lineptr != ',')
		return(0);
	else
		lineptr++;
	skipblanks();
	return(1);
}

getnum(base) {
	register char ch;
	register long newnum;
	register long num;
	register minus;

	skipblanks();
	if (*lineptr == 0)
		return(-1);
	num = 0;
	minus = 1;
	if (*lineptr == '-') {
		lineptr++;
		minus = -1;
	}
	for(;;) {
		ch = *lineptr;
		if (base < (ch - '0'))
			return(minus * num);
		newnum = num * base + ch;
		if ((ch >= '0') && (ch <= '9'))
			num = newnum - '0';
		else if ((ch >= 'A') && (ch <= 'F'))
			num = newnum - 'A' + 10;
		else if ((ch >= 'a') && (ch <= 'f'))
			num = newnum - 'a' + 10;
		else
			return(minus * num);
		lineptr++;
	}
}

skipblanks() {
	register char ch;

	ch = *lineptr;
	while ((ch == ' ') || (ch == '\t') || (ch == '[') || (ch == ']'))
		ch = *++lineptr;
}

copy(buf1,buf2,count)
register char *buf1;
register char *buf2;
register count;
{
	while (count--)
		*buf1++ = *buf2++;
}

#ifdef STANDALONE
xprintf(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9) {
	char buf[256];
	char buf2[256];
	char *ptr, *ptr2;

	sprintf(buf,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9);
	ptr = buf;
	ptr2 = buf2;
	while (*ptr) {
		if ((*ptr2++ = *ptr++) == '\n')
			*ptr2++ = '\r';
	}
	*ptr2++ = 0;
	_xonxoff();
	if ((instream == stdin) || Pflag)
		_putstr(buf2);
}
#endif

frdblk(sector,buf)
char *buf;
{
	register i;

	for (i=0; i<REREADS; i++) {
		if (rdblk(sector,buf) == 512)
			return(512);
		if (_diskerrno != E_DE)
			break;
	}
	return(0);
}

clearcache() {
	register int i;

	for (i=0; i<CACHECNT; i++)
		cache[i].blkno = 0;
}

rdblk(sector,buf)
char *buf;
{
	register i;
	register cnt;

	for (i=0; i<CACHECNT; i++) {
		if (sector && (sector == cache[i].blkno)) {
			copy(buf,cache[i].blkbuf,512);
			return(512);
		}
	}
	if ((cnt = rdbblk(sector,buf,512)) == 512) {
		copy(cache[cachecnt].blkbuf,buf,512);
		cache[cachecnt].blkno = sector;
		cachecnt = (cachecnt + 1) % CACHECNT;
	}
	return(cnt);
}

rdbblk(sector,buf,bytecnt)
char *buf;
{
	if (lseek(sparedisk,sector*512,0) != sector*512)
		return(0);
	if (read(sparedisk,buf,bytecnt) != bytecnt)
		return(0);
	return(bytecnt);
}

wrtblk(sector,buf)
char *buf;
{
	register i;

	for (i=0; i<CACHECNT; i++) {
		if (cache[i].blkno == sector) {
			copy(cache[i].blkbuf,buf,512);
		}
	}
	if (lseek(sparedisk,sector*512,0) != sector*512)
		return(0);
	if (write(sparedisk,buf,512) != 512)
		return(0);
	return(512);
}

clrblk(buf)
char *buf;
{
	register i;

	for (i=0; i<512; i++)
		buf[i] = 0;
}

