/*#define LD    "/usr/new/lx32"         /* loader */
/*#define CPP   "/lib/cpp"              /* C preprocessor for fortran */
/*#define STD   "/usr/newlib"           /* directory for fcom,ccom,pcom */
/*#define AS    "/usr/new/mx32"         /* assembler */
/*#define M4    "/usr/bin/m4"           /* M4 macro preprocessor for fortran */
/*#define EFL   "/usr/bin/efl"          /* efl preprocessor for fortran */
/*#define RATFOR "/usr/bin/ratfor"      /* ratfor preprocessor for fortran */
/*#define LIB   "/usr/newlib"           /* standard library prefix */
/*#define TMP   "/tmp"                  /* place for temporary files */
/*#define INCLUDE "/usr/include"        /* where to find include files */
/*#define AINCLUDE "/usr/include/ansi"  /* where to find ansi include files */
/*#define GENIX 1                       /* defined if Genix target */
/*#define C     1                       /* defined if C */
/*#undef  ANSI                          /* defined if ANSI c is the default */
/*#undef  PASCAL                        /* defined if Pascal */
/*#undef  FORTRAN                       /* defined if Fortran */
/*#undef  AVALON                        /* defined if Avalon target */

#include <stdio.h>
#include <signal.h>
/*
	-UU
	-t[PCF]
	-v: gotv: version number of compiler
	Pascal switches
	Green Hills C switches
	Use /tmp for .s file
	check for no M4, EFL, CPP, RATFOR
*/
#define true 1
#define false 0
char **dellist, **delptr;
char noexec;
char gotc;
char gotS;
char gotL;
char gotm;
char gotv;
char gotF;
char gotE;
char gotp;
char gotpg;
char goterror;
char *onlyofile;

char *copy(s)
{
    char *s1;

    s1 = (char *)calloc(strlen(s)+1, sizeof(char));
    strcpy(s1, s);
    return (s1);
}

char *concat(s1, s2)
char *s1;
char *s2;
{
char *ret;
    ret = (char *)calloc(strlen(s1)+strlen(s2)+1, sizeof(char));
    strcpy(ret, s1);
    strcat(ret, s2);
    return(ret);
}

char *makeshort(s, c)
char *s;
char c;
{
    char *s1;

    s1 = s + strlen(s) - 1;
    while (s1[-1] != '.')
	s1--;
    *s1++ = c;
    *s1 = '\0';
    while (*s1 != '/' && s1 >= s)
	s1--;
    return(s1 + 1);
}

main(argc, argv, envp)
char **argv;
{
char **args,**argp;
char **ldargs, **ldargp;
char **fcomargs, **fcomargp;
char **ccomargs, **ccomargp;
char **pcomargs, **pcomargp;
char **plmcomargs, **plmcomargp;
char **asargs, **asargp;
char **m4args, **m4argp;
char **eflargs, **eflargp;
char **cppargs, **cppargp;
char **ratforargs, **ratforargp;
char **flist, **fptr;
char **liblist, **libptr;
char **x;
char *temp;
char *prog;
int count = 0;
int ocount = 0;
char *fcom;
char *ccom;
char *pcom;
char *plmcom;
int status;
char *comm;
#ifdef ANSI
char ansi=1;
#else
char ansi=0;
#endif

    noexec = false;
    gotc=0;
    gotS=0;
    gotL=0;
    gotm=0;
    gotv=0;
    gotF=0;
    gotE=0;
    gotp=0;
    gotpg=0;
    goterror = 0;
    onlyofile = 0;
    ldargp = ldargs = (char **)calloc(argc+30, sizeof(char *));
    fcomargp = fcomargs = (char **)calloc(argc+20, sizeof(char *));
    ccomargp = ccomargs = (char **)calloc(argc+20, sizeof(char *));
    pcomargp = pcomargs = (char **)calloc(argc+20, sizeof(char *));
    plmcomargp = plmcomargs = (char **)calloc(argc+20, sizeof(char *));
    asargp = asargs = (char **)calloc(argc+20, sizeof(char *));
    cppargp = cppargs = (char **)calloc(argc+20, sizeof(char *));
    m4argp = m4args = (char **)calloc(argc+20, sizeof(char *));
    eflargp = eflargs = (char **)calloc(argc+20, sizeof(char *));
    ratforargp = ratforargs = (char **)calloc(argc+20, sizeof(char *));
    fptr = flist = (char **)calloc(argc+20, sizeof(char *));
    delptr = dellist = (char **)calloc(argc+20, sizeof(char *));
    libptr = liblist = (char **)calloc(argc+20, sizeof(char *));
    *asargp++ = "as";
    *ldargp++ = "ld";

#ifndef PROCESSOR
WHEN YOU COMPILE THIS YOU WILL GET AN ERROR.
You must add the line "debugflags[dbXXXXX] := true", where XXXXX is the name of
your target machine, at the top of the routine initcod in the code generator
initialization source module.  The setting of this debugflag used to be done
by this program.  It no longer does it, so you must do it in the compiler.
#endif

    *fcomargp++ = concat("fcom", PROCESSOR);
    *pcomargp++ = concat("pcom", PROCESSOR);
    *plmcomargp++ = concat("plmcom", PROCESSOR);
    *ccomargp++ = concat("ccom", PROCESSOR);

#ifdef AVALONAMD
#ifdef AMD
    *asargp++ = "-a";
    *ldargp++ = "-T";
    *ldargp++ = "a8";
#endif
#endif
#ifdef NS32000
#ifndef GENIX
    *ccomargp++ = "-X85";
    *pcomargp++ = "-X85";
    *plmcomargp++ = "-X85";
    *fcomargp++ = "-X85";
#endif
    *ccomargp++ = "-X88";
    *pcomargp++ = "-X88";
    *plmcomargp++ = "-X88";
    *fcomargp++ = "-X88";
#ifdef OPUS
    *ccomargp++ = "-X72";
    *pcomargp++ = "-X72";
    *plmcomargp++ = "-X72";
    *fcomargp++ = "-X72";
    *ccomargp++ = "-Dn16";
#endif
#ifndef AVALON
    *ccomargp++ = "-X50";
    *pcomargp++ = "-X50";
    *fcomargp++ = "-X50";
    *plmcomargp++ = "-X50";
#endif
#ifdef GENIX
    *ccomargp++ = "-X51";
    *pcomargp++ = "-X51";
    *plmcomargp++ = "-X51";
    *fcomargp++ = "-X51";
    *ccomargp++ = "-X52";
    *pcomargp++ = "-X52";
    *plmcomargp++ = "-X52";
    *fcomargp++ = "-X52";
#endif
#ifndef AMD
#ifdef AVALON
    *asargp++ = "-a";
#ifndef FOREIGN
    *ldargp++ = "-T";
    *ldargp++ = "400";
#endif
    *ldargp++ = "-X";
#endif
#endif
#endif
    fcom = concat(concat(STD, "/fcom"), PROCESSOR);
    pcom = concat(concat(STD, "/pcom"), PROCESSOR);
    plmcom = concat(concat(STD, "/plmcom"), PROCESSOR);
    ccom = concat(concat(STD, "/ccom"), PROCESSOR);
#ifdef SYSV
    *ccomargp++ = "-X74";
    *pcomargp++ = "-X74";
    *fcomargp++ = "-X74";
    *plmcomargp++ = "-X74";
#endif
#ifdef SINGLELIB
    *fcomargp++ = "-X71";
#endif
#ifdef EXTRA1
    *ccomargp++ = EXTRA1;
    *pcomargp++ = EXTRA1;
    *plmcomargp++ = EXTRA1;
    *fcomargp++ = EXTRA1;
#endif
#ifdef EXTRA2
    *ccomargp++ = EXTRA2;
    *pcomargp++ = EXTRA2;
    *plmcomargp++ = EXTRA2;
    *fcomargp++ = EXTRA2;
#endif
#ifdef EXTRA3
    *ccomargp++ = EXTRA3;
    *pcomargp++ = EXTRA3;
    *plmcomargp++ = EXTRA3;
    *fcomargp++ = EXTRA3;
#endif
#ifdef EXTRA4
    *ccomargp++ = EXTRA4;
    *pcomargp++ = EXTRA4;
    *plmcomargp++ = EXTRA4;
    *fcomargp++ = EXTRA4;
#endif
#ifdef EXTRA5
    *ccomargp++ = EXTRA5;
    *pcomargp++ = EXTRA5;
    *plmcomargp++ = EXTRA5;
    *fcomargp++ = EXTRA5;
#endif
#ifdef EXTRA6
    *ccomargp++ = EXTRA6;
    *pcomargp++ = EXTRA6;
    *plmcomargp++ = EXTRA6;
    *fcomargp++ = EXTRA6;
#endif
    *cppargp++ = "cpp";
    *cppargp++ = "-P";
    *m4argp++ = "m4";
    *eflargp++ = "efl";
    *ratforargp++ = "ratfor";
    for (argp = argv+1; argp < argv+argc; argp++)
	if ((*argp)[0] == '-')
	    switch((*argp)[1]) {
		case 'c':
		case 'S':
		case 'F':
		case 'm':
		case 'L':
		    if (strcmp(*argp, "-c") == 0) {
			gotc = true;
		    } else if (strcmp(*argp, "-S") == 0) {
			gotS = true;
		    } else if (strcmp(*argp, "-L") == 0) {
			gotL = true;
#ifdef FORTRAN
		    } else if (strcmp(*argp, "-F") == 0) {
			gotF = true;
		    } else if (strcmp(*argp, "-m") == 0) {
			gotm = true;
#endif
		    } else
			goto def;
		    break;
		case 'o':
#ifdef FORTRAN
		    if (strcmp(*argp, "-onetrip") == 0)
			goto fortranflag;
		    else
#endif
		    if (strcmp(*argp, "-o") == 0) {
			if (*(argp+1)) {
			    *ldargp++ = *argp++;
			    if ((*argp)[strlen(*argp)-2] == '.')
				switch ((*argp)[strlen(*argp)-1]) {
				case 'f': case 's': case 'c': case 'p':
				case 'F': case 'r': case 'e': case 'o':
				    printf("%s: -o would overwrite %s\n",
					   argv[0], *argp);
				    localexit(1);
				}
			    else if (strcmp(*argp+strlen(*argp)-4, ".plm")==0) {
				printf("%s: -o would overwrite %s\n",
				       argv[0], *argp);
				localexit(1);
			    }
			    *ldargp++ = *argp;
			}
		    } else
			goto def;
		    break;
		case 'v':
		    if (strcmp(*argp, "-v") == 0) {
			gotv = true;
#ifdef AVALON
			*ldargp++ = *argp;
#endif
		    } else
			goto def;
		    break;
		case 'A': case 'a':
		    if (strcmp(*argp,"-ANSI")== 0||strcmp(*argp,"-ansi")==0)
			ansi=1;
		    break;
		case 'K': case 'k': case 'n':
		    if (strcmp(*argp,"-K+R")== 0 || strcmp(*argp,"-K&R")==0 ||
			strcmp(*argp,"-k+r")== 0 || strcmp(*argp,"-k&r")==0 ||
			strcmp(*argp,"-noansi")== 0 )
			ansi=0;
		    break;
		case 'U':
#ifdef FORTRAN
		    if (strcmp(*argp, "-U") == 0)
			goto fortranflag;
#endif
		case 'D':
		    *fcomargp++ = *argp;
		case 'I':
		    *pcomargp++ = *argp;
		    *plmcomargp++ = *argp;
		    *ccomargp++ = *argp;
		    *cppargp++ = *argp;
		    break;
		case 'l':
		    *libptr++ = (*argp)+2;
		    break;
		case 'w':
#ifdef FORTRAN
		    if (strcmp(*argp, "-w66") == 0)
			goto fortranflag;
#endif
		case 'O':
		case 'g':
		case 'p':
		case 'X':
		case 'Z':
generalflag:
		    if (strcmp(*argp, "-go") == 0)
/* until 4.2 symbols supported by all compilers */
			*argp = "-g";
		    else if (strcmp(*argp, "-p") == 0)
			gotp = true;
		    else if (strcmp(*argp, "-pg") == 0)
			gotpg = true;
		    else if (strcmp(*argp, "-R") == 0 || ((*argp)[1] == 'X') ||
				((*argp)[1] == 'Z') ||
			strcmp(*argp, "-ga") == 0 || strcmp(*argp, "-g") == 0 ||
			strcmp(*argp, "-O") == 0 || strcmp(*argp, "-OL") == 0 ||
			strcmp(*argp, "-OM") == 0 || strcmp(*argp, "-O2") == 0 ||
			strcmp(*argp, "-OLM") == 0 || strcmp(*argp, "-OML") == 0 ||
			strcmp(*argp, "-w") == 0)
			;
		    else
			goto def;
		    if (strcmp(*argp, "-g") == 0)
			*libptr++ = "g";
		    *fcomargp++ = *argp;
		    *ccomargp++ = *argp;
		    *pcomargp++ = *argp;
		    *plmcomargp++ = *argp;
		    break;
		case 'C':
		    if (strcmp(*argp, "-C") == 0) {
			*ccomargp++ = *argp;
#ifdef PASCAL
			*pcomargp++ = "-X57";
#endif
		    }
		case 'i':
		case 'u':
#ifdef FORTRAN
fortranflag:
		    if (strcmp(*argp, "-u") == 0 || strcmp(*argp, "-U") == 0 ||
			strcmp(*argp, "-onetrip") == 0 ||
			strcmp(*argp, "-C") == 0 || strcmp(*argp, "-i4") == 0 ||
			strcmp(*argp, "-i2") == 0 || strcmp(*argp, "-w66") == 0)
			*fcomargp++ = *argp;
		    else
#endif
			goto def;
		    break;
		/* -T option truncates identifiers to 8 chars (for m88k) */
		case 'T':
		    *ccomargp++ = *argp;
		    break;
		case 't':
#ifdef FORTRAN
		case 'N':
#endif
		    break;
		case 'E':
		    if (strcmp(*argp, "-E") == 0) {
			gotE = true;
			*ccomargp++ = *argp;
		    }
#ifdef FORTRAN
		    else {
			(*argp)[1] = '-';
			*eflargp++ = (*argp)+1;
		    }
#endif
		    break;
		case 'B':
		    if (strcmp(*argp, "-B") == 0) {
			ccom = concat(concat(STD, "/occom"), PROCESSOR);
			fcom = concat(concat(STD, "/ofcom"), PROCESSOR);
			pcom = concat(concat(STD, "/opcom"), PROCESSOR);
			plmcom = concat(concat(STD, "/oplmcom"), PROCESSOR);
		    } else {
			ccom = concat(concat((*argp)+2, "/ccom"), PROCESSOR);
			fcom = concat(concat((*argp)+2, "/fcom"), PROCESSOR);
			pcom = concat(concat((*argp)+2, "/pcom"), PROCESSOR);
			plmcom = concat(concat((*argp)+2, "/plmcom"),PROCESSOR);
		    }
		    break;
		case 'R':
		    if (strcmp(*argp, "-R") == 0) {
#ifdef AVALON
			*asargp++ = *argp;
#endif
#ifdef VAX
			*asargp++ = *argp;
#endif
			goto generalflag;
		    }
		    (*argp)[1] = '-';
		    *ratforargp++ = (*argp)+1;
		    break;
		case '#':
		    noexec = true;
		    gotv = true;
		    break;
		case 's':
#ifdef PASCAL
		    if (strcmp(*argp, "-s") == 0) {
			*pcomargp++ = "-X56";
			*pcomargp++ = "-X57";
			break;
		    }
#endif
		default:
def:
		    *fptr++ = *argp;
		    break;
	    }
	else if (strcmp(*argp+strlen(*argp)-4, ".plm") == 0) {
	    count++;
	    ocount++;
	    *fptr++ = *argp;
	} else if ((*argp)[strlen(*argp)-2] == '.')
	    switch ((*argp)[strlen(*argp)-1]) {
		case 'f': case 's': case 'c': case 'p': case 'F':
		case 'r': case 'e':
		    count++;
		case 'o':
		    ocount++;
		default:
		    *fptr++ = *argp;
		    break;
	    }
	else
	    *fptr++ = *argp;
#ifdef AINCLUDE
    if ( ansi )
	*ccomargp++ = concat("-I", AINCLUDE);
#endif
#ifdef INCLUDE
    *ccomargp++ = concat("-I", INCLUDE);
#endif
    if ( ansi )
	*ccomargp++ = "-X153";
#ifdef LIB
#define NEWLIB LIB
#else
#define NEWLIB "/lib"
#endif
    if (gotp)
	*ldargp++ = concat(NEWLIB, "/mcrt0.o");
    else if (gotpg)
	*ldargp++ = concat(NEWLIB, "/gcrt0.o");
    else
	*ldargp++ = concat(NEWLIB, "/crt0.o");
#ifdef LDBUG
    *ldargp++ = concat("/lib/default.ld", *libptr);
#endif
    for (fptr = flist; *fptr; fptr++) {
#ifdef PLM
	if (strcmp(*fptr+strlen(*fptr)-4, ".plm") == 0) {
	    if (gotE)
		break;
	    if (count > 1)
		printf("%s:\n", *fptr);
	    prog = plmcom;
	    args = plmcomargs;
	    argp = plmcomargp;
	    goto allcomp;
	} else
#endif
	if ((*fptr)[strlen(*fptr)-2] != '.')
	    *ldargp++ = *fptr;
	else {
	    switch((*fptr)[strlen(*fptr)-1]) {
#ifdef FORTRAN
		case 'e':
		    if (gotE)
			break;
		    prog = EFL;
		    argp = eflargp;
		    args = eflargs;
		    goto bothp;
		case 'r':
		    if (gotE)
			break;
		    prog = RATFOR;
		    argp = ratforargp;
		    args = ratforargs;
    bothp:
		    temp = copy(*fptr);
		    *fptr = makeshort(*fptr, 'f');
		    comm = (char *)calloc(strlen(M4)+strlen(temp)+strlen(prog)+
					  strlen(*fptr)+10, sizeof(char));
		    if (gotm)
			sprintf(comm, "%s %s | %s > %s", M4, temp, prog, *fptr);
		    else
			sprintf(comm, "%s %s > %s", prog, temp, *fptr);
		    if (system(comm)) {
			goterror = true;
			break;
		    }
		    goto dotf;
		case 'F':
		    if (gotE)
			break;
		    cppargp[0] = copy(*fptr);
		    *fptr = makeshort(*fptr, 'f');
		    cppargp[1] = *fptr;
		    if (!gotF)
			*delptr++ = copy(*fptr);
		    if (call(CPP, cppargs, envp) != 0)
			break;
		    goto dotf;
		case 'f':
		    if (gotE)
			break;
    dotf:
		    printf("%s:\n", *fptr);
		    prog = fcom;
		    args = fcomargs;
		    argp = fcomargp;
		    goto allcomp;
#endif
		case 'c':
		    if (count > 1)
			printf("%s:\n", *fptr);
		    prog = ccom;
		    args = ccomargs;
		    argp = ccomargp;
		    goto allcomp;
#ifdef PASCAL
		case 'p':
		    if (gotE)
			break;
		    if (count > 1)
			printf("%s:\n", *fptr);
		    prog = pcom;
		    args = pcomargs;
		    argp = pcomargp;
		    goto allcomp;
#endif
    allcomp:
		    if (gotF)
			break;
		    *argp++ = *fptr;
#ifdef DIRECTOBJ
		    if (!gotS)
			*argp++ = "-X75";
#endif
		    if (call(prog, args, envp) != 0)
			break;
		    if (gotE)
			break;
#ifdef DIRECTOBJ
		    if (!gotS) {
			*fptr = makeshort(*fptr, 'o');
			if (ocount == 1 && !gotc)
			    onlyofile = copy(*fptr);
			goto alldef;
		    }
#endif
		    *fptr = makeshort(*fptr, 's');
		    if (!gotS && !gotL)
			*delptr++ = copy(*fptr);
		    goto dots;
		case 's':
		    if (gotE || gotS)
			break;
		    if (count > 1)
			printf("%s:\n", *fptr);
dots:
		    if (gotE || gotS)
			break;
		    asargp[0] = copy(*fptr);
		    *fptr = makeshort(*fptr, 'o');
#ifndef ACP
#ifndef AMD
		    asargp[1] = "-o";
		    asargp[2] = *fptr;
#endif
#endif
		    if (ocount == 1 && !gotc)
			onlyofile = copy(*fptr);
#ifdef AS_SHELL
if(callsys(AS, asargs, envp) !=0) {
#else
		    if (call(AS, asargs, envp) != 0) {
#endif
			if (strcmp(asargp[0], delptr[-1]) == 0)
			    *--delptr = 0;
			break;
		    }
		default:
alldef:
		    for (x = ldargs + 1; x <= ldargp; x++)
			if (*x != NULL)
			    if (strcmp(*x, *fptr) == 0)
				goto brk;
		    *ldargp++ = *fptr;
brk:
		    break;
	    }
	}
    }
#ifdef N10
    *ldargp++ = "-e";       /* Until Ross gets a default start addr in the loader */
    *ldargp++ = "_start";
#endif
#ifdef FORTRAN
#ifdef GHSLIB
    *libptr++ = "f";
#else
    *libptr++ = "F77";
    *libptr++ = "I77";
    *libptr++ = "U77";
#ifndef SYSV
    *libptr++ = "termcap";
#endif
#endif
#endif
#ifdef PASCAL
    *libptr++ = "p";
#endif
#ifdef FORTRAN
#ifdef SYSV
    *libptr++ = "m2";
#endif
    *libptr++ = "m";
#else
#ifdef PASCAL
    *libptr++ = "m";
#endif
#endif
#ifdef PLM
    *libptr++ = "plm";
#endif
    if ( ansi )
	*libptr++ = "ansi";
    *libptr++ = "c";
    for (libptr = liblist; *libptr; libptr++)
#ifndef LIB
#ifdef GENIX
	if (strcmp(*libptr, "I66") == 0)
	    *ldargp++ = "/usr/lib/libI66.a";
	else
#endif
#ifdef  SYSV
/* Needed for System V profiling libraries */
	    if (gotp || gotpg)  {
		if ((strcmp(*libptr, "c") == 0) ||
		    (strcmp(*libptr, "m") == 0) ||
		    (strcmp(*libptr, "malloc") == 0))
			*ldargp++ =concat(concat("/lib/libp/lib",*libptr),".a");
	    } else
#endif
		*ldargp++ = concat("-l", *libptr);
#else
	if (gotp || gotpg)
	    *ldargp++ = concat(concat(concat(LIB, "/lib"), *libptr), "_p.a");
	else
	    *ldargp++ = concat(concat(concat(LIB, "/lib"), *libptr), ".a");
#endif
    if (/*ocount > 0 &&*/ !gotF && !gotS && !gotc && !gotE && !goterror) {
	goterror = call(LD, ldargs, envp);
	if (goterror)
	    onlyofile = NULL;
    }
    localexit(goterror);
}

/*
 * Like call(), but use a system() call.  I needed this because on the
 * 88000 Motorola uses a shell script for /bin/as, and I can't exec a
 * shell script.
 */
callsys(prog, args, envp)
char *prog;
char **args;
char **envp;
{
    int status;
    char **argp;
    char str[200];
    char *ptr;

    if (gotv) {
	printf("%s", prog);
	for (argp = args+1; *argp; argp++)
	    printf(" %s", *argp);
	printf("\n");
    }
    if ( noexec ) return(0);
    strcpy(str, prog);
    ptr = str + strlen(prog);
    for (argp = args+1; *argp; argp++) {
	sprintf(ptr, " %s", *argp);
	ptr += strlen(*argp) + 1;
    }
    status = system(str);
    return(status);
}

call(prog, args, envp)
char *prog;
char **args;
char **envp;
{
    int status;
    int pid;
    char **argp;

    if (gotv) {
	printf("%s", prog);
	for (argp = args+1; *argp; argp++)
	    printf(" %s", *argp);
	printf("\n");
    }
    if ( noexec ) return(0);
#ifdef VFORK
    pid = vfork();
#else
    pid = fork();
#endif
    if (pid == -1) {
	perror("fork");
	localexit(1);
    } else if (pid == 0) {
	execve(prog, args, envp);
	perror(prog);
	localexit(1);
    } else {
	while (pid != wait(&status)) ;
	if ((status & 0xff) != 0) {
	    if ((status & 0xff) != 2 && (status & 0xff) != 9)
		fprintf(stderr, "Fatal error in %s\n", prog);
	    localexit(1);
	}
	status >>= 8;
	if (status)
	    goterror = true;
	return(status);
    }
}

localexit(status)
{
    if (gotv)
	if (*dellist || onlyofile)
	    printf("rm");
    for (delptr = dellist; *delptr; delptr++) {
	if (gotv)
	    printf(" %s", *delptr);
	unlink(*delptr);
    }
    if (onlyofile) {
	if (gotv)
	    printf(" %s", onlyofile);
	unlink(onlyofile);
    }
    if (gotv)
	if (*dellist || onlyofile)
	    printf("\n");
    exit(status);
}
