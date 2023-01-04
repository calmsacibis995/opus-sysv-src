/* SID @(#)pgc.h	1.11 */

/*
 * Header file for pgc library.
 */

/* external declarations */

extern unsigned char *pgc_bufstart;
extern unsigned char *pgc_bufptr;
extern unsigned char *pgc_bufend;

/* misc defines */

#define PGC_DEFAULT 1024		/* default buffer size */
#define PGC_DEVNAME "/dev/graph"	/* graphics file name */
#define FLT(a,b) (((a) << 16) + (b))  /* construct pgc type float */
#define PGC_CHK(n) (((pgc_bufptr + (n)) > pgc_bufend) ? pgc_bufwrite(n) : 0)
#define PGC_OUTB(b) (*pgc_bufptr++ = (b))
#define PGC_OUTW(w) (*pgc_bufptr++ = (w), *pgc_bufptr++ = (w) >> 8)
#define PGC_OUTF(w) (*pgc_bufptr++ = (w) >> 16,\
		     *pgc_bufptr++ = (w) >> 24,\
		     *pgc_bufptr++ = (w),\
		     *pgc_bufptr++ = (w) >> 8)
#define PGC_OUTBA(p, n) (memcpy(pgc_bufptr, p, n), pgc_bufptr += (n))
#define PGC_OUTWA(p, n) (memcpy(pgc_bufptr, p, n * 2),\
			  swab(pgc_bufptr, pgc_bufptr, (n) * 2),\
			  pgc_bufptr += (n) * 2)
#define PGC_OUTFA(p, n) (memcpy(pgc_bufptr, p, (n) * 4),\
			  swab(pgc_bufptr, pgc_bufptr, (n) * 4),\
			  pgc_bufptr += (n) * 4)

#define PGC_CMD(n, c) (PGC_CHK(n), PGC_OUTB(c))

#define PGC_1B(c, a) (PGC_CMD(2, c), PGC_OUTB(a))
#define PGC_2B(c, a, b) (PGC_CMD(3, c), PGC_OUTB(a), PGC_OUTB(b))
#define PGC_3B(c, a, b, z) (PGC_CMD(4, c), PGC_OUTB(a), PGC_OUTB(b), PGC_OUTB(z))
#define PGC_4B(c, a, b, x, y) (PGC_CMD(5, c), PGC_OUTB(a), PGC_OUTB(b),\
											  PGC_OUTB(x), PGC_OUTB(z))

#define PGC_1W(c, a) (PGC_CMD(3, c), PGC_OUTW(a))
#define PGC_2W(c, a, b) (PGC_CMD(5, c), PGC_OUTW(a), PGC_OUTW(b))
#define PGC_3W(c, a, b, z) (PGC_CMD(7, c), PGC_OUTW(a), PGC_OUTW(b), PGC_OUTW(z))
#define PGC_4W(c, a, b, x, y) (PGC_CMD(9, c), PGC_OUTW(a), PGC_OUTW(b),\
											  PGC_OUTW(x), PGC_OUTW(z))

#define PGC_1F(c, a) (PGC_CMD(5, c), PGC_OUTF(a))
#define PGC_2F(c, a, b) (PGC_CMD(9, c), PGC_OUTF(a), PGC_OUTF(b))
#define PGC_3F(c, a, b, z) (PGC_CMD(13, c), PGC_OUTF(a), PGC_OUTF(b), PGC_OUTF(z))
#define PGC_4F(c, a, b, x, y) (PGC_CMD(17, c), PGC_OUTF(a), PGC_OUTF(b),\
											  PGC_OUTF(x), PGC_OUTF(z))

/* commands */

#define ARC(radius, deg0, deg1) (\
	PGC_CMD(9, 0x3c),\
	PGC_OUTF(radius),\
	PGC_OUTW(deg0),\
	PGC_OUTW(deg1))

#define AREA() (\
	PGC_CMD(1, 0xc0))

#define AREABC(bcolor) (\
	PGC_1B(0xc1, bcolor))

#define AREAPT(pattern) (\
	PGC_CMD(33, 0xe7),\
	PGC_OUTWA(pattern, 16))

#define BKVMOV(x1, y1, x2, y2, xd, yd) (\
	PGC_CMD(25, 0xdd),\
	PGC_OUTF(x1),\
	PGC_OUTF(y1),\
	PGC_OUTF(x2),\
	PGC_OUTF(y2),\
	PGC_OUTF(xd),\
	PGC_OUTF(yd))

#define BLINK(num) (\
	PGC_1B(0xe5, num))

#define BLKMOV(x1, y1, x2, y2, xd, yd) (\
	PGC_CMD(13, 0xdf),\
	PGC_OUTW(x1),\
	PGC_OUTW(y1),\
	PGC_OUTW(x2),\
	PGC_OUTW(y2),\
	PGC_OUTW(xd),\
	PGC_OUTW(yd))

#define BUFFER(mb, db) (\
	PGC_2B(0x4f, mb, db))

#define BUTRD(opt) (\
	PGC_1B(0x54, opt))

#define BUTTBL(b, c) (\
	PGC_2B(0x79, b, c))

#define BUTTON(b) (\
	PGC_1B(0x78, b))

#define CA() (\
	PGC_CMD(3, 0x43),\
	PGC_OUTB(0x41),\
	PGC_OUTB(0x0a))

#define CIRCLE(radius) (\
	PGC_CMD(5, 0x38),\
	PGC_OUTF(radius))

#define CLBEG(clist) (\
	PGC_1B(0x70, clist))

#define CLDEL(clist) (\
	PGC_1B(0x74, clist))

#define CLEARS(color) (\
	PGC_1B(0x0f, color))

#define CLEND() (\
	PGC_CMD(1, 0x71))

#define CLIPH(flag) (\
	PGC_1B(0xaa, flag))

#define CLIPY(flag) (\
	PGC_1B(0xab, flag))

#define CLOOP(clist, count) (\
	PGC_CMD(4, 0x73),\
	PGC_OUTB(clist),\
	PGC_OUTW(count))

#define CLRD(clist) (\
	PGC_1B(0x75, clist))

#define CLRUN(clist) (\
	PGC_1B(0x72, clist))

#define COLOR(color) (\
	PGC_1B(0x06, color))

#define CONVPV(x, y) (\
	PGC_CMD(5, 0xad),\
	PGC_OUTW(x),\
	PGC_OUTW(y))

#define CONVRT() (\
	PGC_CMD(1, 0xaf))

#define CONVVP(x, y) (\
	PGC_2F(0xae, x, y))

#define CX() (\
	PGC_2B(0x43, 0x58, 0x0a))

#define DISPLA(flag) (\
	PGC_1B(0xd0, flag))

#define DISTAN(dist) (\
	PGC_1F(0xb1, dist))

#define DISTH(dist) (\
	PGC_1F(dist))

#define DISTY(dist) (\
	PGC_1F(dist))

#define DRAW(x, y) (\
	PGC_2F(0x28, x, y))

#define DRAW3(x, y, z) (\
	PGC_3F(0x2a, x, y, z))

#define DRAWR(x, y) (\
	PGC_2F(0x29, x, y))

#define DRAWR3(x, y, z) (\
	PGC_3F(0x2b, x, y, z))

#define ELIPSE(xr, yr) (\
	PGC_2F(0x39, xr, yr))

#define FILMSK(mask) (\
	PGC_1B(0xef, mask))

#define FLAGRD(flag) (\
	PGC_1B(0x51, flag))

#define FLOOD(color) (\
	PGC_1B(0x07, color))

#define IMAGER(line, x1, x2) (\
	PGC_CMD(7, 0xd8), \
	PGC_OUTW(line), \
	PGC_OUTW(x1), \
	PGC_OUTW(x2))

#define IMAGEW(line, x1, x2, pixels, n) (\
	PGC_CMD(7 + (n), 0xd9), \
	PGC_OUTW(line), \
	PGC_OUTW(x1), \
	PGC_OUTW(x2), \
	PGC_OUTBA(pixels, n))

#define IMGSIZ(x, y, d, b) (\
	PGC_CMD(7, 0x4e),\
	PGC_OUTW(x),\
	PGC_OUTW(y),\
	PGC_OUTB(d),\
	PGC_OUTB(b))

#define IPREC(flag) (\
	PGC_1B(0xe4, flag))

#define LINFUN(fun) (\
	PGC_1B(0xeb, fun))

#define LINPAT(pat) (\
	PGC_1B(0xea, pat))

#define LOCCUR() (\
	PGC_CMD(1, 0x1e))

#define LOCMAP(opt, x1, x2, y1, y2) (\
	(opt == 1) ? (\
	PGC_CMD(10, 0xb4),\
	PGC_OUTB(opt),\
	PGC_OUTW(x1),\
	PGC_OUTW(x2),\
	PGC_OUTW(y1),\
	PGC_OUTW(y2)) : (\
	PGC_CMD(18, 0xb4),\
	PGC_OUTB(opt),\
	PGC_OUTF(x1),\
	PGC_OUTF(x2),\
	PGC_OUTF(y1),\
	PGC_OUTF(y2)))

#define LOCXH(enable) (\
	PGC_1B(0x6c, enable))

#define LUT(i, r, g, b) (\
	PGC_4B(0xee, i, r, g, b))

#define LUT8(i, r, g, b) (\
	PGC_4B(0xe6, i, r, g, b))

#define LUT8RD(i) (\
	PGC_1B(0x53, i))

#define LUTINT(state) (\
	PGC_1B(0xec, state))

#define LUTRD(i) (\
	PGC_1B(0x50, i))

#define LUTSAV() (\
	PGC_CMD(1, 0xed))

#define MASK(mask) (\
	PGC_1B(0xe8, mask))

#define MATXRD(matrix) (\
	PGC_1B(0x52, matrix))

#define MDIDEN() (\
	PGC_CMD(1, 0x90))

#define MDMATX(array) (\
	PGC_CMD(65, 0x97),\
	PGC_OUTFA(array, 16))

#define MDORG(x, y, z) (\
	PGC_3F(0x91, x, y, z))

#define MDROTX(deg) (\
	PGC_1W(0x93, deg))

#define MDROTY(deg) (\
	PGC_1W(0x94, deg))

#define MDROTZ(deg) (\
	PGC_1W(0x95, deg))

#define MDSCAL(x, y, z) (\
	PGC_3F(0x92, x, y, z))

#define MDTRAN(x, y, z) (\
	PGC_3F(0x96, x, y, z))

#define MOVE(x, y) (\
	PGC_2F(0x10, x, y))

#define MOVE3(x, y, z) (\
	PGC_3F(0x12, x, y, z))

#define MOVER(x, y) (\
	PGC_2F(0x11, x, y))

#define MOVER3(x, y, z) (\
	PGC_3F(0x13, x, y, z))

#define PAN(x, y) (\
	PGC_2W(0xb7, x, y))

#define PLINE(npts, coord) (\
	PGC_CMD(1 + (4 * 2 * (npts)), 0x36),\
	PGC_OUTB(npts),\
	PGC_OUTFA(coord, npts))

#define PLINER(npts, coord) (\
	PGC_CMD(1 + (4 * 2 * (npts)), 0x37),\
	PGC_OUTB(npts),\
	PGC_OUTFA(coord, npts))

#define POINT() (\
	PGC_CMD(1, 0x08))

#define POINT3() (\
	PGC_CMD(1, 0x09))

#define POLY(npts, coord) (\
	PGC_CMD(1 + (4 * 2 * (npts)), 0x30),\
	PGC_OUTB(npts),\
	PGC_OUTFA(coord, npts))

#define POLY3(npts, coord) (\
	PGC_CMD(1 + (4 * 3 * (npts)), 0x32),\
	PGC_OUTB(npts),\
	PGC_OUTFA(coord, npts))

#define POLYR(npts, coord) (\
	PGC_CMD(1 + (4 * 2 * (npts)), 0x31),\
	PGC_OUTB(npts),\
	PGC_OUTFA(coord, npts))

#define POLYR3(npts, coord) (\
	PGC_CMD(1 + (4 * 3 * (npts)), 0x33),\
	PGC_OUTB(npts),\
	PGC_OUTFA(coord, npts))

#define PRMFIL(flag) (\
	PGC_1B(0xe9, flag))

#define PROJECT(angle) (\
	PGC_1W(0xb0, angle))

#define PVEC(npts, coord) (\
	PGC_CMD(1 + (4 * 4 * (npts)), 0x25),\
	PGC_OUTB(npts),\
	PGC_OUTFA(coord, npts))

#define PVECR(npts, coord) (\
	PGC_CMD(1 + (4 * 4 * (npts)), 0x26),\
	PGC_OUTB(npts),\
	PGC_OUTFA(coord, npts))

#define RBAND(parm) (\
	PGC_1B(0xe1, parm))

#define READP() (\
	PGC_CMD(1, 0x55))

#define RECT(x, y) (\
	PGC_2F(0x34, x, y))

#define RECTR(x, y) (\
	PGC_2F(0x35, x, y))

#define RESETF() (\
	PGC_CMD(1, 0x04))

#define SECTOR(r, deg1, deg2) (\
	PGC_CMD(9, 0x3d),\
	PGC_OUTF(r),\
	PGC_OUTW(deg1),\
	PGC_OUTW(deg2))

#define TANGLE(angle) (\
	PGC_1W(0x82, angle))

#define TDEFIN(c, x, y, array) (\
	PGC_CMD(3 + (((x) + 7) / 8) * y, 0x84),\
	PGC_OUTB(c),\
	PGC_OUTB(x),\
	PGC_OUTB(y),\
	PGC_OUTBA(array, (((x) + 7) / 8) * y))

#define TEXT(str, len) (\
	PGC_CMD(1 + (len), 0x88),\
	PGC_OUTBA(str, len))

#define TEXTP(str, len) (\
	PGC_CMD(1 + (len), 0x8b),\
	PGC_OUTBA(str, len))

#define TJUST(h, v) (\
	PGC_2B(0x85, h, v))

#define TSIZE(size) (\
	PGC_1F(0x81, size))

#define VWIDEN() (\
	PGC_CMD(1, 0xa0))

#define VWMATX(array) (\
	PGC_CMD(1 + (16 * 4), 0xa7),\
	PGC_OUTFA(array, 16))

#define VWPORT(x1, x2, y1, y2) (\
	PGC_4W(0xb2, x1, x2, y1, y2))

#define VWROTX(deg) (\
	PGC_1W(0xa3, deg))

#define VWROTY(deg) (\
	PGC_1W(0xa4, deg))

#define VWROTZ(deg) (\
	PGC_1W(0xa5, deg))

#define VWRPT(x, y, z) (\
	PGC_3F(0xa1, x, y, z))

#define WAIT(frames) (\
	PGC_1W(0x05, frames))

#define WINDOW(x1, x2, y1, y2) (\
	PGC_CMD(17, 0xb3),\
	PGC_OUTF(x1),\
	PGC_OUTF(x2),\
	PGC_OUTF(y1),\
	PGC_OUTF(y2))

#define XHAIRDISABLE() (\
	PGC_1B(0xe2, 0))

#define XHAIRSTD(xsize, ysize) (\
	PGC_CMD(6, 0xe2),\
	PGC_OUTB(1),\
	PGC_OUTW(xsize),\
	PGC_OUTW(ysize))

#define XHAIRUSER(c) (\
	PGC_CMD(3, 0xe2),\
	PGC_OUTB(2),\
	PGC_OUTB(c))

#define XMOVE(x, y) (\
	PGC_2W(0xe3, x, y))

#define XVMOVE(x, y) (\
	PGC_2F(0x1d, x, y))
