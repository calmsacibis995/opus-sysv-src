/* %Q% %W% */

/* C cover routines for cgi functions.
 *
 *	10/01/86	change counts from shorts to chars
 *			add vrd_curkeys macro
 *			fix vsm_locator args
 *			add old vdi (non-cgi) calls
 *			fix vsb_mode() code
 *	10/07/86	fix vqtf_description() parameters
 */

#include <sys/cgicomm.h>

#define PUTC(n) (-(n))  /* n char values input to procedure */
#define GETC(n) (n)  /* n char values output from procedure */
#define PUTW(n) (-((n)+(n)))  /* n short values input to procedure */
#define GETW(n) ((n)+(n))  /* n short values output from procedure */
#define VAL 0  /* value parameter */
#define IMM 1  /* command cannot be buffered */
#define BUF 0  /* command can be buffered */
#define STOC(s) strlen(s)  /* take string length */

static int _x, _y;
static char *_p;

#define v_appl(dev, fname, len, data) \
	( _p=(char *)(fname), _x=(int)(len), \
	cgi_cmd(CGI_APPL, 4, BUF, \
		VAL, dev, \
		PUTC(STOC(_p)), _p, \
		VAL, _x, \
		PUTW(_x), data) )

#define vb_pixels(dev, origin, w, h, valw, valh, pixels) \
	( _x=(int)(w), _y=(int)(h), \
	cgi_cmd(CGI_BPA, 7, BUF, \
		VAL, dev, \
		PUTW(2), origin, \
		VAL, _x, \
		VAL, _y, \
		PUTW(2), valw, \
		PUTW(2), valh, \
		PUTC(_x*_y>0?_x*_y:-_x*_y), pixels) )

#define v_clrwk(dev) \
	cgi_cmd(CGI_CRWK, 1, BUF, \
	VAL, dev)

#define fd_close(fd) \
	(-1)

#define cm_stop() \
	(-1)

#define cm_close(chan) \
	(-1)

#define v_clswk(dev) \
	cgi_cmd(CGI_CLWK, 1, IMM, \
		VAL, dev)

#define fd_connect(dir) \
	(-1)

#define vcp_bitmap(dev, map, src, dst) \
	cgi_cmd(CGI_CPBM, 4, BUF, \
		VAL, dev, \
		VAL, map, \
		PUTW(4), src, \
		PUTW(2), dst)

#define fd_copy(fd) \
	(-1)

#define vc_bitmap(dev, xy, typ, bit) \
	cgi_cmd(CGI_CRBM, 4, IMM, \
		VAL, dev, \
		PUTW(4), xy, \
		VAL, typ, \
		GETW(1), bit)

#define vc_cursor(dev, data, mask, x, y, cursor) \
	cgi_cmd(CGI_CRC, 6, IMM, \
		VAL, dev, \
		VAL, data, \
		VAL, mask, \
		VAL, x,	\
		VAL, y, \
		GETW(1), cursor)

#define v_curdown(dev) \
	cgi_cmd(CGI_CDN, 1, BUF, \
		VAL, dev)

#define v_curhome(dev) \
	cgi_cmd(CGI_CHOM, 1, BUF, \
		VAL, dev)

#define v_curleft(dev) \
	cgi_cmd(CGI_CLFT, 1, BUF, \
		VAL, dev)

#define v_curright(dev) \
	cgi_cmd(CGI_CRT, 1, BUF, \
		VAL, dev)

#define v_curup(dev) \
	cgi_cmd(CGI_CUP, 1, BUF, \
		VAL, dev)

#define vd_bitmap(dev, bit) \
	cgi_cmd(CGI_DBM, 2, BUF, \
		VAL, dev, \
		VAL, bit)

#define vd_cursor(dev, cursor) \
	cgi_cmd(CGI_DCD, 2, BUF, \
		VAL, dev, \
		VAL, cursor)

#define fd_delete(fd, name) \
	(-1)

#define vs_curaddress(dev, row, col) \
	cgi_cmd(CGI_DCA, 3, BUF, \
		VAL, dev, \
		VAL, row, \
		VAL, col)

#define fd_disconnect(fd) \
	(-1)

#define v_dspcur(dev, x, y) \
	cgi_cmd(CGI_DGIC, 3, BUF, \
		VAL, dev, \
		VAL, x, \
		VAL, y)

#define v_enter_cur(dev) \
	cgi_cmd(CGI_ECAM, 1, BUF, \
		VAL, dev)

#define v_eeol(dev) \
	cgi_cmd(CGI_EEOL, 1, BUF, \
		VAL, dev)

#define v_eeos(dev) \
	cgi_cmd(CGI_EEOS, 1, BUF, \
		VAL, dev)

#define v_exit_cur(dev) \
	cgi_cmd(CGI_XCAM, 1, BUF, \
		VAL, dev)

#define fd_size(fd) \
	(-1)

#define v_hardcopy(dev) \
	cgi_cmd(CGI_HARD, 1, BUF, \
		VAL, dev)

#define cm_start() \
	(-1)

#define cm_open(chan) \
	(-1)

#define vrq_choice(dev, initchc, finalchc) \
	cgi_cmd(CGI_ICRM, 3, IMM, \
		VAL, dev, \
		VAL, initchc, \
		GETW(1), finalchc)

#define vsm_choice(dev, finalchc) \
	cgi_cmd(CGI_ICSM, 2, IMM, \
		VAL, dev, \
		GETW(1), finalchc)

#define vrq_locator(dev, initxy, ink, band, echo, finalxy, terminator) \
	cgi_cmd(CGI_ILRM, 7, IMM, \
		VAL, dev, \
		PUTW(2), initxy, \
		VAL, ink, \
		VAL, band, \
		VAL, echo, \
		GETW(2), finalxy, \
		GETC(1), terminator)

#define vsm_locator(dev, initxy, finalxy, pressed, released, key_state) \
	cgi_cmd(CGI_ILSM, 6, IMM, \
		VAL, dev, \
		PUTW(2), initxy, \
		GETW(2), finalxy, \
		GETW(1), pressed, \
		GETW(1), released, \
		GETW(1), key_state)

#define vrq_string(dev, maxlen, echomode, echoxy, str) \
	( _x=(int)(maxlen), \
	cgi_cmd(CGI_ISRM, 5, IMM, \
		VAL, dev, \
		VAL, _x, \
		VAL, echomode, \
		PUTW(2), echoxy, \
		GETC(_x), str) )

#define vsm_string(dev, maxlen, echomode, echoxy, str) \
	( _x=(int)(maxlen), \
	cgi_cmd(CGI_ISSM, 5, IMM, \
		VAL, dev, \
		VAL, _x, \
		VAL, echomode, \
		PUTW(2), echoxy, \
		GETC(_x), str) )

#define vrq_valuator(dev, valin, valout) \
	cgi_cmd(CGI_IVRM, 3, IMM, \
		VAL, dev, \
		VAL, valin, \
		GETW(1), valout)

#define vsm_valuator(dev, finalval) \
	cgi_cmd(CGI_IVSM, 2, IMM, \
		VAL, dev, \
		GETW(1), finalval)

#define vq_chcells(dev, rows, cols) \
	cgi_cmd(CGI_QACC, 3, IMM, \
		VAL, dev, \
		GETW(1), rows, \
		GETW(1), cols)

#define vqa_cap(dev, cap) \
	cgi_cmd(CGI_QATC, 2, IMM, \
		VAL, dev, \
		GETW(15), cap)

#define vqa_cell(dev, row, col, prop, xout, yout) \
	cgi_cmd(CGI_QATL, 6, IMM, \
		VAL, dev, \
		VAL, row, \
		VAL, col, \
		GETW(1), prop, \
		GETW(1), xout, \
		GETW(1), yout)

#define vqa_font(dev, font, size, cap) \
	cgi_cmd(CGI_QATF, 4, IMM, \
		VAL, dev, \
		VAL, font, \
		VAL, size, \
		GETW(7), cap)

#define vqa_position(dev, x, y) \
	cgi_cmd(CGI_QATP, 3, IMM, \
		VAL, dev, \
		GETW(1), x, \
		GETW(1), y)

#define vqa_length(dev, str) \
	(_p=(char *)(str), \
	cgi_cmd(CGI_QASL, 2, IMM, \
		VAL, dev, \
		PUTC(STOC(_p)), _p) )

#define vqb_mode(dev) \
	cgi_cmd(CGI_QBGM, 1, IMM, \
		VAL, dev)

#define vqb_format(dev) \
	cgi_cmd(CGI_QBMF, 1, IMM, \
		VAL, dev)

#define vqb_pixels(dev, origin, w, h, valw, valh, pixels) \
	( _x=(int)(w), _y=(int)(h), \
	cgi_cmd(CGI_QBPA, 7, IMM, \
		VAL, dev, \
		PUTW(2), origin, \
		VAL, _x, \
		VAL, _y, \
		GETW(2), valw, \
		GETW(2), valh, \
		GETC(_x*_y>0?_x*_y:-_x*_y), pixels) )

#define vq_cellarray(dev, xy, rlen, nrow, erow, ruse, val, color) \
	( _x=(int)(rlen), _y=(int)(nrow), \
	cgi_cmd(CGI_QCA, 8, IMM, \
		VAL, dev, \
		PUTW(4), xy, \
		VAL, _x, \
		VAL, _y, \
		GETW(1), erow, \
		GETW(1), ruse, \
		GETW(1), val, \
		GETW(_x*_y), color) )

#define vqc_rectangle(dev, clip) \
	cgi_cmd(CGI_QCR, 2, IMM, \
		VAL, dev, \
		GETW(4), clip)

#define vq_color(dev, indin, setflg, rgb) \
	cgi_cmd(CGI_QREP, 4, IMM, \
		VAL, dev, \
		VAL, indin, \
		VAL, setflg, \
		GETW(3), rgb)

#define vq_curaddress(dev, row, col) \
	cgi_cmd(CGI_QCTA, 3, IMM, \
		VAL, dev, \
		GETW(1), row, \
		GETW(1), col)

#define vqf_attributes(dev, att) \
	cgi_cmd(CGI_QFAA, 2, IMM, \
		VAL, dev, \
		GETW(4), att)

#define vqt_attributes(dev, att) \
	cgi_cmd(CGI_QGTA, 2, IMM, \
		VAL, dev, \
		GETW(10), att)

#define vql_attributes(dev, att) \
	cgi_cmd(CGI_QPLA, 2, IMM, \
		VAL, dev, \
		GETW(4), att)

#define vqm_attributes(dev, att) \
	cgi_cmd(CGI_QPMA, 2, IMM, \
		VAL, dev, \
		GETW(4), att)

#define vq_cursor(dev, cursor, data, mask, x, y) \
	cgi_cmd(CGI_QCD, 6, IMM, \
		VAL, dev, \
		VAL, cursor, \
		GETW(1), data, \
		GETW(1), mask, \
		GETW(1), x, \
		GETW(1), y)

#define vqd_displays(dev, n, displays) \
	( _x=(int)(n), \
	cgi_cmd(CGI_QDBM, 3, IMM, \
		VAL, dev, \
		VAL, _x, \
		GETW(_x), displays) )

#define vqd_bitmap(dev, bit, xy) \
	cgi_cmd(CGI_QDWG, 3, IMM, \
		VAL, dev, \
		GETW(1), bit, \
		GETW(4), xy)

#define fd_inq(fd) \
	(-1)

#define vqf_representation(dev, n, attr) \
	( _x=(int)(n), \
	cgi_cmd(CGI_QFAR, 3, IMM, \
		VAL, dev, \
		VAL, _x, \
		GETW(_x), attr) )

#define vqg_cursor(dev, cursor) \
	cgi_cmd(CGI_QGIC, 2, IMM, \
		VAL, dev, \
		GETW(1), cursor)

#define vqt_extent(dev, xin, yin, string, xcon, ycon, rect, bool) \
	( _p=(char *)(string), \
	cgi_cmd(CGI_QGTE, 8, IMM, \
		VAL, dev, \
		VAL, xin, \
		VAL, yin, \
		PUTC(STOC(_p)), _p, \
		GETW(1), xcon, \
		GETW(1), ycon, \
		GETW(8), rect, \
		GETW(5), bool) )

#define vqtf_description(dev, n, desc, name) \
	( _x=(int)(n), \
	cgi_cmd(CGI_QGFD, 4, IMM, \
		VAL, dev, \
		VAL, _x, \
		GETW(_x), desc, \
		GETW(10), name) )

#define vqtf_metrics(dev, n, metrics) \
	( _x=(int)(n), \
	cgi_cmd(CGI_QGFM, 3, IMM, \
		VAL, dev, \
		VAL, _x, \
		GETW(_x), metrics) )

#define vqtf_character(dev, character, n, metrics) \
	( _x=(int)(n), \
	cgi_cmd(CGI_QGFC, 4, IMM, \
		VAL, dev, \
		VAL, character, \
		VAL, _x, \
		GETW(_x), metrics) )

#define vqt_representation(dev, n, attrib) \
	( _x=(int)(n), \
	cgi_cmd(CGI_QGFR, 3, IMM, \
		VAL, dev, \
		VAL, _x, \
		GETW(_x), attrib) )

#define vqi_extent(dev, rect) \
	cgi_cmd(CGI_QIEX, 2, IMM, \
		VAL, dev, \
		GETW(4), rect)

#define vqi_pixels(dev, origin, w, h, valw, valh, pixels) \
	( _x=(int)(w), _y=(int)(h), \
	cgi_cmd(CGI_QIPA, 7, IMM, \
		VAL, dev, \
		PUTW(2), origin, \
		VAL, _x, \
		VAL, _y, \
		GETW(2), valw, \
		GETW(2), valh, \
		GETW(_x*_y>0?_x*_y:-_x*_y), pixels) )

#define vqo_pattern(dev, xy) \
	cgi_cmd(CGI_QOPS, 2, IMM, \
		VAL, dev, \
		GETW(2), xy)

#define vql_representation(dev, n, attrib) \
	( _x=(int)(n), \
	cgi_cmd(CGI_QPLR, 3, IMM, \
		VAL, dev, \
		VAL, _x, \
		GETW(_x), attrib) )

#define vqm_representation(dev, n, attrib) \
	( _x=(int)(n), \
	cgi_cmd(CGI_QPMR, 3, IMM, \
		VAL, dev, \
		VAL, _x, \
		GETW(_x), attrib) )

#define cm_inq(chan) \
	(-1)

#define vq_error() \
	cgi_cmd(CGI_QERR, 0, IMM)

#define vi_pixels(dev, origin, w, h, valw, valh, pixels) \
	( _x=(int)(w), _y=(int)(h), \
	cgi_cmd(CGI_IPA, 7, BUF, \
		VAL, dev, \
		PUTW(2), origin, \
		VAL, _x, \
		VAL, _y, \
		PUTW(2), valw, \
		PUTW(2), valh, \
		PUTW(_x*_y>0?_x*_y:-_x*_y), pixels) )

#define cgi_load(where, para) \
	(-1)

#define v_message(dev, mesg, wait) \
	( _p=(char *)(mesg), \
	cgi_cmd(CGI_MESG, 3, IMM, \
		VAL, dev, \
		PUTC(STOC(_p)), _p, \
		VAL, wait) )

#define fd_open(fd, name, mode) \
	(-1)

#define v_opnwk(in, dev, out) \
	cgi_cmd(CGI_OPWK, 3, IMM, \
		PUTW(19), in, \
		GETW(1), dev, \
		GETW(66), out)

#define v_atext(dev, str, xout, yout) \
	( _p=(char *)(str), \
	cgi_cmd(CGI_OATX, 4, IMM, \
		VAL, dev, \
		PUTC(STOC(_p)), _p, \
		GETW(1), xout, \
		GETW(1), yout) )

#define v_arc(dev, x, y, radius, startangle, endangle) \
	cgi_cmd(CGI_OARC, 6, BUF, \
		VAL, dev, \
		VAL, x, \
		VAL, y, \
		VAL, radius, \
		VAL, startangle, \
		VAL, endangle)

#define v_bar(dev, xy) \
	cgi_cmd(CGI_OBAR, 2, BUF, \
		VAL, dev, \
		PUTW(4), xy)

#define v_cellarray(dev, xy, rlen, erow, nrow, wmod, colors) \
	( _x=(int)(rlen), _y=(int)(nrow), \
	cgi_cmd(CGI_OCAR, 7, BUF, \
		VAL, dev, \
		PUTW(4), xy, \
		VAL, _x, \
		VAL, erow, \
		VAL, _y, \
		VAL, wmod, \
		PUTW(_x*_y), colors) )

#define v_circle(dev, x, y, radius) \
	cgi_cmd(CGI_OCIR, 4, BUF, \
		VAL, dev, \
		VAL, x, \
		VAL, y, \
	        VAL, radius)

#define v_curtext(dev, str) \
	( _p=(char *)(str), \
	cgi_cmd(CGI_OCAT, 2, BUF, \
		VAL, dev, \
		PUTC(STOC(_p)), _p) )

#define v_ellipse(dev, x, y, xradius, yradius) \
	cgi_cmd(CGI_OELL, 5, BUF, \
		VAL, dev, \
		VAL, x, \
		VAL, y, \
	        VAL, xradius, \
	        VAL, yradius)

#define vel_arc(dev, x, y, xradius, yradius, sangle, eangle) \
	cgi_cmd(CGI_OERC, 7, BUF, \
		VAL, dev, \
		VAL, x, \
		VAL, y, \
	        VAL, xradius, \
	        VAL, yradius, \
	        VAL, sangle, \
	        VAL, eangle)

#define vel_pieslice(dev, x, y, xradius, yradius, sangle, eangle) \
	cgi_cmd(CGI_OEPS, 7, BUF, \
		VAL, dev, \
		VAL, x, \
		VAL, y, \
	        VAL, xradius, \
	        VAL, yradius, \
	        VAL, sangle, \
	        VAL, eangle)

#define v_fillarea(dev, count, xy) \
	( _x=(int)(count), \
	cgi_cmd(CGI_OFAR, 3, BUF, \
		VAL, dev, \
		VAL, _x, \
		PUTW(_x<<1), xy) )

#define v_gtext(dev, x, y, str) \
	( _p=(char *)(str), \
	cgi_cmd(CGI_OGTX, 4, BUF, \
		VAL, dev, \
		VAL, x, \
		VAL, y, \
		PUTC(STOC(_p)), _p) )

#define v_pieslice(dev, x, y, radius, startangle, endangle) \
	cgi_cmd(CGI_OPIE, 6, BUF, \
		VAL, dev, \
		VAL, x, \
		VAL, y, \
		VAL, radius, \
		VAL, startangle, \
		VAL, endangle)

#define v_pline(dev, cnt, xy) \
	( _x=(int)(cnt), \
	cgi_cmd(CGI_OPLN, 3, BUF, \
		VAL, dev, \
		VAL, _x, \
		PUTW(_x<<1), xy) )

#define v_pmarker(dev, cnt, xy) \
	( _x=(int)(cnt), \
	cgi_cmd(CGI_OPMK, 3, BUF, \
		VAL, dev, \
		VAL, _x, \
		PUTW(_x<<1), xy) )

#define fd_parse(qname, buf, avail) \
	(-1)

#define cmrx_wait(chan, cp) \
	(-1)

#define cmrx_now(chan, cp) \
	(-1)

#define vrd_curkeys(dev, mode, dir, key) \
	cgi_cmd(CGI_RCKY, 4, IMM, \
		VAL, dev, \
		VAL, mode, \
		GETW(1), dir, \
		GETC(1), key)

#define fd_directory(fd, name, buffer, avail) \
	(-1)

#define fdp_read(fd, buffer, count) \
	(-1)

#define fd_read(fd, buffer, count) \
	(-1)

#define cgi_kill() \
	(-1)

#define v_rmcur(dev) \
	cgi_cmd(CGI_RGIC, 1, BUF, \
	VAL, dev)

#define fd_rename(fd, old, new) \
	(-1)

#define vr_defaults(dev) \
	cgi_cmd(CGI_RDEF, 1, BUF, \
		VAL, dev)

#define v_rvoff(dev) \
	cgi_cmd(CGI_RVOF, 1, BUF, \
		VAL, dev)

#define v_rvon(dev) \
	cgi_cmd(CGI_RVON, 1, BUF, \
		VAL, dev)

#define fd_seek(dev) \
	(-1)

#define vsd_bitmap(dev, bit) \
	cgi_cmd(CGI_SDBM, 2, BUF, \
		VAL, dev, \
		VAL, bit)

#define vsg_cursor(dev, cursor) \
	cgi_cmd(CGI_SGIC, 2, BUF, \
		VAL, dev, \
		VAL, cursor)

#define cmtx_wait(chan, c) \
	(-1)

#define cmtx_now(chan, c) \
	(-1)

#define vsa_color(dev, index) \
	cgi_cmd(CGI_SATC, 2, BUF, \
		VAL, dev, \
		VAL, index)

#define vsa_font(dev, font, size, cap) \
	cgi_cmd(CGI_SATF, 4, IMM, \
		VAL, dev, \
		VAL, font, \
		VAL, size, \
		GETW(8), cap)

#define vsa_spacing(dev, spacing) \
	cgi_cmd(CGI_SATL, 2, BUF, \
		VAL, dev, \
		VAL, spacing)

#define vsa_overstrike(dev, mode) \
	cgi_cmd(CGI_SATO, 2, BUF, \
		VAL, dev, \
		VAL, mode)

#define vsa_passthru(dev, mode) \
	cgi_cmd(CGI_SAPM, 2, BUF, \
		VAL, dev, \
		VAL, mode)

#define vsa_position(dev, xin, yin, xout, yout) \
	cgi_cmd(CGI_SATP, 5, IMM, \
		VAL, dev, \
		VAL, xin, \
		VAL, yin, \
		GETW(1), xout, \
		GETW(1), yout)

#define vsa_quality(dev, mode) \
	cgi_cmd(CGI_SATQ, 2, BUF, \
		VAL, dev, \
		VAL, mode)

#define vsa_supersub(dev, mode) \
	cgi_cmd(CGI_SATS, 2, BUF, \
		VAL, dev, \
		VAL, mode)

#define vsa_underline(dev, mode) \
	cgi_cmd(CGI_SATU, 2, BUF, \
		VAL, dev, \
		VAL, mode)

#define vsb_color(dev, index) \
	cgi_cmd(CGI_SBCI, 2, BUF, \
		VAL, dev, \
		VAL, index)

#define vsb_mode(dev, mode) \
	cgi_cmd(CGI_SBGM, 2, BUF, \
		VAL, dev, \
		VAL, mode)

#define vst_height(dev, hreq, charw, cellw, cellh) \
	cgi_cmd(CGI_SCHT, 5, IMM, \
		VAL, dev, \
		VAL, hreq, \
		GETW(1), charw, \
		GETW(1), cellw, \
		GETW(1), cellh)

#define vsc_rectangle(dev, clip) \
	cgi_cmd(CGI_SCRT, 2, BUF, \
		VAL, dev, \
		PUTW(4), clip)

#define vs_color(dev, index, rgbin, rgbout) \
	cgi_cmd(CGI_SREP, 4, IMM, \
		VAL, dev, \
		VAL, index, \
		PUTW(3), rgbin, \
		GETW(3), rgbout)

#define vsc_table(dev, start, n, rgb) \
	( _x=(int)(n), \
	cgi_cmd(CGI_SCTB, 4, BUF, \
		VAL, dev, \
		VAL, start, \
		VAL, _x, \
		PUTW(_x*3), rgb) )

#define vcur_att(dev, reqatt, selatt) \
	cgi_cmd(CGI_SCTA, 3, IMM, \
		VAL, dev, \
		PUTW(4), reqatt, \
		GETW(4), selatt)

#define vcur_color(dev, forereq, backreq, foresel, backsel) \
	cgi_cmd(CGI_SCTC, 5, IMM, \
		VAL, dev, \
		VAL, forereq, \
		VAL, backreq, \
		GETW(1), foresel, \
		GETW(1), backsel)

#define vsf_representation(dev, n, attrib) \
	( _x=(int)(n), \
	cgi_cmd(CGI_SFAR, 3, BUF, \
		VAL, dev, \
		VAL, _x, \
		PUTW(_x), attrib) )

#define vsf_color(dev, index) \
	cgi_cmd(CGI_SFCI, 2, BUF, \
		VAL, dev, \
		VAL, index)

#define vsf_interior(dev, style) \
	cgi_cmd(CGI_SFIS, 2, BUF, \
		VAL, dev, \
		VAL, style)

#define vsf_style(dev, index) \
	cgi_cmd(CGI_SFSI, 2, BUF, \
		VAL, dev, \
		VAL, index)

#define vst_alignment(dev, hreq, vreq, hreal, vreal) \
	cgi_cmd(CGI_SGTA, 5, IMM, \
		VAL, dev, \
		VAL, hreq, \
		VAL, vreq, \
		GETW(1), hreal, \
		GETW(1), vreal)

#define vst_font(dev, font) \
	cgi_cmd(CGI_SGTF, 2, BUF, \
		VAL, dev, \
		VAL, font)

#define vst_representation(dev, n, attrib) \
	( _x=(int)(n), \
	cgi_cmd(CGI_SGTR, 3, BUF, \
		VAL, dev, \
		VAL, _x, \
		PUTW(_x), attrib) )

#define vst_color(dev, index) \
	cgi_cmd(CGI_SGTC, 2, BUF, \
		VAL, dev, \
		VAL, index)

#define vst_rotation(dev, angle) \
	cgi_cmd(CGI_SGTB, 2, BUF, \
		VAL, dev, \
		VAL, angle)

#define vsi_extent(dev, rect) \
	cgi_cmd(CGI_SINX, 2, BUF, \
		VAL, dev, \
		PUTW(4), rect)

#define vsl_cross(dev, bit) \
	cgi_cmd(CGI_SLCS, 2, BUF, \
		VAL, dev, \
		VAL, bit)

#define vs_editchars(dev, linedel, chardel) \
	cgi_cmd(CGI_SLEC, 3, BUF, \
		VAL, dev, \
		VAL, linedel, \
		VAL, chardel)

#define vs_penspeed(dev, speed) \
	cgi_cmd(CGI_SPSP, 2, BUF, \
		VAL, dev, \
		VAL, speed)

#define vsl_color(dev, index) \
	cgi_cmd(CGI_SPLC, 2, BUF, \
		VAL, dev, \
		VAL, index)

#define vsl_type(dev, type) \
	cgi_cmd(CGI_SPLT, 2, BUF, \
		VAL, dev, \
		VAL, type)

#define vsl_width(dev, width) \
	cgi_cmd(CGI_SPLW, 2, BUF, \
		VAL, dev, \
		VAL, width)

#define vsl_representation(dev, n, attrib) \
	( _x=(int)(n), \
	cgi_cmd(CGI_SPLR, 3, BUF, \
		VAL, dev, \
		VAL, _x, \
		PUTW(_x), attrib) )

#define vsm_color(dev, index) \
	cgi_cmd(CGI_SPMC, 2, BUF, \
		VAL, dev, \
		VAL, index)

#define vsm_height(dev, height) \
	cgi_cmd(CGI_SPMH, 2, BUF, \
		VAL, dev, \
		VAL, height)

#define vsm_representation(dev, n, attrib) \
	( _x=(int)(n), \
	cgi_cmd(CGI_SPMR, 3, BUF, \
		VAL, dev, \
		VAL, _x, \
		PUTW(_x), attrib) )

#define vsm_type(dev, type) \
	cgi_cmd(CGI_SPMC, 2, BUF, \
		VAL, dev, \
		VAL, type)

#define vsul_type(dev, pattern) \
	( _x=(int)(pattern), \
	cgi_cmd(CGI_SUPL, 3, BUF, \
		VAL, dev, \
		VAL, _x, \
		VAL, _x>>16) )

#define vswr_mode(dev, mode) \
	cgi_cmd(CGI_SWMD, 2, BUF, \
		VAL, dev, \
		VAL, mode)

#define v_updwk(dev) \
	cgi_cmd(CGI_UPWK, 1, BUF, \
		VAL, dev)

#define fdp_write(fd, buf, count) \
	(-1)

#define fd_write(fd, buf, count) \
	(-1)

#define vs_curmode(dev, mode) \
	cgi_cmd(VDI_SCUR, 2, BUF, \
		VAL, dev, \
		VAL, mode)

#define vq_curmode(dev) \
	cgi_cmd(VDI_QCUR, 1, IMM, \
		VAL, dev)

#define vs_gclbu(dev, mode) \
	cgi_cmd(VDI_SCLB, 2, BUF, \
		VAL, dev, \
		VAL, mode)

#define vq_gclbu(dev) \
	cgi_cmd(VDI_QCLB, 1, IMM, \
		VAL, dev)

#define v_appl(dev, fname, len, data) \
	( _p=(char *)(fname), _x=(int)(len), \
	cgi_cmd(CGI_APPL, 4, BUF, \
		VAL, dev, \
		PUTC(STOC(_p)), _p, \
		VAL, _x, \
		PUTW(_x), data) )

#define v_get_npels(dev, xy, src, n) \
	( _x=(int)(n), \
	cgi_cmd(VDI_GPEL, 3, IMM, \
		VAL, dev, \
		PUTW(4), xy, \
		GETW(_x), src) )

#define v_put_npels(dev, xy, dest, n) \
	cgi_cmd(VDI_PPEL, 3, BUF, \
		VAL, dev, \
		PUTW(2), xy, \
		PUTW(n), dest)

#define v_copy_npels(dev, xy) \
	cgi_cmd(VDI_CPEL, 2, BUF, \
		VAL, dev, \
		PUTW(6), xy)

#define vc_page(dev, from, to) \
	cgi_cmd(VDI_CPAG, 3, BUF, \
		VAL, dev, \
		VAL, from, \
		VAL, to)

#define vq_page(dev, gmode, cmode) \
	cgi_cmd(VDI_QPAG, 3, IMM, \
		VAL, dev, \
		GETW(3), gmode, \
		GETW(3), cmode)

#define vs_page(dev, gmodein, cmodein, gmodeout, cmodeout) \
	cgi_cmd(VDI_SPAG, 5, IMM, \
		VAL, dev, \
		PUTW(2), gmodein, \
		PUTW(2), cmodein, \
		GETW(2), gmodeout, \
		GETW(2), cmodeout)
