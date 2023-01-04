#include <sys/types.h>
#include <sys/iorb.h>
#include <sys/sysconf.h>
#include <sys/errno.h>
#include "opconfig.h"

#define MAXDEV		64
#define UNIXDEVSIZE	32
#define DOSNAMESIZE	128

char *dev_types[] = {
	"UNDEFINED ",
	"PARAMETER ",
	"SYSTEM    ",
	"DISK      ",
	"TTY       ",
	"LP        ",
	"SPECIAL   ",
	"MISC      ",
	0
};

struct devclass {
	char *dev_name;
	short dev_type;
} dev_class[] = {
	{ "tz",		C_PARAM,	},
	{ "TZ",		C_PARAM,	},
	{ "clock",	C_SYSTEM, 	},
	{ "CLOCK",	C_SYSTEM, 	},
	{ "dos",	C_SYSTEM,	},
	{ "DOS",	C_SYSTEM,	},
	{ "console",	C_SYSTEM,	},
	{ "CONSOLE",	C_SYSTEM,	},
	{ "con1",	C_SYSTEM,	},
	{ "CON1",	C_SYSTEM,	},
	{ "con2",	C_SYSTEM,	},
	{ "CON2",	C_SYSTEM,	},
	{ "con3",	C_SYSTEM,	},
	{ "CON3",	C_SYSTEM,	},
	{ "dsk/",	C_DISK,		},
	{ "DSK/",	C_DISK,		},
	{ "flpA",	C_DISK,		},
	{ "FLPA",	C_DISK,		},
	{ "flpB",	C_DISK,		},
	{ "FLPB",	C_DISK,		},
	{ "flp",	C_DISK,		},
	{ "FLP",	C_DISK,		},
	{ "swap",	C_DISK,		},
	{ "SWAP",	C_DISK,		},
	{ "tty",	C_TTY,		},
	{ "TTY",	C_TTY,		},
	{ "lp",		C_LP,		},
	{ "LP",		C_LP,		},
	{ "vdi",	C_SPECIAL	},
	{ "VDI",	C_SPECIAL	},
	{ "cgi",	C_SPECIAL	},
	{ "CGI",	C_SPECIAL	},
	{ "drv",	C_SPECIAL	},
	{ "DRV",	C_SPECIAL	},
	{ "mt/",	C_SPECIAL	},
	{ "MT/",	C_SPECIAL	},
	{ "ether",	C_SPECIAL	},
	{ "ETHER",	C_SPECIAL	},
	{ "graph",	C_SPECIAL	},
	{ "GRAPH",	C_SPECIAL	},
	{ "gn/",	C_SPECIAL	},
	{ "GN/",	C_SPECIAL	},
	{ "udio/",	C_SPECIAL	},
	{ "UDIO/",	C_SPECIAL	},
	{ "t",		C_PARAM,	},
	{ "T",		C_PARAM,	},
	{ "e",		C_PARAM,	},
	{ "E",		C_PARAM,	},
	{ "s",		C_PARAM,	},
	{ "S",		C_PARAM,	},
	{ "i",		C_PARAM,	},
	{ "I",		C_PARAM,	},
	{ "a",		C_PARAM,	},
	{ "A",		C_PARAM,	},
	{ 0,		0,		}
};

struct configlist dos_dev[] = {
	{ "tz",		"PST8PDT",	C_PARAM,	0 },
	{ "i",		"x",		C_PARAM,	0 },
#if pm100
	{ "s",		"?",		C_PARAM,	0 },
#endif
	{ "a",		"128",		C_PARAM,	0 },
	{ "clock", 	"clock",	C_SYSTEM,	0 },
	{ "dos", 	"dos",		C_SYSTEM,	0 },
	{ "console", 	"console",	C_SYSTEM,	0 },
	{ "con1", 	"con1",		C_SYSTEM,	0 },
	{ "con2", 	"con2",		C_SYSTEM,	0 },
	{ "con3", 	"con3",		C_SYSTEM,	0 },
	{ "dsk/0", 	"c:",		C_DISK,		0 },
	{ "flpa", 	"a:",		C_DISK,		0 },
	{ "flpb", 	"b:",		C_DISK,		0 },
	{ "flpA", 	"a:bios",	C_DISK,		0 },
	{ "flpB", 	"b:bios",	C_DISK,		0 },
	{ "tty0", 	"com1",		C_TTY,		0 },
	{ "tty1", 	"com2",		C_TTY,		0 },
	{ "lp", 	"lpt1",		C_LP,		0 },
	{ "lp1", 	"lpt2",		C_LP,		0 },
#if !m88k
	{ "cgi", 	"gsscgi",	C_SPECIAL,	0 },
	{ "udio", 	"udio",		C_SPECIAL,	0 },
#endif
	{ 0,		0,		0,		0 },
};

struct dev_map {
	char *oldname;
	char *newname;
} devmap[] = {
	{ "dos",	"<dos=dos>",	},
	{ "clock",	"<clock=clock>",},
	{ "console",	"<console=console>",	},
	{ "con1",	"<con1=con1>",	},
	{ "con2",	"<con2=con2>",	},
	{ "con3",	"<con3=con3>",	},
	{ "doscon",	"<console=doscon>",	},
	{ "a:",		"<flpa=a:>",	},
	{ "b:",		"<flpb=b:>",	},
	{ "a:dos",	"<flpa=a:>",	},
	{ "b:dos",	"<flpb=b:>",	},
	{ "a:bios",	"<flpA=a:bios>",},
	{ "b:bios",	"<flpB=b:bios>",},
	{ "c:10.xt",	"<dsk/0=c:xt>"	},
	{ "c:10",	"<dsk/0=c:>",	},	
	{ "c:",		"<dsk/0=c:>",	},	
	{ "c:11",	"<dsk/0=c:11>",	},	
	{ "c:12",	"<dsk/0=c:12>",	},	
	{ "c:13",	"<dsk/0=c:13>",	},	
	{ "d:10.xt",	"<dsk/0=d:xt>"	},
	{ "d:10",	"<dsk/0=d:>",	},	
	{ "d:",		"<dsk/0=d:>",	},	
	{ "d:11",	"<dsk/0=d:11>",	},	
	{ "d:12",	"<dsk/0=d:12>",	},	
	{ "d:13",	"<dsk/0=d:13>",	},	
	{ "e:10",	"<dsk/0=e:>",	},	
	{ "e:11",	"<dsk/0=e:11>",	},	
	{ "e:12",	"<dsk/0=e:12>",	},	
	{ "e:13",	"<dsk/0=e:13>",	},	
	{ "f:10",	"<dsk/0=f:>",	},	
	{ "f:11",	"<dsk/0=f:11>",	},	
	{ "f:12",	"<dsk/0=f:12>",	},	
	{ "f:13",	"<dsk/0=f:13>",	},	
	{ "fil0",	"<dsk/0=fil0>",	},
	{ "fil1",	"<dsk/0=fil1>",	},
	{ "fil2",	"<dsk/0=fil2>",	},
	{ "fil3",	"<dsk/0=fil3>",	},
	{ "fil4",	"<dsk/0=fil4>",	},
	{ "fil5",	"<dsk/0=fil5>",	},
	{ "fil6",	"<dsk/0=fil6>",	},
	{ "fil7",	"<dsk/0=fil7>",	},
	{ "fil8",	"<dsk/0=fil8>",	},
	{ "fil9",	"<dsk/0=fil9>",	},
	{ "swap",	"<swap=swap>",	},
	{ "com1",	"<tty0=com1>"	},
	{ "com2",	"<tty1=com2>"	},
	{ "lpt1",	"<lp=lpt1>"	},
	{ "lpt2",	"<lp1=lpt2>"	},
	{ "lpt3",	"<lp2=lpt3>"	},
	{ "vdi",	"<vdi=vdi>"	},
	{ "cgi",	"<cgi=gsscgi>"	},
	{ "udio",	"<udio=udio>"	},
	{ 0,		0		},
};

#define TZDEFAULT	"tz=PST8PDT"

char *tz_scr[] = {
	"Opus5 keeps time internally in GMT (Greenwich Mean Time); it is",
	"up to individual Opus5 sites to tell Opus5 what time zone they",
	"are really in.  The Opus5 default time zone is Pacific Time.",
	"If you are not in Pacific Time, you should change the timezone",
	"now.  You will later need to edit several files in the Opus5",
#if pm100
	"file system.   (See the Opus 100PM User Manual for more information.)",
#endif
#if pm200
	"file system.   (See the Opus 200PM User Manual for more information.)",
#endif
#if pm300
	"file system.   (See the Opus 300PM User Manual for more information.)",
#endif
#if pm400
	"file system.   (See the Opus 400PM User Manual for more information.)",
#endif
	"",
	"The format for the timezone is xxxnzzz where xxx is the standard",
	"local time zone abbreviation, n is the difference in hours from",
	"GMT, and zzz is the abbreviation for the daylight-saving local",
	"time zone, if any. (See date(1).)",
	"",
	"PST8PDT is the tz entry for Pacific Standard Time, 8 hours west",
	"of Greenwich Mean Time, with Daylight Savings Time.",
	"",
	"In the continental US, the entries are:",
	"",
	"\tPST8PDT    MST7MDT    CST6CDT    EST5EDT",
	"",
	0
};

char *hscr_dev[] = {
	"The configuration file opus.cfg determines the setting of some",
	"system parameters and the association of UNIX devices to DOS",
	"drivers used by opmon(1*).  The basic format is [p=s] for parameters",
	"and <d=x> for devices, where p is the parameter name, s is the",
	"parameter setting, d is the UNIX device name, and x is the opmon",
	"driver name.",
	"",
	"The following devices are the default parameters and devices:",
	"",
#if pm100
	"\t[tz=PST8PDT]    [i=x]           [s=?]           [a=128]",
#else
	"\t[tz=PST8PDT]    [i=x]           [a=128]",
#endif
	"\t<clock=clock>   <dos=dos>       <console=console><con1=con1>",
	"\t<con2=con2>     <con3=con3>     <flpa=a:>       <flpb=b:>",
	"\t<flpA=a:bios>   <flpB=b:bios>   <dsk/0=c:>      <tty0=com1>",
	"\t<tty1=com2>     <lp=lpt1>       <lp1=lpt2>      <cgi=cgi>",
	"",
	"Note that a disk driver name (e.g., dsk/0) corresponds to a logical",
	"disk that includes all the sections of that disk (i.e., /dev/dsk/c1d0s0",
	"through /dev/dsk/c1d0s7).",
	"",
	0
};

setupcfg(typeflag,timezoneok) {
    int fid;
    int i,j;
    char *pos;
    int beenhere;
    int matched;
    char name[UNIXDEVSIZE + DOSNAMESIZE + 1];
    int dtype;
    char filestr[128];
    int ofid;

    beenhere = 0;
    if (Aflag)
	return(0);
    while (1) {
	printdiv("Opus5 Device Configuration (modifying DOS file ");
	printf("%s)\n",configfile);
	nl();
	if (!beenhere++) {
	    while (readconfig() <= 0) {
		printf("There is no configuration file available.  ");
		printf("Would you like to create\n");
		printf("\tone with default values [y,n]? ");
		if (!yesno()) {
		    bpause("Invalid configuration file.  ");
		    return(1);
		}
		for (i=0; dos_dev[i].devtype; i++)
			movedevice(&configlist[i],&dos_dev[i]);
		diskchanged++;
		configlist[i].devtype = 0;
		if (writeconfig() <= 0) {
			printf("Unable to create configuration file.\n");
			return(2);
		}
	    }
	    diskchanged += fixconfig(typeflag,timezoneok++);
	}
	displaycfg();
	printf("Is this configuration file okay [y,n]? ");
	if (yesno())
		goto alldone;
			
	brintf("You may modify this configuration as follows:\n");
	nl();
	brintf("\t(?)  More information\n");
	brintf("\t(1)  Add an entry to the current configuration\n");
	brintf("\t(2)  Delete an entry from the current configuration\n");
	brintf("\t(3)  Modify an existing entry\n");
	brintf("\t(4)  Construct %s from default values\n",configfile);
	brintf("\t(5)  Reread %s from the DOS file system\n",configfile);
if (!Sflag) {
	brintf("\n");
	brintf("\t(q)  Return to top level\n");
}
	nl();
	resp("Choose an item from the list above or type \"y\" if okay: ");
	switch(*lineptr) {
	case '?':
		scrn(hscr_dev);
		pause("");
		break;
	case '1':
		displaycfg();
		printf("To add a parameter [] or device <> to the");
		printf(" above configuration file,\n");
		resp("\ttype the new entry: ");
		if (!*lineptr)
			break;
		strncpy(name,lineptr,sizeof(name));
		if ((matched = addevice(lineptr,0)) < 0) {
			if (matched == -4) {
printf("The basic format for entries is [p=s] for parameters and <d=x>\n");
printf("\tfor devices, where p is the parameter name, s is the parameter\n");
printf("\tsetting, d is the UNIX device name, and x is the opmon driver\n");
resp("\tname.  Type \"q\" to add this entry anyway or <cr> to continue: ");
				nl();
				if (*lineptr != 'q' && *lineptr != 'Q')
					break;
			}
			if (matched == -3) {
				printf("You are trying to add too many");
				printf(" entries.  You need to use a\n");
				printf("\ttext editor under DOS or UNIX");
				printf(" to modify %s\n",configfile);
				bpause("\t");
				break;
			} else if (matched == -2) {
				printf("This name looks like a name that is");
				printf(" already being used.\n");
				printf("\tDo you wish to add \"%s%s\"",
					(*name!='['&&name[strlen(name)-1]==']')?
					"[":"",name);
				printf(" anyway [y,n]? ");
				if (!yesno())
					break;
			} else if (matched == -1) {
				printf("This entry does not match a");
				printf(" standard device.  Do you wish\n");
				printf("\tto add it anyway [y,n]? ");
				if (!yesno())
					break;
			}
			matched = addevice(name,1);
		}
		if (configlist[matched].devtype == C_DISK)
			diskchanged++;
		break;
	case '2':
delentry:
		displaycfg();
		printf("To delete any of the parameters or devices");
		printf(" from the above configuration file,\n");
		printf("\ttype the first few characters of the entry");
		resp(" to be deleted: ");
		if (!*lineptr)
			break;
		dtype = rmdevice(lineptr);
		if (dtype < 0) {
			if (dtype == -1) {
				printf("Did not find entry to match \"%s\";  ",
					lineptr);
				bpause("");
				goto delentry;
			}
			printf("Typed string matches multiple entries;");
			printf(" please be more specific.\n");
			bpause("\t");
			goto delentry;
		}
		if (dtype == C_DISK)
			diskchanged++;
		break;
	case '3':
modentry:
		displaycfg();
		printf("To change an existing entry, type the first");
		printf(" few characters of the\n");
		resp("\tentry you wish to modify: ");
		if (!*lineptr)
			break;
		matched = findconfig(lineptr);
		if (matched < 0) {
			if (matched == -2) {
			    printf("Typed string matched multiple entries;");
			    printf(" please be more specific.\n");
			    bpause("\t");
			}
			if (matched == -1) {
				printf("Did not find entry to match \"%s\";  ",
					lineptr);
				bpause("");
			}
			goto modentry;
		}
		if (configlist[matched].devtype == C_PARAM)
			resp("Type the new setting for parameter \"%s\": ",
				configlist[matched].unixname);
		else
			resp("Type the new driver for device \"%s\": ",
				configlist[matched].unixname);
		if (!*lineptr)
			break;
		/* if the user typed "device=", skip that part, */
		/*  although be careful not to match (arg=...) */
		if (((pos = strchr(lineptr,'=')) == NULL) ||
			((strchr(lineptr,'(') != NULL) &&
				(strchr(lineptr,'(') < pos)))
			pos = lineptr;
		else
			pos++;
		strcpy(configlist[matched].dosname,pos);
		if (configlist[matched].devtype == C_DISK)
			diskchanged++;
		break;
	case '4':
		for (i=0; dos_dev[i].devtype; i++) {
			movedevice(&configlist[i],&dos_dev[i]);
		}
		configlist[i].devtype = 0;
		fixconfig(typeflag,0);
		diskchanged++;
		break;
	case '5':
		printf("Rereading %s from DOS file system\n",configfile);
		beenhere = 0;
		diskchanged = 0;
		break;
	case 'q':
	case 'Q':
		printf("Would you like the new configuration saved [y,n]? ");
		if (yesno()) {
			if (writeconfig() <= 0) {
				printf("Unable to write configuration file\n");
				return(3);
			}
			printf("If you have modified any of the disk related");
			printf(" entries in %s,\n",configfile);
			printf("\tyou must exit and re-execute opconfig so");
			printf(" that opmon can re-initialize\n");
			printf("\twith the new disk configuration.");
			printf("  Type \"q\" to exit, anything \n");
			resp("\telse to continue: ");
			if (*lineptr == 'q' || *lineptr == 'Q')
				exit(3);
		}
		return(0);
	case 'y':
	case 'Y':
alldone:
		if (writeconfig() <= 0) {
			printf("Unable to write configuration file\n");
			return(3);
		}
		printf("The configuration file %s has been",configfile);
		printf(" successfully written to disk.\n");
		nl();
		if (diskchanged) {
			printf("In order to continue,");
			printf(" the system must be rebooted using the");
			printf(" new configuration.\n");
if (Sflag) {
			printf("\tThe system will be rebooted automatically");
			printf(" if this program is being run\n");
			printf("\tas part of the installation procedure.");
}
			pause("  ");
			if ((ofid = open("opinst2.bat",1)) >= 0) {
				lseek(ofid,0,0);
				sprintf(filestr,"opinst noecho %s -W %d -U %d\r\n\032",
					"%1 %2 %3 %4 %5 %6 %7 %8 %9",swapsize,rootsize);
				write(ofid,filestr,strlen(filestr));
				close(ofid);
				exit(4);
			} else
				exit(3);
		}
		return(0);
	}
    }
}

displaycfg() {
	int i;
	int j;
	int pos;
	int dtype;
	char newdev[DOSNAMESIZE + UNIXDEVSIZE + 1];

	printdiv("Your current configuration file includes:\n");
	nl();
    	for (dtype = 1; dtype <= C_MAXCLASS; dtype++) {
		printf("%s\t", dev_types[dtype]);
		pos = 0;
		for (i=0; configlist[i].devtype != 0; i++) {
			if (configlist[i].devtype != dtype)
				continue;
			configlist[i].accessed = 0;
			if (configlist[i].unixname[0])
				sprintf(newdev,"<%s=%s>",configlist[i].unixname,
					configlist[i].dosname);
			else
				sprintf(newdev,"<%s>",configlist[i].dosname);
			if (dtype == C_PARAM) {
				if (configlist[i].unixname[0])
				 	sprintf(newdev,"[%s=%s]",
						configlist[i].unixname,
						configlist[i].dosname);
				else
					sprintf(newdev,"[%s]",
						configlist[i].dosname);
			}
			if ((pos + strlen(newdev)) >= 63) {
				pos = 0;
				printf("\n\t\t");
			}
			pos += strlen(newdev);
			while ((pos%21) != 0) {
				strcat(newdev," ");
				pos++;
			}
			printf("%s",newdev);
		}
		nl();
		nl();
    	}
}

#ifndef STANDALONE
#define open dosopen
#define creat doscreat
#define close dosclose
#define read dosread
#define write doswrite
#endif

readconfig() {
	int size;
	int fid;
	int i;
	int c;
	int found;
	int lparen;
	int dev, pdev;
	char name1[128];
	char name2[128];
	char *cptr;

#ifndef STANDALONE
	if (*configfile == ':')
		configfile++;
#endif
	/* read parameters and devices */
	if ((fid = open(configfile,O_RDWR)) < 0) {
		return(fid);
	}
	c = 0;
	found = 0;
	lparen = 0;
	dev = 0;
	cptr = 0;
	while ((size = read(fid,buf,512)) > 0) {
		for (i=0; i<size; i++) {
			if (buf[i] == '<' || buf[i] == '[') {
				if (buf[i] == '[')
					found++;
				found++;
				cptr = name1;
				*name2 = 0;
				continue;
			}
			if (buf[i] == '(')
				lparen++;
			if (cptr) {
				if (buf[i] == '=' && lparen == 0) {
					*cptr = 0;
					cptr = name2;
					continue;
				}
				if (buf[i] == '>' || buf[i] == ']') {
					*cptr = 0;
					if (!strcmp(name1,"end") ||
							!strcmp(name1,"END")) {
						found = 0;
						goto closefile;
					}
					if (*name2) {
						strcpy(configlist[dev].unixname,
							name1);
						strcpy(configlist[dev].dosname,
							name2);
					} else {
						configlist[dev].unixname[0] = 0;
						strcpy(configlist[dev].dosname,
							name1);
					}
					configlist[dev].devtype =
					    typedev(configlist[dev].unixname);
					if ((configlist[dev].devtype == C_PARAM)
						&& (found != 2))
					    configlist[dev].devtype = C_MISC;
					if (found == 2)
					    configlist[dev].devtype = C_PARAM;
					configlist[++dev].devtype = 0;
					cptr = 0;
					found = 0;
					lparen = 0;
					continue;
				}
				*cptr++ = buf[i];
			}
		}
	}
closefile:
	close(fid);
	if (found)
		return(-2);
	return(pdev + dev);
}

typedev(str) {
	int i;

	for (i=0; dev_class[i].dev_name; i++)
		if (!strncmp(dev_class[i].dev_name,str,
				min(strlen(str),strlen(dev_class[i].dev_name))))
			return(dev_class[i].dev_type);
	return(C_MISC);
}

writeconfig()
{
	int size;
	int i;
	int fid;
	int ofid;
	int bytesread;
	int dtype;
	char entry[128];
	char savefile[64];

	if ((fid = open(configfile,0)) >= 0) {
		strcpy(savefile,configfile);
		savefile[strlen(configfile)-4] = 0;
		strcat(savefile,".sav");
#ifdef STANDALONE
		if ((ofid = open(savefile,O_RDWR)) < 0) {
#else
		if ((ofid = creat(savefile,0)) < 0) {
#endif
			printf("Unable to create backup file %s.\n",savefile);
			resp("\tType \"y\" to continue anyway: ");
			if (response[0] != 'y' && response[0] != 'Y') {
				printf("Did not write configuration file %s.\n",
					configfile);
				nl();
				return(0);
			}
		}
		while ((bytesread = read(fid,entry,128)) > 0)
			write(ofid,entry,bytesread);
		close(fid);
		close(ofid);
	}
#ifdef STANDALONE
	fid = open(configfile,O_RDWR);
#else
	fid = creat(configfile,0);
#endif
	if (fid < 0)
		return(fid);
	size = 0;
	for (dtype = 1; dtype <= C_MAXCLASS; dtype++) {
		for (i=0; configlist[i].devtype; i++) {
			if (configlist[i].devtype != dtype)
				continue;
			if (configlist[i].unixname[0])
				sprintf(entry,"<%s=%s>\r\n",
					configlist[i].unixname,
					configlist[i].dosname);
			else
				sprintf(entry,"<%s>\r\n",
					configlist[i].dosname);
			if (dtype == C_PARAM) {
				if (configlist[i].unixname[0])
					sprintf(entry,"[%s=%s]\r\n",
						configlist[i].unixname,
						configlist[i].dosname);
				else
					sprintf(entry,"[%s]\r\n",
						configlist[i].dosname);
			}
			size += write(fid,entry,strlen(entry));
		}
	}
	if (strcmp(configlist[i-1].dosname,"end") &&
			strcmp(configlist[i-1].dosname,"END")) {
		size += write(fid,"<end>\032",6);
	}
	close(fid);
	return(size);
}

#ifndef STANDALONE
#undef open
#undef close
#undef read
#undef write
#endif

fixconfig(tflag,tzok) {
	int i;
	int j;
	int dc;
	int dn;
	int entry;
	int changed;
	char suffix[64];
	struct configlist tmpconf;

	dc = 0;
	changed = 0;
	if ((entry = findconfig("tz")) < 0) {
		entry = addevice(TZDEFAULT,0);
		tzok = 0;
	}
	if (!tzok && !Aflag) {
		printf("Your UNIX system time will be set to \"%s\"\n", configlist[entry].dosname);
		printf("Make sure DOS time is also set to this timezone.\n");
#if 0
		printf("Is your timezone \"%s\" [y,n,?]? ",
			configlist[entry].dosname);
		if (!yesno()) {
		    printf("Type your correct timezone or \"?\" for");
		    printf(" more information: ");
		    getresponse();
		    nl();
gettz:
		    if (*lineptr == '?') {
			scrn(tz_scr);
			resp("Type the correct timezone: ");
			nl();
		    }
		    if (*lineptr != 'q' && *lineptr != 'Q') {
		    	if (!*lineptr) {
			    printf("Please type the correct timezone or");
			    resp(" \"q\" to continue: ");
			    goto gettz;
		        }
		        if (!strncmp(lineptr,"tz=",3) ||
				    !strncmp(lineptr,"TZ=",3))
			    strcpy(configlist[entry].dosname,&lineptr[3]);
		        else
			    strcpy(configlist[entry].dosname,lineptr);
		    }
		}
#endif
	}
	dn = 0;				/* disk number */
	for (j=0; configlist[j].devtype; j++) {
		if (configlist[j].unixname[0])
			continue;
		for (i=0; devmap[i].oldname; i++) {
			suffix[0] = 0;
			if (strcmp(configlist[j].dosname,devmap[i].oldname)) {
			    if (!strncmp(configlist[j].dosname,
					devmap[i].oldname,
					strlen(devmap[i].oldname)))
				strcpy(suffix,&configlist[j].dosname
					[strlen(devmap[i].oldname)]);
			    else
				continue;
			}
			parsedev(&tmpconf,devmap[i].newname);
			movedevice(&configlist[j],&tmpconf);
			strcat(configlist[j].dosname,suffix);
			if (configlist[j].devtype == C_DISK) {
				if (configlist[j].unixname[4] == '0')
					configlist[j].unixname[4] += dn++;
			}
			changed++;
			break;
		}
	}
			
	if (changed) {
		printf("Your configuration file %s previously had",configfile);
		printf(" old style entries.\n");
		printf("\tIt has been modified to the new format.  Please");
		printf(" examine it\n");
		printf("\tcarefully to be sure it now represents your");
		printf(" system configuration.\n");
		pause("\t");
	}

	switch (tflag) {
	case TYPE_XT:
		if (replace("c:13","c:13.xt") >= 0) dc++;
		if (replace("c:12","c:12.xt") >= 0) dc++;
		if (replace("c:11","c:11.xt") >= 0) dc++;
		if (replace("c:10","c:xt") >= 0) dc++;
		if (replace("c:","c:xt") >= 0) dc++;
		if (replace("d:13","d:13.xt") >= 0) dc++;
		if (replace("d:12","d:12.xt") >= 0) dc++;
		if (replace("d:11","d:11.xt") >= 0) dc++;
		if (replace("d:10","d:xt") >= 0) dc++;
		if (replace("d:","d:xt") >= 0) dc++;
		if (replace("e:","e:xt") >= 0) dc++;
		if (replace("f:","f:xt") >= 0) dc++;
		break;
	case TYPE_63:
		if ((entry = findconfig("clock")) >= 0)
			rmdevice("clock");
		addevice("<clock=clock(1875,100)>",0);
	default:
		while (replace(".xt","") >= 0) dc++;
		while (replace(":xt",":") >= 0) dc++;
		break;
	}
	return(dc);
}

/*
 * Findconfig searches the configlist for an exact match and returns
 *   the index of the entry.  If an exact match cannot be found, it
 *   searches for an unambiguous shortened match (e.g., cons will match
 *   console) and returns that index.  If no match is found, -1 is
 *   returned.  If multiple matches are found -2 is returned.
 */

findconfig(str)
char *str;
{
	register i;
	register cpos;
	register match;
	char name[128];

	cpos = -1;
	match = 0;
	strcpy(name,str);
	if (name[strlen(name)-1] == ']')
		str[strlen(name)-1] = 0;
	if (*name == '<') {
		strcpy(name,++str);
		if (str[strlen(name)-1] == '>')
			str[strlen(name)-1] = 0;
	}
	for (i=0; i<128; i++)
		if (name[i] == '=')
			name[i] = 0;
	for (i=0; configlist[i].devtype; i++) {
		if (!strcmp(name,configlist[i].unixname)) {
			if (!configlist[i].unixname[0])
				continue;
			return(i);
		}
	}
	for (i=0; configlist[i].devtype; i++) {
		if (!strncmp(name,configlist[i].unixname,strlen(name))) {
			if (!configlist[i].unixname[0])
				continue;
			cpos = i;
			if (match++)	/* found more than one */
				return(-2);
		}
	}
	return(cpos);
}

findname(str)
char *str;
{
	int i;
	int cpos;
	int match;

	match = 0;
	cpos = -1;
	for (i=0; configlist[i].devtype; i++)
		if (!strcmp(str,configlist[i].dosname))
			return(i);
	for (i=0; configlist[i].devtype; i++) {
		if (!strncmp(str,configlist[i].dosname,strlen(str))) {
			cpos = i;
			if (match++)	/* found more than one */
				return(-2);
		}
	}
	return(cpos);
}
		
replace(old,new)
char *old;
char *new;
{
	register i;
	register pos;
	register match;
	char suffix[128];

	if ((pos = findname(old)) < 0)
		return(pos);
	if (!strncmp(configlist[pos].dosname,new,strlen(new)))
		return(-1);
	if (new) {
		strcpy(suffix,&configlist[pos].dosname[strlen(old)]);
		strcpy(configlist[pos].dosname,new);
		strcat(configlist[pos].dosname,suffix);
	}
	return(pos);
}

rmdevice(str)
char *str;
{
	int i;
	int match;
	int dtype;
	char name[64];

	match = 0;
	strcpy(name,str);
	if (name[strlen(name)-1] == ']')
		str[strlen(name)-1] = 0;
	if (*name == '<') {
		strcpy(name,++str);
		if (str[strlen(name)-1] == '>')
			str[strlen(name)-1] = 0;
	}
	match = findconfig(name);
	if (match == -2)
		return(match);
	if (match == -1)
		match = findname(name);
	if (match < 0)		/* found more than one or none */
		return(match);
	i = match;
	dtype = configlist[i].devtype;
	for (; configlist[i+1].devtype; i++) 
		movedevice(&configlist[i],&configlist[i+1]);
	configlist[i].devtype = 0;
	return(dtype);
}

addevice(str,flag)
char *str;
int flag;			/* forces add even if already there */
{
	int i;
	int dtype;
	int insertslot;
	struct configlist tmpconf;

	insertslot = MAXDEV;
	parsedev(&tmpconf,str);
	if ((!flag) && tmpconf.unixname[0] == 0)
		return(-4);
	i = findconfig(tmpconf.unixname);
	if ((!flag) && (i >= 0 || i == -2))
		return(-2);
	for (i=0; configlist[i].devtype; i++) {
					/* if same type device, put in order  */
		if (configlist[i].devtype == tmpconf.devtype) {
	 	    if (configlist[i].unixname[strlen(tmpconf.unixname)-1]+1
			    == tmpconf.unixname[strlen(tmpconf.unixname)-1]) {
			insertslot = i + 1;
		    }
		}
	}
	if (i > MAXDEV)
		return(-3);
	configlist[i+1].devtype = 0;
	for (; insertslot < i; i--) {
		movedevice(&configlist[i],&configlist[i-1]);
	}
	movedevice(&configlist[i],&tmpconf);
	return(i);
}

parsedev(tconf,str)
struct configlist *tconf;
char *str;
{
	int i;
	int ntype;
	int lparen;

	tconf->devtype = C_MISC;
	tconf->accessed = 0;
	if (str[strlen(str)-1] == ']') {
		str[strlen(str)-1] = 0;
		tconf->devtype = C_PARAM;
	}
	if (*str == '<') {
		str++;
		if (str[strlen(str)-1] == '>')
			str[strlen(str)-1] = 0;
	}
	lparen = 0;
	for (i=0; i<UNIXNAMESIZE; i++) {
		if (!*str) {
			break;
		} else if (*str == '(') {
			lparen++;
		} else if (*str == '=' && lparen == 0) {
			str++;
			break;
		}
		tconf->unixname[i] = *str++;
	}
	tconf->unixname[i] = 0;
	for (i=0; i<DOSNAMESIZE; i++) {
		if (!*str)
			break;
		tconf->dosname[i] = *str++;
	}
	tconf->dosname[i] = 0;
	if (i == 0) {
		strcpy(tconf->dosname,tconf->unixname);
		tconf->unixname[0] = 0;
	}
	if (tconf->devtype != C_PARAM) {
		tconf->devtype =  typedev(tconf->unixname);
		if (tconf->devtype == C_PARAM)
			tconf->devtype = C_MISC;
	}
}

movedevice(tostruct,fromstruct)
struct configlist *tostruct;
struct configlist *fromstruct;
{
	strcpy(tostruct->dosname,fromstruct->dosname);
	strcpy(tostruct->unixname,fromstruct->unixname);
	tostruct->devtype = fromstruct->devtype;
	tostruct->accessed = fromstruct->accessed;
}
