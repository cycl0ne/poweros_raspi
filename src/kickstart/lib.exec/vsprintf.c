#include "types.h"
#include "sysbase.h"

#include "exec_funcs.h"
#define noinline __attribute__((noinline))

/* some reluctance to put this into a new limits.h, so it is here */
#define INT_MAX		((int)(~0U>>1))
#define NUM_TYPE long long

const char hex_asc[] = "0123456789abcdef";
#define hex_asc_lo(x)   hex_asc[((x) & 0x0f)]
#define hex_asc_hi(x)   hex_asc[((x) & 0xf0) >> 4]

#define _U	0x01	/* upper */
#define _L	0x02	/* lower */
#define _D	0x04	/* digit */
#define _C	0x08	/* cntrl */
#define _P	0x10	/* punct */
#define _S	0x20	/* white space (space/lf/tab) */
#define _X	0x40	/* hex digit */
#define _SP	0x80	/* hard space (0x20) */

const unsigned char _ctype[] = {
_C,_C,_C,_C,_C,_C,_C,_C,			/* 0-7 */
_C,_C|_S,_C|_S,_C|_S,_C|_S,_C|_S,_C,_C,		/* 8-15 */
_C,_C,_C,_C,_C,_C,_C,_C,			/* 16-23 */
_C,_C,_C,_C,_C,_C,_C,_C,			/* 24-31 */
_S|_SP,_P,_P,_P,_P,_P,_P,_P,			/* 32-39 */
_P,_P,_P,_P,_P,_P,_P,_P,			/* 40-47 */
_D,_D,_D,_D,_D,_D,_D,_D,			/* 48-55 */
_D,_D,_P,_P,_P,_P,_P,_P,			/* 56-63 */
_P,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U,	/* 64-71 */
_U,_U,_U,_U,_U,_U,_U,_U,			/* 72-79 */
_U,_U,_U,_U,_U,_U,_U,_U,			/* 80-87 */
_U,_U,_U,_P,_P,_P,_P,_P,			/* 88-95 */
_P,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L,	/* 96-103 */
_L,_L,_L,_L,_L,_L,_L,_L,			/* 104-111 */
_L,_L,_L,_L,_L,_L,_L,_L,			/* 112-119 */
_L,_L,_L,_P,_P,_P,_P,_C,			/* 120-127 */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		/* 128-143 */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		/* 144-159 */
_S|_SP,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,   /* 160-175 */
_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,       /* 176-191 */
_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,       /* 192-207 */
_U,_U,_U,_U,_U,_U,_U,_P,_U,_U,_U,_U,_U,_U,_U,_L,       /* 208-223 */
_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,       /* 224-239 */
_L,_L,_L,_L,_L,_L,_L,_P,_L,_L,_L,_L,_L,_L,_L,_L};      /* 240-255 */

#define __ismask(x) (_ctype[(int)(unsigned char)(x)])

#define isalnum(c)	((__ismask(c)&(_U|_L|_D)) != 0)
#define isalpha(c)	((__ismask(c)&(_U|_L)) != 0)
#define iscntrl(c)	((__ismask(c)&(_C)) != 0)
#define isdigit(c)	((__ismask(c)&(_D)) != 0)
#define isgraph(c)	((__ismask(c)&(_P|_U|_L|_D)) != 0)
#define islower(c)	((__ismask(c)&(_L)) != 0)
#define isprint(c)	((__ismask(c)&(_P|_U|_L|_D|_SP)) != 0)
#define ispunct(c)	((__ismask(c)&(_P)) != 0)
#define isspace(c)	((__ismask(c)&(_S)) != 0)
#define isupper(c)	((__ismask(c)&(_U)) != 0)
#define isxdigit(c)	((__ismask(c)&(_D|_X)) != 0)

/*
 * Rather than doubling the size of the _ctype lookup table to hold a 'blank'
 * flag, just check for space or tab.
 */
#define isblank(c)	(c == ' ' || c == '\t')

#define isascii(c) (((unsigned char)(c))<=0x7f)
#define toascii(c) (((unsigned char)(c))&0x7f)

static inline unsigned char __tolower(unsigned char c)
{
	if (isupper(c))
		c -= 'A'-'a';
	return c;
}

static inline unsigned char __toupper(unsigned char c)
{
	if (islower(c))
		c -= 'a'-'A';
	return c;
}

#define tolower(c) __tolower(c)
#define toupper(c) __toupper(c)

static UINT32 strnlen(char const *s, UINT32 count)
{
	const char *sc;

	for(sc = s; count-- && *sc != '\0'; ++sc);
	return sc - s;
}

int lib_SNPrintF(struct SysBase *SysBase,char *buf, size_t size, const char *fmt, ...) {
	va_list ap;
	int len;
	va_start(ap, fmt);
	len = VSNPrintF(buf, size, fmt, ap);
	va_end(ap);
	return len;
}

/*
 * Scaled down version of printf(3), It does not support %f !!!
 *
 * One additional format:
 *
 * The format %b is supported to decode error registers.
 * Its usage is:
 *
 *	printf("reg=%b\n", regval, "<base><arg>*");
 *
 * where <base> is the output base expressed as a control character, e.g.
 * \10 gives octal; \20 gives hex.  Each arg is a sequence of characters,
 * the first of which gives the bit number to be inspected (origin 1), and
 * the next characters (up to a control character, i.e. a character <= 32),
 * give the name of the register.  Thus:
 *
 *	printf("reg=%b\n", 3, "\10\2BITTWO\1BITONE\n");
 *
 * would produce output:
 *
 *	reg=3<BITTWO,BITONE>
 */

static void kdoprnt(void (*put)(char), const char *fmt, va_list ap);
static char *sbuf, *ebuf;

static void
sputchar(char c) 
{
	if (sbuf < ebuf)*sbuf++ = c;
}

void lib_UART_send ( unsigned int c );
void putchar(char x)
{
	lib_UART_send(x);
}

void lib_VPrintF(struct SysBase *SysBase, const char *fmt, va_list ap) {
	kdoprnt(putchar, fmt, ap);
}

int lib_VSNPrintF(struct SysBase *SysBase, char *buf, size_t size, const char *fmt, va_list ap) {
	sbuf = buf;
	ebuf = buf + size - 1;
	kdoprnt(sputchar, fmt, ap);
	*sbuf = '\0';
	return (sbuf - buf);
}

static void
kprintn(void (*put)(char), UINT32 ul, int base) {
	// hold a long in base 8
	char *p, buf[(sizeof(long) * 8 / 3) + 1];
	
	p = buf;
	
	do {
		*p++ = "0123456789abcdef"[ul % base];
	} while (ul /= base);
	
	do {
		put(*--p);
	} while (p > buf);
}

static void
kdoprnt(void (*put)(char), const char *fmt, va_list ap) {
	char *p;
	int ch, n;
	unsigned long ul;
	int lflag, set;
	
	for (;;) {
		while ((ch = *fmt++) != '%') {
			if (ch == '\0')
				return;
			put(ch);
		}
		lflag = 0;
		
reswitch:
	
	switch (ch = *fmt++) {
		case '\0':
			// XXX Print the last format character?
			return;
		
		case 'l':
			lflag = 1;
			goto reswitch;
		
		case 'b':
			ul = va_arg(ap, int);
			p = va_arg(ap, char *);
			kprintn(put, ul, *p++);
			if (!ul)
				break;
			
			for (set = 0; (n = *p++) != 0;) {
				if (ul & (1 << (n - 1))) {
					put(set ? ',' : '<');
					for (; (n = *p) > ' '; ++p)
						put(n);
					set = 1;
				} else
					for (; *p > ' '; ++p);
			}
			if (set)
				put('>');
			break;
		
		case 'c':
			ch = va_arg(ap, int);
				put(ch & 0x7f);
			break;
		
		case 's':
			p = va_arg(ap, char *);
			while ((ch = *p++) != 0)
				put(ch);
			break;
		
		case 'd':
			ul = lflag ? va_arg(ap, long) : va_arg(ap, int);
			if ((long)ul < 0) {
				put('-');
				ul = -(long)ul;
			}
			kprintn(put, ul, 10);
			break;
		
		case 'o':
			ul = va_arg(ap, UINT32);
			kprintn(put, ul, 8);
			break;
		
		case 'u':
			ul = va_arg(ap, UINT32);
			kprintn(put, ul, 10);
			break;
		
		case 'p':
			put('0');
			put('x');
			lflag = 1;
			// fall through
		case 'X':
		case 'x':
			ul = va_arg(ap, UINT32);
			kprintn(put, ul, 16);
			break;
		
		default:
			put('%');
			if (lflag)
				put('l');
			put(ch);
		}
	}
	va_end(ap);
}



