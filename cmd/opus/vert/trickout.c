#include <stdio.h>
#include "vert.h"

char vbuf[16384];
char bigbuf[256000];

main(argc, argv)
int argc;
char **argv;
{
	int l, b, w, h;
	int fc, bc, cc, wm;
	int len;

	if (argc < 5) {
		fprintf(stderr, "usage: blkout l b w h\n");
		exit(1);
	}
	l = atoi(argv[1]);
	b = atoi(argv[2]);
	w = atoi(argv[3]);
	h = atoi(argv[4]);
	fc = atoi(argv[5]);
	bc = atoi(argv[6]);
	wm = atoi(argv[7]);
	cc = atoi(argv[8]);
	v_open(vbuf, 16384, 0, 0);
	v_readwritearea(l, b, l + w - 1, b + h - 1);
	v_movecolors(bc, fc);
	v_setwritingmode(wm, cc);
	len = read(0, bigbuf, 256000);
	if (len > 0) {
		v_write(bigbuf, len);
	}
	v_close();
}
