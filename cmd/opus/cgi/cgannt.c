#include "stdio.h"
#include "cgi.h"
#include "termio.h"
#include "signal.h"

typedef short	INT16;	/* Define signed 16 bit integer */

/* This is a program to use the CGI C interface to draw a Gannt chart */

INT16	savary51;
INT16	savary52;

struct termio 	cgitty;     

/* Catch interrupts from the keyboard while CREAD is disabled. Allows the */
/* program to be terminated using input from the keyboard.                */

catchsig()
{
	cgi_close();

	ioctl(0,TCGETA,&cgitty);   /* Re-enable console input */
	cgitty.c_cflag|=CREAD;     /* through OPMON.          */ 
	ioctl(0,TCSETA,&cgitty);
}

main()
{
	static char *tasks[] = {"Production", "Evaluation",
				"Plant", "Purchasing",
				"Design", "Review", "Design"};
	static char *title = {"Processing "};
	static char *y_label = {"As of June 15"};
	static char *y_ticks[] = {"July", "Aug", "Sept", "Oct"};
	static char buf[8192];

	extern box();
	extern INT16	fnxtr();
	extern INT16	fnytr();

	INT16	dev_handle,
		xheight,
		istring[2],
		gdms_err,
		xy[10],
		savary[66],
		xwidth,
		cwidth,
		i, j,
		cheight;


	static INT16	echo_xy[2] = {0,0};

	static INT16	nominate[] = {0, 1, 1, 3, 1, 1, 1, 0,
				      0, 1, 1, 'D', 'I', 'S',
				      'P', 'L', 'A', 'Y', ' '};

	static INT16	start_dates[] = {83, 72, 70, 48, 48, 45, 40};

	static INT16	end_dates[] = {95, 83, 79, 75, 67, 60, 49};

	signal(SIGINT,catchsig);    /* Enable signal trapping routine */
	signal(SIGQUIT,catchsig);
	signal(SIGSEGV,catchsig);
	signal(SIGTERM,catchsig);

	ioctl(0,TCGETA,&cgitty);    /* Turn CREAD off. Enables input */
	cgitty.c_cflag&=~CREAD;     /* to be passed to CGI bypassing */
	ioctl(0,TCSETA,&cgitty);    /* OPMON.                        */

	cgi_open(0,buf,8192,0);/* buffer output in 8192 byte buffer */

	gdms_err = v_opnwk(nominate,&dev_handle,savary);
					/* nominate the device */
	savary51 = savary[51];
	savary52 = savary[52];

	xy[1] = fnytr(10);		/* set the constants for the grid */
	xy[3] = fnytr(80);

	for (i=50; i<=80; i+=15) {
		xy[0] = fnxtr(i);	/* set variable elements in array */
		xy[2] = xy[0];		/*  for grid */
		gdms_err = v_pline(dev_handle,2,xy); /* draw the line */
	}

	xheight = vst_height(dev_handle,fnytr(4),&xwidth,&cwidth,&cheight);
					/* set character height */
	gdms_err = vst_alignment(dev_handle,1,2,&i,&j);
					/* set text alignment */

	j = 0;
					/* index into tick labels */
	for (i=43; i<=88; i+=15) {
		gdms_err = v_gtext(dev_handle,fnxtr(i),fnytr(10),y_ticks[j++]);
					/* write text */
	}

	gdms_err = vst_alignment(dev_handle,2,1,&i,&j);
					/* set text alignment */

	j = 0;
	for (i=15; i<=75; i+=10) {
		gdms_err = v_gtext(dev_handle,fnxtr(33),fnytr(i),tasks[j++]);
					/* write out text */
	}

	gdms_err = vst_alignment(dev_handle,0,0,&i,&j);
					/* set text alignment */
	gdms_err = v_gtext(dev_handle,fnxtr(35),fnytr(82),y_label);
					/* write out the y axis label */
	gdms_err = vst_height(dev_handle,fnytr(9),&xwidth,&cwidth,&cheight);
					/* set new character height */
	gdms_err = v_gtext(dev_handle,fnxtr(35),fnytr(88),title);
					/* write out title text */
	gdms_err = vsf_style(dev_handle,2);
	gdms_err = vsf_interior(dev_handle,3);
					/* set fill pattern */

	j = 0;
					/* set index into data arrays */
	for (i=12; i<=72; i+=10) {
		xy[1] = fnytr(i);
		xy[3] = fnytr(i+6);
		xy[0] = fnxtr(start_dates[j]);
		xy[2] = fnxtr(end_dates[j++]);
					/* set dimension for bars */
		gdms_err = v_bar(dev_handle,xy);
					/* draw the bars */
	}

	box(35,95,10,80,xy);		/* create box points */
	gdms_err = v_pline(dev_handle,5,xy);
					/* draw frame around chart */
	box(0,100,0,100,xy);		/* create page border */
	gdms_err = v_pline(dev_handle,5,xy);
					/* draw page border */
	cgi_flush();

	gdms_err = vrq_string(dev_handle,2,0,echo_xy,istring); 
					/* wait for <CR> */
	gdms_err = v_enter_cur(dev_handle);
	gdms_err = v_clswk(dev_handle);

	cgi_close();

	ioctl(0,TCGETA,&cgitty);   /* Re-enable console input */
	cgitty.c_cflag|=CREAD;     /* through OPMON.          */ 
	ioctl(0,TCSETA,&cgitty);
}

box(xmin,xmax,ymin,ymax,xyout)
INT16	xmin, xmax, ymin, ymax, xyout[];
{
	extern INT16 fnxtr();
	extern INT16 fnytr();

	xyout[0] = fnxtr(xmin);
	xyout[1] = fnytr(ymin);

	xyout[2] = fnxtr(xmax);
	xyout[3] = xyout[1];

	xyout[4] = xyout[2];
	xyout[5] = fnytr(ymax);

	xyout[6] = xyout[0];
	xyout[7] = xyout[5];

	xyout[8] = xyout[0];
	xyout[9] = xyout[1];
}

INT16 fnxtr(percent)
INT16 percent;
{
	return((float) percent / 100.0 * savary51);
}

INT16 fnytr(percent)
INT16 percent;
{
	return((float) percent / 100.0 * savary52);
}
