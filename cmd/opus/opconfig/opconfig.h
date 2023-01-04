/*
 * opconfig.h -- defines and externs for opconfig
 */

#include <fcntl.h>
#include <stdio.h>

#ifdef STANDALONE
#define printf xprintf
#define exit(x) _syscmd(1,x)
#else
#define printf PRINTF
#endif
#define DKMAX	16
#define NUMLOGVOL	8
#define FLPSECTS 2350
#define SWAPSIZE 8000
#define ROOTSIZE 60000
#define CONFIGFILE ":opus.cfg"
#define DEFAULTGAP 1
#if pm400
#define MANUAL	"400PM"
#else
#if pm300
#define MANUAL	"300PM"
#else
#if pm200
#define MANUAL	"200PM"
#else	/* ns32000 */
#define MANUAL	"100PM"
#endif	/* pm200 */
#endif	/* pm300 */
#endif  /* pm400 */
#define min(a,b)	(a>b ? b : a)
#define max(a,b)	(a<b ? b : a)

/*
 * Standalone opconfig.  Used to configure block 0 of a disk drive.
 */

#define MEGSX10(x)	((x*16)/3125)
#define MINUNIXSIZE	(2000)

#define SCREENLEN	24
#define RESPLEN		128
#define BIGBLK		70

#define REREADS		1		/* controller does rereads by itself */

#define TYPE_AT	1
#define TYPE_XT	2
#define TYPE_PC	3
#define TYPE_63	4
#define TYPE_TI	5

					/* for use with block 0 updating */
#define READINIT	0
#define INITONLY	1
#define READONLY	2
#define BLKBAD		-1
#define BLKOK		0
#define BLKMOD		1
#define NOPROMPT	0
#define WITHPROMPT	1

#define CACHECNT	10

struct cache {
	int blkno;
	char blkbuf[512];
};

short parmagic;

#if m88k
struct partable {
	unsigned char bootind;
	unsigned char beghead;
	unsigned hibegcyl : 2;
	unsigned begsect : 6;
	unsigned char begcyl;
	unsigned char systind;
	unsigned char endhead;
	unsigned hiendcyl : 2;
	unsigned endsect : 6;
	unsigned char endcyl;
	unsigned relsect;
	int numsects;
} partblk[4];
#else
struct partable {
	unsigned char bootind;
	unsigned char beghead;
	unsigned begsect : 6;
	unsigned hibegcyl : 2;
	unsigned char begcyl;
	unsigned char systind;
	unsigned char endhead;
	unsigned endsect : 6;
	unsigned hiendcyl : 2;
	unsigned char endcyl;
	unsigned relsect;
	int numsects;
} partblk[4];
#endif

struct partinfo {
	long	status;
	long	nblk;
	long	cylcnt;
	long	headcnt;
	long	secttrk;
	long	parttype;
	long	sectspercyl;
	long	sectsperpart;
	long	totsects;
	char	drivername[64];
	long	driveargs[16];
} partinfo[DKMAX];

#define MAXDEV		64
#define UNIXDEVSIZE	32
#define DOSNAMESIZE	128

struct configlist {
	char unixname[UNIXDEVSIZE];
	char dosname[DOSNAMESIZE];
	short devtype;
	short accessed;
} configlist[MAXDEV];

#define C_UNUSED	-1
#define C_UNDEFINED	0
#define C_PARAM		1
#define C_SYSTEM	2
#define C_DISK		3
#define C_TTY	        4
#define C_LP	        5
#define C_SPECIAL	6
#define C_MISC		7
#define C_MAXCLASS	7

extern errno;
#if STANDALONE
extern _diskerrno;
#else
int _diskerrno;
#endif

int cachecnt;
int sparedisk;			/* fid for disk currently being spared */
int sparedrive;			/* drive number for disk being spared */
struct confinfo block0;
struct confinfo *blk0;
int validblk0;
char buf[50];
char partbuffer[512];		/* partition information buffer */
int cfgbuf[128];
char *bptr;
long maxblks;
int diskchanged;		/* specifies opus.cfg has new disk entries */
int drive;
int globchanged;
int eolcnt;
int blocksize;		/* Default block size */
int swapsize;			/* Default size of swap area */
int rootsize;			/* Default size of root area */
int sectionsize;		/* Default size of the sub sections (genesis) */
int flpsects;			/* Number of sectors on root diskette */
char *msgfilename;		/* File name containing "Insert Root" msg */
int outfilid;			/* Output file name file descriptor */
char *configfile;		/* Name of config file (default is opus.cfg) */
FILE *instream;			/* Input stream for script */

char nodename[NODENAMESIZE];	/* -E(asy) option nodename */
char timezone[TZSIZE];		/* -E(asy) option timezone */
int segment;			/* -E(asy) option segment */
int intlev;			/* -E(asy) option interrupt level */
int filsiz[DKMAX];		/* -E(asy) option file system sizes */

int gap;			/* default disk interleave */

int heads;
int sects;
int cyls;

int Aflag;			/* signifies reboot after opus.cfg mods */
int dflag;			/* use default settings for block0 */
int Cflag;			/* copy root into swap area and boot */
int Dflag;			/* print out dots in silent mode */
int Eflag;			/* easy installation with DOS files */
int Lflag;			/* load on top of old system */
int Rflag;			/* reload over top of old system (with mkfs) */
int Tflag;			/* type flag (at, xt, etc) */
int Sflag;			/* standard initialization */
int Bflag;			/* block zero initialization */
int Pflag;			/* print out input args and pause */
int Iflag;			/* specify input file instead of stdin */
int Oflag;			/* specify script file for later use with I */
int Vflag;			/* verify Root to swap copy */
int Uflag;			/* root file system size has been changed */
int Wflag;			/* swap size has been changed */
int Yflag;			/* answer yes to all questions */
int diskcnt;			/* number of disks on system */
int diskchg;			/* signifies disk change in opus.cfg */
int altcopydev;			/* alternate dev for root fs copy (sted swap) */
int gflag;			/* genesis installation */
int fflag;			/* other section fs size (genesis) */

char response[RESPLEN];
char *lineptr;
char sectbuf[BIGBLK*512];

char *strchr();
