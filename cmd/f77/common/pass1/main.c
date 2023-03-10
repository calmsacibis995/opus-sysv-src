/*	@(#)main.c	1.5	*/

#include "defs"
#include <signal.h>
#include <a.out.h>


main(argc, argv)
int argc;
char **argv;
{
char *s;
int k, retcode, *ip;
FILEP opf();
int flovflo();

#define DONE(c)	{ retcode = c; goto finis; }

signal(SIGFPE, flovflo);  /* catch overflows */

--argc;
++argv;

while (argc > 0 && argv[0][0] == '-') {
	for(s = argv[0]+1 ; *s ; ++s) switch(*s) {
		case 'w':
			if(s[1]=='6' && s[2]=='6')
				{
				ftn66flag = YES;
				s += 2;
				}
			else
				nowarnflag = YES;
			break;

		case 'U':
			shiftcase = NO;
			break;

		case 'u':
			undeftype = YES;
			break;

		case 'O':
#if TARGET != OPUS
			optimflag = YES;
#endif
			if( isdigit(s[1]) )
				{
				k = *++s - '0';
				if(k > MAXREGVAR)
					{
					warn1("-O%d: too many register variables", (char *)k);
					maxregvar = MAXREGVAR;
					}
				else
					maxregvar = k;
				}
			break;

		case 'd':
			debugflag = YES;
			break;

		case 'e':
			exprflag = YES;
			break;

		case 'p':
			profileflag = YES;
			break;

		case 'C':
			checksubs = YES;
			break;

		case '6':
			no66flag = YES;
			noextflag = YES;
			break;

		case '1':
			onetripflag = YES;
			break;

#ifdef SDB
		case 'G':
			cdbflag = YES;
			break;
#endif

#ifdef SDB
		case 'g':
			sdbflag = YES;
			break;
#endif

#if TARGET == OPUS
		case 'x':
			xrefflag = YES;
			break;
#endif
#if clipper
		case 's':
			vaxalign();
			break;
#endif

		case 'N':
			switch(*++s)
				{
				case 'q':
					ip = &maxequiv; goto getnum;
				case 'x':
					ip = &maxext; goto getnum;
				case 's':
					ip = &maxstno; goto getnum;
				case 'c':
					ip = &maxctl; goto getnum;
				case 'n':
					ip = &maxhash; goto getnum;
				case 'l':
					ip = &maxlab; goto getnum;

				default:
					fatali("invalid flag -N%c", *s);
				}
		getnum:
			k = 0;
			while( isdigit(*++s) )
				k = 10*k + (*s - '0');
			if(k <= 0)
				fatal("Table size too small");
			*ip = k;
			break;

		case 'I':
			if(*++s == '2')
				tyint = TYSHORT;
			else if(*s == '4')
				{
				shortsubs = NO;
				tyint = TYLONG;
				}
			else if(*s == 's')
				shortsubs = YES;
			else
				fatali("invalid flag -I%c\n", *s);
			tylogical = tyint;
			break;

		default:
			fatali("invalid flag %c\n", *s);
		}
	--argc;
	++argv;
	}

if(argc != 4)
	fatali("arg count %d", argc);
asmfile  = opf(argv[1]);
initfile = opf(argv[2]);
#if TARGET == OPUS
if ( xrefflag )
{
	if ((xreffile = fopen("xref.out", "w")) == NULL)
		fatali("cannot open xref.out");
}
#endif

#ifdef FONEPASS
	if (!freopen(argv[3], "w", stdout))
	{
		perror(argv[3]);
		fatal("cannot reopen stdout");
		/* NOTREACHED */
	}
#else
textfile = opf(argv[3]);
#if TARGET == OPUS
if (!( textfile = freopen(argv[3], "r+", textfile)))
	fatalstr("cannot reopen intermediate file %s", argv[3]);
#endif
#endif /*FONEPASS*/

initkey();
if(inilex( copys(argv[0]) ))
	DONE(1);
fprintf(diagfile, "%s:\n", argv[0]);


#ifdef SDB
#	if TARGET == PDP11
	for(s = argv[0] ; ; s += 8)
	{
	prstab(s,N_SO,0,NULL);
	if( strlen(s) < 8 )
		break;
	}
#	else
	{
	if (cdbflag || sdbflag)		/* put out Complex and Dcomplex */
		{		/* structure definitions */
#if TARGET == OPUS && m88k
		p2pass("\ttext\n\tsdef\tComplex\n\tscl\t10\n\ttype\t010\n\tsize\t8\n\tendef");
		p2pass("\tsdef\treal\n\tval\t0\n\tscl\t8\n\ttype\t6\n\tendef");
		p2pass("\tsdef\timag\n\tval\t4\n\tscl\t8\n\ttype\t6\n\tendef");
		p2pass("\tsdef\t.eos\n\tval\t8\n\tscl\t102\n\ttag\tComplex\n\tendef");
		p2pass("\tsdef\tDcomplex\n\tscl\t10\n\ttype\t010\n\tsize\t16\n\tendef");
		p2pass("\tsdef\treal\n\tval\t0\n\tscl\t8\n\ttype\t7\n\tendef");
		p2pass("\tsdef\timag\n\tval\t8\n\tscl\t8\n\ttype\t7\n\tendef");
		p2pass("\tsdef\t.eos\n\tval\t16\n\tscl\t102\n\ttag\tDcomplex\n\tendef");
#else
		p2pass("\t.text\n\t.def\tComplex;\t.scl\t10;\t.type\t010;\t.size\t8;\t.endef");
		p2pass("\t.def\treal;\t.val\t0;\t.scl\t8;\t.type\t6;\t.endef");
		p2pass("\t.def\timag;\t.val\t4;\t.scl\t8;\t.type\t6;\t.endef");
		p2pass("\t.def\t.eos;\t.val\t8;\t.scl\t102;\t.tag\tComplex;\t.endef");
		p2pass("\t.def\tDcomplex;\t.scl\t10;\t.type\t010;\t.size\t16;\t.endef");
		p2pass("\t.def\treal;\t.val\t0;\t.scl\t8;\t.type\t7;\t.endef");
		p2pass("\t.def\timag;\t.val\t8;\t.scl\t8;\t.type\t7;\t.endef");
		p2pass("\t.def\t.eos;\t.val\t16;\t.scl\t102;\t.tag\tDcomplex;\t.endef");
#endif /* OPUS */
		}
	}
#	endif
#endif

fileinit(debugflag);
procinit();
if(k = yyparse())
	{
	fprintf(diagfile, "Bad parse, return code %d\n", k);
	DONE(1);
	}
if(nerr > 0)
	DONE(1);
if(parstate != OUTSIDE)
	{
	warn("missing END statement");
	endproc();
	}
doext();
preven(ALIDOUBLE);
prtail();
#if FAMILY==PCC
	puteof();
#endif

if(nerr > 0)
	DONE(1);
DONE(0);


finis:
	done(retcode);
	return(retcode);
}



done(k)
int k;
{
void exit();
static int recurs	= NO;

if(recurs == NO)
	{
	recurs = YES;
	clfiles();
	}
exit(k);
}


LOCAL FILEP opf(fn)
char *fn;
{
FILEP fp;
if( fp = fopen(fn, "a") )
	return(fp);

fatalstr("cannot open intermediate file %s", fn);
/* NOTREACHED */
}



LOCAL clfiles()
{
clf(&textfile);
clf(&asmfile);
clf(&initfile);
#if TARGET == OPUS
if ( xrefflag )
	clf(&xreffile);
#endif
}


clf(p)
FILEP *p;
{
if(p!=NULL && *p!=NULL && *p!=stdout)
	{
	if(ferror(*p))
		fatal("writing error");
	fclose(*p);
	}
*p = NULL;
}




flovflo()
{
err("floating exception during constant evaluation");
#if HERE == VAX
	fatal("vax cannot recover from floating exception");
	/* vax returns a reserved operand that generates
	   an illegal operand fault on next instruction,
	   which if ignored causes an infinite loop.
	*/
#endif
#if HERE == OPUS
	fatal("cpu cannot recover from floating exception");
#endif
signal(SIGFPE, flovflo);
}
