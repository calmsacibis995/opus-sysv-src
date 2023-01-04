/* SID @(#)vert.h	1.10 */

/*
 * Header file for verticom library.
 */

/* external declarations */

extern unsigned char *v_bufstart;
extern unsigned char *v_bufptr;
extern unsigned char *v_bufend;
extern unsigned char v_imm;

/* misc defines */

#define V_DEFAULT 1024		/* default buffer size */
#define V_MAXLEN 190		/* max buffer for single frame buf op */
#define V_DEVNAME "/dev/graph"	/* verticom file name */
#define V_CHK(n) \
	if ((v_bufptr + (n)) > v_bufend) {\
		v_bufwrite(n);\
	}
#define V_FLUSH v_bufwrite(0)
#ifdef V_IMMIDIATE
#define V_WRITE if (v_imm) v_bufwrite(0)
#else
#define V_WRITE
#endif
#define V_DRAIN v_bufwrite(-1)
#define V_OUTB(b) (*v_bufptr++ = (b))
#define V_OUTW(w) (*v_bufptr++ = (w) & 0xff,\
		      *v_bufptr++ = (w) >> 8)
#define V_DUMMY(n) (v_bufptr += (n))
#define V_INB(i, p) (*(p) = v_bufstart[i])
#define V_INW(i, p) (*(p) = (v_bufstart[(i) + 1] << 8) | v_bufstart[i])
#define V_OUTBA(p, n) (memcpy(v_bufptr, p, n), v_bufptr += (n))
#define V_OUTWA(p, n) (memcpy(v_bufptr, p, n * 2),\
			  swab(v_bufptr, v_bufptr, (n) * 2),\
			  v_bufptr += (n) * 2)
#define V_INBA(p, i, n) (memcpy(p, &v_bufstart[i], n))

/* commands */

#define v_flush()\
	V_FLUSH

#define v_drain()\
	V_DRAIN

#define v_select(ver)\
	V_CHK(1)\
	V_OUTB(0x31);\
	V_OUTB(ver);\
	V_WRITE

#define v_ibmmode()\
	V_CHK(1)\
	V_OUTB(0x40);\
	V_WRITE

#define v_graphicsmode()\
	V_CHK(1)\
	V_OUTB(0x41);\
	V_WRITE

#define v_screenblank(blank)\
	V_CHK(2)\
	V_OUTB(0x42);\
	V_OUTB(blank);\
	V_WRITE

#define v_screencolor(color)\
	V_CHK(2)\
	V_OUTB(0x44);\
	V_OUTB(color);\
	V_WRITE

#define v_bitplanes(mask)\
	V_CHK(2)\
	V_OUTB(0x46);\
	V_OUTB(mask);\
	V_WRITE

#define v_videodata(mask)\
	V_CHK(2)\
	V_OUTB(0x47);\
	V_OUTB(mask);\
	V_WRITE

#define v_clipstatus(status)\
	V_CHK(2)\
	V_OUTB(0x48);\
	V_OUTB(status);\
	V_WRITE

#define v_page(view, write)\
	V_CHK(3)\
	V_OUTB(0x4a);\
	V_OUTB(view);\
	V_OUTB(write);\
	V_WRITE

#define v_window(x1, y1, x2, y2)\
	V_CHK(9)\
	V_OUTB(0x50);\
	V_OUTW(x1);\
	V_OUTW(y1);\
	V_OUTW(x2);\
	V_OUTW(y2);\
	V_WRITE

#define v_viewport(x1, y1, x2, y2)\
	V_CHK(9)\
	V_OUTB(0x51);\
	V_OUTW(x1);\
	V_OUTW(y1);\
	V_OUTW(x2);\
	V_OUTW(y2);\
	V_WRITE

#define v_absmove(x, y)\
	V_CHK(5)\
	V_OUTB(0x60);\
	V_OUTW(x);\
	V_OUTW(y);\
	V_WRITE

#define v_relmove(x, y)\
	V_CHK(5)\
	V_OUTB(0x61);\
	V_OUTW(x);\
	V_OUTW(y);\
	V_WRITE

#define v_abspoint(x, y)\
	V_CHK(5)\
	V_OUTB(0x62);\
	V_OUTW(x);\
	V_OUTW(y);\
	V_WRITE

#define v_relpoint(x, y)\
	V_CHK(5)\
	V_OUTB(0x63);\
	V_OUTW(x);\
	V_OUTW(y);\
	V_WRITE

#define v_absline(x, y)\
	V_CHK(5)\
	V_OUTB(0x64);\
	V_OUTW(x);\
	V_OUTW(y);\
	V_WRITE

#define v_relline(x, y)\
	V_CHK(5)\
	V_OUTB(0x65);\
	V_OUTW(x);\
	V_OUTW(y);\
	V_WRITE

#define v_absrect(x, y)\
	V_CHK(5)\
	V_OUTB(0x66);\
	V_OUTW(x);\
	V_OUTW(y);\
	V_WRITE

#define v_relrect(x, y)\
	V_CHK(5)\
	V_OUTB(0x67);\
	V_OUTW(x);\
	V_OUTW(y);\
	V_WRITE

#define v_absrectfill(x, y)\
	V_CHK(5)\
	V_OUTB(0x68);\
	V_OUTW(x);\
	V_OUTW(y);\
	V_WRITE

#define v_relrectfill(x, y)\
	V_CHK(5)\
	V_OUTB(0x69);\
	V_OUTW(x);\
	V_OUTW(y);\
	V_WRITE

#define v_abspoly(n, vert)\
	V_CHK((4*n) + 2)\
	V_OUTB(0x6a);\
	V_OUTB(n);\
	V_OUTWA(vert, n);\
	V_WRITE

#define v_relpoly(n, vert)\
	V_CHK((4*n) + 2)\
	V_OUTB(0x6b);\
	V_OUTB(n);\
	V_OUTWA(vert, n);\
	V_WRITE

#define v_abspolyfill(n, vert)\
	V_CHK((4*n) + 2)\
	V_OUTB(0x6c);\
	V_OUTB(n);\
	V_OUTWA(vert, n);\
	V_WRITE

#define v_relpolyfill(n, vert)\
	V_CHK((4*n) + 2)\
	V_OUTB(0x6d);\
	V_OUTB(n);\
	V_OUTWA(vert, n);\
	V_WRITE

#define v_arc1(x1, y1, x2, y2)\
	V_CHK(9)\
	V_OUTB(0x6e);\
	V_OUTW(x1);\
	V_OUTW(y1);\
	V_OUTW(x2);\
	V_OUTW(y2);\
	V_WRITE

#define v_arc1fill(x1, y1, x2, y2)\
	V_CHK(9)\
	V_OUTB(0x6f);\
	V_OUTW(x1);\
	V_OUTW(y1);\
	V_OUTW(x2);\
	V_OUTW(y2);\
	V_WRITE

#define v_arc2(rad, sang, eang)\
	V_CHK(7)\
	V_OUTB(0x70);\
	V_OUTW(rad);\
	V_OUTW(sang);\
	V_OUTW(eang);\
	V_WRITE

#define v_arc2fill(rad, sang, eang)\
	V_CHK(7)\
	V_OUTB(0x71);\
	V_OUTW(rad);\
	V_OUTW(sang);\
	V_OUTW(eang);\
	V_WRITE

#define v_circle(rad)\
	V_CHK(3)\
	V_OUTB(0x73);\
	V_OUTW(rad);\
	V_WRITE

#define v_circlefill(rad)\
	V_CHK(3)\
	V_OUTB(0x75);\
	V_OUTW(rad);\
	V_WRITE

#define v_pie(rad, sang, eang)\
	V_CHK(7)\
	V_OUTB(0x76);\
	V_OUTW(rad);\
	V_OUTW(sang);\
	V_OUTW(eang);\
	V_WRITE

#define v_piefill(rad, sang, eang)\
	V_CHK(7)\
	V_OUTB(0x77);\
	V_OUTW(rad);\
	V_OUTW(sang);\
	V_OUTW(eang);\
	V_WRITE

#define v_ellipse(rad1, rad2, rot)\
	V_CHK(7)\
	V_OUTB(0x78);\
	V_OUTW(rad1);\
	V_OUTW(rad2);\
	V_OUTW(rot);\
	V_WRITE

#define v_ellipsefill(rad1, rad2, rot)\
	V_CHK(7)\
	V_OUTB(0x79);\
	V_OUTW(rad1);\
	V_OUTW(rad2);\
	V_OUTW(rot);\
	V_WRITE

#define v_polymarker()\
	V_CHK(1)\
	V_OUTB(0x85);\
	V_WRITE

#define v_setalphatextpos(x, y)\
	V_CHK(5)\
	V_OUTB(0x84);\
	V_OUTW(x);\
	V_OUTW(y);\
	V_WRITE

#define v_alphatext(str, n)\
	V_CHK(n + 2)\
	V_OUTB(0x7c);\
	V_OUTBA(str, n);\
	V_WRITE

#define v_graphicstext(str, n)\
	V_CHK(n + 2)\
	V_OUTB(0x7d);\
	V_OUTBA(str, n);\
	V_WRITE

#define v_readpixel(x, y, pixel)\
	V_CHK(5)\
	V_OUTB(0x7e);\
	V_OUTW(x);\
	V_OUTW(y);\
	V_DRAIN;\
	V_INB(1, pixel)

#define v_writepixel(x, y, pixel)\
	V_CHK(6)\
	V_OUTB(0x7f);\
	V_OUTW(x);\
	V_OUTW(y);\
	V_OUTB(pixel);\
	V_WRITE

#define v_readwritearea(x1, y1, x2, y2)\
	V_CHK(9)\
	V_OUTB(0x80);\
	V_OUTW(x1);\
	V_OUTW(y1);\
	V_OUTW(x2);\
	V_OUTW(y2);\
	V_WRITE

#define v_readframebuf(buf, n)\
	V_FLUSH;\
	V_OUTB(0x81);\
	V_OUTB(*n);\
	V_DUMMY(*n);\
	V_DRAIN;\
	V_INB(1, n);\
	V_INBA(buf, 2, v_bufstart[1])

#define v_writeframebuf(buf, n)\
	V_CHK(n + 2)\
	V_OUTB(0x82);\
	V_OUTB(n);\
	V_OUTBA(buf, n);\
	V_WRITE

#define v_move(sx1, sy1, sx2, sy2, dx, dy, flg, cnt, color)\
	V_CHK(16)\
	V_OUTB(0x83);\
	V_OUTW(sx1);\
	V_OUTW(sy1);\
	V_OUTW(sx2);\
	V_OUTW(sy2);\
	V_OUTW(dx);\
	V_OUTW(dy);\
	V_OUTB(flg);\
	V_OUTB(cnt);\
	V_OUTB(color);\
	V_WRITE

#define v_setalphatextattr(flg, linesp, width, height, bcolor, fcolor)\
	V_CHK(8)\
	V_OUTB(0x90);\
	V_OUTB(flg);\
	V_OUTW(linesp);\
	V_OUTB(width);\
	V_OUTB(height);\
	V_OUTB(bcolor);\
	V_OUTB(fcolor);\
	V_WRITE

#define v_settextrot(rot)\
	V_CHK(2)\
	V_OUTB(0x86);\
	V_OUTB(rot);\
	V_WRITE

#define v_setgraphicstextattr(flg, linesp, width, height, bcolor, fcolor, align, rot)\
	V_CHK(13)\
	V_OUTB(0x91);\
	V_OUTB(flg);\
	V_OUTW(linesp);\
	V_OUTW(width);\
	V_OUTW(height);\
	V_OUTB(bcolor);\
	V_OUTB(fcolor);\
	V_OUTB(align);\
	V_OUTW(rot);\
	V_WRITE

#define v_setcolor(color, red, green, blue)\
	V_CHK(8)\
	V_OUTB(0x92);\
	V_OUTB(color);\
	V_OUTW(red);\
	V_OUTW(green);\
	V_OUTW(blue);\
	V_WRITE

#define v_setcursorattr(color, style)\
	V_CHK(3)\
	V_OUTB(0x94);\
	V_OUTB(color);\
	V_OUTB(style);\
	V_WRITE

#define v_cursor(x, y, buf)\
	V_CHK((((x + 7) >> 3) * y) + 5)\
	V_OUTB(0x95);\
	V_OUTBA(buf, ((x + 7) >> 3) * y);\
	V_WRITE

#define v_setlineattr(width, bcolor, fcolor, style)\
	V_CHK(6)\
	V_OUTB(0x96);\
	V_OUTW(width);\
	V_OUTB(bcolor);\
	V_OUTB(fcolor);\
	V_OUTB(style);\
	V_WRITE

#define v_texture(style, len, buf)\
	V_CHK(((len + 7) >> 3) + 3)\
	V_OUTB(0x97);\
	V_OUTB(style);\
	V_OUTB(len);\
	V_OUTBA(buf, (len + 7) >> 3);\
	V_WRITE

#define v_setfillattr(bcolor, fcolor, style)\
	V_CHK(4)\
	V_OUTB(0x98);\
	V_OUTB(bcolor);\
	V_OUTB(fcolor);\
	V_OUTB(style);\
	V_WRITE

#define v_fillpat(style, x, y, buf)\
	V_CHK((((x + 7) >> 3) * y) + 6)\
	V_OUTB(0x99);\
	V_OUTB(style);\
	V_OUTW(x);\
	V_OUTW(y);\
	V_OUTBA(buf, ((x + 7) >> 3) * y);\
	V_WRITE

#define v_setpolymarkerattr(i, height, bcolor, fcolor)\
	V_CHK(6)\
	V_OUTB(0x9a);\
	V_OUTB(i);\
	V_OUTW(height);\
	V_OUTB(bcolor);\
	V_OUTB(fcolor);\
	V_WRITE

#define v_definepolymarker(i, n, buf)\
	V_CHK(3 + n)\
	V_OUTB(0x9b);\
	V_OUTB(i);\
	V_OUTB(n);\
	V_OUTBA(buf, n);\
	V_WRITE

#define v_movecolors(bcolor, fcolor)\
	V_CHK(3)\
	V_OUTB(0xa0);\
	V_OUTB(bcolor);\
	V_OUTB(fcolor);\

#define v_setwritingmode(type, color)\
	V_CHK(3)\
	V_OUTB(0x9c);\
	V_OUTB(type);\
	V_OUTB(color);\
	V_WRITE

#define v_fontstyle(type)\
	V_CHK(2)\
	V_OUTB(0x9d);\
	V_OUTB(type);\
	V_WRITE

#define v_fontattr(type, style)\
	V_CHK(3)\
	V_OUTB(0x9e);\
	V_OUTB(type);\
	V_OUTB(style);\
	V_WRITE

#define v_colortable(sel)\
	V_CHK(2)\
	V_OUTB(0x9f);\
	V_OUTB(sel);\
	V_WRITE

#define v_inqident(buf)\
	V_FLUSH;\
	V_OUTB(0xb0);\
	V_DUMMY(14);\
	V_DRAIN;\
	V_INBA(buf, 0, 15)

#define v_inqcolorrep(color, r, g, b)\
	V_FLUSH;\
	V_OUTB(0xb1);\
	V_OUTB(color);\
	V_DUMMY(5);\
	V_DRAIN;\
	V_INW(1, r);\
	V_INW(3, g);\
	V_INW(5, b)

#define v_inqalphatextattr(flgs, line, width, height, bcolor, fcolor)\
	V_FLUSH;\
	V_OUTB(0xb2);\
	V_DUMMY(7);\
	V_DRAIN;\
	V_INB(1, flgs);\
	V_INW(2, line);\
	V_INB(4, width);\
	V_INB(5, height);\
	V_INB(6, bcolor);\
	V_INB(7, fcolor)

#define v_inqalphatextloc(x, y)\
	V_FLUSH;\
	V_OUTB(0xb3);\
	V_DUMMY(4);\
	V_DRAIN;\
	V_INW(1, x);\
	V_INW(3, y)

#define v_inqfillattr(bcolor, fcolor, style)\
	V_FLUSH;\
	V_OUTB(0xb4);\
	V_DUMMY(3);\
	V_DRAIN;\
	V_INB(1, bcolor);\
	V_INB(2, fcolor);\
	V_INB(3, style)

#define v_inqlineattr(width, bcolor, fcolor, style)\
	V_FLUSH;\
	V_OUTB(0xb5);\
	V_DUMMY(5);\
	V_DRAIN;\
	V_INW(1, width);\
	V_INB(3, bcolor);\
	V_INB(4, fcolor);\
	V_INB(5, style)

#define v_inqgraphicstextattr(flg, linesp, width, height, bcolor, fcolor, align, rot)\
	V_FLUSH;\
	V_OUTB(0xb6);\
	V_DUMMY(12);\
	V_DRAIN;\
	V_INB(1, flg);\
	V_INW(2, linesp);\
	V_INW(4, width);\
	V_INW(6, height);\
	V_INB(8, bcolor);\
	V_INB(9, fcolor);\
	V_INB(10, align);\
	V_INW(11, rot)

#define v_inqpropspace(code, width, height)\
	V_FLUSH;\
	V_OUTB(0xb7);\
	V_OUTB(code);\
	V_DUMMY(3);\
	V_DRAIN;\
	V_INW(1, width);\
	V_INW(3, height)

#define v_inqpolymarkerattr(i, height, bcolor, fcolor)\
	V_FLUSH;\
	V_OUTB(0xb8);\
	V_OUTB(i);\
	V_DUMMY(3);\
	V_DRAIN;\
	V_INW(1, height);\
	V_INB(3, bcolor);\
	V_INB(4, fcolor)

#define v_inqcolor(x, y, color)\
	V_FLUSH;\
	V_OUTB(0xb9);\
	V_OUTW(x);\
	V_OUTW(y);\
	V_DRAIN;\
	V_INB(1, color)

#define v_locator(x, y, status)\
	V_CHK(6)\
	V_OUTB(0xba);\
	V_OUTW(x);\
	V_OUTW(y);\
	V_OUTB(status);\
	V_WRITE
